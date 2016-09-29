#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include "TCP/TCPAcceptor.h"
#include "TCP/TCPConnector.h"
#include "TCP/TCPStream.h"
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
    string send_request = "/file";
    stream->send(send_request.c_str(), send_request.size());//send file transferring request
    sleep(1);
    string file_location = "/tmp/";
    string file_name = "1.txt";
    stream->send(file_name.c_str(), file_name.size());//send file name
    sleep(1);
    char *buffer;
    long size;
    file_location += file_name;
    ifstream in(file_location.c_str(), ios::binary);
    in.seekg(0, ios::end);//Set position to the end, in order to know the size of file
    size = in.tellg();
    if (size < 0) {//In case the file does not exist.
        cout << "Cannot get the file!\n";
        return false;
    }
    buffer = new char[size];
    in.seekg(0, ios::beg);
    in.read(buffer, size);
    cout<<"Transferring...";
    stream->send(buffer, size);//send file content
    sleep(5);
    return true;
}

bool rec_file() {
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
                printf("Received:%s\n", line);
            }
        }
    }
}
