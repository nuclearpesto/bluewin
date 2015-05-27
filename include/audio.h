
  /* $Id: patest_read_record.c 757 2004-02-13 07:48:10Z rossbencina $
  *
  * This program uses the PortAudio Portable Audio Library.
  * For more information see: http://www.portaudio.com
  * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
  *
  * Permission is hereby granted, free of charge, to any person obtaining
  * a copy of this software and associated documentation files
  * (the "Software"), to deal in the Software without restriction,
  * including without limitation the rights to use, copy, modify, merge,
  * publish, distribute, sublicense, and/or sell copies of the Software,
  * and to permit persons to whom the Software is furnished to do so,
  * subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be
  * included in all copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
  * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  */
 
 /*
  * The text above constitutes the entire PortAudio license; however, 
  * the PortAudio community also makes the following non-binding requests:
  *
  * Any person wishing to distribute modifications to the Software is
  * requested to send the modifications to the original developer so that
  * they can be incorporated into the canonical version. It is also 
  * requested that these non-binding requests be included along with the 
  * license above.
  */
  
  
  /*change by David Boeryd to allow for separate threads doing the reading and writing to the streams
	also serialize it as json and send it over the network
  */

/*modified by david boeryd */

 #ifndef AUDIO
 #define AUDIO
 /* #define SAMPLE_RATE  (17932) // Test failure to open with this value. */
 #define SAMPLE_RATE  (8000)
 #define FRAMES_PER_BUFFER (1024)
 #define NUM_CHANNELS    (2)
 #define NUM_SECONDS     (15)
 /* #define DITHER_FLAG     (paDitherOff)  */
 #define DITHER_FLAG     (0) 
 
 /* @todo Underflow and overflow is disabled until we fix priming of blocking write. */
 #define CHECK_OVERFLOW  (0)
 #define CHECK_UNDERFLOW  (0)
 
 
 /* Select sample format. */
 #if 0
 #define PA_SAMPLE_TYPE  paFloat32
 #define PA_SAMPLE_TYPE_PRINTABLE  "paFloat32"
 #define SAMPLE_SIZE (4)
 #define SAMPLE_SILENCE  (0.0f)
 #define CLEAR(a) memset( (a), 0, FRAMES_PER_BUFFER * NUM_CHANNELS * SAMPLE_SIZE )
 #define PRINTF_S_FORMAT "%.8f"
 #elif 1
 #define PA_SAMPLE_TYPE  paInt16
 #define PA_SAMPLE_TYPE_PRINTABLE  "paint 16"
 #define SAMPLE_SIZE (2)
 #define SAMPLE_SILENCE  (0)
 #define CLEAR(a) memset( (a), 0,  FRAMES_PER_BUFFER * NUM_CHANNELS * SAMPLE_SIZE )
 #define PRINTF_S_FORMAT "%d"
 #elif 0
 #define PA_SAMPLE_TYPE  paInt24
 #define PA_SAMPLE_TYPE_PRINTABLE  "paint 24"
 #define SAMPLE_SIZE (3)
 #define SAMPLE_SILENCE  (0)
 #define CLEAR(a) memset( (a), 0,  FRAMES_PER_BUFFER * NUM_CHANNELS * SAMPLE_SIZE )
 #define PRINTF_S_FORMAT "%d"
 #elif 0
 #define PA_SAMPLE_TYPE  paInt8
 #define PA_SAMPLE_TYPE_PRINTABLE  "paint8"
 #define SAMPLE_SIZE (1)
 #define SAMPLE_SILENCE  (0)
 #define CLEAR(a) memset( (a), 0,  FRAMES_PER_BUFFER * NUM_CHANNELS * SAMPLE_SIZE )
 #define PRINTF_S_FORMAT "%d"
 #else
 #define PA_SAMPLE_TYPE  paUInt8
 #define PA_SAMPLE_TYPE_PRINTABLE  "uint8"
 #define SAMPLE_SIZE (1)
 #define SAMPLE_SILENCE  (128)
 #define CLEAR( a ) { \
     int i; \
     for( i=0; i<FRAMES_PER_BUFFER*NUM_CHANNELS; i++ ) \
         ((unsigned char *)a)[i] = (SAMPLE_SILENCE); \
 }
 #define PRINTF_S_FORMAT "%d"
#endif


#include <portaudio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

struct audiostruct{
  
  PaStream *readstream, *writestream;
  char * writeBlock, *readBlock;
  
};typedef struct audiostruct audiostruct_t;

struct audiothreadstruct{

  audiostruct_t * audiostruct;
  TCPsocket socket; 
  bool *loggedInCheck;
  SDL_mutex * writeMutex;
  
};typedef struct audiothreadstruct audiothreadstruct_t;

void errorHappened(PaStream * stream,char *sampleBlock, PaError er);	//abort the stream and remove exit the program	  

void xrun(PaStream * stream,char *sampleBlock, PaError err);
void init_sound( audiostruct_t *audiostruct ); /*initializes two streams, one for input and one for output,start the streams and put refferences to those streams in the supplied audiostruct,
												allocates buffers for the two streams as well.	*/
int  readthread(void*data); /*continuosly reads audio from the microphone, converts the resulting byte array to a base 64 string and writes it to the server*/
int  playaudio(json_t *obj, char* sampleBlock, PaStream *stream ); /*decodes a b64 string found in the jsonobject with key audio and plays it. */
#endif