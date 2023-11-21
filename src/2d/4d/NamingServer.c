#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_STORAGE_SERVERS 10
#define MAX_CLIENTS 100
#define NAMING_SERVER_PORT 8000
#define HEARTBEAT_INTERVAL 5
#define MAX_CACHE_SIZE 100
#define LOCALIPADDRESS "192.168.1.1"

typedef struct FileSystem {
    // Simplified for example
    char fileTree[1000];  // Placeholder for file tree representation
} FileSystem;

typedef struct StorageServer {
    char ipAddress[16];  // IPv4 Address
    int nmPort;          // Port for NM Connection
    int clientPort;      // Port for Client Connection
    char accessiblePaths[1000]; // List of accessible paths
    // Other metadata as needed
} StorageServer;


void handleClientRequest();
FileSystem fileSystem[MAX_STORAGE_SERVERS];
StorageServer storageServers[MAX_STORAGE_SERVERS];
int storageServerCount = 0;

void initializeNamingServer() {
    printf("Initializing Naming Server...\n");
    // Initialize file system and directory structure
    memset(fileSystem, 0, sizeof(fileSystem));

    // Initialize storage server list
    memset(storageServers, 0, sizeof(storageServers));
    storageServerCount = 0;

}


void registerStorageServer(char* ipAddress, int nmPort, int clientPort, char* accessiblePaths) {
    if (storageServerCount < MAX_STORAGE_SERVERS) {
        StorageServer server;
        strcpy(server.ipAddress, ipAddress);
        server.nmPort = nmPort;
        server.clientPort = clientPort;
        strcpy(server.accessiblePaths, accessiblePaths);

        storageServers[storageServerCount++] = server;
    } else {
        printf("Storage server limit reached.\n");
    }
}

pthread_mutex_t storageServerMutex = PTHREAD_MUTEX_INITIALIZER;

void *handleStorageServer(void *socketDesc) {
    int sock = *(int*)socketDesc;
    char buffer[1024]; // Adjust size as needed
    int readSize;

    // Read data from the socket
    if ((readSize = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        buffer[readSize] = '\0';

        // Parse data to extract Storage Server details
        // For example, if the data is sent as a comma-separated string
        char *token;
        char *rest = buffer;
        StorageServer newServer;

        // Extract IP Address
        token = strtok_r(rest, ",", &rest);
        strncpy(newServer.ipAddress, token, sizeof(newServer.ipAddress));

        // Extract NM Port
        token = strtok_r(rest, ",", &rest);
        newServer.nmPort = atoi(token);

        // Extract Client Port
        token = strtok_r(rest, ",", &rest);
        newServer.clientPort = atoi(token);

        // Extract Accessible Paths
        token = strtok_r(rest, ",", &rest);
        strncpy(newServer.accessiblePaths, token, sizeof(newServer.accessiblePaths));



        // Lock mutex before updating global storage server array
        pthread_mutex_lock(&storageServerMutex);
        if (storageServerCount < MAX_STORAGE_SERVERS) {
            storageServers[storageServerCount++] = newServer;
            pthread_mutex_unlock(&storageServerMutex);
            
            // Printing
            printf("Received storage server details:\n");
            printf("IP Address: %s\n", newServer.ipAddress);
            printf("NM Port: %d\n", newServer.nmPort);
            printf("Client Port: %d\n", newServer.clientPort);
            printf("Accessible Paths: %s\n", newServer.accessiblePaths);

            // Send ACK
            const char* ackMessage = "Registration Successful";
            send(sock, ackMessage, strlen(ackMessage), 0);
        } else {
            pthread_mutex_unlock(&storageServerMutex);
            // Send server limit reached message
            const char* limitMessage = "Storage server limit reached";
            send(sock, limitMessage, strlen(limitMessage), 0);
        }
    }

    close(sock);
    free(socketDesc);
    return 0;
}

void startStorageServerListener() {
    int server_fd, new_socket, *new_sock;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the port
    address.sin_family = AF_INET;
    // address.sin_addr.s_addr = LOCALIPADDRESS;
    address.sin_addr.s_addr = INADDR_ANY;
    
    address.sin_port = htons(NAMING_SERVER_PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, MAX_STORAGE_SERVERS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        pthread_t sn_thread;
        new_sock = malloc(sizeof(int));
        *new_sock = new_socket;

        if (pthread_create(&sn_thread, NULL, handleStorageServer, (void*) new_sock) < 0) {
            perror("could not create thread");
            free(new_sock);
        }

        // Optionally, join the thread or detach it
        // pthread_join(sn_thread, NULL); // For synchronous handling
        pthread_detach(sn_thread); // For asynchronous handling
    }
}

void* clientRequestHandler(void* arg) {
    int clientSocket = *(int*)arg;

    // Read request from client
    char buffer[1024];
    read(clientSocket, buffer, sizeof(buffer));

    // Parse and handle request
    // ...

    close(clientSocket);
    return NULL;
}

void handleClientRequest() {
    printf("Starting to accept client requests...\n");

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(NAMING_SERVER_PORT);

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, MAX_CLIENTS);

    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

        if (clientSocket < 0) {
            perror("Error accepting client connection");
            continue;
        }

        pthread_t threadId;
        pthread_create(&threadId, NULL, clientRequestHandler, &clientSocket);
    }
}




int main() {
    // Initialize the Naming Server
    initializeNamingServer();

    // Start a thread to detect Storage Server
    startStorageServerListener();


    // Start accepting client requests
    handleClientRequest();


    // The rest of the main function
    // ...

    return 0;
}
