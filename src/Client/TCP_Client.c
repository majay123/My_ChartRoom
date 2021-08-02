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
 * @LastEditTime : 2021-08-02 16:25:25
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
* @date  		2021-08-02-10:27
* @details		send msg
*/
static int send_simple_msg(int fd, int cmd, int state)
{
    protolcol_t msg;

    memset(&msg, 0, sizeof(protolcol_t));
    msg.cmd = cmd;
    msg.state = state;

    return write(fd, &msg, sizeof(protolcol_t));
}

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

    while (sockfd > 0)
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
        if(msg->cmd == CMD_LOGOUT)
            return NULL;

        // buf[len] = '\0';
        print_mcd("msg data = %s", buf);
    }
    print_mcd("pthread exit");

    return NULL;
}

/**
* @author  		MCD
* @date  		2021-07-22-17:11
* @details		broadcast
*/
static int broadcast_msg(int fd)
{
    protolcol_t msg;
    size_t len;

    memset(&msg, 0, sizeof(protolcol_t));
    msg.cmd = CMD_BROADCAST;
    printf("say: \n#");
    scanf("%s", msg.data);

    len = write(fd, &msg, sizeof(protolcol_t));
    if(len < 0)
        return -1;
    return 0;
}

/**
* @author  		MCD
* @date  		2021-07-22-17:13
* @details		private
*/
static int private_msg(int fd)
{
    protolcol_t msg;
    size_t len = 0;
    
    memset(&msg, 0, sizeof(protolcol_t));
    msg.cmd = CMD_PRIVATE;
    printf("input name who you want to talk: \n#");
    scanf("%s", msg.name);

    printf("say: \n#");
    scanf("%s", msg.data);

    len = write(fd, &msg, sizeof(protolcol_t));
    if(len < 0)
        return -1;
    return 0;
    
}

static int list_online_user(int fd)
{
    protolcol_t msg;
    size_t len = 0;

    memset(&msg, 0, sizeof(protolcol_t));
    msg.cmd = CMD_ONLINEUSER;
    len = write(fd, &msg, sizeof(protolcol_t));

    getchar();
    getchar();

    if(len < 0)
        return -1;
    return 0;
    
}

