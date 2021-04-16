#include <stdio.h>
#include <openssl/bio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024
#define SERVER_ADDR "localhost:8888"




int main(){
    BIO *sbio = NULL;
    char *addr = NULL;

    sbio = BIO_new_accept(SERVER_ADDR);
    int acceptOk = BIO_do_accept(sbio);
    if(acceptOk <= 0){
        
    }

    while(true){
        memset(out, 0, OUT_SIZE);
        len = BIO_read(sbio, out, 80);
        if(out[0] == 'q'){
            break;
        }
        printf("%s", out);
    }

    BIO_free(sbio);
    return 0;
}