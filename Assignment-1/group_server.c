#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NO_GROUPS_MAX 5
#define BUFFER_SIZE 1024
#define GROUP_NAME_MAX 20
#define NO_CLIENTS_MAX 100

int fd;    
int conn;  
int connections[NO_GROUPS_MAX][NO_CLIENTS_MAX];
char group_names[NO_GROUPS_MAX][GROUP_NAME_MAX];
int connections_Count[NO_GROUPS_MAX];
int group_count = 0;

pthread_mutex_t newConnectionLock = PTHREAD_MUTEX_INITIALIZER;

struct JoinRequest {
    char groupName[20];
    char name[20];
};

struct JoinResponse {
    int id;
    int groupId;
};

struct Message {
    int id;
    int groupId;
    char name[20];
    char message[200];
};
void closeServer() {
    printf(
        "Closing server (Y/N) \n");
    char response;
    scanf("%c", &response);
    if (response == 'Y' || response == 'y') {
        printf("Closed\n");
        for (int i = 0; i < group_count; i++) {
            for (int j = 0; j < connections_Count[i]; j++) {
                close(connections[i][j]);
            }
        }
        close(fd);
        exit(EXIT_SUCCESS);
    }
}

void handle_my(int sig) {
    switch (sig) {
        case SIGINT:
            closeServer();
            break;
    }
}

struct sockaddr_in serv;  

void *connection_handler(void *connec) {
    int nsd = *(int *)connec;

    struct JoinRequest request;
    struct JoinResponse response;
    read(nsd, &request, sizeof(request));  
    char *name = request.name;
    char *groupname = request.groupName;
    printf("%s\n", name);
    printf("%s\n", groupname);

    int flag = 1;
    int groupId;
    int id;

    pthread_mutex_lock(&newConnectionLock);
    for (int i = 0; i < group_count; i++) {
        if (!strcmp(groupname, group_names[i])) {
            groupId = i;
            id = connections_Count[i]++;
            connections[groupId][id] = nsd;
            flag = 0;
            break;
        }
    }

    if (flag) {
        int i = group_count++;
        strcpy(group_names[i], request.groupName);
        groupId = i;
        id = connections_Count[i]++;
        connections[groupId][id] = nsd;
    }
    pthread_mutex_unlock(&newConnectionLock);
    response.id = id;
    response.groupId = groupId;

    write(nsd, &response, sizeof(response));
    struct Message message;
    while (read(nsd, &message, sizeof(message))) {
      printf("%d: ", message.id);
      printf("%s\n", message.message);
        for (int i = 0; i < connections_Count[groupId]; i++) {
            if (~connections[groupId][i] && i != message.id) {
                write(connections[groupId][i], &message, sizeof(message));
            }
        }
    }
    connections[groupId][id] = -1;
    return NULL;
}

int main(int argc, char *argv[]) {
    int fd, new_socket, valread;
    struct sockaddr_in serv, client;

    int addrlen = sizeof(serv);

    pthread_t thread;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    signal(SIGINT, handle_my);
    socklen_t clientLen;
 
    int port_no = atoi(argv[1]);
    printf("%d\n", port_no);
    serv.sin_family = AF_INET;
    serv.sin_port =
        htons(port_no);  
    serv.sin_addr.s_addr = INADDR_ANY;

    int true = 1;

    if (bind(fd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    clientLen = sizeof(client);
    while (1) {
        printf("Start typing\n");
        conn = accept(fd, (struct sockaddr *)&serv, (socklen_t *)&addrlen);
        printf("%d\n", conn);
        if (pthread_create(&thread, NULL, connection_handler, (void *)&conn) <
            0) {
            perror("thread not created");
            exit(0);
        }
    }
    pthread_exit(NULL);
    close(fd);
    return 0;
}