/**
* @author  		MCD
* @date  		2021-07-22-17:19
* @details		registe
*/
static int registe(int fd)
{
    protolcol_t msg, reply;
    // ssize_t len = 0;

    memset(&msg, 0, sizeof(protolcol_t));
    memset(&reply, 0, sizeof(protolcol_t));
    msg.cmd = CMD_REGISTER;
    print_mcd("input your name");
    scanf("%s", msg.name);
    print_mcd("input you passwd");
    scanf("%s", msg.data);
    
    // print_mcd("regist name = %s, passwd = %s", msg.name, msg.data);
    // print_mcd("fd = %d", fd);
    write(fd, &msg, sizeof(protolcol_t));
    // print_mcd("write len = %d", len);
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
static int login(int fd)
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
static int logout(int fd)
{
    protolcol_t msg;
    size_t len = 0;
    
    size_t size =  sizeof(protolcol_t);
    memset(&msg, 0, size);
    msg.cmd = CMD_LOGOUT;
    len = write(fd, &msg, size);
    getchar();
    // getchar();

    close(fd);
    login_f = -1;

    if(len < 0)
        return -1;
    return 0;
}

/**
* @author  		MCD
* @date  		2021-07-27-10:24
* @details		send file
*/
// #define SIZE
static int send_files(int fd)
{
    protolcol_t msg;
    // char file_name[PATH_SIZE] = {0};
    files_info_t file_info;
    int ret = 0;
    int cnt = 0;

    memset(&msg, 0, sizeof(protolcol_t));
    memset(&file_info, 0, sizeof(files_info_t));
    printf("\n请输入你需要传送的文件名,带扩展名\n");
    scanf("%s", file_info.file_name);

    FILE *fp = NULL;
    // int total;
    int file_block_length;
    int count = 0;
    int send_sum = 0;
    int current = 0;
    int progress = 50;
    char sendBuf[DATA_SIZE] = {0};
    
    fp = fopen(file_info.file_name, "rb");
    if (fp == NULL)
    {
        print_mcd("there is no file");
        return -1;
    }
    // 将文件光标偏移到文件尾部
    fseek(fp, 0, SEEK_END);
    file_info.files_size = ftell(fp);
    print_mcd("file %s total size is %ld bytes", file_info.file_name, file_info.files_size);

    msg.cmd = CMD_SENDFILES_INFO;
    msg.state = OP_OK;
    memcpy(msg.data, &file_info, sizeof(files_info_t));
    ret = write(fd, &msg, sizeof(protolcol_t));
    if(ret < 0)
    {
        print_mcd("send msg failed");
        goto Error;
    }
    memset(&msg, 0, sizeof(protolcol_t));
    read(fd, &msg, sizeof(protolcol_t));
    if((msg.cmd != CMD_SENDFILES_INFO) || (msg.state != OP_OK))
    {
        print_mcd("server back error");
        goto Error;
    }

    memset(&msg, 0, sizeof(protolcol_t));
    msg.cmd = CMD_SENDFILES_DATA;
    msg.state = OP_OK;
    memcpy(msg.data, &file_info, sizeof(files_info_t));
    ret = write(fd, &msg, sizeof(protolcol_t));
    if(ret < 0)
    {
        print_mcd("send msg failed");
        goto Error;
    }
    memset(&msg, 0, sizeof(protolcol_t));
    read(fd, &msg, sizeof(protolcol_t));
    if((msg.cmd != CMD_SENDFILES_DATA) || (msg.state != OP_OK))
    {
        print_mcd("server back error");
        goto Error;
    }

    print_mcd("ready to send file data");
    cnt = file_info.files_size / DATA_SIZE;
    if(file_info.files_size % DATA_SIZE)
    {
        // lenlast = file_info.files_size - (cnt * DATA_SIZE);
        cnt += 1;
    }
    print_mcd("\n\n 文件长度为%ld字节，\n每次传送%d字节，\n需要分%d次传送", file_info.files_size, DATA_SIZE, cnt);

    rewind(fp);

    sleep(1);
    while(!feof(fp))
    {
        file_block_length = fread(sendBuf, sizeof(char), DATA_SIZE, fp);
        send_sum += file_block_length;
        current = send_sum / (file_info.files_size / progress);
        printf("\r");                                 //光标回车
		printf("[");
		for (int i = 0; i < progress; i++) {
			putchar(i < current ? '=' : '+');
		}
		printf("]");
        printf(" %8dKB/%ldKB %6.2f%%", send_sum, file_info.files_size, (float)send_sum / file_info.files_size * 100);
        if(file_block_length < DATA_SIZE)
        {
            int rest_length = file_info.files_size - (count * DATA_SIZE);
            if(write(fd, sendBuf, rest_length) < 0)
            {
                print_mcd("send file:%s failed", file_info.file_name);
                ret = -1;
                break;
            }
        }
        else if(write(fd, sendBuf, DATA_SIZE) < 0)
        {
            print_mcd("send file %s failed", file_info.file_name);
            ret = -1;
            break;
        }
        memset(sendBuf, 0, sizeof(sendBuf));
        count++;
    }
    print_mcd("\n 程序传送了%d次", count);

Error:
    if(fp != NULL)
    {
        fclose(fp);
        fp = NULL;
    }
    return ret;
}

/**
* @author  		MCD
* @date  		2021-07-27-10:24
* @details		recv file
*/
static int recv_files(int fd)
{
    protolcol_t *msg;
    protolcol_t reply;
    int ret;
    FILE *fp;
    // long int file_size = 0;     //files size
    int recv_sum = 0;           //当前接收文件长度
    int current = 0;            //进度
    int progress = 50;          //统计接收大小
    // char sendBuf[DATA_SIZE] = {0};
    char recvBuf[DATA_SIZE] = {0};
    char fileName[PATH_SIZE] = {0};
    files_info_t file_info;

    // memset(recvBuf, 0, sizeof(recvBuf));
    memset(recvBuf, 0, sizeof(recvBuf));
    memset(&reply, 0, sizeof(reply));
    ret = read(fd, recvBuf, sizeof(recvBuf));
    if(ret < 0)
        return -1;
    msg = (protolcol_t *)recvBuf;
    if((msg->cmd == CMD_RECVFILES_INFO) && (msg->state == OP_OK))
    {
        memset(&file_info, 0, sizeof(files_info_t));
        memcpy(&file_info, msg->data, sizeof(files_info_t));
        print_mcd("file size = %ld, file_name = %s", file_info.files_size, file_info.file_name);
        reply.cmd = CMD_RECVFILES_INFO;
        reply.state = OP_OK;
        write(fd, &reply, sizeof(protolcol_t));
    }
    else
    {
        print_mcd("can not get recv file info");
        return -2;
    }

    print_mcd("请输入你希望保存的文件名（带扩展名）");
    scanf("%s", fileName);
    fp = fopen(fileName, "wb");
    if(fp == NULL)
        return -2;

    memset(recvBuf, 0, sizeof(recvBuf));
    ret = read(fd, recvBuf, sizeof(recvBuf));
    if(ret < 0)
        return -1;
    msg = (protolcol_t *)recvBuf;
    if((msg->cmd == CMD_RECVFILES_DATA) && (msg->state == OP_OK))
    {
        print_mcd("ready to recv file!");
        memset(&reply, 0, sizeof(protolcol_t));
        reply.cmd = CMD_RECVFILES_DATA;
        reply.state = OP_OK;
        write(fd, &reply, sizeof(protolcol_t));
    }
    else
    {
        print_mcd("can nbt get recv file start!");
        return -3;
    }

    memset(recvBuf, 0, sizeof(recvBuf));
    while ((ret = read(fd, recvBuf, sizeof(recvBuf))))
    {
        if(ret < 0)
        {
            print_mcd("current length < 0");
            fclose(fp);
            return -4;
        }
        int write_len = fwrite(recvBuf, sizeof(char), ret, fp);
        if(write_len < ret)
        {
            print_mcd("file %s write failed write length %d < length %d", file_info.file_name, write_len, ret);
            fclose(fp);
            return -5;
        }
        memset(recvBuf, 0, sizeof(recvBuf));
        recv_sum += ret;
        current = recv_sum / (file_info.files_size + progress);
        printf("\r");
        putchar('[');
        for (size_t i = 0; i < progress; i++)
            putchar(i < current ? '>' : ' ');
        putchar(']');
        printf(" %8d/%ld %6.2f%%", recv_sum, file_info.files_size, (float)recv_sum / file_info.files_size * 100);

        if(recv_sum == file_info.files_size)
        {
            print_mcd("#### recv file finished ...####");
            break;
        }
        else if(recv_sum > file_info.files_size)
        {
            print_mcd("#### recv file err recv_sum[%d] ...####", recv_sum);
			break;
        }
        usleep(1000 * 10);
    }
    
    fclose(fp);
    sleep(2);
    return 0;
}

/**
* @author  		MCD
* @date  		2021-07-27-10:30
* @details		delete files 
*/
static int delete_files(int fd)
{
    protolcol_t msg;
    int file_num = 0, i = 0, ret = 0;
    files_info_t *listfile = NULL;
    // char file_name[PATH_SIZE] = {0};

    memset(&msg, 0, sizeof(protolcol_t));
    while (read(fd, &msg, sizeof(protolcol_t)) != -1)
    {
        if(msg.cmd == CMD_CHECKFILES_END) break;
        else if(msg.cmd == CMD_CHECKFILES_NUM)
        {
            int filses_size = 0;
            memcpy(&filses_size, msg.data, sizeof(int));
            if(filses_size > 0)
            {
                listfile = (files_info_t *)calloc(filses_size, sizeof(files_info_t));
                if(listfile == NULL)  
                {
                    print_mcd("malloc failed!");
                    ret = -1;
                    goto Error;
                } 
            }
            else
            {
                print_mcd("server have no files");
                ret = -1;
                goto Error;    
            }
        }
        //ready recv file info
        else if(msg.cmd == CMD_CHECKFILES)
        {
            memcpy(&listfile[i], msg.data, sizeof(files_info_t));
            print_mcd("序号[%d] %s", i + 1, listfile[i].file_name);
            memset(&msg, 0, sizeof(protolcol_t));
            i++;
            usleep(10);
        } 
    }
    file_num = i;
    print_mcd("\n文件有%d个:\n",file_num);

    memset(&msg, 0, sizeof(protolcol_t));
    printf("\n 是否需要删除某个文件 Y(y)/N(n): ");
    char c = getchar();
    if(c == 'Y' || c == 'y')
    {
        printf("\n 请选择你要下载的文件序号: \n");
        scanf("%d", &i);
        // snprintf(msg.data, PATH_SIZE, "%s", listfile[i - 1].file_name);
        memcpy(msg.data, &listfile[i - 1], sizeof(files_info_t));
        msg.cmd = CMD_DELETEFILES;
        ret = write(fd, &msg, sizeof(protolcol_t));
        if(ret < 0)
        {
            print_mcd("send file name failed!");
            goto Error;
        }
        memset(&msg, 0, sizeof(protolcol_t));
        read(fd, &msg, sizeof(protolcol_t));
        if((msg.cmd == CMD_DELETEFILES) && (msg.state == OP_OK))
        {
            print_mcd("start recv file!");
            // recv_files(fd);
            print_mcd("delete file success!");
        }
    }
    else
    {
        print_mcd("dont delete file");
    }

Error:
    if(listfile != NULL)
    {
        free(listfile);
        listfile = NULL;
    }

    return ret;    
}

/**
* @author  		MCD
* @date  		2021-07-27-10:30
* @details		check files
*/
static int check_files(int fd)
{
    protolcol_t msg;
    int file_num = 0, i = 0, ret = 0;
    files_info_t *listfile = NULL;

    memset(&msg, 0, sizeof(protolcol_t));
    while(read(fd, &msg, sizeof(protolcol_t)) != -1)
    {
        if(msg.cmd == CMD_CHECKFILES_END) 
        {
            print_mcd("get file list success!");
            send_simple_msg(fd, CMD_CHECKFILES_END, OP_FAILED);
            break;
        }
        else if(msg.cmd == CMD_CHECKFILES_NUM)
        {
            int file_size = 0;

            memcpy(&file_size, msg.data, sizeof(int));
            if(file_size > 0)
            {
                listfile = (files_info_t *)calloc(file_size, sizeof(files_info_t));
                if(listfile == NULL)
                {
                    print_mcd("malloc failed");
                    ret = -1;
                    goto Error;
                }
            }
            else
            {
                print_mcd("server have no files");
                ret = -1;
                goto Error;
            }
            send_simple_msg(fd, CMD_CHECKFILES_NUM, OP_OK);
            memset(&msg, 0, sizeof(protolcol_t));
        }
        else if(msg.cmd == CMD_CHECKFILES)
        {
            memcpy(&listfile[i], msg.data, sizeof(protolcol_t));
            print_mcd("序号[%d] %s", i + 1, listfile[i].file_name);
            memset(&msg, 0, sizeof(protolcol_t));
            i++;
        }
    }
    file_num = i;
    print_mcd("\n文件有%d个:\n", file_num);
    
    memset(&msg, 0, sizeof(protolcol_t));
    printf("\n 是否需要删除某个文件 Y(y)/N(n): ");
    char c = getchar();
    if(c == 'Y' || c == 'y')
    {
        printf("\n 请选择你要下载的文件序号: \n");
        scanf("%d", &i);
        snprintf(msg.data, PATH_SIZE, "%s", listfile[i + 1].file_name);
        msg.cmd = CMD_DELETEFILES;
        ret = write(fd, &msg, sizeof(protolcol_t));
        if(ret < 0)
        {
            print_mcd("send file name failed!");
            goto Error;
        }
        read(fd, &msg, sizeof(protolcol_t));
        if((msg.cmd == CMD_DELETEFILES) && (msg.state == OP_OK))
            print_mcd("delete file success!");
    }
    else
    {
        print_mcd("dont delete file");
    }

Error:
    if(listfile != NULL)
    {
        free(listfile);
        listfile = NULL;
    }
    return ret;
}

static client_cmd_t cc_func[] = {
    {S_LOGOUT,      logout},
    {S_REGISET,     registe},
    {S_LOGIN,       login},
    {S_BROADCAST,   broadcast_msg},
    {S_PRIVATE,     private_msg},
    {S_ONLINE_LIST, list_online_user},
    {S_SEND_FILE,   send_files},
    {S_RECV_FILE,   recv_files},
    {S_DELETE_FILE, delete_files},
    {S_CHECK_FILE,  check_files},
};

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
    int i = 0;

    //  protolcol_t msg;

    if (argc < 3)
    {
        printf("cmd: %s ip portnumber\n", argv[0]);
        return -1;
    }

    //argv 2 参数为端口号
    if ((protnumber = atoi(argv[2])) < 0)
    {
        fprintf(stderr, "Usage:%s hostname portnumber\a\n", argv[0]);
        exit(1);
    }

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    print_mcd("socket fd = %d", sockfd);
    if (sockfd < 0)
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
        if (login_f == -1)
        {
            printf("\t 1 注册 \n");
            printf("\t 2 登陆 \n");
        }
        else if (login_f == 1)
        {
            printf("\t 3 公聊 \n");
            printf("\t 4 私聊 \n");
            printf("\t 5 在线列表 \n");
            printf("\t 6 发送文件 \n");
        }
        printf("\t 0 退出 \n");

        fflush(stdin);
        scanf("%d", &sel);
        if (sel == 0)
        {
            print_mcd("client logout!!");
            logout(sockfd);
            sockfd = -1;
            goto Exit;
        }

        if (login_f == 1)
        {
            min_sel = 3;
            max_sel = 5;
        }
        else if (login_f == -1)
        {
            min_sel = 1;
            max_sel = 2;
        }

        if (sel < min_sel || sel > max_sel)
        {
            print_mcd("Vaild choice, try again!");
            continue;
        }
        for ( i = 0; i < ARRAY_SIZE(cc_func); i++)
        {
            if(sel == cc_func[i].cmd)
            {
                // print_mcd("sel = %d", sel);
                cc_func[i].func(sockfd);
                break;
            }
        }
    }
Exit:
    pthread_join(tid, NULL);
    exit(0);
}
