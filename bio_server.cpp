#include <stdio.h>
#include <openssl/bio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define SERVER_ADDR "localhost:8088"

void process_handle(BIO *connection);
void* handle(void* arg);


int main(){
    BIO *accept = NULL;
    char *addr = NULL;
    pthread_t tid;

    accept = BIO_new_accept(SERVER_ADDR);

    int accept_ok = BIO_do_accept(accept);
    if(accept_ok <= 0){
        printf("failed to set up accept");
        exit(1);
    }


    while(true){
        accept_ok = BIO_do_accept(accept);
        if(accept_ok <= 0){
            printf("failed to accept new connection");
            exit(1);
        }

        BIO *connection = BIO_pop(accept);
        int thread_ok = pthread_create(&tid, NULL, handle, connection);
        if(thread_ok < 0){
            printf("failed to spwan new thread");
            exit(1);
        }
    }

    BIO_free(accept);
    return 0;
}


void* handle(void *arg){
    BIO* connection = (BIO *)arg;
    process_handle(connection);
    pthread_exit(NULL);
}

void process_handle(BIO* connection){
    int len;
    char recvBuf[BUFFER_SIZE];

    printf("connection established\n");

    while(true){
        memset(recvBuf, 0, BUFFER_SIZE);
        len = BIO_read(connection, recvBuf, BUFFER_SIZE);
        if(len <= 0){
            break;
        }
        printf("%s", recvBuf);
    }
    
    BIO_free(connection);
}