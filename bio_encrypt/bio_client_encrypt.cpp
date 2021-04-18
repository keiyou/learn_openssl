#include <openssl/bio.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define SERVER_ADDR "localhost:8088"
#define EVP_ENCRYPT_MODE 1
#define KEY "297B9DF04F9AF6B7438A541DE6F3F23751EFC3EBD8DCDE8F46B5062DE389E760"
#define IV "0F2B798406508FB10352B8256222C51B"

int main(){
    BIO *cbio, *cipherBio, *b;
    int len;
    char plaintext[BUFFER_SIZE], ciphertext[BUFFER_SIZE], cipherbuf[BUFFER_SIZE];

    unsigned char key[EVP_MAX_KEY_LENGTH] = "";
    unsigned char iv[EVP_MAX_IV_LENGTH] = "";
    memcpy((void *)key, KEY, EVP_MAX_KEY_LENGTH);
    memcpy((void *)iv, IV, EVP_MAX_IV_LENGTH);
    // memset((void *)key, 'k', EVP_MAX_KEY_LENGTH);
    // memset((void *)iv, 'i', EVP_MAX_IV_LENGTH);

    printf("%s\n%s\n", key, iv);

    cbio = BIO_new_connect(SERVER_ADDR);
    if(BIO_do_connect(cbio) <= 0){
        fprintf(stderr, "Error connecting to server\n");
        exit(1);
    }
    // cbio = BIO_push(cipherBio, cbio);

    while(true){
        memset(plaintext, 0, BUFFER_SIZE);
        memset(ciphertext, 0, BUFFER_SIZE);
        memset(cipherbuf, 0, BUFFER_SIZE);

        cipherBio = BIO_new(BIO_f_cipher());
        if(BIO_set_cipher(cipherBio, EVP_aes_256_cbc(), key, iv, EVP_ENCRYPT_MODE) <= 0){
            fprintf(stderr, "Error setting the aes 256 cbc cipher\n");
            exit(1);
        }
        b = BIO_new_mem_buf(cipherbuf, -1);
        cipherBio = BIO_push(cipherBio, b);
        
        fgets(plaintext, BUFFER_SIZE, stdin);
        len = BIO_write(cipherBio, plaintext, strlen(plaintext)-1);
        printf("length of plaintext:%d\n", len);
        BIO_flush(cipherBio);
        len = BIO_read(cipherBio, ciphertext, BUFFER_SIZE);
        printf("length of ciphertext:%d\n", len);
        printf("ciphertext: %s[endofcipertext]\n", ciphertext);

        len = BIO_write(cbio, ciphertext, BUFFER_SIZE);
        printf("===============================\n");
    }

    BIO_free(cbio);
    return 0;
}