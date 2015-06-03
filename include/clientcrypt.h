#ifndef CLIENTCRYPT
#define CLIENTCRYPT
    void encrypt(unsigned int k[], unsigned int tmp[]);
    unsigned int encrypt_Handler(char* msg);
    void decrypt(unsigned int k[], unsigned int tmp[]);
    void decrypt_Handler(char* msg, int size);

#endif // CRYPT
