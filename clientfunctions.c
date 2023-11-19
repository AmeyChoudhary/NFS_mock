#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#define NAMING_SERVER_PORT 8001
#define IP_ADDRESS "127.0.0.1"

char request[100]; // global variable to take input of from client

// read function when client requests to read a file
void clientRead(int clientSocket)
{

    // send the request to the Naming Server
    send(clientSocket, request, strlen(request), 0);

    // receive the response from the Naming Server
    char response[100];
    recv(clientSocket, response, sizeof(response), 0);

    if (strcmp(response, "File not Found") == 0)
    {
        printf("The file doesn't exist\n");
    }
    else
    {

        // tokenising the response into a token array
        char *tokenArrayresponse[10];
        char *tokenresponse = strtok(response, " ");
        int j = 0;
        while (tokenresponse != NULL)
        {
            tokenArrayresponse[j++] = tokenresponse;
            tokenresponse = strtok(NULL, " ");
        }

        // using the response, client tries to connect to the Storage Server TCP socket
        int storageServerSocket = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in storageServerAddress;
        storageServerAddress.sin_family = AF_INET;
        storageServerAddress.sin_port = htons(atoi(tokenArrayresponse[1]));
        storageServerAddress.sin_addr.s_addr = inet_addr(tokenArrayresponse[0]);

        int connectionStatus = connect(storageServerSocket, (struct sockaddr *)&storageServerAddress, sizeof(storageServerAddress));
        if (connectionStatus < 0)
        {
            printf("Error in connection establishment\n");
            exit(1);
        }

        // client sends request to Storage Server
        send(storageServerSocket, request, strlen(request), 0);

        // client receives response from Storage Server
        char responseStorage[100];
        recv(storageServerSocket, responseStorage, sizeof(responseStorage), 0);

        if (strcmp(responseStorage, "Read error") == 0 || strcmp(responseStorage, "No content") == 0 || strcmp(responseStorage, "File not Found") == 0)
        {
            printf("The following error was encountered: \n");
            printf("%s\n", responseStorage);
        }
        else
        {

            printf("File is read as :\n");
            printf("%s\n", responseStorage);
        }

        // client closes connection to Storage Server
        close(storageServerSocket);
    }
}

// write function when client requests to write a file
void clientWrite(int clientSocket)
{
    // send request to Naming Server
    send(clientSocket, request, strlen(request), 0);

    // receive response from Naming Server
    char response[100];
    recv(clientSocket, response, sizeof(response), 0);

    if (strcmp(response, "File not Found") == 0)
    {
        printf("The file doesn't exist\n");
    }
    else
    {

        // tokenising the response into a token array
        char *tokenArrayresponse[10];
        char *tokenresponse = strtok(response, " ");
        int j = 0;
        while (tokenresponse != NULL)
        {
            tokenArrayresponse[j++] = tokenresponse;
            tokenresponse = strtok(NULL, " ");
        }

        // using the response, client tries to connect to the Storage Server TCP socket
        int storageServerSocket = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in storageServerAddress;
        storageServerAddress.sin_family = AF_INET;
        storageServerAddress.sin_port = htons(atoi(tokenArrayresponse[1]));
        storageServerAddress.sin_addr.s_addr = inet_addr(tokenArrayresponse[0]);

        int connectionStatus = connect(storageServerSocket, (struct sockaddr *)&storageServerAddress, sizeof(storageServerAddress));
        if (connectionStatus < 0)
        {
            printf("Error in connection establishment\n");
            exit(1);
        }

        // client sends request to Storage Server
        send(storageServerSocket, request, strlen(request), 0);

        // client receives response from Storage Server
        char responseStorage[100];
        recv(storageServerSocket, responseStorage, sizeof(responseStorage), 0);

        if (strcmp(responseStorage, "Error opening file for writing") == 0 || strcmp(responseStorage, "Write error") == 0 || strcmp(responseStorage, "File not Found") == 0)
        {
            printf("The following error was encountered: \n");
            printf("%s\n", responseStorage);
        }
        else
        {

            // printf("File has been written :\n");
            printf("%s\n", responseStorage);
        }

        // client closes connection to Storage Server
        close(storageServerSocket);
    }
}

// getsize function when client requests to get the size of a file
void clientGetSize(int clientSocket)
{
    // send request to Naming Server
    // strcpy(request, "COPY ./src/hello ./src/jainit\n");
    send(clientSocket, request, strlen(request), 0);

    // receive response from Naming Server
    char response[100];
    recv(clientSocket, response, sizeof(response), 0);

    if (strcmp(response, "File not Found") == 0)
    {
        printf("The file doesn't exist\n");
    }
    else
    {

        // tokenising the response into a token array
        char *tokenArrayresponse[10];
        char *tokenresponse = strtok(response, " ");
        int j = 0;
        while (tokenresponse != NULL)
        {
            tokenArrayresponse[j++] = tokenresponse;
            tokenresponse = strtok(NULL, " ");
        }

        // using the response, client tries to connect to the Storage Server TCP socket
        int storageServerSocket = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in storageServerAddress;
        storageServerAddress.sin_family = AF_INET;
        storageServerAddress.sin_port = htons(atoi(tokenArrayresponse[1]));
        storageServerAddress.sin_addr.s_addr = inet_addr(tokenArrayresponse[0]);

        int connectionStatus = connect(storageServerSocket, (struct sockaddr *)&storageServerAddress, sizeof(storageServerAddress));
        if (connectionStatus < 0)
        {
            printf("Error in connection establishment\n");
            exit(1);
        }

        // client sends request to Storage Server
        send(storageServerSocket, request, strlen(request), 0);

        // client receives response from Storage Server
        char responseStorage[100];
        recv(storageServerSocket, responseStorage, sizeof(responseStorage), 0);

        if (strcmp(responseStorage, "Error getting file size") == 0 || strcmp(responseStorage, "File not Found") == 0)
        {

            printf("The following error was encountered: \n");
            printf("%s\n", responseStorage);
        }
        else
        {

            // printf("File is read as :\n");
            printf("%s\n", responseStorage);
        }
        // client closes connection to Storage Server
        close(storageServerSocket);
    }
}

