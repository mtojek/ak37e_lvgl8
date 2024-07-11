#include "base.h"

//自定义anyka API错误输出函数
void anyka_perror(const char* str , int v)
{
    const char tip[] = "[ the error information str you set is too long in your code ]\n";
    if(strlen(str) > 70)
        fwrite(tip , sizeof(char) , strlen(tip) , stderr);
    else
    {
        char s[120];
        sprintf(s , "%s : return is %d\n" , str , v);
        fwrite(s , sizeof(char) , strlen(s) , stderr);
    }
}

//Anyka SDK初始化
void anyka_sdk_init()
{
	static sdk_run_config config;
    memset(&config, 0, sizeof(config));
    config.mem_trace_flag = SDK_RUN_NORMAL;
    config.isp_tool_server_flag = 0;
    ak_sdk_init(&config);
}