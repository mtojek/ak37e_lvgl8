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
    //例程
    /*Init the style for the default state*/
    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_radius(&style, 3);

    lv_style_set_bg_opa(&style, LV_OPA_100);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_color(&style, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);

    lv_style_set_border_opa(&style, LV_OPA_40);
    lv_style_set_border_width(&style, 2);
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_GREY));

    lv_style_set_shadow_width(&style, 8);
    lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_ofs_y(&style, 8);

    lv_style_set_outline_opa(&style, LV_OPA_COVER);
    lv_style_set_outline_color(&style, lv_palette_main(LV_PALETTE_BLUE));

    lv_style_set_text_color(&style, lv_color_white());
    lv_style_set_pad_all(&style, 10);

    /*Init the pressed style*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);

    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&style_pr, 30);
    lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);

    lv_style_set_translate_y(&style_pr, 5);
    lv_style_set_shadow_ofs_y(&style_pr, 3);
    lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 4));

    /*Add a transition to the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);

    lv_style_set_transition(&style_pr, &trans);

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
    lv_obj_remove_style_all(btn1);                          /*Remove the style coming from the theme*/
    lv_obj_add_style(btn1, &style, 0);
    lv_obj_add_style(btn1, &style_pr, LV_STATE_PRESSED);
    lv_obj_set_size(btn1, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(btn1);

    lv_obj_t * label = lv_label_create(btn1);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);
}