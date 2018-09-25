/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis 'TREMOR' CODEC SOURCE CODE.   *
 *                                                                  *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis 'TREMOR' SOURCE CODE IS (C) COPYRIGHT 1994-2002    *
 * BY THE Xiph.Org FOUNDATION http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: simple example decoder using vorbisidec

 ********************************************************************/

/* Possible preprocessor defines for the TTA version:
 *
 * MEMORY_IO                  When defined, source data is read from 
 *                            a preloaded memory buffer starting at 
 *                            char* source_buffer, with size defined in 
 *                            variable source_buffer_size. Result is stored in
 *                            a ring buffer at char* destination_buffer, with
 *                            size defined in variable destination_buffer_size.
 *
 * SPECIAL_OPERATION_IO       When defined, source data is read from a special
 *                            operation INPUT_DATA, writing to operand 1 advances the
 *                            read cursor, data is read from operand 2. Operand 3
 *                            is 1 in case new data is still coming (no EOF). Result
 *                            data is written to a special operation OUTPUT_DATA,
 *                            of which only (input) operand write triggers and
 *                            saves a 8 bit byte.
 *
 * STANDARD_IO                Use standard input and output.
 *
 * USE_TCE_LIB                Use tcelib.h instead of standard C libraries.
 */

#include "debug.h"

#ifdef TCE

#ifdef __TCE_V1__
# include "tceops.h"
#else
#include "userdef.h"
#endif

#undef stdout
int putchar(int ch) {
   #ifdef __TCE_V1__
    _TCE_STDOUT(ch);
   #else
    WRITETO(stdout.1, ch);
   #endif
}
#endif

#if !defined(MEMORY_IO) && !defined(STANDARD_IO) && !defined(SPECIAL_OPERATION_IO)
#define STANDARD_IO
#endif

#ifdef STANDARD_IO

#undef long
#include <stdio.h>
#include <stdlib.h>
#define long int

#endif

#include <vorbis/ivorbiscodec.h>
#include <vorbis/ivorbisfile.h>

#if defined(SPECIAL_OPERATION_IO) || defined(DUMP_MEM_BUFFER_TO_SPECIAL_OPERATION)

#endif

#if defined(SPECIAL_OPERATION_IO)

/* assumes size or nmemb is 1, which it seems to be in the only call at 
   vorbisfile.c (size is hardcoded to 1) */
size_t tta_read_function(
    void* ptr, size_t size, size_t nmemb, void* datasource) {
    char no_eof = 1;
    size_t bytes_read = 0; 
    size = nmemb;
    while (no_eof && bytes_read < size) {
        char byte;

#ifdef __TCE_V1__
       int ne = no_eof;
       int b = byte;
        _TCE_INPUT_DATA(b, ne, 1);
        *(char*)ptr = byte;
        ++ptr;
        ++bytes_read;
#else
        WRITETO(input_data.1, 1);
        READINT(input_data.2, byte);
        *(char*)ptr = byte;
        ++ptr;
        ++bytes_read;
        READINT(input_data.3, no_eof);
#endif
    }
    
    if (no_eof) 
        return bytes_read;    
    else
        return 0;
}

#elif defined(MEMORY_IO)

/*
#include "initial_data.c"
*/

/*
#define INITIAL_DATA_SIZE 2784313 
*/

extern char initial_data[]; /*[INITIAL_DATA_SIZE]; */
extern int output_buffer_size;
extern int initial_data_size;

/*
#define OUTPUT_BUFFER_SIZE 33502976
*/

extern char output_data[];
/*static char output_data[OUTPUT_BUFFER_SIZE]; */

/* assumes size or nmemb is 1, which it seems to be in the only call at 
   vorbisfile.c (size is hardcoded to 1) */
size_t tta_read_function(
    void* ptr, size_t size, size_t nmemb, void* datasource) {
    static size_t cursor = 0;
    char eof = 0;
    size_t bytes_read = 0; 
    size = nmemb;
    DPRINTF(("tta_read_function(?, %d, %d, ?)\n", size, nmemb)); 

    if (cursor >= initial_data_size)
        eof = 1;

    while (!eof && bytes_read < size) {
        *(char*)ptr = initial_data[cursor];
        ++cursor;
        ++ptr;
        ++bytes_read;
        if (cursor >= initial_data_size)
            eof = 1;
    }
    
    return bytes_read;
}

