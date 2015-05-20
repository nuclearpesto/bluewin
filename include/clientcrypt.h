#ifndef CLIENTCRYPT
#define CLIENTCRYPT

void encrypt(unsigned long k[], unsigned long tmp[]);
void encrypt_Handler(char* msg);
void decrypt(unsigned long k[], unsigned long tmp[]);
void decrypt_Handler(char* msg);
#endif // CRYPT
