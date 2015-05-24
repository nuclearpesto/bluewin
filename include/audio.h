 #ifndef AUDIO
 #define AUDIO
 /* #define SAMPLE_RATE  (17932) // Test failure to open with this value. */
 #define SAMPLE_RATE  (16000)
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
 #elif 0
 #define PA_SAMPLE_TYPE  paInt16
 #define PA_SAMPLE_TYPE_PRINTABLE  "int 16"
 #define SAMPLE_SIZE (2)
 #define SAMPLE_SILENCE  (0)
 #define CLEAR(a) memset( (a), 0,  FRAMES_PER_BUFFER * NUM_CHANNELS * SAMPLE_SIZE )
 #define PRINTF_S_FORMAT "%d"
 #elif 1
 #define PA_SAMPLE_TYPE  paInt24
 #define PA_SAMPLE_TYPE_PRINTABLE  "int 24"
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
  
};typedef struct audiothreadstruct audiothreadstruct_t;

void errorHappened(PaStream * stream,char *sampleBlock, PaError er);		  

void xrun(PaStream * stream,char *sampleBlock, PaError err);
void init_sound( audiostruct_t *audiostruct );
int  readthread(void*data);
int  playaudio(json_t *obj, char* sampleBlock, PaStream *stream );
#endif