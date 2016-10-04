#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include "TCP/TCPAcceptor.h"
#include "TCP/TCPConnector.h"
#include "TCP/TCPStream.h"
#include "Utilities.h"
#include <pthread.h>
#include <fstream>
#include <cstring>

using namespace std;

bool test_tcp_connection(TCPStream *stream);

void *send_messege(void *ptr);

bool send_file(string fileName, string fileLocation);

bool rec_file(std::string filename, long filesize);

TCPStream *stream = NULL;

int main(int argc, char **argv) {
    pthread_t id;
    std::string cmd;
    TCPConnector *connector = NULL;
    while (1) {
        connector = new TCPConnector();
        stream = connector->connect(argv[2], atoi(argv[1]));
        if (!test_tcp_connection(stream)) {
            cout << "No server is found!\n";
            if (connector)delete connector;
            if (stream)delete stream;
            cout << "Do you want to re-connect? Input yes to continue.\n";
            while (1){
                cin.clear();
                std::getline(cin, cmd);
                if (cmd.compare("yes") == 0 || cmd.compare("YES") == 0 || cmd.compare("Yes") == 0)
                    break;
            }
            continue;
        }

        for (;;) {
            cin.clear();
            std::getline(cin, cmd);
            if (cmd.compare("") == 0)
                continue;
            std::vector<std::string> splitCommand = split(cmd, ' ');
            if (splitCommand[0].compare("CONNECT") == 0 && splitCommand.size() == 2) {
                stream->send(cmd.c_str(), cmd.size());
                break;
            } else {
                cout << "INVALID COMMAND ENTERED\n";
                continue;
            }
        }

        int ret = pthread_create(&id, NULL, send_messege, NULL);
        if (ret) {
            cout << "Create pthread error!" << endl;
            continue;
        }

        if (stream != NULL) {
            ssize_t len;
            char line[1000];
            while ((len = stream->receive(line, sizeof(line))) > 0) {
                line[len] = 0;
                std::string msg(line);
                std::vector<std::string> splitCommand = split(msg, ' ');
                if (splitCommand[0].compare("FILE") == 0) {
                    rec_file(splitCommand[1], stol(splitCommand[2], nullptr, 10));
                    printf("Have received a file\n");
                    continue;
                }
                if (splitCommand[0].compare("EXIT") == 0) {
                    cout << "The Chatting Channel is closed!\n\n";
                    break;
                } else
                    printf("\n::%s\n", line);
            }
        }
        pthread_cancel(id);
        delete connector;
        delete stream;
        sleep(2);
    }

}

bool test_tcp_connection(TCPStream *stream) {
    if (stream) {
        char line[50];
        string testPing = "PING";
        stream->send(testPing.c_str(), testPing.size());
        stream->receive(line, sizeof(line));
        string response(line);
        if (response.compare("PONG") == 0) {
            cout << "=============CONNECTING==============\n";
            cout << " SUCCESSFULLY CONNECTED TO THE SERVER\n";
            cout << "ENTER CONNECT \"USERNAME\" TO BE ADDED TO THE QUEUE\n";
            return true;
        }
    }
    cout << "Server is offline!!!\n";
    return false;
}

void *send_messege(void *ptr) {
    while (stream) {
        string sendString;
        cin.clear();
        std::getline(cin, sendString);
        if (sendString.compare("") == 0)
            continue;
        std::vector<std::string> splitCommand = split(sendString, ' ');
        if (sendString.compare("EXIT") == 0) {
            stream->send(sendString.c_str(), sendString.size());
            return ptr;
        }
        if (splitCommand[0].compare("HELP") == 0) {
            stream->send(splitCommand[0].c_str(), splitCommand[0].size());
            continue;
        }
        if ((splitCommand[0].compare("FILE") == 0) && (splitCommand.size() == 3)) {
            send_file(splitCommand[1], splitCommand[2]);
            continue;
        }
        if (splitCommand[0].compare("MSG") == 0) {
            stream->send(sendString.c_str(), sendString.size());
            continue;
        }
        if (sendString.compare("QUIT") == 0) {
            stream->send(sendString.c_str(), sendString.size());
            continue;
        }
        if (sendString.compare("CHAT") == 0) {
            stream->send(sendString.c_str(), sendString.size());
            continue;
        }
        cout << "Invalid command!\nTo get help, please input: HELP\n";
    }
}

bool send_file(string fileName, string fileLocation) {
    char *buffer;
    long size;
    int sizeCheck = 0;
    int bytesSent = 0;
    ifstream in(fileLocation.c_str(), ios::binary);
    in.seekg(0, ios::end);//Set position to the end, in order to know the size of file
    size = in.tellg();//get size of the file
    if (size < 0) {//In case the file does not exist.
        cout << "Cannot get the file!\n";
        return false;
    }
    if (size > 104857600) {
        cout << "Error:File Size Exceeding 100 MB\n";
        return false;
    }
    /*make a sending request*/
    string send_request = "FILE";
    send_request.append(" ");
    send_request = send_request + fileName;
    send_request.append(" ");
    send_request = send_request + to_string(size);
    stream->send(send_request.c_str(), send_request.size());
    sleep(0.5);
    cout << "Transfering a file\n";
    in.seekg(0, ios::beg);
    buffer = new char[1500];
    while (sizeCheck < size) {

        if ((size - sizeCheck) <= 1500) {

            delete[] buffer;
            buffer = new char[(size - sizeCheck)];
            in.read(buffer, size - sizeCheck);
            bytesSent = stream->send(buffer, size - sizeCheck);
        } else {
            in.read(buffer, 1500);
            bytesSent = stream->send(buffer, 1500);
        }
        sizeCheck += bytesSent;
        in.seekg(sizeCheck, ios::beg);
    }
    delete[] buffer;
    in.close();
    printf("File has been sent!\n");
    return true;
}


bool rec_file(std::string filename, long filesize) {

    string file_location = "/tmp/" + filename;
    char *buffer;
    long sizeCheck = 0;
    int received = 0;
    ofstream outfile(file_location, ios::binary | ios::trunc);

    buffer = new char[1500];
    std::memset(buffer, 0, sizeof(*buffer) * 1500);
    while (sizeCheck < filesize) {
        if ((filesize - sizeCheck) <= 1500) {
            delete[] buffer;
            buffer = new char[(filesize - sizeCheck)];
            received = stream->receive(buffer, (filesize - sizeCheck));
        } else
            received = stream->receive(buffer, 1500);
        outfile.seekp(sizeCheck, ios::beg);
        sizeCheck += received;
        outfile.write(buffer, received);
        outfile.flush();
        if (received == 0)
            break;
    }
    delete[] buffer;
    outfile.close();
    return true;
}
