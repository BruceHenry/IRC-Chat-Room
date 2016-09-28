//
// Created by Erik Nadel on 9/28/16.
//

#ifndef PROJECT1_UTILITIES_H
#define PROJECT1_UTILITIES_H

#include <string>
#include <vector>
#include <sstream>

// You could also take an existing vector as a parameter.
std::vector<std::string> split(std::string str, char delimiter) {
    std::vector<std::string> internal;
    std::stringstream ss(str); // Turn the string into a stream.
    std::string tok;

    while(getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }

    return internal;
}

void send(string msg, TCPStream *stream){
    stream->send(msg.c_str(), msg.length());
}

#endif //PROJECT1_UTILITIES_H
