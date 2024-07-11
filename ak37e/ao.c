#include "ao.h"

int anyka_ao_handle_id = -1;

//操控Anyka有关音频输出接口，初始化相关内容，用于用户播放音频，若初始化失败，则anyka_ao_handle_id保持为-1
void anyka_ao_init()
{
    anyka_ao_handle_id = -1;

    struct ak_audio_out_param anyka_ao_param;
    anyka_ao_param.pcm_data_attr.sample_bits = AK_AUDIO_SMPLE_BIT_16;
    anyka_ao_param.pcm_data_attr.channel_num = AUDIO_CHANNEL_STEREO;
    anyka_ao_param.pcm_data_attr.sample_rate = AK_AUDIO_SAMPLE_RATE_8000;
    anyka_ao_param.dev_id = DEV_DAC;
    int r = ak_ao_open(&anyka_ao_param, &anyka_ao_handle_id);
    if(r != 0)
    {
        anyka_perror("[ ak_ao_open call failed ]" , r);
        goto ANYKA_AO_INIT_FAILED;
    }

    r = ak_ao_set_gain(anyka_ao_handle_id , 6);
    if(r != 0)
    {
        anyka_perror("[ ak_ao_set_gain call failed ]" , r);
        goto ANYKA_AO_INIT_FAILED;
    }

    return;

ANYKA_AO_INIT_FAILED:
    if(anyka_ao_handle_id >= 0)
        ak_ao_close(anyka_ao_handle_id);
    anyka_ao_handle_id = -1;
}

//调节anyka AO模块的音量
void anyka_ao_set_volume(int db)
{
    if(anyka_ao_handle_id >= 0)
    {
        int r = ak_ao_set_volume(anyka_ao_handle_id , db);
        if(0 != r)
            anyka_perror("[ ak_ao_set_volume call failed ]" , r);
    }
}

//播放WAV文件
void anyka_ao_play_wav(char* wav_file)
{
    if(anyka_ao_handle_id < 0)
    {
        anyka_perror("[ anyka_ao_handle_id is invalid ]" , -1);
        return;
    }

    int fd = open(wav_file , O_RDONLY);
    if(fd < 0)
    {
        perror("[ wav file open failed ]");
        return;
    }

    WAVE_HEADER wav_header;
    if(sizeof(WAVE_HEADER) != read(fd , &wav_header , sizeof(WAVE_HEADER)))
    {
        perror("[ wav file read failed ]");
        goto ANYKA_AO_PLAY_WAV_END_STATE;
    }

    struct ak_audio_out_param anyka_ao_param;
    anyka_ao_param.pcm_data_attr.sample_bits = wav_header.BitsPerSample;
    anyka_ao_param.pcm_data_attr.channel_num = wav_header.NumChannels;
    anyka_ao_param.pcm_data_attr.sample_rate = wav_header.SampleRate;
    int r;
    r = ak_ao_reset_params(anyka_ao_handle_id , &anyka_ao_param);
    if(0 != r && 100663304 != r)
    {
        anyka_perror("[ ak_ao_reset_params call failed ]" , r);
        goto ANYKA_AO_PLAY_WAV_END_STATE;
    }

    int pcm_len = 512;  //默认采样率8000到12000时的传输长度
    if(wav_header.SampleRate >= 16000 && wav_header.SampleRate <= 24000)
        pcm_len = 1024;
    if(wav_header.SampleRate > 24000)
        pcm_len = 2048;
    pcm_len *= wav_header.NumChannels;

    uint8_t pcm_data[8192];
    ssize_t read_len;
    while(1)
    {
        ak_sleep_ms(5);

        read_len = read(fd , pcm_data , pcm_len);
        if(0 == read_len)
            goto ANYKA_AO_PLAY_WAV_END_STATE;
        if(read_len < 0)
        {
            perror("[ wav file read failed ]");
            goto ANYKA_AO_PLAY_WAV_END_STATE;
        }

        r = ak_ao_send_frame(anyka_ao_handle_id , pcm_data , read_len , NULL);
        if(0 != r)
        {
            anyka_perror("[ ak_ao_send_frame call failed ]" , r);
            goto ANYKA_AO_PLAY_WAV_END_STATE;
        }
    }

ANYKA_AO_PLAY_WAV_END_STATE:
    close(fd);
    return;
}