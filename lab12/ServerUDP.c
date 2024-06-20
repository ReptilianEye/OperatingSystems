#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include "socketutil.h"
#include <stdlib.h>
#include <stdio.h>

struct AcceptedSocket
{
    int socketID;
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
    char name[MAX_NAME_SIZE];
};

struct AcceptedSocket acceptedSockets[MAX_CONNECTIONS];

struct AcceptedSocket *acceptIncomingConnection(int serverSocketFD);

void start_accepting(int serverSocketFD);

int check_if_client_exists(int port);

int add_client(struct AcceptedSocket *pSocket);

void receive_and_print_incoming_data(struct AcceptedSocket *pSocket, char *buffer, int serverSocketFD);

void receive_and_print_incoming_data_thread(struct AcceptedSocket *pSocket);

void broadcast(char *buffer, int socketFD, int serverSocketFD);

void unicast(int from, int to, char *message, int serverSocketFD);

void terminate_socket(struct AcceptedSocket *pSocket);

void list_clients(int socketFD, int serverSocketFD);

int check_if_client_exists(int port)
{
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (acceptedSockets[i].address.sin_port == port)
        {
            return i;
        }
    }
    return -1;
}

int add_client(struct AcceptedSocket *pSocket)
{
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (acceptedSockets[i].acceptedSocketFD == 0)
        {
            acceptedSockets[i] = *pSocket;
            acceptedSockets[i].socketID = i;
            return i;
        }
    }
    return MAX_CONNECTIONS;
}

struct AcceptedSocket *acceptIncomingConnection(int serverSocketFD)
{
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);

    char buffer[MAX_BUFFER_SIZE];
    ssize_t amountReceived = recvfrom(serverSocketFD, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &clientAddressSize);

    struct AcceptedSocket *acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = serverSocketFD;
    acceptedSocket->acceptedSuccessfully = amountReceived > 0;

    return acceptedSocket;
}

void start_accepting(int serverSocketFD)
{
    char buffer[MAX_BUFFER_SIZE];
    for (size_t i = 0; i < MAX_CONNECTIONS; i++)
        acceptedSockets[i] = (struct AcceptedSocket){0};

    while (true)
    {
        struct AcceptedSocket *clientSocket = acceptIncomingConnection(serverSocketFD);
        int id = check_if_client_exists(clientSocket->address.sin_port);
        if (id < 0)
        {
            id = add_client(clientSocket);
            if (id < MAX_CONNECTIONS)
            {
                ssize_t amountReceived = recvfrom(serverSocketFD, buffer, sizeof(buffer), 0, NULL, NULL);
                buffer[amountReceived] = 0;
                strcpy(acceptedSockets[id].name, buffer);
                printf("Client %s connected: %d\n", acceptedSockets[id].name, ntohs(acceptedSockets[id].address.sin_port));
            }
            else
            {
                printf("Server is full\n");
            }
        }
        else
        {
            printf("Client %s already connected: %d\n", acceptedSockets[id].name, ntohs(acceptedSockets[id].address.sin_port));
            ssize_t amountReceived = recvfrom(serverSocketFD, buffer, sizeof(buffer), 0, NULL, NULL);
            buffer[amountReceived] = 0;
            receive_and_print_incoming_data(&acceptedSockets[id], buffer, serverSocketFD);
        }
    }
}

void receive_and_print_incoming_data(struct AcceptedSocket *pSocket, char *buffer, int serverSocketFD)
{
    printf("%s: %s\n", pSocket->name, buffer);

    char command[MAX_COMMAND_SIZE], content[MAX_BUFFER_SIZE - MAX_COMMAND_SIZE - 1];
    int id;

    if (sscanf(buffer, "%9s %d %255[^\n]", command, &id, content) == 3 && strcmp(command, "2ONE") == 0)
    {
        printf("sending message to client %d\n", id);
        unicast(pSocket->socketID, id, content, serverSocketFD);
    }
    else if (sscanf(buffer, "%9s %255[^\n]", command, content) == 2 && strcmp(command, "2ALL") == 0)
    {
        printf("broadcasting message\n");
        broadcast(content, pSocket->socketID, serverSocketFD);
    }
    else if (sscanf(buffer, "%9s", command) == 1)
    {
        if (strcmp(command, "LIST") == 0)
        {
            list_clients(pSocket->socketID, serverSocketFD);
        }
        else if (strcmp(command, "STOP") == 0)
        {
            terminate_socket(pSocket);
        }
        else
        {
            printf("Invalid command from client %s\n", pSocket->name);
        }
    }
    else
    {
        printf("Invalid command from client %s\n", pSocket->name);
    }
}

void broadcast(char *buffer, int socketID, int serverSocketFD)
{

    for (int i = 0; i < MAX_CONNECTIONS; i++)
        if (i != socketID && acceptedSockets[i].acceptedSocketFD != 0)
        {
            unicast(socketID, i, buffer, serverSocketFD);
        }
}

void unicast(int from, int to, char *message, int serverSocketFD)
{
    char buffer[MAX_BUFFER_SIZE];
    sprintf(buffer, "message from %s: %s", acceptedSockets[from].name, message);

    sendto(serverSocketFD,
           buffer, MAX_BUFFER_SIZE, 0,
           (struct sockaddr *)&acceptedSockets[to].address,
           sizeof(acceptedSockets[to].address));
}

void list_clients(int id, int serverSocketFD)
{
    char list[MAX_BUFFER_SIZE] = "Active clients:\n";
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (acceptedSockets[i].acceptedSocketFD > 0)
        {
            char buffer[MAX_BUFFER_SIZE];
            sprintf(buffer, "%d: %s", acceptedSockets[i].socketID, acceptedSockets[i].name);
            strcat(list, buffer);
            strcat(list, "\n");
        }
    }
    sendto(serverSocketFD,
           list, MAX_BUFFER_SIZE, 0,
           (struct sockaddr *)&acceptedSockets[id].address,
           sizeof(acceptedSockets[id].address));
}

void terminate_socket(struct AcceptedSocket *pSocket)
{
    acceptedSockets[pSocket->socketID] = (struct AcceptedSocket){0};
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }
    char *ipAddress = argv[1];
    if (strcmp(ipAddress, "def") == 0)
        ipAddress = "";

    int port = atoi(argv[2]);

    int serverSocketFD = createUDPSocket();
    struct sockaddr_in *serverAddress = createIPv4Address(ipAddress, port);

    int result = bind(serverSocketFD, (const struct sockaddr *)serverAddress, sizeof(*serverAddress));
    if (result == 0)
        printf("socket was bound successfully\n");

    start_accepting(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);

    return 0;
}