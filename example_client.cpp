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
#include<pthread.h>
#include <fstream>

#include <fstream>

using namespace std;

bool test_tcp_connection(TCPStream *stream);

bool send_messege(TCPStream *stream);

bool send_file(TCPStream *stream);

void *receiver(void *ptr);

TCPStream *stream = NULL;

int main() {
    string ip;
    int port;
    TCPConnector *connector = new TCPConnector();
    while (1) {
        cout << "Please input IP:\n";
        cin >> ip;
        cout << "Please input port:\n";
        cin >> port;
        //TCPConnector *connector = new TCPConnector();
        //TCPStream *stream = connector->connect(ip.c_str(), port);
        //streamR = stream;
        stream = connector->connect(ip.c_str(), port);
        if (!test_tcp_connection(stream))
            continue;
        pthread_t id;
        int ret = pthread_create(&id, NULL, receiver, NULL);
        if (ret) {
            cout << "Create pthread error!" << endl;
            continue;
        }

        char a;
        for (;;) {
            cout << "\nPlease choose: \n\t(1)Send Messeges\n\t(2)Send Files\n\t(3)Exit\n";
            cin >> a;
            switch (a) {
                case '1':
                    send_messege(stream);
                    continue;
                case '2':
                    send_file(stream);
                    continue;
                case '3':
                    stream->send("/exit", 6);
                    cout << endl << "Exiting......" << endl;
                    break;
                default:
                    cout << "\nNo such option!\n";
                    continue;
            }
            break;
        }
        pthread_cancel(id);
        //delete connector;
        delete stream;
    }
}

bool send_messege(TCPStream *stream) {
    cout << "\n\nNow you are chatting\nInput \"/quit\" to quit chatting127" << endl;
    cout << "=====================================\n" << endl;
    while (stream) {
        string testString;
        cin >> testString;
        if (testString.compare("/quit") == 0)
            return true;
        stream->send(testString.c_str(), testString.size());
    }
}

bool send_file(TCPStream *stream) {
    char *buffer;
    long size;
    string file_location = "/tmp/";
    string file_name = "1.txt";
    file_location += file_name;
    ifstream in(file_location.c_str(), ios::binary);
    in.seekg(0, ios::end);//Set position to the end, in order to know the size of file
    size = in.tellg();//get size of the file
    if (size < 0) {//In case the file does not exist.
        cout << "Cannot get the file!\n";
        return false;
    }
    /*make a sending request*/
    string send_request = "/file";
    send_request.append(" ");
    send_request = send_request + file_name;
    send_request.append(" ");
    send_request = send_request + to_string(size);

    file_location += file_name;
    in.seekg(0, ios::beg);
    int times=1500;
    for(size;size>=1500;size-=1500,times+=1500)
    {
        buffer = new char[1500];
        in.read(buffer, 1500);
        stream->send(buffer, 1500);//send a piece of file
        in.seekg(times, ios::beg);
        delete buffer;
    }
    buffer = new char[size];
    in.read(buffer, size);
    stream->send(buffer, size);//send last piece of file
    delete buffer;
    in.close();
    return true;
}


bool rec_file(std::string filename,long filesize,TCPStream *stream) {
    string file_location="/tmp/"+filename;
    char * buffer;
    long sizeCheck=0;
    int received=0;
    ofstream outfile;
    outfile.open(file_location,ios::out);

    if(filesize>1499){
        buffer=(char *) malloc(1500);
        while(sizeCheck<filesize){
            received=stream->receive(buffer,1500);
            sizeCheck+=received;
            outfile<<buffer;
        }

    }
    else{
        buffer=(char *) malloc(1500);
        received=stream->receive(buffer,1500);
        outfile<<buffer;

    }
    outfile.close();

    return true;
    /*const char *filename2 = "/tmp/2.mp4";
    ofstream out(filename2, ios::binary | ios::trunc);
    out.write(buffer, size);
    out.close();*/

}

bool test_tcp_connection(TCPStream *stream) {
    if (stream) {
        char line[50];
        string testPing = "Ping";
        //cout << "Sent \"Ping\" to test server" <<endl;
        sleep(5);
        stream->send(testPing.c_str(), testPing.size());
        stream->receive(line, sizeof(line));
        string response(line);
        //delete stream;
        if (response.compare("Pong") == 0) {
            cout << "=====================================" << endl;
            cout << "Connected to server\n";
            return true;
        }
    }
    cout << "Server is offline!!!\n";
    return false;
}

void *receiver(void *ptr) {
    for (;;) {
        if (stream != NULL) {
            ssize_t len;
            char line[1000];
            while ((len = stream->receive(line, sizeof(line))) > 0) {
                line[len] = 0;
                std::string msg(line);
                std::vector<std::string> splitCommand=split(msg,' ');
                if(splitCommand[0].compare("/file"))
                    rec_file(splitCommand[1],stol(splitCommand[2]),stream);
                if(line)
                printf("Received:%s\n", line);
            }
        }
    }
}
