#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <stdio.h>
#include <string.h>

int main(){
    BIO *cipherBio1, *cipherBio2, *b, *bd, *m;
    int len;
    char tmp[1024], a[1024];
    static const unsigned char KEY[] = {
        0x51, 0x50, 0xd1, 0x77, 0x2f, 0x50, 0x83, 0x4a,
        0x50, 0x3e, 0x06, 0x9a, 0x97, 0x3f, 0xbd, 0x7c,
        0xe6, 0x1c, 0x43, 0x2b, 0x72, 0x0b, 0x19, 0xd1,
        0x8e, 0xc8, 0xd8, 0x4b, 0xdc, 0x63, 0x15, 0x1b
    };

    static const unsigned char IV[] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };


    cipherBio1 = BIO_new(BIO_f_cipher());
    cipherBio2 = BIO_new(BIO_f_cipher());
    if(BIO_set_cipher(cipherBio1, EVP_aes_256_ecb(), KEY, IV, 1) <= 0){
        printf("1");
        exit(1);
    }
   if(BIO_set_cipher(cipherBio2, EVP_aes_256_ecb(), KEY, IV, 0) <= 0){
       printf("2");
       exit(1);
   }

   b = BIO_new(BIO_s_null());
   b = BIO_push(cipherBio1, b);
   len = BIO_write(b, "openssl", 7);
   printf("length1:%d\n", len);
   len = BIO_read(b, tmp, 1024);
   printf("length2:%d\n", len);
   printf("cipher status:%ld\n", BIO_get_cipher_status(b));
   printf("%s\n", tmp);

   BIO_free(b);

    
   bd = BIO_new(BIO_s_null());
   bd = BIO_push(cipherBio2, bd);
//    m = BIO_new(BIO_s_null());
//    bd = BIO_push(m, bd);
   len = BIO_write(bd, tmp, len);
   printf("length3:%d\n", len);
   memset(tmp, 0, 1024);
//    BIO_flush(bd);
   len = BIO_read(bd, tmp, 1024);
   printf("length4:%d\n", len);
   printf("cipher status:%ld\n", BIO_get_cipher_status(bd));
   printf("%s\n", tmp);
   BIO_free(bd);

    return 0;
}