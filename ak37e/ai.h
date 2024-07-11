#ifndef ANYKA_AI_H
#define ANYKA_AI_H

#include "base.h"

#include "include/ak_ai.h"

extern int anyka_ai_handle_id;

//操控Anyka有关音频输入接口，初始化相关内容，用于用户获取MIC输入，若初始化失败，则anyka_ai_handle_id保持为-1
void anyka_ai_init();

//录制指定时长音频
void anyka_ai_record_pcm(char* pcm_file , unsigned time);

#endif