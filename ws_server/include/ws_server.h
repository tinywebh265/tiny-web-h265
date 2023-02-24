#ifndef _WEB_WS_SERVER_H_
#define _WEB_WS_SERVER_H_

#ifdef __cplusplus 
extern "C" {
#endif

typedef enum
{
	THECLIENT_LOGIN=1,   //有连接并且登录成功；
	LAST_THECLIENT_DISCONNECT  //最后一个连接断开；
}ws_cmd_event_e;


/*
type = 1为cmd 
ws_cmd_event_e event = pdatabuf[0];
*/



/*
type=2为framedata(视频数据)
*/


typedef int (*ws_recvdata_callback)(int channel, int type, char *pdatabuf, int datalen);

/*
开启流通道

channel 自定流通道号(不能够重复开启);

返回值：
-1 未初始化；
-2 通道已经开启；
-3 超过最大通道个数；

0 正常;
*/
int wsserver_openstream(int channel);

/*
关闭流通道

channel 流通道号;

返回值：
-1 没有找到channel；
0 正常;
*/
int wsserver_closestream(int channel);


/*
设置 流通道音频格式；

channel 流通道号;
audio_sample_fmt 1 PCM_ALAW, 2 PCM_MUALAW;

返回值：
-1 没有找到channel；
0 正常;
*/
int wsserver_setchannelaudio(int channel, unsigned char audio_sample_fmt);

/*
发送帧数据到监看通道

channel 流通道号;
frametype 帧类型和ws_av_frame_type_e对应;
pdatabuf 裸流:(h264和h265 I 帧部分，要求带上sps vps pps或者sei 合并成跪1个I帧发送 ) 
datalen 数据长度;

返回值：
-1 未初始化；
-2 申请内存失败；
-3 没有client 监看；
0 正常;
*/
int wsserver_sendframedata(int channel, unsigned char frametype, char *pdatabuf, int datalen);


/*
模块初始化；

userid 用户 id;
pkey 用户 key;
demo 时候配置: 
char userid[32] = "TinyWebh265_DemoT";
char key[64] = "280c2b15a03182684c210f77a8m3845fa0cn";

port websocket server 监听端口
webcallback web client 回调事件

返回值：
-1 参数错误；
-2 userid 和 key 不匹配；
0 正常;
*/

int wsserver_init(char *userid, char *pkey, unsigned short port, ws_recvdata_callback webcallback);



#ifdef __cplusplus 
}
#endif



#endif 
