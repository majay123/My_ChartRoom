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
 * @Date         : 2021-07-22 14:13:31
 * @LastEditors  : MCD
 * @LastEditTime : 2021-07-22 15:48:45
 * @FilePath     : /My_ChartRoom/src/Server/TCP_Server.c
 * @Description  : 
 * 
 * ******************************************
 */

#include "common.h"


/**
* @author  		MCD
* @date  		2021-07-22-15:02
* @details		rec_func
*/
void *rec_func(void *arg)
{
    int new_fd, nbytes;
    char buffer[RECV_BUFF_SIZE] = {0};

    new_fd = *((int *) arg);
    free(arg);

    while(1)
    {
        if((nbytes = recv(new_fd, buffer, RECV_BUFF_SIZE, 0)) == -1)
        {
            fprintf(stderr, "Read error:%s \n", strerror(errno));
            exit(1);
        }

        if(nbytes <= 0)
        {
            print_mcd("recv null buffer");
            close(new_fd);
            break;
        }

        buffer[nbytes] = '\0';
        print_mcd("received data: %s", buffer);
        
        if(send(new_fd, buffer, strlen(buffer), 0) == -1)
        {
            fprintf(stderr, "Send error:%s \n", strerror(errno));\
            exit(1);
        }
    }
}


/**
* @author  		MCD
* @date  		2021-07-22-15:02
* @details		main
*/
int main(int argc, char const *argv[])
{
    // char buff[RECV_BUFF_SIZE] = {0};
    int sockfd, new_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int portnumber;
    // char hello[] = "Hello! Socket communication workd\n";
    pthread_t tid;
    int *pconnsocket = NULL;
    int ret;

    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s, portnumber \a\n", argv[0]);
        exit(1);
    }

    /* port number error exit */
    if((portnumber = atoi(argv[1])) < 0)
    {
        fprintf(stderr, "Usage: %s portnumber\a\n", argv[0]);
        exit(1);
    }
    print_mcd("set port number = %d", portnumber);
    // 服务器端开始建立socket描述符，sockfd用于监听
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Socket error: %s\a\n", strerror(errno));
        exit(1);
    }

    // 服务器端填充sockaddr结构
    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;                       // 自动填充主机IP
    server_addr.sin_port = htons(portnumber);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);        // 自动获取网卡地址

    // 绑定sockfd套接字
    if(bind(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)))
    {
        fprintf(stderr, "Bind error: %s\a\n", strerror(errno));
        exit(1);
    }

    // 监听sockfd套接字
    // listen for connections
    // the second parameter marks max number of clients
    if(listen(sockfd, 100) == -1)
    {
        fprintf(stderr, "Listen error: %s\a\n", strerror(errno));
        exit(1);
    }

    while(1)
    {
        socklen_t sin_size = sizeof(struct sockaddr_in);
        if((new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size)) == -1)
        {
            fprintf(stderr, "Accept error: %s\a\n", strerror(errno));
            exit(1);
        }

        pconnsocket = (int *)malloc(sizeof(int));
        *pconnsocket = new_fd;

        ret = pthread_create(&tid, NULL, rec_func, (void *)pconnsocket);
        if(ret < 0)
        {
            perror("pthread create error!");
            return -1;
        }
        
    }

    exit(0);   
}
