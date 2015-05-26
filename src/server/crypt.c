#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_thread.h>
#include <stdbool.h>
#include <jansson.h>
#include "misc.h"
#include "clienthandler.h"
#include "crypt.h"



void encrypt(unsigned long k[], unsigned long tmp[])   // Den här funktionen är direkt kopierad från boken,
{                                                      // distributed systems concepts and design fifth edition,
    unsigned long y = tmp[0],z = tmp[1];               // författare: George Coulouris, Jean Dollimore, Tim Kindberg och Gordon Blair
    unsigned long delta = 0x9e3779b9, sum = 0;         // Publicerad: Maj, 2011
    int n;                                             // // Krypteringen är designad av: David Wheeler och Roger Needham
    for (n=0;n<32;n++)
    {
        sum += delta;
        y += ((z<<4)+k[0])^(z+sum)^((z>>5)+k[1]);
        z += ((y<<4)+k[2])^(y+sum)^((y>>5)+k[3]);
    }
    tmp[0]=y; tmp[1]=z;
}

void encrypt_Handler(SerializedMessage_t * Message)    // den här funktionen är ommodifierad men har tagit inspiration från boken,
{                                                      // distributed systems concepts and design fifth edition,
    unsigned long k[4];
    k[0]=11111111111111;
    k[1]=22222222222222;
    k[2]=33333333333333;
    k[3]=44444444444444;
    char tmp[8];
    int numBlock=0,totalBlock=strlen(Message->jsonstring)+1;
    int pad=8-(totalBlock%8);
    int msgLenght=totalBlock+pad;
    Message->size=msgLenght;
    realloc(Message->jsonstring,msgLenght);
    int i;
    for (i=totalBlock;i<msgLenght;i++)
    {
        Message->jsonstring[i]=' ';
    }
    totalBlock=msgLenght/8;
    while(numBlock<totalBlock)
    {
        for (i=0;i<8;i++)
        {
            tmp[i]=Message->jsonstring[i+(numBlock*8)];
        }
        encrypt(k,(unsigned long *)tmp);
        for (i=0;i<8;i++)
        {
            Message->jsonstring[i+(numBlock*8)]=tmp[i];
        }
        numBlock++;
    }
}

void decrypt(unsigned long k[], unsigned long tmp[])                      // den här funktionen är också tagen från samma bok som funktionen encrypt.
{
    unsigned long y = tmp[0],z = tmp[1];
    unsigned long delta = 0x9e3779b9, sum = delta << 5;
    int n;
    for (n=0;n<32;n++)
    {
        z -= ((y<<4)+k[2])^(y+sum)^((y>>5)+k[3]);
        y -= ((z<<4)+k[0])^(z+sum)^((z>>5)+k[1]);
        sum -= delta;
    }
    tmp[0]=y;tmp[1]=z;
}

void decrypt_Handler(char* msg, int size)               //  likt encrypt_Handler är denna fukntion inspirerad från boken,
{                                                       // distributed systems concepts and design fifth edition.
    printf("decrypting \n");
    char tmp[8];
    unsigned long k[4];
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
            tmp[i]=msg[i+(numBlock*8)];
        }
        decrypt (k,(unsigned long *)tmp);
        for (i=0;i<8;i++)
        {
            msg[i+(numBlock*8)]=tmp[i];
        }
        numBlock++;
    }
    printf("decrypted string: %s \n",msg);
}
