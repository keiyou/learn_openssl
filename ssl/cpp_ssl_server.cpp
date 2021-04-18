#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/ssl.h> 
#include <openssl/err.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_DATA_SIZE 1024
#define LISTEN_QUEUE_SIZE 16
#define LISTEN_PORT 8088
#define CERTIFICATE_FILE "test.pem"
#define PRIVATEKEY_FILE "test.key"


int main(){
    int listenfd, connfd;
    struct sockaddr_in servaddr, client;
    struct Attribute *att;
    socklen_t len;
    SSL_CTX *ctx;

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    ctx = SSL_CTX_new(SSLv23_server_method());
    if(ctx == NULL){
        ERR_print_errors_fp(stdout);
        exit(1);
    }

    if(SSL_CTX_use_certificate_file(ctx, CERTIFICATE_FILE, SSL_FILETYPE_PEM) <= 0){
        ERR_print_errors_fp(stdout);
        exit(1);
    }

    if(SSL_CTX_use_PrivateKey_file(ctx, PRIVATEKEY_FILE, SSL_FILETYPE_PEM) <= 0){
        ERR_print_errors_fp(stdout);
        exit(1);
    }

    if(!SSL_CTX_check_private_key(ctx)){
        ERR_print_errors_fp(stdout);
        exit(1);
    }


    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0){
        printf("failed to create socket");
        exit(1);
    }
    
    // init servaddr
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(LISTEN_PORT);
    
    //bind socket to addr
    int bind_ok = bind(listenfd,(struct sockaddr *) &servaddr, sizeof(servaddr));
    if(bind_ok < 0){
        perror("failed to bind socket with server address");
        exit(1);
    }

    int listen_ok = listen(listenfd, LISTEN_QUEUE_SIZE);
    if(listen_ok < 0){
        printf("failed to listen" );
        exit(1);
    }

    while(true){
        char recvBuf[MAX_DATA_SIZE];
        int num;

        connfd = accept(listenfd, NULL, NULL);
        if(connfd < 0){
            perror("failed to accept connection");
            continue;
        }

        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, connfd);

        if(SSL_accept(ssl) <= 0){
            perror("failed to use ssl");
            exit(1);
        }

        while(true){
            memset(recvBuf, 0, MAX_DATA_SIZE);
            num = SSL_read(ssl, recvBuf, MAX_DATA_SIZE);
            if(num <= 0){
                break;
            }
            printf("received message from client: %s",  recvBuf);
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(connfd);
    }

    close(listenfd);
    return 0;
}
