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
 * @Date         : 2021-07-22 15:10:02
 * @LastEditors  : MCD
 * @LastEditTime : 2021-07-22 15:51:33
 * @FilePath     : /My_ChartRoom/src/Client/TCP_Client.c
 * @Description  : 
 * 
 * ******************************************
 */
#include "common.h"

void *client_func(void *arg)
{
    int new_fd, nbytes;
    char buffer[RECV_BUFF_SIZE] = {0};

    new_fd = *((int *)arg);

    free(arg);

    while (1)
    {
        if((nbytes = recv(new_fd, buffer, RECV_BUFF_SIZE, 0)) == -1)
        {
            fprintf(stderr, "Read error:%s \n", strerror(errno));
            exit(1);
        }
        buffer[nbytes] = '\0';

        print_mcd("received data: %s", buffer);
    }
    
}

int main(int argc, char const *argv[])
{
    int sockfd;
    char buffer[RECV_BUFF_SIZE] = {0};
    struct sockaddr_in server_addr;
    int portnumber;
    pthread_t tid;
    int *pconnsocket = NULL;
    int ret;

    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s, portnumber \a\n", argv[0]);
        exit(1);
    }

    /* port number error exit */
    if((portnumber = atoi(argv[2])) < 0)
    {
        fprintf(stderr, "Usage: %s portnumber\a\n", argv[0]);
        exit(1);
    }

    // 创建套接字
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {   
        fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
        exit(1);
    }

    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnumber);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);       // server ip

    if((connect(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr))) == -1)
    {
        fprintf(stderr,"Connect Error:%s\a\n",strerror(errno));
        exit(1);
    }

    pconnsocket = (int *)malloc(sizeof(int));
    *pconnsocket = sockfd;

    ret =  pthread_create(&tid, NULL, client_func, (void *)pconnsocket);
    if(ret < 0)
    {
        perror("pthread_create err");
        return -1;
    }

    while (1)
    {
        printf("input msg: ");
        scanf("%s", buffer);
        if(send(sockfd, buffer, strlen(buffer), 0) < 0)
        {
            fprintf(stderr,"Write Error:%s\n",strerror(errno));
            exit(1);
        }
    }

    close(sockfd);
    
    exit(0);
}

