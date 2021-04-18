#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define LISTEN_PORT 8088
#define LISTEN_QUEUE_SIZE 16
#define MAX_DATA_SIZE 1024

void process_handle(int connfd, struct sockaddr_in client);
void* handle(void* arg);
struct Attribute {
    int connfd;
    struct sockaddr_in client;
};


int main(){
    int listenfd, connfd;
    struct sockaddr_in servaddr, client;
    struct Attribute *att;
    socklen_t len;
    pthread_t tid;


    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0){
        std::cout << "failed to create socket" << std::endl;
        exit(1);
    }

    // setup keepalive
    // int KEEP_ALIVE = 1000;
    // int KEEP_IDLE = 10000;
    // int KEEP_INTERVAL = 5000;
    // int KEEP_COUNT = 3000;

    // setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&KEEP_ALIVE, sizeof(KEEP_ALIVE));
    // setsockopt(listenfd, SOL_TCP, TCP_KEEPIDLE,(void *)&KEEP_IDLE, sizeof(KEEP_IDLE));
    // setsockopt(listenfd, SOL_TCP, TCP_KEEPINTVL,(void *)&KEEP_INTERVAL, sizeof(KEEP_INTERVAL));
    // setsockopt(listenfd, SOL_TCP, TCP_KEEPCNT,(void *)&KEEP_COUNT, sizeof(KEEP_COUNT));

    // init servaddr
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(LISTEN_PORT);
    
    //bind socket to addr
    int bind_ok = bind(listenfd,(struct sockaddr *) &servaddr, sizeof(servaddr));
    if(bind_ok < 0){
        std::cout << "failed to bind socket with server address" << std::endl;
        exit(1);
    }

    int listen_ok = listen(listenfd, LISTEN_QUEUE_SIZE);
    if(listen_ok < 0){
        std::cout << "failed to listen" << std::endl;
        exit(1);
    }

    while(true){
        connfd = accept(listenfd, NULL, NULL);
        if(connfd < 0){
            std::cout << "failed to accept connection" << std::endl;
            continue;
        }

        att = (struct Attribute *)malloc(sizeof(struct Attribute));
        att->connfd = connfd;
        memcpy((void*)&att->client, &client, sizeof(client));

        int thread_ok = pthread_create(&tid, NULL, handle, (void*)att);
        if(thread_ok < 0) {
            std::cout << "failed to spawn new thread" << std::endl;
            continue;
        }
    }

    close(listenfd);
    return 0;
}

void* handle(void *arg){
    struct Attribute *att = (struct Attribute *)arg;
    process_handle(att->connfd, att->client);
    free(arg);
    pthread_exit(NULL);
}

void process_handle(int connfd, struct sockaddr_in client){
    int num;
    char recvBuf[MAX_DATA_SIZE], sendBuf[MAX_DATA_SIZE];

    printf("connection established with %s\n", inet_ntoa(client.sin_addr));

    sprintf(sendBuf, "%s", "Greeting From Server!\n");
    write(connfd, sendBuf, strlen(sendBuf));
    
    while(true){
        memset(recvBuf, 0, MAX_DATA_SIZE);
        num = recv(connfd, recvBuf, MAX_DATA_SIZE, 0);
        if(num <= 0){
            break;
        }
        printf("received message from client: %s",  recvBuf);
    }

    printf("connection with %s closed\n", inet_ntoa(client.sin_addr));
    
    close(connfd);
}