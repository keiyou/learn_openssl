#include <openssl/bio.h>
#include <stdio.h>

int main(){
    BIO *cbio;;
    int len;
    char buf[1024];

    cbio = BIO_new_connect("localhost:8088");

    if(BIO_do_connect(cbio) <= 0){
        fprintf(stderr, "Error connecting to server\n");
        return 1;
    }

    while(true){
        fgets(buf, 1024, stdin);
        BIO_puts(cbio, buf);
    }

    BIO_free(cbio);
    return 0;
}