#ifndef CRYPT
#define CRYPT

void encrypt(unsigned int k[], unsigned int tmp[]);
void encrypt_Handler(SerializedMessage_t * sermes);
void decrypt(unsigned int k[], unsigned int tmp[]);
void decrypt_Handler(char msg[], int size);
#endif // CRYPT
