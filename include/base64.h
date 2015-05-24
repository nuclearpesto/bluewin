#ifndef BASE64
#define BASE46

void decodeblock(unsigned char in[], char *clrstr, int index);
void b64_decode(const char *b64src, char *clrdst, int nrbytes);
void encodeblock( unsigned char in[], char b64str[], int len );
void b64_encode(char *clrstr, char *b64dst, int numbytes);
#endif