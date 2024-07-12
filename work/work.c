#include "work.h"

//TCP客户端模板，若已经连接，则断开旧连接，创建新连接，若失败，则anyka_client_sockfd保持为-1
int anyka_client_sockfd = -1;
void anyka_client_connect(const char* ip_str , uint16_t port)
{
    if(0 < anyka_client_sockfd)
        close(anyka_client_sockfd);

    anyka_client_sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if(-1 == anyka_client_sockfd)
    {
        perror("[ create client socket failed ]");
        return;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip_str);
    if(0 != connect(anyka_client_sockfd , (const struct sockaddr *)(&addr) , sizeof(addr)))
    {
        close(anyka_client_sockfd);
        anyka_client_sockfd = -1;
        perror("[ client connect failed ]");
    }
}

//TCP服务端模板，若创建失败，anyka_server_sockfd保持为-1
int anyka_server_sockfd = -1;
void anyka_server_create(uint16_t port , int backlog)
{
    anyka_server_sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if(-1 == anyka_server_sockfd)
    {
        perror("[ create server socket failed ]");
        goto ANYKA_SERVER_CREATE_FAILED;
    }
    
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(-1 == bind(anyka_server_sockfd , (const struct sockaddr*)(&addr) , sizeof(addr)))
    {
        perror("[ server socket bind failed ]");
        goto ANYKA_SERVER_CREATE_FAILED;
    }

    if(-1 == listen(anyka_server_sockfd , backlog))
    {
        perror("[ server socket listen failed ]");
        goto ANYKA_SERVER_CREATE_FAILED;
    }

ANYKA_SERVER_CREATE_FAILED:
	if(0 < anyka_server_sockfd)
		close(anyka_server_sockfd);
	anyka_server_sockfd = -1;
}

void work()
{
    /* 此处添加你的工作代码 */
}