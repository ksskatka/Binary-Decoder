#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <string>
#include <functional>
#include <sstream>
#include <cstdlib>

using namespace std;

struct Node {
    char symbol;
    int tuple;
    Node* left = nullptr;
    Node* right = nullptr;

    Node(char s, int t, Node* l, Node* r) : symbol(s), tuple(t), left(l), right(r) {}
};

struct CompareNode {
    bool operator()(Node* left, Node* right) {
        if (left->tuple == right->tuple) {
            if (int(left->symbol) == int(right->symbol)) {
                return left < right;
            }
            return (int(left->symbol) > int(right->symbol));
        }
        return left->tuple > right->tuple;
    }
};

Node* CreateHuffTree(char symbols[], int tuples[], int ARR_SIZE) {
    Node *left, *right;

    //create priority_queue
    priority_queue<Node*, vector<Node*>, CompareNode > priorityQueue;

    for (int i = 0; i < ARR_SIZE; i++) {
        Node* node = new Node(symbols[i], tuples[i], nullptr, nullptr);
        priorityQueue.push(node);
    }

    while (priorityQueue.size() != 1) {
        left = priorityQueue.top();
        priorityQueue.pop();

        right = priorityQueue.top();
        priorityQueue.pop();

        Node* top = new Node('\0', left->tuple + right->tuple, left, right);
        priorityQueue.push(top);
    }
    return priorityQueue.top();

}

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

void Decompress(Node* root, ifstream &inFile) {
    vector<string> codes;
    string tempString;
    string code;
    string pos;

    char insert = ' ';
    Node* currNode = root;
    string decodedMsg = "";
    int msgSize = 0;
    while (getline(inFile, tempString)) {
        stringstream ss(tempString);
        ss >> code;

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
        while (ss >> pos) {
            int i = stoi(pos);
            if ((i + 1) > msgSize) {
                msgSize = (i + 1);
                decodedMsg.resize(msgSize);
            }
            decodedMsg[i] = insert;

        }
    }
    cout << decodedMsg;
}

int main() {

    ifstream fileIn;
    string fileInName = ""; cin >> fileInName; fileIn.open(fileInName);

    vector<char> arr1;
    vector<char> arr2;

    char tempChar;
    int tempInt;
    int size_count = 0;

    //vectors to find size of the future array
    while (fileIn >> tempChar) {
        arr1.push_back(tempChar);
        fileIn >> tempInt; arr2.push_back(tempInt);
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

    fileIn.close();

    fileInName = ""; cin >> fileInName; fileIn.open(fileInName);

    Decompress(rootNode, fileIn);

    fileIn.close();
    return 0;
}