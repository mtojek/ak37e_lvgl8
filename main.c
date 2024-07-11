#include "work/work.h"

uint8_t anyka_lvgl_run = 1;         //是否继续运行lvgl事件循环
const int anyka_lvgl_period_ms = 5; //lvgl的处理周期

//lvgl时钟线程相关
ak_pthread_t tick_tid;
void* tick_thread(void* arg)
{
    while(anyka_lvgl_run)
    {
        ak_sleep_ms(anyka_lvgl_period_ms);
        lv_tick_inc(anyka_lvgl_period_ms); //tick 提供时钟，一般为 5ms 即可
    }

    return NULL;
}

int main(int argc , char* argv[])
{
    anyka_sdk_init();

	lv_init();
    lv_port_disp_init();        // 显示器初始化
    lv_port_indev_init();       // 触摸屏初始化

    //创建时钟线程
    int r = ak_thread_create(&tick_tid , tick_thread , NULL , ANYKA_THREAD_MIN_STACK_SIZE , ANYKA_THREAD_PRIO_HIGH);
    if(0 != r)
    {
        anyka_perror("[ create tick thread failed ]" , r);
        return -1;
    }
    ak_thread_detach(tick_tid);

    work();

	while(anyka_lvgl_run)
    {
        ak_sleep_ms(anyka_lvgl_period_ms);
        lv_timer_handler(); //运行所有lvgl的timer，调用之前先lv_tick_inc
    }

    if(touch_fd > 0)
        close(touch_fd);

	ak_vo_destroy_layer(AK_VO_LAYER_GUI_1);
	ak_vo_close(DEV_NUM);
	ak_tde_close();

	ak_sdk_exit();
    
	return 0;
}
