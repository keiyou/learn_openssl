#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/evp.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8088
#define LISTEN_QUEUE_SIZE 16
#define MAX_DATA_SIZE 1024


int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext);

int main(){
    int requestfd;
    struct sockaddr_in servaddr;
    struct Attribute *att;
    char plaintext[MAX_DATA_SIZE], ciphertext[MAX_DATA_SIZE];
    int ciphertext_len;

    unsigned char key[EVP_MAX_KEY_LENGTH];
    unsigned char iv[EVP_MAX_IV_LENGTH];
    memset((void *)key, 'k', EVP_MAX_KEY_LENGTH);
    memset((void *)iv, 'i', EVP_MAX_IV_LENGTH);

    
    requestfd = socket(AF_INET, SOCK_STREAM, 0);
    if(requestfd < 0){
        perror("failed to create socket");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);

    if(inet_aton(SERVER_IP, &servaddr.sin_addr) < 0){
        perror("failed to translate IP");
        exit(1);
    }

    if(connect(requestfd, (sockaddr *)&servaddr, sizeof(struct sockaddr_in)) < 0){
        perror("failed to connect to server");
        exit(1);
    }
    printf("connection established\n");

    while(true){
        memset(plaintext, 0, MAX_DATA_SIZE);
        memset(ciphertext, 0, MAX_DATA_SIZE);
        fgets((char *)plaintext, MAX_DATA_SIZE, stdin);
        if(strcmp(plaintext, "exit\n") == 0){
            break;
        }

        ciphertext_len = encrypt((unsigned char *) plaintext, strlen((char *)plaintext), key, iv, (unsigned char *)ciphertext);
        printf("length:%d, ciphertext:%s\n", ciphertext_len, ciphertext);

        if(send(requestfd, ciphertext, ciphertext_len, 0) < 0){
            perror("failed to send message");
            exit(1);
        }
    }

    close(requestfd);
    printf("connection closed\n");

    return 0;
}

void handleErrors(void){
    abort();
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext){
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}
