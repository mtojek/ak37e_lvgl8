#include "vo.h"

struct ak_vo_layer_out anyka_gui_info = {0 , 0 , NULL};

//操控Anyka有关LCD屏幕接口，初始化相关内容，最后在LCD屏幕中创建一个GUI图层，用于用户绘画
void anyka_vo_init()
{
	int r = ak_tde_open();
    if(r != 0)
    {
        anyka_perror("[ ak_tde_open call failed ]" , r);
        goto ANYKA_VO_INIT_FAILED_CLOSE_TDE;
    }

	static struct ak_vo_param param;
    param.width = ANYKA_LCD_W;
    param.height = ANYKA_LCD_H;
    param.format = GP_FORMAT_RGB888;
    param.rotate = AK_GP_ROTATE_NONE; 
    param.init_mode = AK_VO_INIT_WHIHE;
    r = ak_vo_open(&param, DEV_NUM);
    if(r != 0)
    {
	    anyka_perror("[ ak_vo_open call failed ]" , r);
        goto ANYKA_VO_INIT_FAILED_CLOSE_VO;
    }

	static struct ak_vo_layer_in gui_layer;
    gui_layer.create_layer.height = ANYKA_LCD_H;
    gui_layer.create_layer.width  = ANYKA_LCD_W;
    gui_layer.create_layer.left  = 0;
    gui_layer.create_layer.top   = 0;
    gui_layer.format             = GP_FORMAT_RGB888;
    gui_layer.layer_opt = GP_OPT_NONE;
    
    r = ak_vo_create_gui_layer(&gui_layer, AK_VO_LAYER_GUI_1, &anyka_gui_info);
    if(r != 0)
    {
	    anyka_perror("[ ak_vo_create_gui_layer call failed ]" , r);
        goto ANYKA_VO_INIT_FAILED_DESTROY_GUI_LAYER;
    }

	r = ak_vo_set_fbuffer_mode(AK_VO_BUFF_SINGLE);
    if(r != 0)
    {
	    anyka_perror("[ ak_vo_set_fbuffer_mode call failed ]" , r);
        goto ANYKA_VO_INIT_FAILED_DESTROY_GUI_LAYER;
    }

    return;

ANYKA_VO_INIT_FAILED_DESTROY_GUI_LAYER:
    ak_vo_destroy_layer(AK_VO_LAYER_GUI_1);
ANYKA_VO_INIT_FAILED_CLOSE_VO:
    ak_vo_close(DEV_NUM);
ANYKA_VO_INIT_FAILED_CLOSE_TDE:
    ak_tde_close();
    exit(-1);
}

//打印LCD分辨率
void anyka_vo_print_resolution()
{
    int w , h , fb_w , fb_h;
	int r = ak_vo_get_resolution(&w , &h , &fb_w , &fb_h);
    if(0 != r)
	    anyka_perror("[ ak_vo_get_resolution call failed ]" , r);
    else
	    printf("[########]anyka vo res is (w , h , fb_w , fb_h) : (%d , %d , %d , %d)\n" , w , h , fb_w , fb_h);
}