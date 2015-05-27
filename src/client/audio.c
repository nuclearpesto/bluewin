
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
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "portaudio.h"
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_net.h>
 #include <jansson.h>
 #include "base64.h"
 #include "audio.h"
#include "client.h"



//void errorHappened(PaStream * stream,char *sampleBlock, PaError er);		  

//void xrun(PaStream * stream,char *sampleBlock, PaError err);


/*******************************************************************/
void init_sound( audiostruct_t *audiostruct )
 {
	PaStream *readstream = NULL, *writestream = NULL;
	PaError err;
	PaStreamParameters inputParameters, outputParameters;
	char *readBlock, *writeBlock;
	int i;
	int numBytes;
	printf("SAMPLETYPE %s\n", PA_SAMPLE_TYPE_PRINTABLE);
 
	//initialize audio streams
	    err = Pa_Initialize();
     if( err != paNoError ){
       //errorHappened(readstream, sampleBlock, err);
     }

    
     numBytes = FRAMES_PER_BUFFER * NUM_CHANNELS * SAMPLE_SIZE ;
     readBlock = (char *) malloc( numBytes );
     if( readBlock == NULL )
     {
         printf("Could not allocate record array.\n");
         exit(1);
     }
     CLEAR( readBlock );
	  writeBlock = (char *) malloc( numBytes );
     if( writeBlock == NULL )
     {
         printf("Could not allocate record array.\n");
         exit(1);
     }
     CLEAR( writeBlock );
  
  
     outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
     printf( "Output device # %d.\n", outputParameters.device );
     printf( "Output LL: %g s\n", Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency );
     printf( "Output HL: %g s\n", Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency );
     outputParameters.channelCount = NUM_CHANNELS;
     outputParameters.sampleFormat = PA_SAMPLE_TYPE;
     outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency;
     outputParameters.hostApiSpecificStreamInfo = NULL;
     
	 inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
     printf( "Input device # %d.\n", inputParameters.device );
     printf( "Input LL: %g s\n", Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency );
     printf( "Input HL: %g s\n", Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency );
     inputParameters.channelCount = NUM_CHANNELS;
     inputParameters.sampleFormat = PA_SAMPLE_TYPE;
     inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency ;
     inputParameters.hostApiSpecificStreamInfo = NULL;
     
	     err = Pa_OpenStream(
			 &writestream,
			 NULL,//output only stream;
			 &outputParameters,
			 SAMPLE_RATE,
			 FRAMES_PER_BUFFER,
			 paClipOff,      /* we won't output out of range samples so don't bother clipping them */
			 NULL, /* no callback, use blocking API */
               NULL ); /* no callback, so no callback userData */

	 
	 
      err = Pa_OpenStream(
			 &readstream,
			 &inputParameters,//input only stream;
			 NULL,
			 SAMPLE_RATE,
			 FRAMES_PER_BUFFER,
			 paClipOff,      /* we won't output out of range samples so don't bother clipping them */
			 NULL, /* no callback, use blocking API */
               NULL ); /* no callback, so no callback userData */
	
	
	err = Pa_StartStream( readstream );
     if( err != paNoError ){
       errorHappened(readstream, readBlock, err);
     }
	 
	err = Pa_StartStream( writestream );
     if( err != paNoError ){
       errorHappened(writestream, writeBlock, err);
     }
		audiostruct ->readstream = readstream;
		audiostruct->writestream = writestream;
		audiostruct->readBlock = readBlock;
		audiostruct-> writeBlock = writeBlock;
     
     
     
 }

