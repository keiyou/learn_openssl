#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>



void handleErrors(void){
    abort();
}

int encrypt(EVP_CIPHER_CTX *ctx, unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext){
    int len;
    int ciphertext_len;

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


int decrypt(EVP_CIPHER_CTX *ctx, unsigned char *ciphertext, int ciphertext_len, unsigned char *plaintext){
    int len;
    int plaintext_len;

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

int main(){
    EVP_CIPHER_CTX *ectx, *dctx;

    unsigned char key[EVP_MAX_KEY_LENGTH];
    unsigned char iv[EVP_MAX_IV_LENGTH];
    memset((void *)key, 'k', EVP_MAX_KEY_LENGTH);
    memset((void *)iv, 'i', EVP_MAX_IV_LENGTH);


    /* Message to be encrypted */
    unsigned char plaintext[1024], ciphertext[1024], decryptedtext[1024];
    int decryptedtext_len, ciphertext_len;

    while(true){
        if(!(ectx = EVP_CIPHER_CTX_new()))
            handleErrors();
         if(!(dctx = EVP_CIPHER_CTX_new()))
            handleErrors();
        if(1 != EVP_EncryptInit_ex(ectx, EVP_aes_256_cbc(), NULL, key, iv))
            handleErrors();
        if(1 != EVP_DecryptInit_ex(dctx, EVP_aes_256_cbc(), NULL, key, iv))
            handleErrors();
        memset(plaintext, 0, 1024);
        memset(ciphertext, 0, 1024);
        memset(decryptedtext, 0, 1024);
        fgets((char *)plaintext, 1024, stdin);
        /* Encrypt the plaintext */
        ciphertext_len = encrypt(ectx, plaintext, strlen ((char *)plaintext), ciphertext);

        /* Do something useful with the ciphertext here */
        printf("Ciphertext is:\n");
        BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);

        /* Decrypt the ciphertext */
        decryptedtext_len = decrypt(dctx,ciphertext, ciphertext_len, decryptedtext);

        /* Add a NULL terminator. We are expecting printable text */
        decryptedtext[decryptedtext_len] = '\0';

        /* Show the decrypted text */
        printf("Decrypted text is:\n");
        printf("%s", decryptedtext);
    }
}