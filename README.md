# ak37e_lvgl8

## 一、项目介绍
这是一个 **安凯37E系列开发平台** 移植 **LVGL v8.2** 的工程项目，可作为模板工程，用于开发者快速上手在 AK37E 平台上开发图形界面。

## 二、编译环境准备
编译该项目建议在以下环境：

- Ubuntu 16 操作系统
- cmake、make 已安装
- 交叉编译环境已配置（购买 AK37E 开发板后，向官方索要开发文档，里有流程）

运行 run.sh 脚本会在 build 目录下生成目标程序。

## 三、项目配置
不同 AK37E 平台在外设配置上会有差异，该节主要介绍对 LVGL 的 **显示驱动** 和 **触摸驱动** 源码进行对应修改。

### 1. 显示驱动
可能需要修改的源码文件包括：`lv_conf.h`、`lv_port_disp.c`、`ak37e/vo.h`。

- lv_conf.h：此文件仅需要配置颜色位深，项目默认 32 位深。
```c
/*Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#define LV_COLOR_DEPTH 32
```

- lv_port_disp.c：在此文件找到 **disp_flush** 函数，该函数会将 lvgl 显存数据复制到 anyka gui 图层内存中，每次操作单个像素。由于默认 32 深度，且透明度不使用，因此每次仅从 color_p 指向的像素内存中抽取 3 字节（即 24 bit RGB）数据，用结构体 uint888 指示，如果你选择了其它深度，请酌情修改。除此之外，复制像素时，请注意 lvgl 与 anyka gui 的像素排序是否一致（例如：内存由低到高，lvgl 为 RGBRGB... ，anyka gui 可能为 BGRBGR...）。
```c
//获取anyka gui图层的虚拟内存地址，并将color_p内存的像素写入gui图层
uint888* anyka_gui_addr = (uint888*)(anyka_gui_info.vir_addr);
uint32_t addrIndex = 0;
int16_t x , y;
for(y = area->y1 ; y <= area->y2 ; y++)
{
    for(x = area->x1 ; x <= area->x2 ; x++)
    {
        addrIndex = y * ANYKA_LCD_W + x;
        //注意color_p与anyka_gui_addr的RGB排序方式是否一致
        anyka_gui_addr[addrIndex] = *((uint888*)color_p);

        color_p++;
    }
}
```

- ak37e/vo.h：请根据你的开发板显示屏的实际分辨率进行修改。
```c
#define ANYKA_LCD_W 800
#define ANYKA_LCD_H 1280
```

### 2. 触摸驱动
可能需要修改的源码文件包括：`lv_port_indev.c`。

- lv_port_indev.c：该文件实际为 lvgl 的输入设备驱动文件，由于该项目仅使用触摸，故将 mouse、keypad、encoder、button 等设备相关变量及函数全部用 `#if 0` 屏蔽掉，如有需要，请自行更改。对于触摸驱动，找到 **touchpad_init** 与 **touchpad_is_pressed** 函数，前者用于触摸设备初始化，后者用于触摸按压事件处理。
    - touchpad_init：根据你的实际情况修改触摸设备文件路径。
    ```c
    touch_fd = open("/dev/input/event0" , O_RDONLY | O_NONBLOCK);
    ```

    - touchpad_is_pressed：同样根据实际情况重写触摸事件处理，你的触摸事件 code 与 value 的值甚至顺序可能与本项目不一样。
    ```c
    //读取触摸屏的触摸输入的方式需要根据不同的触摸屏进行对应处理
    if(read(touch_fd , &touch_in_buf , sizeof(touch_in_buf)) > 0)
    {
        if(EV_ABS == touch_in_buf.type && ABS_MT_POSITION_X == touch_in_buf.code)
            touch_x = touch_in_buf.value;
    }
    if(read(touch_fd , &touch_in_buf , sizeof(touch_in_buf)) > 0)
    {
        if(EV_ABS == touch_in_buf.type && ABS_MT_POSITION_Y == touch_in_buf.code)
            touch_y = touch_in_buf.value;
    }
    if(read(touch_fd , &touch_in_buf , sizeof(touch_in_buf)) > 0)
    {
        if(EV_ABS == touch_in_buf.type && ABS_MT_TOUCH_MAJOR == touch_in_buf.code && 0 == touch_in_buf.value)
            return false;
    }
    ```

## 四、开发