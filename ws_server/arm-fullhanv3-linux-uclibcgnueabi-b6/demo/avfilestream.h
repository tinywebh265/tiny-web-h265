#ifndef AV_FILE_STREAM_H_
#define AV_FILE_STREAM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAXFRAMEDATALEN   (512<<10)

//1 帧回调 2 退出
typedef void (*pGetOneFrame_CallBack)(int eventtype, void *pavframe, int frame_type, unsigned char *framebuf, int framelen);  


typedef struct AvFileFrame {

	unsigned char exitflag;
	pthread_t threadhandle;

	pGetOneFrame_CallBack pcallback;
	
	unsigned char fileendflag; 
	FILE *pfilehandle;

	int readffset;
	int writeoffset;
	unsigned char *pdatbuf;
	
} AvFileFrame;

AvFileFrame* open_av_stream_file(char *pfilepathname, pGetOneFrame_CallBack callback);


int close_av_stream_file(AvFileFrame *pavframe);


#endif // AV_FILE_STREAM_H_
