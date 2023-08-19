#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"
//#include "w5300_config.h"
#include "socket.h"
#include "w5300.h"
//#include "ov2640.h"
//#include "malloc.h"

#define W5300_VIDEO_SOCKET 7

/* 定义输出JPEG图像尺寸 */
#define DEMO_JPEG_OUTPUT_WIDTH 320
#define DEMO_JPEG_OUTPUT_HEIGHT 240

/* 定义JPEG图像缓存空间大小 */
#define DEMO_JPEG_BUF_SIZE (60 * 1024)

uint8_t test_remote_ip[4] = {192, 168, 124, 100};
uint16_t test_remote_port = 5001;
uint16_t test_local_port = 5000;

uint8_t data_buff[1460] = {0};
uint32_t data_len = 0;

uint32_t *jpeg_buf;
uint8_t *p_jpeg_buf;
uint32_t jpeg_len;
uint32_t jpeg_index;

//void eth_video_init(void)
//{
//    my_mem_init(SRAMIN); /* 初始化内部SRAM内存�?? */

//    /* 初始化ATK-MC2640模块 */
//    atk_mc2640_init();
//    atk_mc2640_set_output_format(ATK_MC2640_OUTPUT_FORMAT_JPEG);
//    atk_mc2640_set_output_size(DEMO_JPEG_OUTPUT_WIDTH, DEMO_JPEG_OUTPUT_HEIGHT);
//    atk_mc2640_set_light_mode(ATK_MC2640_LIGHT_MODE_SUNNY);
//    atk_mc2640_set_color_saturation(ATK_MC2640_COLOR_SATURATION_1);
//    atk_mc2640_set_brightness(ATK_MC2640_BRIGHTNESS_1);
//    atk_mc2640_set_contrast(ATK_MC2640_CONTRAST_2);
//    atk_mc2640_set_special_effect(ATK_MC2640_SPECIAL_EFFECT_NORMAL);

//    /* 为JPEG缓存空间申请内存  */
//    jpeg_buf = (uint32_t *)mymalloc(SRAMIN, DEMO_JPEG_BUF_SIZE);
//    
//    p_jpeg_buf = (uint8_t *)jpeg_buf;
//    jpeg_len = DEMO_JPEG_BUF_SIZE / (sizeof(uint32_t));
//    memset((void *)jpeg_buf, 0, DEMO_JPEG_BUF_SIZE);
//}

void eth_video_run(void)
{
    uint16_t len;

    switch (getSn_SSR(W5300_VIDEO_SOCKET))
    {
    case SOCK_UDP:
			  len = getSn_RX_RSR(W5300_VIDEO_SOCKET);
        if (len > 0)
        {
            len = recvfrom(W5300_VIDEO_SOCKET, data_buff, len, test_remote_ip, &test_remote_port);

            printf("%d\r\n", sendto(W5300_VIDEO_SOCKET, data_buff, len, test_remote_ip, test_remote_port));
            printf("%s\r\n", data_buff);
            memset(data_buff, 0, len);
        }


        // memset((void *)jpeg_buf, 0, DEMO_JPEG_BUF_SIZE);

        // /* 获取ATK-MC2640模块输出的一帧JPEG图像数据 */
        // atk_mc2640_get_frame((uint32_t)jpeg_buf, ATK_MC2640_GET_TYPE_DTS_32B_INC, NULL);

        // /* 获取JPEG图像数据的长�??? */
        // while (jpeg_len > 0)
        // {
        //     if (jpeg_buf[jpeg_len - 1] != 0)
        //     {
        //         break;
        //     }
        //     jpeg_len--;
        // }
        // jpeg_len *= sizeof(uint32_t);

        // /* 发�?�JPEG图像数据 */
        // HAL_Delay(1000);
        // printf("sendto: %d\r\n", sendto(W5300_VIDEO_SOCKET, p_jpeg_buf, jpeg_len, remote_ip, remote_port));
        

        break;

    case SOCK_CLOSED:                                         // CLOSED
        close(W5300_VIDEO_SOCKET);                            // close the SOCKET
        socket(W5300_VIDEO_SOCKET, Sn_MR_UDP, test_local_port, 0); // open the SOCKET with UDP mode
        break;

    default:
        break;
    }
}

//void demo_run(void)
//{
//    memset((void *)jpeg_buf, 0, DEMO_JPEG_BUF_SIZE);

//    /* 获取ATK-MC2640模块输出的一帧JPEG图像数据 */
//    atk_mc2640_get_frame((uint32_t)jpeg_buf, ATK_MC2640_GET_TYPE_DTS_32B_INC, NULL);

//    /* 获取JPEG图像数据的长�??? */
//    while (jpeg_len > 0)
//    {
//        if (jpeg_buf[jpeg_len - 1] != 0)
//        {
//            break;
//        }
//        jpeg_len--;
//    }
//    jpeg_len *= sizeof(uint32_t);

//    printf("jpeg_len: %d\r\n", jpeg_len);

//    /* 发�?�JPEG图像数据 */
//    for (jpeg_index = 0; jpeg_index < jpeg_len; jpeg_index++)
//    {
//        USART3->DR = p_jpeg_buf[jpeg_index];
//        while ((USART3->SR & 0x40) == 0)
//            ;
//    }
//}
