#include <stdio.h>
#include <openssl/bio.h>
#include <openssl/ssl.h> 
#include <openssl/err.h> 
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define SERVER_ADDR "localhost:8088"
#define CERTIFICATE_FILE "test.pem"
#define PRIVATEKEY_FILE "test.key"

int main(){
    BIO *acceptBio, *sslBio, *bufferBio;
    SSL_CTX *ctx;
    SSL *ssl;
    pthread_t tid;

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

    sslBio = BIO_new_ssl(ctx, 0);
    BIO_get_ssl(sslBio, &ssl);
    if(ssl == NULL){
        fprintf(stderr, "Cannot locate SSL pointer\n");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    bufferBio = BIO_new(BIO_f_buffer());
    sslBio = BIO_push(bufferBio, sslBio);
    acceptBio = BIO_new_accept(SERVER_ADDR);

    BIO_set_accept_bios(acceptBio, sslBio);
    if(BIO_do_accept(acceptBio) <= 0){
        fprintf(stderr, "Error setting up accept BIO\n");
        ERR_print_errors_fp(stderr);
        exit(1);
    }


    while(true){
        int len;
        char recvBuf[BUFFER_SIZE];

        if(BIO_do_accept(acceptBio) <= 0){
            fprintf(stderr, "Error setting up accept BIO\n");
            ERR_print_errors_fp(stderr);
            exit(1);
        }

        sslBio = BIO_pop(acceptBio);

        printf("connection established\n");

        while(true){
            memset(recvBuf, 0, BUFFER_SIZE);
            len = BIO_read(sslBio, recvBuf, BUFFER_SIZE);
            if(len <= 0){
                break;
            }
            printf("%s", recvBuf);
        }
        
        BIO_free(sslBio);
    }

    BIO_free(acceptBio);
    return 0;
}

