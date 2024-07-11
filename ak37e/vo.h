#ifndef ANYKA_VO_H
#define ANYKA_VO_H

#include "base.h"

#include "include/ak_vo.h"
#include "include/ak_tde.h"

#define ANYKA_LCD_W 800
#define ANYKA_LCD_H 1280

typedef struct uint888
{
    unsigned char a;
    unsigned char b;
    unsigned char c;
}uint888;

extern struct ak_vo_layer_out anyka_gui_info; //gui图层结构体，包含图层的内存地址

//操控Anyka有关LCD屏幕接口，初始化相关内容，最后在LCD屏幕中创建一个GUI图层，用于用户绘画
void anyka_vo_init();

//打印LCD分辨率
void anyka_vo_print_resolution();

#endif