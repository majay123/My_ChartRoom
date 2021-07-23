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
 * @LastEditTime : 2021-07-23 14:50:49
 * @FilePath     : /My_ChartRoom/src/Server/TCP_Server.c
 * @Description  : 
 * 
 * ******************************************
 */

#include "common.h"

Online_info_t online_info[MAX_USER_NUM];
int sockfd;
int addrlen;
struct sockaddr_in server_addr;
pthread_t tid;

/**
* @author  		MCD
* @date  		2021-07-23-09:44
* @details		del user online
*/
void del_user_online(int index)
{
    int i;
    char buf[GET_BUFFER_SIZE] = {0};
    
    if(index < 0)
        return;
    
    online_info[index].fd = -1;
    sprintf(buf, "%s offline\n", online_info[index].name);
    // 通知所有用户客户端 某个客户端下线了
    for(i = 0; i < MAX_USER_NUM; i++)
    {
        if(online_info[i].fd < 0)
            continue;
        write(online_info[i].fd, buf, strlen(buf));
    }

    return;
}

/**
* @author  		MCD
* @date  		2021-07-23-09:44
* @details		add user
*/
int add_user(int fd, protolcol_t *msg)
{
    int i;
    int index = -1;
    // char buf[GET_BUFFER_SIZE] = {0};

    for(i = 0; i < MAX_USER_NUM; i++)
    {
        if(online_info[i].flage < 0)
        {
            online_info[i].flage = 1;
            snprintf(online_info[i].name, BUFFER_SIZE, "%s", msg->name);
            snprintf(online_info[i].passwd, BUFFER_SIZE * 2, "%s", msg->data);

            print_mcd("regist %s to %d", msg->name, i);
            index = i;
            return index;
        }
    }

    return index;
}

/**
* @author  		MCD
* @date  		2021-07-23-09:52
* @details		broadcast msg
*/
void broadcast_msg(int index, protolcol_t *msg)
{
    // int i;

    print_mcd("client %s, %d", online_info[index].name, index);
    print_mcd("data: %s", msg->data);
    
}


/**
* @author  		MCD
* @date  		2021-07-23-09:53
* @details		find dest user online
*/
int find_dest_user_online(int fd, int *index, protolcol_t *msg)
{
    int i;

    for ( i = 0; i < MAX_USER_NUM; i++)
    {
        if(online_info[i].flage < 0)
            continue;
        
        if((strcmp(msg->name, online_info[i].name) == 0) && (strcmp(msg->data, online_info[i].passwd) == 0))
        {
            if(online_info[i].fd < 0)
            {
                online_info[i].fd = fd;
                *index = i;
                return OP_OK;
            }
            else
            {
                print_mcd("%s had login", online_info[i].name);
                return USER_LOGED;
            }
        }
    }
    return NAME_PWD_NMATCH;
}

/**
* @author  		MCD
* @date  		2021-07-23-09:58
* @details		find dest user
*/
int find_dest_user(char *name)
{
    int i;

    for ( i = 0; i < MAX_USER_NUM; i++)
    {
        if(online_info[i].flage < 0)
            continue;
        
        if(strcmp(name, online_info[i].name) == 0)
            return i;
    }

    return -1;
    
}

void private_msg(int index, protolcol_t *msg)
{

}
void list_online_user(int index)
{

}

/**
* @author  		MCD
* @date  		2021-07-23-10:01
* @details		registe
*/
int registe(int fd, int *index, protolcol_t *msg)
{
    int dest_index;
    // char buf[GET_BUFFER_SIZE] = {0};
    protolcol_t reply;

    memset(&reply, 0, sizeof(protolcol_t));
    reply.cmd = CMD_REGISTER;
    dest_index = find_dest_user(msg->name);
    if(dest_index < 0)
    {
        *index = add_user(fd, msg);
        online_info[*index].flage = 1;
        reply.state = OP_OK;

        print_mcd("user %s regist success!", msg->name);
        write(fd, &reply, sizeof(protolcol_t));
        return 0;
    }
    else
    {
        reply.state = NAME_EXIST;
        print_mcd("user %s exist!", msg->name);
        write(fd, &reply, sizeof(protolcol_t));
        return -1;
    }
}

