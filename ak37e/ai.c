#include "ai.h"

int anyka_ai_handle_id = -1;

//操控Anyka有关音频输入接口，初始化相关内容，用于用户获取MIC输入，若初始化失败，则anyka_ai_handle_id保持为-1
void anyka_ai_init()
{
    anyka_ai_handle_id = -1;

    struct ak_audio_in_param ai_param;
    memset(&ai_param , 0 , sizeof(struct ak_audio_in_param));
    ai_param.pcm_data_attr.sample_rate = AK_AUDIO_SAMPLE_RATE_16000;
    ai_param.pcm_data_attr.sample_bits = AK_AUDIO_SMPLE_BIT_16;
    ai_param.pcm_data_attr.channel_num = AUDIO_CHANNEL_MONO;
    ai_param.dev_id = DEV_ADC;
    int r = ak_ai_open(&ai_param , &anyka_ai_handle_id);
    if(r != 0)
    {
        anyka_perror("[ ak_ai_open call failed ]" , r);
        goto ANYKA_AI_INIT_FAILED;
    }

    r = ak_ai_set_source(anyka_ai_handle_id , AI_SOURCE_MIC);
    if(r != 0)
    {
        anyka_perror("[ ak_ai_set_source call failed ]" , r);
        goto ANYKA_AI_INIT_FAILED;
    }

    r = ak_ai_set_gain(anyka_ai_handle_id , 5);
    if(r != 0)
    {
        anyka_perror("[ ak_ai_set_gain call failed ]" , r);
        goto ANYKA_AI_INIT_FAILED;
    }

    struct ak_audio_nr_attr nr_attr = {-40 , 0 , 1};
    r = ak_ai_set_nr_attr(anyka_ai_handle_id , &nr_attr);
    if(r != 0)
    {
        anyka_perror("[ ak_ai_set_nr_attr call failed ]" , r);
        goto ANYKA_AI_INIT_FAILED;
    }

    struct ak_audio_agc_attr agc_attr = {24576 , 4 , 0 , 80 , 0 , 1};
    r = ak_ai_set_agc_attr(anyka_ai_handle_id , &agc_attr);
    if(r != 0)
    {
        anyka_perror("[ ak_ai_set_agc_attr call failed ]" , r);
        goto ANYKA_AI_INIT_FAILED;
    }

    r = ak_ai_set_volume(anyka_ai_handle_id , 5);
    if(r != 0)
    {
        anyka_perror("[ ak_ai_set_volume call failed ]" , r);
        goto ANYKA_AI_INIT_FAILED;
    }

    r = ak_ai_start_capture(anyka_ai_handle_id);
    if(r != 0)
    {
        anyka_perror("[ ak_ai_start_capture call failed ]" , r);
        goto ANYKA_AI_INIT_FAILED;
    }

    return;

ANYKA_AI_INIT_FAILED:
    if(anyka_ai_handle_id >= 0)
        ak_ai_close(anyka_ai_handle_id);
    anyka_ai_handle_id = -1;
}

//录制指定时长音频
void anyka_ai_record_pcm(char* pcm_file , unsigned duration)
{
    if(anyka_ai_handle_id < 0)
    {
        anyka_perror("[ anyka_ai_handle_id is invalid ]" , -1);
        return;
    }

    int fd = open(pcm_file , O_WRONLY | O_CREAT | O_TRUNC);
    if(fd < 0)
    {
        perror("[ pcm file open failed ]");
        return;
    }

    int r;
    struct frame frame;
    frame.data = NULL;
    time_t t = time(NULL);
    while(time(NULL) - t < duration) //开始读取音频pcm数据
    {
        r = ak_ai_get_frame(anyka_ai_handle_id , &frame , 0); 
        if(r != 0)
            ak_sleep_ms(10);
        else
        {
            write(fd , frame.data , frame.len);
            ak_ai_release_frame(anyka_ai_handle_id , &frame);
        }
    }

    close(fd);
    return;
}