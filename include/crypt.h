#ifndef CRYPT
#define CRYPT

void encrypt(unsigned long k[], unsigned long tmp[]);
void encrypt_Handler(SerializedMessage_t * sermes);
void decrypt(unsigned long k[], unsigned long tmp[]);
void decrypt_Handler(char msg[]);
#endif // CRYPT
