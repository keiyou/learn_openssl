#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8088
#define LISTEN_QUEUE_SIZE 16
#define MAX_DATA_SIZE 1024


struct Attribute {
    int connfd;
    struct sockaddr_in servaddr;
};

void* handle(void *arg);
void send_handle(int connfd, struct sockaddr_in client);

bool closed = false;

int main(){
    int requestfd;
    struct sockaddr_in servaddr;
    struct Attribute *att;
    char recvBuf[MAX_DATA_SIZE];
    pthread_t tid;
    
    requestfd = socket(AF_INET, SOCK_STREAM, 0);
    if(requestfd < 0){
        std::cout << "failed to create socket" << std::endl;
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);

    int addr_ok = inet_aton(SERVER_IP, &servaddr.sin_addr);
    if(addr_ok < 0){
        std::cout << "failed to translate IP" << std::endl;
        exit(1);
    }


    int connect_ok = connect(requestfd, (sockaddr *)&servaddr, sizeof(struct sockaddr_in));
    if(connect_ok < 0){
        std::cout << "failed to connect to server" << std::endl;
        exit(1);
    }

    std::cout << "connection established" << std::endl;


    att = (struct Attribute *)malloc(sizeof(struct Attribute));
    att->connfd = requestfd;
    memcpy((void*)&att->servaddr, &servaddr, sizeof(servaddr));
    int thread_ok = pthread_create(&tid, NULL, handle, (void*)att);
    if(thread_ok < 0) {
        std::cout << "failed to spawn new thread" << std::endl;
        exit(1);
    }


    int num;
    while(!closed){
        memset(recvBuf, 0, MAX_DATA_SIZE);
        num = recv(requestfd, recvBuf, MAX_DATA_SIZE, 0);
        if(num <= 0){
            closed = true;
        }
        printf("received message from server: %s", recvBuf);
    }

    printf("connection with %s closed\n", inet_ntoa(servaddr.sin_addr));

    close(requestfd);

    pthread_join(tid, NULL);

    std::cout << "connection closed" << std::endl;
    return 0;
}

void* handle(void *arg){
    struct Attribute *att = (struct Attribute *)arg;
    send_handle(att->connfd, att->servaddr);
    free(arg);
    pthread_exit(NULL);
}

void send_handle(int connfd, struct sockaddr_in server){
    int num;
    char sendBuf[MAX_DATA_SIZE];

    memset(sendBuf, 0, MAX_DATA_SIZE);
    sprintf(sendBuf, "Greeting From Client!\n");
    int send_ok = send(connfd, sendBuf, MAX_DATA_SIZE, 0);
    if(send_ok < 0){
        std::cout << "failed to send message" << std::endl;
        exit(1);
    }
    
    while(!closed){
        memset(sendBuf, 0, MAX_DATA_SIZE);
        fgets(sendBuf, MAX_DATA_SIZE, stdin);

        if(strcmp(sendBuf, "exit\n") == 0){
            break;
        }

        int send_ok = send(connfd, sendBuf, MAX_DATA_SIZE, 0);
        if(send_ok < 0){
            std::cout << "failed to send message" << std::endl;
            exit(1);
        }
    }
}

