#ifndef ANYKA_BASE_H
#define ANYKA_BASE_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "include/ak_common.h"
#include "include/ak_thread.h"
//#include "include/ak_common_graphics.h"

typedef struct wave_header        //WAV文件格式结构体定义
{
    char          ChunkID[4];     //内容为"RIFF"
    uint32_t      ChunkSize;      //存储文件的字节数（不包含ChunkID和ChunkSize这8个字节）
    char          Format[4];      //内容为"WAVE"

    char          Subchunk1ID[4]; //内容为"fmt "
    uint32_t      Subchunk1Size;  //存储该子块的字节数（不含前面的Subchunk1ID和Subchunk1Size这8个字节）
    uint16_t      AudioFormat;    //存储音频文件的编码格式，例如若为PCM则其存储值为1。
    uint16_t      NumChannels;    //声道数，单声道(Mono)值为1，双声道(Stereo)值为2，等等
    uint32_t      SampleRate;     //采样率，如8k，44.1k等
    uint32_t      ByteRate;       //每秒存储的bit数，其值 = SampleRate * NumChannels * BitsPerSample / 8
    uint16_t      BlockAlign;     //块对齐大小，其值 = NumChannels * BitsPerSample / 8
    uint16_t      BitsPerSample;  //每个采样点的bit数，一般为8,16,32等。

    char          Subchunk2ID[4]; //内容为"data"
    uint32_t      Subchunk2Size;  //接下来的正式的数据部分的字节数，其值 = NumSamples * NumChannels * BitsPerSample / 8
}WAVE_HEADER;

//自定义anyka API错误输出函数
void anyka_perror(const char* str , int v);

//Anyka SDK初始化
void anyka_sdk_init();

#endif