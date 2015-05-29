//
//Written  Manjinder Singh
//
// "Distributed systems concepts and design fifth edition"
// Written by George Coulouris, Jean Dollimore, Tim Kindberg och Gordon Blair
// Published: May, 2011
// the encryption is designed by: David Wheeler and Roger Needham
//


#include <string.h>
#include <stdlib.h>

void encrypt_cli(unsigned long k[], unsigned long tmp[])       //<----This function is copied from the referensed book
{
    unsigned long y = tmp[0],z = tmp[1];
    unsigned long delta = 0x9e3779b9, sum = 0;
    int n;
    for (n=0;n<32;n++)
    {
        sum += delta;
        y += ((z<<4)+k[0])^(z+sum)^((z>>5)+k[1]);               // this is the encryption algoritm.
        z += ((y<<4)+k[2])^(y+sum)^((y>>5)+k[3]);
    }
    tmp[0]=y; tmp[1]=z;
}

unsigned long int encrypt_Handler(char* msg)                                //<------ this function is ispiried by the referensed book
{
    unsigned long k[4];
    k[0]=11111111111111;
    k[1]=22222222222222;    //This is a hardcoded key
    k[2]=33333333333333;
    k[3]=44444444444444;
    char tmp[8];
    unsigned long int numBlock=0,totalBlock=strlen(msg);  //totalblock is the size of the string
    unsigned long int msgLenght=strlen(msg)+1,pad=msgLenght%8; //we need to pad so the string is devided by 8 without rest
    msgLenght=msgLenght+8-pad;
    realloc(msg, msgLenght); // need to allocate more space to this string.
    unsigned long int i;
    for (i=totalBlock;i<msgLenght;i++)
    {
        msg[i]=' ';     // padding with spaces

    }
    totalBlock=msgLenght/8;
    while(numBlock<totalBlock)
    {
        for (i=0;i<8;i++)
        {
            tmp[i]=msg[i+(numBlock*8)];     // move every block to temporary string and encrypt, then move it back to the message string.
        }
        encrypt_cli(k,(unsigned long *)tmp);
        for (i=0;i<8;i++)
        {
            msg[i+(numBlock*8)]=tmp[i];
        }
        numBlock++;
    }
    return totalBlock*8;
}

void decrypt(unsigned long k[], unsigned long tmp[])           //<--- This function is also copied from the referensed book
{
    unsigned long y = tmp[0],z = tmp[1];
    unsigned long delta = 0x9e3779b9, sum = delta << 5;
    int n;
    for (n=0;n<32;n++)
    {
        z -= ((y<<4)+k[2])^(y+sum)^((y>>5)+k[3]);                // this is the decryption algoritm.
        y -= ((z<<4)+k[0])^(z+sum)^((z>>5)+k[1]);
        sum -= delta;
    }
    tmp[0]=y;tmp[1]=z;
}

void decrypt_Handler(char* msg, int size)        // <--- as encrypt_Handler, is this fumction inspired by the referensed book.
{
    char tmp[8];
    unsigned long k[4];
    k[0]=11111111111111;     // same key as in encryption_Handler.
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
            tmp[i]=msg[i+(numBlock*8)];             // move every block to temporary string and decrypt, then move it back to the message string
        }
        decrypt (k,(unsigned long *)tmp);
        for (i=0;i<8;i++)
        {
            msg[i+(numBlock*8)]=tmp[i];
        }
        numBlock++;
    }
}
