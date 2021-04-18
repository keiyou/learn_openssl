#include <stdio.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <string.h>


#define BUFFER_SIZE 1024
#define SERVER_ADDR "localhost:8088"
#define EVP_DECRYPT_MODE 0
#define KEY "297B9DF04F9AF6B7438A541DE6F3F23751EFC3EBD8DCDE8F46B5062DE389E760"
#define IV "0F2B798406508FB10352B8256222C51B"

int main(){
    BIO *acceptBio, *cipherBio, *connection, *b;
    int len;
    char plaintext[BUFFER_SIZE], ciphertext[BUFFER_SIZE], cipherbuf[BUFFER_SIZE];

    unsigned char key[EVP_MAX_KEY_LENGTH] = "";
    unsigned char iv[EVP_MAX_IV_LENGTH] = "";
    memcpy(key, KEY, EVP_MAX_KEY_LENGTH);
    memcpy(iv, IV, EVP_MAX_IV_LENGTH);
    // memset((void *)key, 'k', EVP_MAX_KEY_LENGTH);
    // memset((void *)iv, 'i', EVP_MAX_IV_LENGTH);

    printf("%s\n%s\n", key, iv);

    acceptBio = BIO_new_accept(SERVER_ADDR);
    if(BIO_do_accept(acceptBio) <= 0){
        printf("failed to accept new connection");
        exit(1);
    }
   
    // cipherBio = BIO_new(BIO_f_cipher());
    // BIO_set_cipher(cipherBio, EVP_aes_256_ecb(), userkey, iv, 0);
    // BIO_set_accept_bios(acceptBio, cipherBio);
    // acceptBio = BIO_push(cipherBio, acceptBio);



    while(true){
        if(BIO_do_accept(acceptBio) <= 0){
            printf("failed to accept new connection");
            exit(1);
        }

        connection = BIO_pop(acceptBio);

        printf("cipher status:%ld\n", BIO_get_cipher_status(connection)); 
        printf("connection established\n");

        while(true){
            memset(plaintext, 0, BUFFER_SIZE);
            memset(ciphertext, 0, BUFFER_SIZE);
            memset(cipherbuf, 0, BUFFER_SIZE);
            cipherBio = BIO_new(BIO_f_cipher());
            if(BIO_set_cipher(cipherBio, EVP_aes_256_cbc(), key, iv, EVP_DECRYPT_MODE) <= 0){
                fprintf(stderr, "Error setting the aes 256 cbc cipher\n");
                exit(1);
            }
            b = BIO_new_mem_buf(cipherbuf, -1);
            cipherBio = BIO_push(cipherBio, b);

            len = BIO_read(connection, ciphertext, BUFFER_SIZE);
            if(len <= 0){
                break;
            }
            printf("length of ciphertext: %ld\n", strlen(ciphertext));
            printf("ciphertext: %s[endofciphertext]\n", ciphertext);
            
            len = BIO_write(cipherBio, ciphertext, strlen(ciphertext));
            len = BIO_read(cipherBio, plaintext, BUFFER_SIZE);
            if(BIO_get_cipher_status(cipherBio) == 0){
                printf("cipher status:%ld\n", BIO_get_cipher_status(cipherBio));
                fprintf(stderr, "Error decrpting the ciphertext\n");
                exit(1);
            }
            plaintext[len] = '\0';
            printf("length of plaintext: %d\n", len);
            printf("plaintext: %s\n", plaintext);
            printf("===============================\n");
        }
        BIO_free(connection);
    }

    BIO_free(acceptBio);
    return 0;
}