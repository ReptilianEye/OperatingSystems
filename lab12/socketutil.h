#ifndef SOCKETUTIL_SOCKETUTIL_H
#define SOCKETUTIL_SOCKETUTIL_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <malloc.h>

#define MAX_CONNECTIONS 10
#define MAX_BUFFER_SIZE 1024
#define MAX_NAME_SIZE 100
#define MAX_COMMAND_SIZE 10

struct sockaddr_in *createIPv4Address(char *ip, int port);

int createUDPSocket();

#endif // SOCKETUTIL_SOCKETUTIL_H