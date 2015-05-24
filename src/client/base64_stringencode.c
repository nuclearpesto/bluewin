/* ------------------------------------------------------------------------ *
 * file:        base64_stringencode.c v1.0                                  *
 * purpose:     tests encoding/decoding strings with base64                 *
 * author:      02/23/2009 Frank4DD                                         *
 *                                                                          *
 * source:      http://base64.sourceforge.net/b64.c for encoding            *
 *              http://en.literateprograms.org/Base64_(C) for decoding      *
 * ------------------------------------------------------------------------ */
 /*modified by David Boeryd*/
 
#include <stdio.h>
#include <string.h>
#include "base64.h"
/* ---- Base64 Encoding/Decoding Table --- */
char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* decodeblock - decode 4 '6-bit' characters into 3 8-bit binary bytes */
void decodeblock(unsigned char in[], char *clrstr, int index) {
  unsigned char out[4];
  int i, j=4;
  out[0] = in[0] << 2 | in[1] >> 4;
  out[1] = in[1] << 4 | in[2] >> 2;
  out[2] = in[2] << 6 | in[3] >> 0;
  out[3] = '\0';
 /* below is modified to make the base64 decoding work wihtout depending on stringfunctions which incorrectly assumes 
	a byt with value 0 is an end byte
 */
	//printf("decoded characters %c%c%c%c\n", out[0],out[1],out[2],out[3]);
	  //like this the last 3 bytes will be corupted;
	int rounds = index/4 -1;
	for( i=0; i<3; i++){
		clrstr[index-j-rounds]=out[i];
		j--;
	}
  //strncat(clrstr, out, sizeof(out));
}
/*changed this to run for an exact number of bytes instead of stopping when finding a byte with value 0*/
void b64_decode(const char *b64src, char *clrdst, int nrbytes) {
  int c, phase, i;
  unsigned char in[4];
  char *p;

  clrdst[0] = '\0';
  phase = 0; i=0;
  while(i<nrbytes) {
    c = (int) b64src[i];
    if(c == '=') {
      decodeblock(in, clrdst, i); 
      break;
    }
    p = strchr(b64, c);
    if(p) {
      in[phase] = p - b64;
      phase = (phase + 1) % 4;
      if(phase == 0) {
		  //printf("decoding characters %c%c%c%c\n", in[0],in[1],in[2],in[3]);
        decodeblock(in, clrdst, i);
        in[0]=in[1]=in[2]=in[3]=0;
      }
    }
    i++;
  }
}

/* encodeblock - encode 3 8-bit binary bytes as 4 '6-bit' characters */
void encodeblock( unsigned char in[], char b64str[], int len ) {
    unsigned char out[5];
    out[0] = b64[ in[0] >> 2 ];
    out[1] = b64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? b64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? b64[ in[2] & 0x3f ] : '=');
    out[4] = '\0';
    strncat(b64str, reinterpret_cast<char *>(out), sizeof(out));
}

/* encode - base64 encode a stream, adding padding if needed */
/* changed this to run for exact number of bytes instead of ending at a byte with value 0 */

void b64_encode(char *clrstr, char *b64dst, int numbytes) {
  unsigned char in[3];
  int i, len = 0;
  int j = 0;
  b64dst[0] = '\0';
  while(j<numbytes) {
    len = 0;
    for(i=0; i<3; i++) {
     in[i] = (unsigned char) clrstr[j];
     if(j<numbytes) {
        len++; 
		//printf("j = %d\n", j);
		fflush(stdout);
	  }
      else{
		in[i] = 0;
		printf("adding zerobytes at j=%d\n", j);
		} 
	  j++;
    }
    if( len ) {
      encodeblock( in, b64dst, len );
    }
  }
  //printf("exiting encode at j=%d", j);
}
/*removed main function from example code*/