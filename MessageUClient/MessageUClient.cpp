// MessageUClient.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "Client.h"

int main()
{
    try {
        Client* client = new Client();
        client->start();
        delete client;
    }
    catch (exception e) {
        cout << e.what() << "\nexisting" << endl;
    }
    exit(0);
}
