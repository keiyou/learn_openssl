#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <openssl/evp.h>

#define LISTEN_PORT 8088
#define LISTEN_QUEUE_SIZE 16
#define MAX_DATA_SIZE 1024

struct Attribute {
    int connfd;
    struct sockaddr_in client;
};

void process_handle(int connfd, struct sockaddr_in client);
void* handle(void* arg);
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext);


int main(){
    int listenfd, connfd;
    struct sockaddr_in servaddr, client;
    struct Attribute *att;
    socklen_t len;
    pthread_t tid;


    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0){
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
        exit(1);
    }

    int listen_ok = listen(listenfd, LISTEN_QUEUE_SIZE);
    if(listen_ok < 0){
        exit(1);
    }

    while(true){
        connfd = accept(listenfd, NULL, NULL);
        if(connfd < 0){
            continue;
        }

        att = (struct Attribute *)malloc(sizeof(struct Attribute));
        att->connfd = connfd;
        memcpy((void*)&att->client, &client, sizeof(client));

        int thread_ok = pthread_create(&tid, NULL, handle, (void*)att);
        if(thread_ok < 0) {
            continue;
        }
    }

    close(listenfd);
    return 0;
}

void* handle(void *arg){
    struct Attribute *att = (struct Attribute *)arg;
    process_handle(att->connfd, att->client);
    free(arg);
    pthread_exit(NULL);
}

void process_handle(int connfd, struct sockaddr_in client){
    int ciphertext_len, decryptedtext_len;
    char plaintext[MAX_DATA_SIZE], ciphertext[MAX_DATA_SIZE];

    unsigned char key[EVP_MAX_KEY_LENGTH];
    unsigned char iv[EVP_MAX_IV_LENGTH];
    memset((void *)key, 'k', EVP_MAX_KEY_LENGTH);
    memset((void *)iv, 'i', EVP_MAX_IV_LENGTH);

    printf("connection established with %s\n", inet_ntoa(client.sin_addr));
    
    while(true){
        memset(ciphertext, 0, MAX_DATA_SIZE);
        ciphertext_len = recv(connfd, ciphertext, MAX_DATA_SIZE, 0);
        if(ciphertext_len <= 0){
            break;
        }
        printf("ciphertext: %s\n",  ciphertext);
        decryptedtext_len = decrypt((unsigned char *)ciphertext, ciphertext_len, key, iv, (unsigned char *)plaintext);

        printf("received message from client: %s\n",  plaintext);
    }

    printf("connection with %s closed\n", inet_ntoa(client.sin_addr));
    
    close(connfd);
}

void handleErrors(void){
    abort();
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext){
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}