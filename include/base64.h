/* ------------------------------------------------------------------------ *
 * file:        base64_stringencode.c v1.0                                  *
 * purpose:     tests encoding/decoding strings with base64                 *
 * author:      02/23/2009 Frank4DD                                         *
 *                                                                          *
 * source:      http://base64.sourceforge.net/b64.c for encoding            *
 *              http://en.literateprograms.org/Base64_(C) for decoding      *
 * ------------------------------------------------------------------------ */
 /*modified by David Boeryd*/
 
#ifndef BASE64
#define BASE46

void decodeblock(unsigned char in[], char *clrstr, int index);
void b64_decode(const char *b64src, char *clrdst, int nrbytes);
void encodeblock( unsigned char in[], char b64str[], int len );
void b64_encode(char *clrstr, char *b64dst, int numbytes);
#endif