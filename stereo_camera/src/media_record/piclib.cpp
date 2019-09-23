#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

//SLM
#ifdef __CYGWIN__
#include <sys/types.h>
#elif defined WIN32
#if defined __GNUWIN32__
#include <stdint.h>
#else
#define uint32_t unsigned __int32
#define uint8_t unsigned __int8
#define uint16_t unsigned __int16
#define uint64_t unsigned __int64
#endif
#else
#include <unistd.h>
#include <inttypes.h>
#endif


#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "piclib.h"
#include "huffman.h"








int is_huffman(unsigned char *buf)
{
    unsigned char *ptbuf;
    int i = 0;
    ptbuf = buf;
    while (((ptbuf[0] << 8) | ptbuf[1]) != 0xffda) {
        if (i++ > 2048)
            return 0;
        if (((ptbuf[0] << 8) | ptbuf[1]) == 0xffc4)
            return 1;
        ptbuf++;
    }
    return 0;
}

int get_picture(char *name, unsigned char *buf,int size)
{
    FILE *file;
    unsigned char *ptdeb,*ptcur = buf;
    int sizein;

    file = fopen(name, "wb");
    if (file != NULL) {
        if (!is_huffman(buf)) {
            ptdeb = ptcur = buf;
            while (((ptcur[0] << 8) | ptcur[1]) != 0xffc0)
                ptcur++;
            sizein = ptcur - ptdeb;
            fwrite(buf, sizein, 1, file);
            fwrite(dht_data,  DHT_SIZE, 1, file);
            fwrite(ptcur, size - sizein, 1, file);
        } else {
            fwrite(ptcur,size,1,file); /* ptcur was uninit -wsr */
        }
        fclose(file);
    }

    return 0;
}















