#ifndef CLIENTCRYPT
#define CLIENTCRYPT
    void encrypt(unsigned long k[], unsigned long tmp[]);
    int encrypt_Handler(char* msg);
    void decrypt(unsigned long k[], unsigned long tmp[]);
    void decrypt_Handler(char* msg, int size);

#endif // CRYPT
