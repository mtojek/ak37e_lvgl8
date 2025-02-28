#ifndef ANYKA_VO_H
#define ANYKA_VO_H

#include "base.h"

#include "include/ak_vo.h"
#include "include/ak_tde.h"
#include "include/ak_mem.h"

#define ANYKA_LCD_W 1024
#define ANYKA_LCD_H 600

extern struct ak_vo_layer_out anyka_gui_info; //gui图层结构体，包含图层的内存地
					      //
extern unsigned char * gui_buffer;

//操控Anyka有关LCD屏幕接口，初始化相关内容，最后在LCD屏幕中创建一个GUI图层，用于用户绘画
void anyka_vo_init();

//打印LCD分辨率
void anyka_vo_print_resolution();

#endif