#endif

#if defined(SPECIAL_OPERATION_IO) || defined(MEMORY_IO) 

int tta_close_function(void* datasource) {
    return 0;
}
/* Not seekable, thus we need to provide only dummy implementations
   of seek_function() and tell_function(). */
int tta_seek_function(void* datasource, ogg_int64_t offset, int whence) {
    return -1;
}
long tta_tell_function(void* datasource) {
    return 0;
}

#endif

char pcmout[4096]; /* take 4k out of the data segment, not the stack */

int main(void)
{ 
    OggVorbis_File vf;
    int eof = 0;
    int current_section;

#ifdef MEMORY_IO
    static size_t bytes_written = 0;
#endif

#ifdef STANDARD_IO
    if(ov_open(stdin, &vf, NULL, 0) < 0) {
        fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
        exit(1);
    }
#else

    ov_callbacks callbacks;

#if defined(SPECIAL_OPERATION_IO) || defined(MEMORY_IO)

    callbacks.read_func = tta_read_function;
    callbacks.seek_func = tta_seek_function;
    callbacks.close_func = tta_close_function;
    callbacks.tell_func = NULL;
#endif

#if defined(MEMORY_IO)    
    DPR("calling ov_open_callbacks");
    ov_open_callbacks(&initial_data, &vf, NULL, 0, callbacks);
#elif defined(SPECIAL_OPERATION_IO)
    ov_open_callbacks(NULL, &vf, NULL, 0, callbacks);
#endif
   
#endif
  
    /* The main decoding loop. */
    while(!eof){
        long ret = ov_read(&vf, pcmout, sizeof(pcmout), &current_section);
        DPRINTF(("ret = %ld\n", ret));
        if (ret == 0) {
            /* EOF */
            eof = 1;
        } else if (ret < 0) {
            /* error in the stream.  Not a problem, just reporting it in
               case we (the app) cares.  In this case, we don't. */
        } else {
#ifdef STANDARD_IO
            /* we don't bother dealing with sample rate changes, etc, but
               you'll have to*/
            fwrite(pcmout, 1, ret, stdout);

#elif defined(SPECIAL_OPERATION_IO)
            {
                int i = 0;
                for (; i < ret; ++i)
                    WRITETO(outputdata.1, pcmout[i]);
            }
#elif defined(MEMORY_IO)
            {
                int i = 0;
                /* Write to the output buffer, in case it gets filled up,
                   starts writing from the beginning and overwrites old
                   data. */
                for (; i < ret; ++i, ++bytes_written) {
                    output_data[bytes_written % output_buffer_size] = 
                        pcmout[i];
                }
		
		/* print 16bit samples */
		i = 0;
		for (; i < ret/2; ++i) {
//#if BYTE_ORDER==LITTLE_ENDIAN
#ifndef __BIG_ENDIAN__
		    DPRINTF(("0x%02hhx ", pcmout[i*2]));
		    DPRINTF(("0x%02hhx ", pcmout[i*2 + 1]));
#else
		    DPRINTF(("0x%02hhx ", pcmout[i*2 + 1]));
		    DPRINTF(("0x%02hhx ", pcmout[i*2]));
#endif
		}
		    
                DPRINTF(("bytes_written = %d\n", bytes_written));
            }
#endif
        }
    }
    
    /* cleanup */
    ov_clear(&vf);

#ifdef DUMP_MEMORY_BUFFER_TO_STDOUT
    /* for verifying that the memory I/O works correctly */
    fwrite(output_data, 1, output_buffer_size, stdout);
#elif defined(DUMP_MEM_BUFFER_TO_SPECIAL_OPERATION)
    {
        int i = 0;
        char first, second;
        for (; i < output_buffer_size; ++i) {
            first = output_data[i % output_buffer_size];
            ++i;
            second = output_data[i % output_buffer_size];
            /* we need to swap the output be cause TTA is big endian */

#ifdef __TCE_V1__
	   int s = second;
	   int f = first;
#ifdef __BIG_ENDIAN__
            _TCE_OUTPUTDATA(s);
            _TCE_OUTPUTDATA(f);
#else
            _TCE_OUTPUTDATA(f);
            _TCE_OUTPUTDATA(s);
#endif

#else
            WRITETO(outputdata.1, second);
            WRITETO(outputdata.1, first);
#endif
        }
    }

#endif

    return(0);
}
