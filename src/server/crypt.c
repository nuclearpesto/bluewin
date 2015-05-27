#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_thread.h>
#include <stdbool.h>
#include <jansson.h>
#include "misc.h"
#include "clienthandler.h"
#include "crypt.h"                                        // code written by Manjinder Singh



void encrypt(unsigned long k[], unsigned long tmp[])   // <----This function is copied from a book called,
{                                                      // distributed systems concepts and design fifth edition.
    unsigned long y = tmp[0],z = tmp[1];               // Written by George Coulouris, Jean Dollimore, Tim Kindberg och Gordon Blair
    unsigned long delta = 0x9e3779b9, sum = 0;         // Published: May, 2011
    int n;                                             // the encryption is designed by: David Wheeler and Roger Needham
    for (n=0;n<32;n++)
    {
        sum += delta;
        y += ((z<<4)+k[0])^(z+sum)^((z>>5)+k[1]);    // this is the encryption algoritm.
        z += ((y<<4)+k[2])^(y+sum)^((y>>5)+k[3]);
    }
    tmp[0]=y; tmp[1]=z;
}

void encrypt_Handler(SerializedMessage_t * Message)    // <------ this function is ispiried by the book that i mention in the function above.
{
    unsigned long k[4];        //
    k[0]=11111111111111;       //This is a hardcoded key
    k[1]=22222222222222;       //
    k[2]=33333333333333;
    k[3]=44444444444444;
    char tmp[8];
    int numBlock=0,totalBlock=strlen(Message->jsonstring)+1; //totalblock is the size of the string
    int pad=8-(totalBlock%8);  //we need to pad so the string is devided by 8 without rest
    int msgLenght=totalBlock+pad;
    Message->size=msgLenght;  // need to allocate more space to this string.
    realloc(Message->jsonstring,msgLenght);
    int i;
    for (i=totalBlock;i<msgLenght;i++)
    {
        Message->jsonstring[i]=' ';       // padding with spaces
    }
    totalBlock=msgLenght/8;
    while(numBlock<totalBlock)
    {
        for (i=0;i<8;i++)
        {
            tmp[i]=Message->jsonstring[i+(numBlock*8)];      // move every block to temporary string and encrypt, then move it back to the message string.
        }
        encrypt(k,(unsigned long *)tmp);
        for (i=0;i<8;i++)
        {
            Message->jsonstring[i+(numBlock*8)]=tmp[i];
        }
        numBlock++;
    }
}

void decrypt(unsigned long k[], unsigned long tmp[])                      // <--- This function is also copied from the same book as the function encrypt.
{
    unsigned long y = tmp[0],z = tmp[1];
    unsigned long delta = 0x9e3779b9, sum = delta << 5;
    int n;
    for (n=0;n<32;n++)
    {
        z -= ((y<<4)+k[2])^(y+sum)^((y>>5)+k[3]);           // this is the decryption algoritm.
        y -= ((z<<4)+k[0])^(z+sum)^((z>>5)+k[1]);
        sum -= delta;
    }
    tmp[0]=y;tmp[1]=z;
}

void decrypt_Handler(char* msg, int size)               //  as encrypt_Handler, is this fumction inspired by the book that i mention earlier.
{
    char tmp[8];
    unsigned long k[4];    // same key as in encryption_Handler.
    k[0]=11111111111111;
    k[1]=22222222222222;
    k[2]=33333333333333;
    k[3]=44444444444444;
    int numBlock=0,totalBlock=size;
    totalBlock=totalBlock/8;
    while(numBlock<totalBlock)
    {
        int i;
        for (i=0;i<8;i++)
        {
            tmp[i]=msg[i+(numBlock*8)];       // move every block to temporary string and decrypt, then move it back to the message string
        }
        decrypt (k,(unsigned long *)tmp);
        for (i=0;i<8;i++)
        {
            msg[i+(numBlock*8)]=tmp[i];
        }
        numBlock++;
    }
}
