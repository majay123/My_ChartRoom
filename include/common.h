/*
 * 
 * 　　┏┓　　　┏┓+ +
 * 　┏┛┻━━━┛┻┓ + +
 * 　┃　　　　　　　┃ 　
 * 　┃　　　━　　　┃ ++ + + +
 *  ████━████ ┃+
 * 　┃　　　　　　　┃ +
 * 　┃　　　┻　　　┃
 * 　┃　　　　　　　┃ + +
 * 　┗━┓　　　┏━┛
 * 　　　┃　　　┃　　　　　　　　　　　
 * 　　　┃　　　┃ + + + +
 * 　　　┃　　　┃
 * 　　　┃　　　┃ +  神兽保佑
 * 　　　┃　　　┃    代码无bug　　
 * 　　　┃　　　┃　　+　　　　　　　　　
 * 　　　┃　 　　┗━━━┓ + +
 * 　　　┃ 　　　　　　　┣┓
 * 　　　┃ 　　　　　　　┏┛
 * 　　　┗┓┓┏━┳┓┏┛ + + + +
 * 　　　　┃┫┫　┃┫┫
 * 　　　　┗┻┛　┗┻┛+ + + +
 * 
 * 
 * 
 * ************Copyright 2021 MCD************
 * 
 * @version      : 
 * @Company      : HOPE
 * @Author       : MCD
 * @Date         : 2021-07-22 13:43:51
 * @LastEditors  : MCD
 * @LastEditTime : 2021-07-26 12:32:13
 * @FilePath     : /My_ChartRoom/include/common.h
 * @Description  : 
 * 
 * ******************************************
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>



#define PRINT_MCD_ENABLE 1


#if PRINT_MCD_ENABLE == 1
    #define print_mcd(format, arg...)                                                               \
    do                                                                                     			\
    {                                                                                      			\
        char ctime[30] = { 0 };																		\
        char ctime1[30] = { 0 };                                                                    \
        struct tm tm1 = { 0 };																		\
        struct timespec ts; 																		\
        clock_gettime(CLOCK_REALTIME, &ts); 														\
        localtime_r(&ts.tv_sec,&tm1);																\
        strftime(ctime,sizeof(ctime),"%Y-%m-%d %H:%M:%S",&tm1); 									\
        snprintf(ctime1,sizeof(ctime),"%s.%.3ld",ctime,ts.tv_nsec/1000/1000);	                	\
        printf("\033[1;31m[--mcd--][%s]\033[0m:%s,%s,%d--- "format"\n",ctime1, __FILE__,__func__,__LINE__, ##arg);\
    } while (0)
#else
    #define print_mcd(format, arg...)   do {} while (0)
#endif

#define RECV_BUFF_SIZE      (2048)
#define SERVER_PORT         (8879)
#define BUFFER_SIZE         (32)
#define MAX_USER_NUM        (64)
#define GET_BUFFER_SIZE     BUFFER_SIZE * 5

// 在线用户
typedef struct
{
    int fd;                     // -1
    char flage;                 //reagsted or not
    char name[BUFFER_SIZE];
    char passwd[BUFFER_SIZE * 2];
}Online_info_t;

// Client
typedef struct
{
    int sock_fd;
    char login;
}Client_info_t;

typedef int(*cmd_do_func)(int);

typedef struct{
    char cmd;
    cmd_do_func func;
}client_cmd_t;


//C/S通信结构体
typedef struct 
{
    int cmd;
    int state;
    char name[BUFFER_SIZE];
    char data[BUFFER_SIZE * 2];
}protolcol_t;

enum Client_select
{
    S_LOGOUT = 0,
    S_REGISET,
    S_LOGIN,
    S_BROADCAST,
    S_PRIVATE,
    S_ONLINE_LIST,
};

// cmd
#define CMD_BROADCAST       0x00000001
#define CMD_PRIVATE         0x00000002
#define CMD_REGISTER        0x00000004
#define CMD_LOGIN           0x00000008
#define CMD_ONLINEUSER      0x00000010
#define CMD_LOGOUT          0x00000020


// return code
#define OP_OK               0x80000000
#define ONLINEUSER_OK       0x80000001
#define ONLINEUSER_OVER     0x80000002
#define NAME_EXIST          0x80000003
#define NAME_PWD_NMATCH     0x80000004
#define USER_LOGED          0x80000005
#define USER_NOT_REGISTER   0x80000006

#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#define ARRAY_SIZE(A)    (sizeof(A)/sizeof((A)[0]))


#endif // !__COMMON_H__