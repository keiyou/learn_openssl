# __learn_openssl__
## __Background__
USTC LoCE 学习任务，使用openssl构建简单的服务器/客户端通讯，并实现从普通通讯到加密通讯的变化。

该学习项目使用了socket和openssl/bio分别实现了服务器/客户端模型，并尝试使用AES 256 CBC对传输数据进行加密，此外还额外实现了SSL 服务器/客户端。
___
## __Structure__
### *socket*
使用c的socket实现的服务器及客户端
### *socket_encrypt*
在socket的基础上使用openssl/evp对发送数据进行aes-256-cbc加密
### *bio*
使用openssl/bio实现的服务器及客户端
### *bio_encrypt*
在openssl/bio的基础上使用cipher bio对发送数据进行aes-256-cbc加密
### *ssl*
使用openssl/bio实现的ssl服务器及客户端，以及使用socket实现的ssl服务器
___
## __Install__
___
## __Usage__
___
## __TODO__

### *代码*
* aes-256-cbc加密时，碰到某些字符组合时会使加密解密失效 
  >*目前猜测是因为加密成了某些特殊字符，例如'/0'导致加密过程中断，对此需要在加密前改变字符编码已规避特数字字符，例如code-64*。 因为原理知识的匮乏还需要进一步考察底层实现
* openssl/evp和openssl/bio所创建的加密解密对象，均只能使用一次，第二次使用会导致cipher_status变成0
    >*evp从官方example来看可能就是一次性使用对象，而cipher_bio从文档描述看来应当可以重复使用，还需进一步测试*

### *原理*
* 需要补充密码学及网络安全的知识，理解底层原理

## __Reference__

* <https://www.openssl.org/docs/man1.1.1/>
* <https://wiki.openssl.org/index.php/Main_Page>
* <https://stackoverflow.com/questions/51672133/what-are-openssl-bios-how-do-they-work-how-are-bios-used-in-openssl>
* <https://www.openssl.net.cn/> (mostly deprecated)