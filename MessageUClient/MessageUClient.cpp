// MessageUClient.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "Client.h"

int main()
{
    Client* client = nullptr;
    try {
        client = new Client();
        client->start();
    }
    catch (exception& e) {
        cout << e.what() << "\nexisting" << endl;
    }
    delete client;
    exit(0);
}
