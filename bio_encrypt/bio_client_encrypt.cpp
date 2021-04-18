#include <openssl/bio.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>

int main(){
    BIO *cbio, *cipherBio;
    int len;

    unsigned char userkey[EVP_MAX_KEY_LENGTH];
    unsigned char iv[EVP_MAX_IV_LENGTH];
    memset((void*)userkey, 'k', EVP_MAX_KEY_LENGTH);
    memset((void*)iv, 'i', EVP_MAX_IV_LENGTH);

    cipherBio = BIO_new(BIO_f_cipher());
    BIO_set_cipher(cipherBio, EVP_aes_256_ecb(), userkey, iv, 1);

    cbio = BIO_new_connect("localhost:8088");
    if(BIO_do_connect(cbio) <= 0){
        fprintf(stderr, "Error connecting to server\n");
        return 1;
    }
    cbio = BIO_push(cipherBio, cbio);

    // while(true){
        char sendBuf[1024] = "opesnssl";
        memset(sendBuf, 0, 1024);
       
        // fgets(sendBuf, 1024, stdin); 
        len = BIO_write(cbio, "openssl\n", 7);
        // BIO_flush(cbio);
        printf("length:%d\n", len);
        printf("cipher status:%ld\n", BIO_get_cipher_status(cbio));
    // }

    BIO_free(cbio);
    return 0;
}