#ifndef _WS_AV_FRAME_COM_H
#define _WS_AV_FRAME_COM_H

#ifdef __cplusplus 
extern "C" {
#endif


typedef enum
{
	WS_AV_FRAME_TYPE_I=1, // I帧
	WS_AV_FRAME_TYPE_P,	  // P帧 
	WS_AV_FRAME_TYPE_B,	  // B帧
	WS_AV_FRAME_TYPE_A	  // 音频帧
}ws_av_frame_type_e;


#ifdef __cplusplus 
}
#endif




#endif



