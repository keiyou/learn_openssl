#include <openssl/bio.h>
#include <openssl/bio.h>
#include <openssl/ssl.h> 
#include <openssl/err.h> 
#include <stdio.h>

#define MAX_DATA_SIZE 1024
#define LISTEN_QUEUE_SIZE 16
#define SERVER_ADDR "0.0.0.0:8088"
#define LISTEN_PORT 8088
#define CERTIFICATE_FILE "test.pem"
#define PRIVATEKEY_FILE "test.key"

int main(){
    BIO *sbio, *out;
    int len;
    char tmpbuf[1024];
    SSL_CTX *ctx;
    SSL *ssl;

    /* XXX Seed the PRNG if needed. */

    ctx = SSL_CTX_new(TLS_client_method());

    /* XXX Set verify paths and mode here. */

    sbio = BIO_new_ssl_connect(ctx);
    BIO_get_ssl(sbio, &ssl);
    if (ssl == NULL) {
        fprintf(stderr, "Can't locate SSL pointer\n");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    /* Don't want any retries */
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    /* XXX We might want to do other things with ssl here */

    /* An empty host part means the loopback address */
    BIO_set_conn_hostname(sbio, "localhost:8088");

    out = BIO_new_fp(stdout, BIO_NOCLOSE);
    if (BIO_do_connect(sbio) <= 0) {
        fprintf(stderr, "Error connecting to server\n");
        ERR_print_errors_fp(stderr);
        exit(1);
    }
    if (BIO_do_handshake(sbio) <= 0) {
        fprintf(stderr, "Error establishing SSL connection\n");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    /* XXX Could examine ssl here to get connection info */

    char buf[1024];
    while(true){
        fgets(buf, 1024, stdin);
        BIO_write(sbio, buf, MAX_DATA_SIZE);
    }
    BIO_free_all(sbio);
    BIO_free(out);
}