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
 * @LastEditTime : 2021-07-23 15:08:35
 * @FilePath     : /My_ChartRoom/src/Client/TCP_Client.c
 * @Description  : 
 * 
 * ******************************************
 */
#include "common.h"

int sockfd;
int addrlen;
struct sockaddr_in server_addr;
pthread_t tid;
int login_f = -1;

/**
* @author  		MCD
* @date  		2021-07-22-17:03
* @details		client func
*/
void *client_func(void *arg)
{
    int len;
    char buf[128] = {0};
    protolcol_t *msg;

    while (1)
    {
        if(login_f != 1)
            continue;
        memset(buf, 0, sizeof(buf));
        // print_mcd("login flag = %d", login_f);
        len = read(sockfd, buf, sizeof(buf));
        // print_mcd("read len = %d", len);
        if(len <= 0)
        {
            close(sockfd);
            return NULL;
        }
        msg = (protolcol_t *)buf;
        if((msg->state == ONLINEUSER_OK) && (msg->cmd == CMD_ONLINEUSER))
        {
            print_mcd("msg name = %s", msg->name);
            continue;
        }
        if((msg->state == ONLINEUSER_OVER) && (msg->cmd == CMD_ONLINEUSER))
        {
            print_mcd("online user over!");
            continue;
        }

        // buf[len] = '\0';
        print_mcd("msg data = %s", buf);
    }
    
}

/**
* @author  		MCD
* @date  		2021-07-22-17:11
* @details		broadcast
*/
void broadcast_msg(int fd)
{
    protolcol_t msg;

    memset(&msg, 0, sizeof(protolcol_t));
    msg.cmd = CMD_BROADCAST;
    printf("say: \n#");
    scanf("%s", msg.data);

    write(fd, &msg, sizeof(protolcol_t));
}

/**
* @author  		MCD
* @date  		2021-07-22-17:13
* @details		private
*/
void private_msg(int fd)
{
    protolcol_t msg;
    
    memset(&msg, 0, sizeof(protolcol_t));
    msg.cmd = CMD_PRIVATE;
    printf("input name who you want to talk: \n#");
    scanf("%s", msg.name);

    printf("say: \n#");
    scanf("%s", msg.data);

    write(fd, &msg, sizeof(protolcol_t));
}

void list_online_user(int fd)
{
    protolcol_t msg;

    memset(&msg, 0, sizeof(protolcol_t));
    msg.cmd = CMD_ONLINEUSER;
    write(fd, &msg, sizeof(protolcol_t));

    getchar();
    getchar();
    
}

/**
* @author  		MCD
* @date  		2021-07-22-17:19
* @details		registe
*/
int registe(int fd)
{
    protolcol_t msg, reply;
    ssize_t len = 0;

    memset(&msg, 0, sizeof(protolcol_t));
    memset(&reply, 0, sizeof(protolcol_t));
    msg.cmd = CMD_REGISTER;
    print_mcd("input your name");
    scanf("%s", msg.name);
    print_mcd("input you passwd");
    scanf("%s", msg.data);
    
    // print_mcd("regist name = %s, passwd = %s", msg.name, msg.data);
    print_mcd("fd = %d", fd);
    len = write(fd, &msg, sizeof(protolcol_t));
    print_mcd("write len = %d", len);
    read(fd, &reply, sizeof(protolcol_t));

    if(reply.state != OP_OK)
    {
        print_mcd("Name had exist, try again!");
        getchar();
        getchar();
        return -1;
    }
    else
    {
        print_mcd("Regist success!");
        getchar();
        getchar();
        return 0;
    }
}


/**
* @author  		MCD
* @date  		2021-07-23-08:40
* @details		login
*/
int login(int fd)
{
    protolcol_t msg, reply;

    msg.cmd = CMD_LOGIN;
    print_mcd("input your name");
    scanf("%s", msg.name);
    print_mcd("input your passwd");
    scanf("%s", msg.data);
    write(fd, &msg, sizeof(protolcol_t));
    read(fd, &reply, sizeof(protolcol_t));
    
    if(reply.state != OP_OK)
    {
        print_mcd("Account had login, try other account !");
        getchar();
        getchar();
        login_f = -1;
        return NAME_PWD_NMATCH;
    }
    else
    {
        print_mcd("Login success");
        getchar();
        getchar();
        login_f = 1;
        return OP_OK;
    }
    
}

/**
* @author  		MCD
* @date  		2021-07-23-08:47
* @details		logout
*/
void logout(int fd)
{
    // protolcol_t msg;
    close(fd);
    login_f = -1;
}


/**
* @author  		MCD
* @date  		2021-07-22-17:16
* @details		main
*/
int main(int argc, char const *argv[])
{
     int sel;
    //  int ret;
     int min_sel, max_sel;
     int protnumber;

    //  protolcol_t msg;

     if(argc < 3)
     {
        printf("cmd: %s ip portnumber\n",argv[0]);
        return -1;
     }

     //argv 2 参数为端口号 
     if((protnumber = atoi(argv[2])) < 0)
     {
        fprintf(stderr,"Usage:%s hostname portnumber\a\n",argv[0]);
        exit(1);
     }

     sockfd = socket(PF_INET, SOCK_STREAM, 0);
     print_mcd("socket fd = %d",  sockfd);
     if(sockfd < 0)
     {
         perror("socket failed!\n");
         return -1;
     }

     server_addr.sin_addr.s_addr = inet_addr(argv[1]);
     server_addr.sin_family = PF_INET;
     server_addr.sin_port = htons(protnumber);

     addrlen = sizeof(struct sockaddr_in);

     connect(sockfd, (struct sockaddr *)&server_addr, addrlen);
     pthread_create(&tid, NULL, client_func, NULL);
     while (1)
     {
        // system("clear");
        if(login_f == -1)
        {
            printf("\t 1 注册 \n");
            printf("\t 2 登陆 \n");
        }
        else if(login_f == 1)
        {
            printf("\t 3 公聊 \n");
            printf("\t 4 私聊 \n");
            printf("\t 5 在线列表 \n");
        }
        printf("\t 0 退出 \n");
        
        fflush(stdin);
        scanf("%d", &sel);
        if(sel == 0)    
            goto Exit;

        if(login_f == 1)
        {
            min_sel = 3;
            max_sel = 5;
        }
        else if (login_f == -1)
        {
            min_sel = 1;
            max_sel = 2;
        }

        if(sel < min_sel || sel > max_sel)
        {
            print_mcd("Vaild choice, try again!");
            continue;
        }

        switch (sel)
        {
        case 1:
            registe(sockfd);
            break;
        case 2:
            login(sockfd);
            break;
        case 3:
            broadcast_msg(sockfd);
            break;
        case 4:
            private_msg(sockfd);
            break;
        case 5:
            list_online_user(sockfd);
            break;
        case 0:
            logout(sockfd);
            break;
        
        default:
            break;
        }
     }       
Exit:
    exit(0);
     
}