/**
* @author  		MCD
* @date  		2021-07-23-10:13
* @details		login
*/
void login(int fd, int *index, protolcol_t *msg)
{
    int i;
    int ret;
    char buf[GET_BUFFER_SIZE] = {0};
    protolcol_t reply;

    memset(&reply, 0, sizeof(protolcol_t));
    reply.cmd = CMD_LOGIN;
    ret = find_dest_user_online(fd, index, msg);
    if(ret != OP_OK)
    {
        reply.state = ret;
        // strcpy(reply.data, "there is no this user\n");
        snprintf(reply.data, BUFFER_SIZE * 2, "%s", "there is no this user\n");
        print_mcd("user %s login failed!", msg->name);
        write(fd, &reply, sizeof(protolcol_t));
        return;
    }
    else
    {
        reply.state = OP_OK;
        snprintf(reply.data, BUFFER_SIZE * 2, "%s", "login success!\n");
        print_mcd("user %s login success! index = %d %d ", msg->name, *index, fd);
        write(fd, &reply, sizeof(protolcol_t));
        // return;
    }

    //通知所有就客户端 某个用户登陆了
    snprintf(buf, GET_BUFFER_SIZE, "%s online", online_info[*index].name);
    for ( i = 0; i < MAX_USER_NUM; i++)
    {
        if(online_info[i].fd < 0)
            continue;
        print_mcd("fd = %d, %s", online_info[i].fd, buf);
        int len = write(online_info[i].fd, buf, strlen(buf));
        print_mcd("len = %d", len);
    }
    
}

/**
* @author  		MCD
* @date  		2021-07-22-15:02
* @details		rec_func
*/
void *rec_func(void *arg)
{
    int new_fd, nbytes;
    // char buffer[RECV_BUFF_SIZE] = {0};
    int index = -1;
    protolcol_t msg;

    memset(&msg, 0, sizeof(protolcol_t));
    new_fd = *((int *) arg);
    free(arg);

    // 进入聊天
    while(1)
    {
        if(new_fd > 0)
        {
            nbytes = read(new_fd, &msg, sizeof(protolcol_t));
            if(nbytes <= 0)
            {
                //offline
                
                print_mcd("index = %d, %s offline", index, online_info[index].name);
                //delete user
                del_user_online(index);
                close(new_fd);
                return NULL;
            }
            switch (msg.cmd)
            {
            case CMD_REGISTER:
                registe(new_fd, &index, &msg);
                break;
            case CMD_LOGIN:
                login(new_fd, &index, &msg);
                break;
            case CMD_BROADCAST:
                broadcast_msg(index, &msg);
                break;
            case CMD_PRIVATE:
                private_msg(index, &msg);
                break;
            case CMD_ONLINEUSER:
                list_online_user(index);
                break;
            default:
                break;
            }
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
    // char buf[BUFFER_SIZE * 2] = "this is mcd's chartroom\n";
    pthread_t tid;
    int *pconnsocket = NULL;
    int ret;
    int i = 0;

    if(argc < 2)
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
    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "Socket error: %s\a\n", strerror(errno));
        exit(1);
    }

    // 服务器端填充sockaddr结构
    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = PF_INET;                       // 自动填充主机IP
    server_addr.sin_port = htons(portnumber);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);        // 自动获取网卡地址

    // 绑定sockfd套接字
    if((bind(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr))) < 0)
    {
        fprintf(stderr, "Bind error: %s\a\n", strerror(errno));
        exit(1);
    }

    // 监听sockfd套接字
    // listen for connections
    // the second parameter marks max number of clients
    if(listen(sockfd, 100) < 0)
    {
        fprintf(stderr, "Listen error: %s\a\n", strerror(errno));
        exit(1);
    }
    for ( i = 0; i < MAX_USER_NUM; i++)
    {
        online_info[i].fd = -1;
        online_info[i].flage = -1;
    }
    
    while(1)
    {
        socklen_t sin_size = sizeof(struct sockaddr_in);
        if((new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size)) < 0)
        {
            fprintf(stderr, "Accept error: %s\a\n", strerror(errno));
            exit(1);
        }
        // print_mcd("client ip = %s, port = %d", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
        pconnsocket = (int *)malloc(sizeof(int));
        *pconnsocket = new_fd;

        ret = pthread_create(&tid, NULL, rec_func, (void *)pconnsocket);
        if(ret < 0)
        {
            perror("pthread create error!");
            if(pconnsocket != NULL)
            {
                free(pconnsocket);
                pconnsocket = NULL;
                exit(1);
            }
        
        }
        
    }
    exit(0);   
}
