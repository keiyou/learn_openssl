#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <stdio.h>
#include <string.h>

int main(){
    BIO *cipherBio1, *cipherBio2, *b, *bd;
    int len;
    char tmp[1024], a[1024];
    char ciphertext[1024], plaintext[1024];
    unsigned char key[EVP_MAX_KEY_LENGTH];
    unsigned char iv[EVP_MAX_IV_LENGTH];
    memset((void *)key, 'k', EVP_MAX_KEY_LENGTH);
    memset((void *)iv, 'i', EVP_MAX_IV_LENGTH);

    cipherBio1 = BIO_new(BIO_f_cipher());
    cipherBio2 = BIO_new(BIO_f_cipher());
    if(BIO_set_cipher(cipherBio1, EVP_aes_256_cbc(), key, iv, 1) <= 0){
        printf("1");
        exit(1);
    }
    b = BIO_new_mem_buf(plaintext, -1);
    b = BIO_push(cipherBio1, b);

    if(BIO_set_cipher(cipherBio2, EVP_aes_256_cbc(), key, iv, 0) <= 0){
        printf("2");
        exit(1);
    }
    bd = BIO_new_mem_buf(ciphertext, -1);
    bd = BIO_push(cipherBio2, bd);

    while(true){
        char asd[1024];
        memset(asd, 0, 1024);
        memset(tmp, 0, 1024);
        BIO_reset(b);
        BIO_reset(bd);
        BIO_reset(cipherBio1);
        BIO_reset(cipherBio2);

        fgets(asd, 1024, stdin);

        len = BIO_write(b, asd, strlen(asd)-1);
        printf("length1:%d\n", len);
        len = BIO_read(b, tmp, 1024);
        printf("length2:%d\n", len);
        printf("%s\n", tmp);

        len = BIO_write(bd, tmp, len);
        printf("length3:%d\n", len);
        memset(tmp, 0, 1024);
        //    BIO_flush(bd);
        len = BIO_read(bd, tmp, 1024);
        printf("length4:%d\n", len);
        tmp[len] = '\0';
        printf("cipher status:%ld\n", BIO_get_cipher_status(bd));
        printf("%s\n", tmp);

        printf("+++++++++++++++++++++++++++++++++++++++++++++++\n");
    }

    BIO_free(b);
    BIO_free(bd);

    return 0;
}