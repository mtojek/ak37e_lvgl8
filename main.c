#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <string.h>

#define BITS_PER_PIXEL              24
#define LEN_COLOR                   8

enum color_offset {
        OFFSET_RED = 16,
        OFFSET_GREEN = 8,
        OFFSET_BLUE = 0,
};

#define DUAL_FB_FIX                 finfo.reserved[ 0 ]
#define DUAL_FB_VAR                 vinfo.reserved[ 0 ]

#include "ak37e/base.h"
#include "ak37e/include/ak_mem.h"
#include "ak37e/include/ak_common_graphics.h"
#include "ak37e/include/ak_vo.h"
#include "ak37e/include/ak_tde.h"

static struct ak_tde_layer tde_layer_screen = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , GP_FORMAT_RGB888 } ;
static struct ak_tde_layer tde_layer_tgt = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , GP_FORMAT_RGB888 } ;
static struct ak_tde_layer tde_layer_bg = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , GP_FORMAT_RGB888 } ;
static struct ak_tde_cmd tde_cmd_param; 
												 
unsigned char * p_vaddr_bg = NULL;

int main() {
    sdk_run_config config= {0};

    config.mem_trace_flag = SDK_RUN_NORMAL;
    ak_sdk_init( &config );

    int r = ak_tde_open();
    if (r != ERROR_TYPE_NO_ERROR) {
        perror("ak_tde_open failed");
	return 1;
    }

    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
    int fbfd = open("/dev/fb0", O_RDWR);

    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return 1;
    }

    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        close(fbfd);
        return 1;
    }

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        close(fbfd);
        return 1;
    }


    if ( DUAL_FB_FIX == AK_TRUE )  {
	    printf("dual fb mode\n");
    }

    extern void *osal_fb_mmap_viraddr(int fb_len, int fb_fd);
    extern int osal_fb_munmap_viraddr(void *addr, int fb_len);

    void *fbp = osal_fb_mmap_viraddr(finfo.smem_len, fbfd);
    if (fbp == MAP_FAILED) {
        perror("Failed to mmap framebuffer");
        close(fbfd);
        return 1;
    }

    printf("vinfo.xres_virtual = %d, vinfo.yres_virtual = %d\n", vinfo.xres_virtual, vinfo.yres_virtual);

    vinfo.activate |= FB_ACTIVATE_FORCE;
    vinfo.activate |= FB_ACTIVATE_NOW;
    vinfo.xres = vinfo.xres_virtual;
    vinfo.yres = vinfo.yres_virtual;
    vinfo.bits_per_pixel = BITS_PER_PIXEL;
    vinfo.red.offset = OFFSET_RED;
    vinfo.red.length = LEN_COLOR;
    vinfo.green.offset = OFFSET_GREEN;
    vinfo.green.length = LEN_COLOR;
    vinfo.blue.offset = OFFSET_BLUE;
    vinfo.blue.length = LEN_COLOR;
    
    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
        perror("Failed to put framebuffer");
        close(fbfd);
        return 1;
    }

    // Clear screen (black)
    memset(fbp, 0, finfo.smem_len);

    tde_layer_screen.width      = vinfo.xres;
    tde_layer_screen.height     = vinfo.yres;
    tde_layer_screen.pos_left   = 0;
    tde_layer_screen.pos_top    = 0;
    tde_layer_screen.pos_width  = vinfo.xres;
    tde_layer_screen.pos_height = vinfo.yres;
    tde_layer_screen.phyaddr    = finfo.smem_start;
    tde_layer_screen.format_param = GP_FORMAT_RGB888;

    if ( DUAL_FB_FIX == AK_TRUE )  {
        DUAL_FB_VAR ^= 1;
        tde_layer_screen.phyaddr = finfo.smem_start + DUAL_FB_VAR * tde_layer_screen.width * tde_layer_screen.height * 3;
    }

    tde_layer_bg.width = vinfo.xres;
    tde_layer_bg.height = vinfo.yres;
    tde_layer_bg.pos_left = 0;
    tde_layer_bg.pos_top = 0;
    tde_layer_bg.pos_width = vinfo.xres;
    tde_layer_bg.pos_height = vinfo.yres;
    tde_layer_bg.format_param = tde_layer_screen.format_param;

    p_vaddr_bg = ak_mem_dma_alloc(1, tde_layer_screen.width * tde_layer_screen.height * 3);
    memset(p_vaddr_bg, 0, tde_layer_screen.width * tde_layer_screen.height * 3);
    ak_mem_dma_vaddr2paddr( p_vaddr_bg , ( unsigned long * )&tde_layer_bg.phyaddr );

    printf("finfo.smem_start = %lu\n", finfo.smem_start);
    printf("tde_layer_screen.phyaddr = %lu\n", tde_layer_screen.phyaddr);

    // Draw a red rectangle
    long x, y;

    printf("vinfo.yres: %d\n", vinfo.yres);
    printf("vinfo.xres: %d\n", vinfo.xres);
    printf("finfo.line_length: %d\n", finfo.line_length);
    printf("vinfo_bits_per_pixel: %d\n", vinfo.bits_per_pixel);

    for (y = 0; y < vinfo.yres; y++) {
    for (x = 0; x < vinfo.xres; x++) {
        char *location = ((char *) p_vaddr_bg) + (y * finfo.line_length) + (x * 3);  // Ensure correct pointer arithmetic

        if (y <= vinfo.yres / 7 || x <= vinfo.xres / 7 || y >= (6 * vinfo.yres / 7) || x >= (6 * vinfo.xres / 7)) {
            *(location) = 0;     // Red
            *(location + 1) = 0;
            *(location + 2) = 254;
        } else {
            *(location) = 254;
            *(location + 1) = 0; // Green
            *(location + 2) = 0;
        }

    }
    }

    r = ak_tde_opt_scale(&tde_layer_bg, &tde_layer_screen);
    if (r != ERROR_TYPE_NO_ERROR) {
        perror("ak_tde_opt_scale failed");
        return 1;
    }

    /*tde_layer_tgt.width = vinfo.xres;
    tde_layer_tgt.height = vinfo.yres;
    tde_layer_tgt.phyaddr = tde_layer_screen.phyaddr;
    tde_layer_tgt.format_param = tde_layer_screen.format_param;

    memset( &tde_cmd_param , 0 , sizeof( struct ak_tde_cmd ) );
    tde_cmd_param.opt = tde_cmd_param.opt | GP_OPT_ROTATE ;
    tde_cmd_param.rotate_param = 0;
    tde_cmd_param.tde_layer_src = tde_layer_bg;
    tde_cmd_param.tde_layer_dst = tde_layer_tgt;*/

    //r = ak_tde_opt_scale(&tde_layer_bg, &tde_layer_screen);
    /*r = ak_tde_opt(&tde_cmd_param);
    if (r != ERROR_TYPE_NO_ERROR) {
        perror("ak_tde_opt failed");
        return 1;                                         
    }*/
    if ( DUAL_FB_FIX == AK_TRUE )  {                                            //如果使用双buffer的话则调用ioctl
										//
        if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
         perror("Failed to put framebuffer 2");
         close(fbfd);
         return 1;
        }	

    }
    sleep(1);

    ak_mem_dma_free(p_vaddr_bg);

    osal_fb_munmap_viraddr(fbp, finfo.smem_len);
    close(fbfd);
    ak_tde_close();
    ak_sdk_exit();

    return 0;
}

