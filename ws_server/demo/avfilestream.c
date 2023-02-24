#include <stdio.h>
#include <string.h>

#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/select.h>

#include "avfilestream.h"

static int get_frame_type(unsigned char datatype)
{
	int frame_type = 0;

	//I frame
	if(datatype == 0x26)
	{
		frame_type = 1;
	}
	//P frame
	else if(datatype == 0x02)
	{
		frame_type = 2;
	}
	//VPS frame
	else if(datatype == 0x40)
	{
		frame_type = 10;
	}
	//SPS frame
	else if(datatype == 0x42)
	{
		frame_type = 11;
	}
	//PPS frame
	else if(datatype == 0x44)
	{
		frame_type = 12;
	}
	//SEI frame
	else if(datatype == 0x4e)
	{
		frame_type = 13;
	}

	return frame_type;
}

static int get_one_frame(AvFileFrame *pavframe, int *pframe_type, unsigned char **pframebuf, int *pframelen)
{
	if(pavframe == NULL)
		return -1;

	if(pavframe->pdatbuf == NULL || pavframe->pfilehandle == NULL)
		return -1;

	if(pavframe->fileendflag == 0)
	{
		if(pavframe->writeoffset < MAXFRAMEDATALEN)
		{
			int readlen = fread(pavframe->pdatbuf+pavframe->writeoffset, 1, MAXFRAMEDATALEN-pavframe->writeoffset, pavframe->pfilehandle);			
			if(readlen <= 0)
			{
				pavframe->fileendflag = 1;
			}
			else
			{
				pavframe->writeoffset += readlen;
			}
		}
	}

	int frame_type = 0;
	int curframe_type = 0;
	
	unsigned char *framebeginaddr = NULL;	
	unsigned char *pdatbuf = pavframe->pdatbuf;
	
	for(int i=0; i<pavframe->writeoffset-5; i++)
	{
		if(pdatbuf[i] == 0x00 && pdatbuf[i+1] == 0x00 && pdatbuf[i+2] == 0x00 && pdatbuf[i+3] == 0x01) 
		{
			frame_type = get_frame_type(pdatbuf[i+4]);
			if(frame_type != 0)
			{
				if(framebeginaddr == NULL)
				{
					framebeginaddr = pdatbuf+i;
					curframe_type = frame_type;
					continue;
				}
			}

			if(framebeginaddr != NULL)
			{
				*pframebuf = framebeginaddr;
				*pframelen = pdatbuf+i-framebeginaddr;	
				
				*pframe_type = curframe_type;
				pavframe->readffset = i;
				return 0;
			}
        }
	}

	pavframe->readffset = 0;
	pavframe->writeoffset = 0;
	pavframe->fileendflag = 0;
	
	return -2;
}

static int release_cur_frame(AvFileFrame *pavframe)
{
	if(pavframe == NULL)
		return -1;

	if(pavframe->pdatbuf == NULL || pavframe->pfilehandle == NULL)
		return -1;

	if(pavframe->writeoffset > pavframe->readffset)
	{
		memmove(pavframe->pdatbuf, pavframe->pdatbuf+pavframe->readffset, pavframe->writeoffset-pavframe->readffset);
		pavframe->writeoffset = pavframe->writeoffset-pavframe->readffset;
		pavframe->readffset = 0;
	}

	return 0;
}


void *av_stream_file_thread(void *arg) 
{
	int ret;
	int frame_type;
	struct timeval t;

	int framelen;
	unsigned char *pframebuf;
	
	printf("av_stream_file_thread begin \n");
	AvFileFrame* pavframe = (AvFileFrame*)arg;
	if(pavframe == NULL)
	{
		printf("av_stream_file_thread exit \n");
		return NULL;
	}	
	
	while(1)
	{
		if(pavframe->exitflag)
			break;
		
		ret = get_one_frame(pavframe, &frame_type, &pframebuf, &framelen);		
		if(ret != 0)
		{
			printf("get_one_frame SEEK_SET ret = %d\n", ret, framelen);
			
			fseek(pavframe->pfilehandle, 0, SEEK_SET);
			printf("end get_one_frame ret = %d\n", ret, framelen);
			
			continue;
		}

		if(frame_type == 1)
			printf("get one I frame frame_type = %d framelen = %d\n", frame_type, framelen);
		
		if(pavframe->pcallback != NULL)
		{
			pavframe->pcallback(1, pavframe, frame_type, pframebuf, framelen);
		}
	
		release_cur_frame(pavframe);

		if(frame_type == 1 || frame_type == 2)
		{
			t.tv_sec = 0;
			t.tv_usec = 40000;
	
			select(0, NULL, NULL, NULL, &t);
		}
	}

	if(pavframe->pcallback != NULL)
	{
		pavframe->pcallback(2, NULL, 0, NULL, 0);
	}
	
	fclose(pavframe->pfilehandle);
	free(pavframe->pdatbuf);
	free(pavframe);

	printf("avfilepalythread exit \n");
	pthread_exit((void*)0);
}

AvFileFrame* open_av_stream_file(char *pfilepathname, pGetOneFrame_CallBack callback)
{	
	FILE *pfilehandle = fopen(pfilepathname, "rb");
	if(pfilehandle == NULL)
	{
		printf("fopen err\n");
		return NULL;
	}

	AvFileFrame *pavframe = malloc(sizeof(AvFileFrame));
	if(pavframe == NULL)
	{
		fclose(pfilehandle);
		printf("malloc err\n");
		return NULL;
	}

	memset(pavframe, 0, sizeof(AvFileFrame));
	pavframe->pfilehandle = pfilehandle;

	pavframe->pdatbuf = malloc(MAXFRAMEDATALEN);
	if(pavframe->pdatbuf == NULL)
	{
		fclose(pfilehandle);
		free(pavframe);
		printf("malloc err\n");
		return NULL;
	}
	
	pavframe->pcallback = callback;
	memset(pavframe->pdatbuf, 0, MAXFRAMEDATALEN);
	
	int ret = pthread_create(&pavframe->threadhandle, NULL, av_stream_file_thread, (void*)pavframe);
	if(ret != 0)
	{
		fclose(pfilehandle);
		free(pavframe->pdatbuf);
		
		free(pavframe);
		printf("pthread_create err\n");
		return NULL;
	}
	
	return pavframe;
}

int  close_av_stream_file(AvFileFrame *pavframe)
{
	if(pavframe != NULL)
	{
		pavframe->exitflag = 1;
		pthread_join(pavframe->threadhandle, NULL);
	}

	return 0;
}
