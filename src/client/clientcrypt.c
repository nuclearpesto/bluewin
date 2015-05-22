#include <string.h>
#include <stdlib.h>

void encrypt_cli(unsigned long k[], unsigned long tmp[])
{
    unsigned long y = tmp[0],z = tmp[1];
    unsigned long delta = 0x9e3779b9, sum = 0;
    int n;
    for (n=0;n<32;n++)
    {
        sum += delta;
        y += ((z<<4)+k[0])^(z+sum)^((z>>5)+k[1]);
        z += ((y<<4)+k[2])^(y+sum)^((y>>5)+k[3]);
    }
    tmp[0]=y; tmp[1]=z;
}

int encrypt_Handler(char* msg)
{
    unsigned long k[4];
    k[0]=11111111111111;
    k[1]=22222222222222;
    k[2]=33333333333333;
    k[3]=44444444444444;
    char tmp[8];
    int numBlock=0,totalBlock=strlen(msg);
    int msgLenght=strlen(msg)+1,pad=msgLenght%8;
    msgLenght=msgLenght+8-pad;
    realloc(msg, msgLenght);
    int i;
    for (i=totalBlock;i<msgLenght;i++)
    {
        msg[i]=' ';

    }
    totalBlock=msgLenght/8;
    //totalBlock=totalBlock/8;
   // int i;
    while(numBlock<totalBlock)
    {
        for (i=0;i<8;i++)
        {
            tmp[i]=msg[i+(numBlock*8)];
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

void decrypt(unsigned long k[], unsigned long tmp[])
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

void decrypt_Handler(char* msg, int size)
{
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
}
