#include <iostream>
#include <fstream>
#include <stdlib.h>
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

void fireman(int) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

struct Node {
    char symbol;
    int tuple;
    int pqcounter;
    Node* left = nullptr;
    Node* right = nullptr;

    Node(char s, int t, int c, Node* l, Node* r) : symbol(s), tuple(t), pqcounter(c), left(l), right(r) {}
};

struct CompareNode {
    bool operator()(Node* left, Node* right) {
        if (left->tuple == right->tuple) {
            if (int(left->symbol) == int(right->symbol)) {
                return left->pqcounter < right->pqcounter;
            }
            return (int(left->symbol) > int(right->symbol));
        }
        return left->tuple > right->tuple;
    }
};

char TranslateCode (Node* root, string code) {
    Node* currNode = root;
    char insert;
    for (int i = 0; i < code.length(); i++) {
        if (code[i] == '0') {
            currNode = currNode->left;
        } else {
            currNode = currNode->right;
        }
        if (currNode->left == nullptr && currNode->right == nullptr) {
            insert = currNode->symbol;
            currNode = root;
        }
    }

    return insert;
}

//creates the huffman tree
Node* CreateHuffTree(char symbols[], int tuples[], int ARR_SIZE) {
    int counter = 0;
    Node *left, *right;

    //create priority_queue
    priority_queue<Node*, vector<Node*>, CompareNode > priorityQueue;
    for (int i = 0; i < ARR_SIZE; i++) {

        Node* node = new Node(symbols[i], tuples[i], counter++, nullptr, nullptr);
        node->pqcounter = i;

        priorityQueue.push(node);
    }

    //counter = 0;
    while (priorityQueue.size() != 1) {
        left = priorityQueue.top();
        priorityQueue.pop();

        right = priorityQueue.top();
        priorityQueue.pop();

        Node* top = new Node('\0', left->tuple + right->tuple, counter++, left, right);
        priorityQueue.push(top);
    }
    return priorityQueue.top();

}

//prints the symbol, frequency, and code of each huffman tree node
void PrintCodes(Node* root, string stringCode) {
    if (!root) {
        return;
    }
    if (root->symbol != '\0') {
        cout << "Symbol: " << root->symbol << ", Frequency: " << root->tuple << ", Code: " << stringCode << endl;
    }

    PrintCodes(root->left, stringCode + "0");
    PrintCodes(root->right, stringCode + "1");
}

int main(int argc, char* argv[]) {
    int sockfd, newsockfd, portno, clilen, n;
    char mess[1500];
    signal(SIGCHLD, fireman);
    struct sockaddr_in serv_addr, cli_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cout << "Socket open ERROR";
        exit(1);
    }
    bzero((char*)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(sockfd, 100);
    clilen = sizeof(cli_addr);

    vector<char> arr1;
    vector<char> arr2;

    char tempChar;
    int tempInt;
    int size_count = 0;

    //vectors to find size of the future array
    string tempString = "";

    while (getline(cin, tempString)) {
        stringstream ss(tempString);
        tempChar = tempString[0];
        arr1.push_back(tempChar);
        ss.ignore();
        ss >> tempInt; arr2.push_back(tempInt);
        size_count += 1;
    }

    char charArr[size_count];
    int intArr[size_count];

    //populating arrays for processing
    for (int i = 0; i < size_count; i++) {
        charArr[i] = arr1[i];
        intArr[i] = arr2[i];
    }

    Node* rootNode = CreateHuffTree(charArr, intArr, size_count);
    PrintCodes(rootNode, "");

    while (true)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        if (fork() == 0)
        {
            close(sockfd);
            recv(newsockfd, mess, sizeof(mess), 0);
            //translate the code to a symbol
            char letter = TranslateCode(rootNode, mess);
            memset(&mess, 0, sizeof(mess));
            send(newsockfd, (char *)&letter, 1, 0);
            memset(&mess, 0, sizeof(mess));
            close(newsockfd);
            _exit(0);
        }
    }
    wait(0);
    close(sockfd);
    return 0;
}
