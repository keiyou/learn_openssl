#include <stdio.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <string.h>


#define BUFFER_SIZE 1024
#define SERVER_ADDR "localhost:8088"

int main(){
    BIO *acceptBio, *cipherBio, *connection, *b;

    unsigned char userkey[EVP_MAX_KEY_LENGTH];
    unsigned char iv[EVP_MAX_IV_LENGTH];
    memset((void*)userkey, 'k', EVP_MAX_KEY_LENGTH);
    memset((void*)iv, 'i', EVP_MAX_IV_LENGTH);

    acceptBio = BIO_new_accept(SERVER_ADDR);
    if(BIO_do_accept(acceptBio) <= 0){
        printf("failed to accept new connection");
        exit(1);
    }
   
    cipherBio = BIO_new(BIO_f_cipher());
    BIO_set_cipher(cipherBio, EVP_aes_256_ecb(), userkey, iv, 0);
    BIO_set_accept_bios(acceptBio, cipherBio);
    // acceptBio = BIO_push(cipherBio, acceptBio);



    while(true){
        if(BIO_do_accept(acceptBio) <= 0){
            printf("failed to accept new connection");
            exit(1);
        }

        connection = BIO_pop(acceptBio);
        int len;
        char recvBuf[BUFFER_SIZE];

        printf("cipher status:%ld\n", BIO_get_cipher_status(connection)); 
        printf("connection established\n");

        while(true){
            memset(recvBuf, 0, BUFFER_SIZE);
            len = BIO_read(connection, recvBuf, BUFFER_SIZE);
            if(len <= 0){
                printf("???\n");
                break;
            }
            printf("length: %d\n", len);
            printf("decrypted: %s\n", recvBuf);

            // len = BIO_write(cipherBio, recvBuf, len);
            // memset(recvBuf, 0, BUFFER_SIZE);
            // len = BIO_read(cipherBio, recvBuf, BUFFER_SIZE);
            // printf("cipher status:%ld\n", BIO_get_cipher_status(cipherBio));
            // printf("decrypted: %s\n", recvBuf);
        }
        BIO_free(connection);
    }

    BIO_free(acceptBio);
    return 0;
}