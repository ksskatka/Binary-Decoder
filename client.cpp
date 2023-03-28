#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cstring>
#include <queue>
#include <vector>
#include <string>
#include <strings.h>
#include <cstring>
#include <functional>
#include <sstream>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>

using namespace std;

struct arg {
    string finalMessage;
    string code;
    string pos;
    int msgSize = 0;
    char insert;
    string tempString;
    vector<pthread_t> pids;
    int portno;
    char* server;
    int n, sockfd;
    struct sockaddr_in serv_addr;
};

void* DecompressHelper(void* args) {
    //(struct arg*)args;
    char mess[1500];
    ((struct arg*)args)->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *)&((struct arg*)args)->serv_addr, sizeof(((struct arg*)args)->serv_addr));
    ((struct arg*)args)->serv_addr.sin_family = AF_INET;
    ((struct arg*)args)->serv_addr.sin_port = htons(((struct arg*)args)->portno);
    connect(((struct arg*)args)->sockfd, (struct sockaddr *)&((struct arg*)args)->serv_addr, sizeof(((struct arg*)args)->serv_addr));

    ((struct arg*)args)->pids.push_back(pthread_self());
    stringstream ss(((struct arg*)args)->tempString);
    ss >> ((struct arg*)args)->code;

    // send args.code to the server

    memset(&mess, 0, sizeof(mess));
    ::strcpy(mess, ((struct arg*)args)->code.c_str());
    send(((struct arg*)args)->sockfd, (char *)&mess, sizeof(mess), 0);
    // sent
    // receive the symbol to be printed
    recv(((struct arg*)args)->sockfd, (char *)&mess, sizeof(mess), 0);
    //received
    char insert = *mess;

    while (ss >> ((struct arg*)args)->pos) {
        int i = stoi(((struct arg*)args)->pos);
        if ((i + 1) > ((struct arg*)args)->msgSize) {
            ((struct arg*)args)->msgSize = (i + 1);
            ((struct arg*)args)->finalMessage.resize(((struct arg*)args)->msgSize);
        }
        ((struct arg*)args)->finalMessage[i] = insert;
    }
    memset(&mess, 0, sizeof(mess));
    close(((struct arg*)args)->sockfd);
    return nullptr;
}

//decompresses the compressed file given. generates a new pthread every time the original message has a unique symbol inserted during construction
void Decompress(struct arg *args) {
    string tempString;
    string code;
    string pos;
    string decodedMsg = "";
    int msgSize = 0;
    int counter = 0;

    struct timespec req, rem; req.tv_sec = 0; req.tv_nsec = 10000000;
    while (getline(cin, tempString)) {
        args->tempString = tempString;

        pthread_t thread;
        if (pthread_create(&thread, nullptr, DecompressHelper, (void*)args) == 0) {
            counter++;
            nanosleep(&req, &rem);
        }
    }
    for (int j = 0; j < counter; j++) {
        //cout << args.pids[j] << endl; ***testing for unique pthreads being created***
        pthread_join(args->pids[j], nullptr);
    }
    cout << "Original message: " << args->finalMessage;
}

int main (int argc, char* argv[]) {
    struct arg *args = new struct arg;
    args->portno = atoi(argv[2]);
    args->server = argv[1];

    Decompress(args);

    delete args;
    return 0;
}
