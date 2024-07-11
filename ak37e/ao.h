#ifndef ANYKA_AO_H
#define ANYKA_AO_H

#include "base.h"

#include "include/ak_ao.h"

extern int anyka_ao_handle_id;

//操控Anyka有关音频输出接口，初始化相关内容，用于用户播放音频，若初始化失败，则anyka_ao_handle_id保持为-1
void anyka_ao_init();

//调节anyka AO模块的音量
void anyka_ao_set_volume(int db);

//播放WAV文件
void anyka_ao_play_wav(char* wav_file);

#endif