//threadreadaudio read audio from microphone and send to server
int  readthread(void*data){
  audiothreadstruct *p = (audiothreadstruct * ) data;
  PaStream *stream = p->audiostruct->readstream;
  char *sampleBlock = p->audiostruct->readBlock;
  TCPsocket socket = p->socket;
  PaError err;
  SDL_mutex *writeMutex = p->writeMutex;
  char *b64encoded;
  int i, j;
  int numBytes;
  json_t *obj = json_object();
  json_t *cmd, *audio64;
  const char* jsonstring;
  numBytes = FRAMES_PER_BUFFER * NUM_CHANNELS * SAMPLE_SIZE ;
  //printf("socket is %d", socket);
		//allocate enough space for the encoded version of our string
		b64encoded = (char *) malloc( (numBytes*1.37)+814 +1 );
		if( b64encoded == NULL )
     {
         printf("Could not allocate base64 array.\n");
         exit(1);
     }
     CLEAR( sampleBlock );
	 //prepare json msg
    // json cmd for this thread will always be "add call"" 
		serialize_cmd(obj,"add call");
		serialize_room(obj,"default");
     int test;
	 //send audio while we are logged in
     while(1)
     {
		 //printf("reading frm mic");
		 err = Pa_ReadStream( stream, sampleBlock, FRAMES_PER_BUFFER );
		 b64_encode(sampleBlock, b64encoded, numBytes);
		 audio64 = json_string(b64encoded);
		 if(!audio64){
			 printf("ADUIO JSON STRING DOES NOT WORK\n");
		 }
		 json_object_set_new(obj,"audio", audio64 );
		 jsonstring = json_dumps(obj, 0);
		 test = strlen(jsonstring);
		 //printf("sending %s\n", jsonstring);
		 /*SDLNet_TCP_Send(socket, &test, sizeof(int));
			SDLNet_TCP_Send(socket, jsonstring, test); */
			write_to_server(obj, &socket, writeMutex);
			fflush(stdout);
		  if( err && CHECK_UNDERFLOW ){ 
			xrun(stream, sampleBlock, err); 
		 } 
		 
     }
     if( err && CHECK_OVERFLOW ){
       xrun(stream, sampleBlock, err);
     }
     err = Pa_StopStream( stream );
     if( err != paNoError ){
       errorHappened(stream, sampleBlock, err);
     }
     
	
     CLEAR( sampleBlock );
     
     free( sampleBlock );
     
     return 1;     
 
     
}

// write audio to the speakers
int  playaudio(json_t *obj, char* sampleBlock, PaStream *stream ){
  PaError err;
  json_t *jsonencoded;
  json_error_t error;
  const char *b64encoded;
  int i, j, len;
  int numBytes;
  int numtorecieve;

     numBytes = FRAMES_PER_BUFFER * NUM_CHANNELS * SAMPLE_SIZE ;
   
    	jsonencoded = json_object_get(obj,"audio");
		b64encoded= json_string_value(jsonencoded);
		len = strlen(b64encoded);
		b64_decode(b64encoded, sampleBlock, len );
		err = Pa_WriteStream(stream, sampleBlock, FRAMES_PER_BUFFER );
		CLEAR(sampleBlock);
	 
     if( err && CHECK_OVERFLOW ){
       xrun(stream, sampleBlock, err);
     }
    // err = Pa_StopStream( stream );
     //if( err != paNoError ){
      // errorHappened(stream, sampleBlock, err);
    // }
     
     //CLEAR( sampleBlock );
     
    // free( sampleBlock );

          
     return 1;

     

}




void xrun(PaStream * stream,char *sampleBlock, PaError err){
     if( stream ) {
        Pa_AbortStream( stream );
        Pa_CloseStream( stream );
     }
     //free( sampleBlock );
     Pa_Terminate();
     if( err & paInputOverflow )
        fprintf( stderr, "Input Overflow.\n" );
     if( err & paOutputUnderflow )
        fprintf( stderr, "Output Underflow.\n" );
     exit(1);
}


void errorHappened(PaStream * stream,char *sampleBlock, PaError err){
     if( stream ) {
        Pa_AbortStream( stream );
        Pa_CloseStream( stream );
     }
     //free( sampleBlock );
     Pa_Terminate();
     fprintf( stderr, "An error occured while using the portaudio stream\n" );
     fprintf( stderr, "Error number: %d\n", err );
     fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
     exit(1);

}
