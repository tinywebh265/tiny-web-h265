#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>


#include "ws_server.h" 
#include "avfilestream.h"
#include "ws_avframecom.h"

#define SERVERPORT		8536
#define HEADERDATALEN	4096

int headeroffset = 0;
unsigned char *pheaderbuf = NULL;

void OneFrameFile_CallBack(int eventtype, void *pavframe, int frame_type, unsigned char *framebuf, int framelen)
{
	int ret;
	int realframelen;
	unsigned char *realframebuf;
	unsigned char realframe_type;
			
				
	if(eventtype == 1)
	{
		if(frame_type == 10)
		{
			if(!pheaderbuf)
				pheaderbuf = malloc(HEADERDATALEN);

			headeroffset = 0;
		}

		if(frame_type == 10 || frame_type == 11 || frame_type == 12 || frame_type == 13 )
		{
			if(pheaderbuf)
			{
				memcpy(pheaderbuf+headeroffset, framebuf, framelen);
				headeroffset += framelen;
			}

			return ;
		}

		if(frame_type == 1)
		{
			realframe_type = WS_AV_FRAME_TYPE_I;
			realframebuf = malloc(framelen+headeroffset+16);
			if(realframebuf)
			{
				memcpy(realframebuf, pheaderbuf, headeroffset);
				memcpy(realframebuf+headeroffset, framebuf, framelen);

				realframelen = headeroffset+framelen;
			}
			
		}
		else
		{
			realframelen = framelen;
			realframebuf = framebuf;
			realframe_type = WS_AV_FRAME_TYPE_P;
		}
			
		for(int i=0; i<32; i++)
		{
			ret = wsserver_sendframedata(i, realframe_type, realframebuf, realframelen);			
			if(ret != 0)
			{
				;//printf("wsserver_sendframedata err \n");
			}
		}
		if(frame_type == 1)
			free(realframebuf);
			
	}
	else if(eventtype == 2)
	{
		printf("OneFrameFile_CallBack eventtype = %d  datalen = %d\n", eventtype, framelen);
	}
}

//type 1为cmd 2为framedata
int websocket_server_callback(int channel, int type, char *pdatabuf, int datalen)
{
	printf("websocket_server_callback type = %d  datalen = %d\n", type,datalen);
	if(type == 1)
	{
		unsigned char cmddata = pdatabuf[0];
		if(cmddata == THECLIENT_LOGIN)
		{
			;//m_video_request_idr(channel);
		}
	}
	return 0;
}

static void handle_signal_action (int sig_number) {
  if (sig_number == SIGINT) {
    printf ("SIGINT caught!\n");
    /* if it fails to write, force stop */
  } else if (sig_number == SIGPIPE) {
    printf ("SIGPIPE caught!\n");
  }
}

static int setup_signals (void) {
  struct sigaction sa;
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler = handle_signal_action;
  if (sigaction (SIGINT, &sa, 0) != 0) {
    perror ("sigaction()");
    return -1;
  }
  if (sigaction (SIGPIPE, &sa, 0) != 0) {
    perror ("sigaction()");
    return -1;
  }
  return 0;
}

int main()
{
	char userid[32] = "TinyWebh265_DemoT";
	char key[64] = "280c2b15a03182684c210f77a8m3845fa0cn";

	int ret = wsserver_init(userid, key, SERVERPORT, websocket_server_callback);
	if(ret)
	{
		printf("websocket_server_init err \n");
		return -1;
	}

	for(int i=0; i<32; i++)
	{
		ret = wsserver_openstream(i);
		if(ret)
		{
			printf("websocket_server_openstream err \n");
			return -1;
		}

		
		ret = wsserver_setchannelaudio(i, 1);
		if(ret)
		{
			printf("websocket_server_openstream err \n");
			return -1;
		}
	}

	setup_signals();

	AvFileFrame*  pavframe = open_av_stream_file("demo/test.h265", OneFrameFile_CallBack);
	if(pavframe == NULL)
	{
		printf("open_av_stream_file err \n");
		return -1;
	}

	while(1)
	{
		sleep(5000);
	}

}