// creat function to send request to Naming Server to create a file
void clientCreate(int clientSocket)
{
    // send request to Naming Server
    send(clientSocket, request, strlen(request), 0);

    char response[100];
    recv(clientSocket, response, sizeof(response), 0);

    int index_case;
    index_case = atoi(response);

    if (index_case == 1)
    {
        printf("Error creating directory\n");
    }
    else if (index_case == 2)
    {
        printf("Directory created\n");
    }
    else if (index_case == 3)
    {
        printf("Error creating file\n");
    }
    else if (index_case == 4)
    {
        printf("File created\n");
    }
}

// delete function to send request to Naming Server to delete a file
void clientDelete(int clientSocket)
{
    // send request to Naming Server
    send(clientSocket, request, strlen(request), 0);

    char response[100];
    recv(clientSocket, response, sizeof(response), 0);

    int index_case;
    index_case = atoi(response);

    if (index_case == 1)
    {
        printf("Error deleting directory\n");
    }
    else if (index_case == 2)
    {
        printf("Directory deleted\n");
    }
    else if (index_case == 3)
    {
        printf("File deleted\n");
    }
    else if (index_case == 4)
    {
        printf("Error deleting file\n");
    }
    else if (index_case == 5)
    {
        printf("File not Found\n");
    }
}

// copy function to send request to Naming server to copy
void clientCopy(int clientSocket)
{
    // send request to Naming Server
    // strcpy(request, "COPY ./src/hello ./src/jainit\n");
    send(clientSocket, request, strlen(request), 0);
}

// ls function to send request to naming server to list all accessible paths
void clientListAll(int clientSocket)
{
    // send request to Naming Server
    send(clientSocket, request, strlen(request), 0);

    char response[40960];
    recv(clientSocket, response, sizeof(response), 0);

    char *token_1 = strtok(response, "*");

    while (token_1 != NULL)
    {
        // Tokenize using the second delimiter within each token from the first step
        char *token2 = strtok(token_1, "$");

        printf("Storage Server: %s\n", token2);

        while (token2 != NULL)
        {
            token2 = strtok(NULL, "$");
            if (token2 != NULL)
            {

                printf("Accessible path: %s\n", token2);
            }
        }

        token_1 = strtok(NULL, "*");
    }
}

void removeWhitespace(char *inputString)
{
    // Initialize indices for reading and writing in the string
    int readIndex = 0;
    int writeIndex = 0;

    // Iterate through the string
    while (inputString[readIndex] != '\0')
    {
        // If the current character is not a whitespace character, copy it
        if (!isspace(inputString[readIndex]))
        {
            inputString[writeIndex] = inputString[readIndex];
            writeIndex++;
        }

        // Move to the next character in the string
        readIndex++;
    }

    // Null-terminate the new string
    inputString[writeIndex] = '\0';
}

int main()
{
    // client tries to connect to Naming Server TCP socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        printf("Error in client socket creation\n");
        exit(1);
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(NAMING_SERVER_PORT);
    serverAddress.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    int connectionStatus = connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectionStatus < 0)
    {
        printf("Error in connection establishment\n");
        exit(1);
    }

    // client sends request to Naming Server
    while (1)
    {
        printf("Enter request: ");

        // taking input from client using fgets
        fgets(request, 100, stdin);

        char request_copy[100];
        strcpy(request_copy, request);

        // tokenising the request into a token array
        char *tokenArray[10];
        char *token = strtok(request_copy, " ");
        int i = 0;
        while (token != NULL)
        {
            removeWhitespace(token);
            tokenArray[i++] = token;
            token = strtok(NULL, " ");
        }

        // if the request is to read a file
        if (strcmp(tokenArray[0], "READ") == 0)
        {
            clientRead(clientSocket);
        }
        else if (strcmp(tokenArray[0], "WRITE") == 0)
        {
            clientWrite(clientSocket);
        }
        else if (strcmp(tokenArray[0], "GETSIZE") == 0)
        {
            clientGetSize(clientSocket);
        }
        else if (strcmp(tokenArray[0], "CREATE") == 0)
        {
            clientCreate(clientSocket);
        }
        else if (strcmp(tokenArray[0], "DELETE") == 0)
        {
            clientDelete(clientSocket);
        }
        else if (strcmp(tokenArray[0], "COPY") == 0)
        {
            clientCopy(clientSocket);
        }
        else if (strcmp(tokenArray[0], "LISTALL") == 0)
        {
            clientListAll(clientSocket);
        }
        else
        {
            printf("Invalid request\n");
        }

        fflush(stdin);
        memset(request, '\0', sizeof(request));
    }

    return 0;
}