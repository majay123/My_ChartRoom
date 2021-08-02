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
 * @LastEditTime : 2021-08-02 16:40:48
 * @FilePath     : /My_ChartRoom/src/Server/TCP_Server.c
 * @Description  : 
 * 
 * ******************************************
 */

#include "common.h"

Online_info_t online_info[MAX_USER_NUM];
// int sockfd;
int addrlen;
struct sockaddr_in server_addr;
pthread_t tid;
// files_info_t listFile[FILES_MAX_SIZE];
listFile_inof_t listFile;


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

#if 0
static int recv_simple_msg(int fd, void *msg)
{
    protolcol_t *reply = msg;

    memset(reply, 0, sizeof(protolcol_t));
    
    return read(fd, reply, sizeof(protolcol_t));

}
#endif

/**
* @author  		MCD
* @date  		2021-07-23-09:44
* @details		del user online
*/
void del_user_online(int index)
{
    int i;
    protolcol_t msg;
    char buf[GET_BUFFER_SIZE] = {0};
    
    if(index < 0)
        return;
    
    memset(&msg, 0, sizeof(protolcol_t));
    msg.cmd = CMD_LOGOUT;
    write(online_info[index].fd, &msg, sizeof(protolcol_t));
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
            snprintf(online_info[i].passwd, strlen(msg->data) < (BUFFER_SIZE * 2) ? strlen(msg->data) : (BUFFER_SIZE * 2), "%s", msg->data);

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
    int i;
    char buf[GET_BUFFER_SIZE] = {0};

    // print_mcd("client %s, %d", online_info[index].name, index);
    // print_mcd("data: %s", msg->data);
    snprintf(buf, GET_BUFFER_SIZE, "%s say: %s", online_info[index].name, msg->data);
    for ( i = 0; i < MAX_USER_NUM; i++)
    {
        if(online_info[i].fd < 0)
            continue;
        // 登陆客户端 就不用发了
        if(strcmp(online_info[index].name, online_info[i].name) == 0)
            continue;
        write(online_info[i].fd, buf, strlen(buf));
    }
    
    
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
static int find_dest_user(char *name)
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

/**
* @author  		MCD
* @date  		2021-07-23-16:14
* @details		private msg
*/
static void private_msg(int index, protolcol_t *msg)
{
    int ret = -1;
    char buf[GET_BUFFER_SIZE] = {0};

    if(msg == NULL)
        return;
    
    ret = find_dest_user(msg->name);
    if(ret < 0)
    {
        snprintf(buf, GET_BUFFER_SIZE, "%s", "there is no user");
        write(online_info[index].fd, buf, strlen(buf));
        return;
    }
        
    if(online_info[ret].fd < 0)
    {
        snprintf(buf, GET_BUFFER_SIZE, "%s offline", online_info[ret].name);
        write(online_info[index].fd, buf, strlen(buf));
        return;
    }
    snprintf(buf, GET_BUFFER_SIZE, "%s to %s: %s", online_info[index].name, online_info[ret].name, msg->data);
    write(online_info[ret].fd, buf, strlen(buf));
}

/**
* @author  		MCD
* @date  		2021-07-23-16:14
* @details		list online user
*/
static void list_online_user(int index)
{
    int i;
    // int ret = -1;
    char buf[GET_BUFFER_SIZE] = {0};
    // char buf1[GET_BUFFER_SIZE] = {0};

    for ( i = 0; i < MAX_USER_NUM; i++)
    {
        if(online_info[i].flage == 1)
        {
            strcat(buf, online_info[i].name);
            strcat(buf, "\t");

            // snprintf(buf, GET_BUFFER_SIZE, "%s \t", online_info[i].name);
            // snprintf(buf1, GET_BUFFER_SIZE, "%s%s \t", buf, online_info[i].name);
        }
    }
    
     
    write(online_info[index].fd, buf, strlen(buf));
}

/**
* @author  		MCD
* @date  		2021-07-23-10:01
* @details		registe
*/
static int registe(int fd, int *index, protolcol_t *msg)
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
static void login(int fd, int *index, protolcol_t *msg)
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
        // 登陆客户端 就不用发了
        if(strcmp(online_info[*index].name, online_info[i].name) == 0)
            continue;
        // print_mcd("fd = %d, %s", online_info[i].fd, buf);
        write(online_info[i].fd, buf, strlen(buf));
        // print_mcd("len = %d", len);
    }
    
}

/**
* @author  		MCD
* @date  		2021-07-27-10:25
* @details		send file
*/
static int send_files(int server_fd, char *filename)
{
    FILE *fp;
    protolcol_t msg;
    long totlen;
    int file_block_length;
	int count=0;            //发送的次数
	int send_sum = 0;       //当前已发送文件长度
	int current = 0;       //进度条长度
	int progress = 50;      //统计接受大小
    int ret = 0;
    char sendBuf[DATA_SIZE] = {0};
    
    fp = fopen(filename, "rb");
    if(fp == NULL)
    {
        print_mcd("============ file open failed ============");
        msg.cmd = CMD_RECVFILES_INFO;
        msg.state = OP_FAILED;
        write(server_fd, &msg, sizeof(msg));
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    totlen = ftell(fp);
    print_mcd("文件%s的大小为%ld字节", filename, totlen);
    
    memset(&msg, 0, sizeof(protolcol_t));
    ret = read(server_fd, &msg, sizeof(protolcol_t));
    if((msg.cmd == CMD_RECVFILES_INFO) && (msg.state = OP_OK))
    {
        print_mcd("客户端获取了文件信息");
    }
    else 
    {
        print_mcd("未收到文件信息确认消息");
        ret = -1;
        goto Error;
    }

    // 准备发送文件
    memset(&msg, 0, sizeof(protolcol_t));
    msg.cmd = CMD_RECVFILES_DATA;
    msg.state = OP_OK;
    write(server_fd, &msg, sizeof(protolcol_t));
    memset(&msg, 0, sizeof(protolcol_t));
    read(server_fd, &msg, sizeof(protolcol_t));
    if((msg.cmd = CMD_RECVFILES_DATA) && (msg.state == OP_OK))
    {
        print_mcd("server can send file data");
    }
    else
    {
        print_mcd("未收到文件发送确认消息");
        ret = -1;
        goto Error;
    }

    //重新指向文件头
    rewind(fp);
    while(!feof(fp))
    {
        file_block_length = fread(sendBuf, sizeof(char), DATA_SIZE, fp);
        send_sum += file_block_length;
		current = send_sum / (totlen / progress);
		printf("\r");
		printf("[");
		for (int i = 0; i < progress; i++) {
			putchar(i < current ? '=' : '+');
		}
		printf("]");
		printf(" %8d/%ld %6.2f%%", send_sum, totlen, (float)send_sum / totlen * 100);

        if(file_block_length < DATA_SIZE) 
        {
			int rest_length = totlen-count*DATA_SIZE;
            if(write(server_fd, sendBuf, rest_length) < 0)
            {
                print_mcd("send file:%s failed\n", filename);
				break;
            }
        }
        else if (write(server_fd, sendBuf, DATA_SIZE) < 0)
        {
            print_mcd("send file:%s failed\n", filename);
            break;
        }
        memset(sendBuf,0,sizeof(sendBuf));
        count++;
    }
    print_mcd("这个程序传送了%d次\n", count);
	print_mcd("..........发送成功..........\n");

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
* @date  		2021-07-27-10:25
* @details		recv file
*/
static int recv_files(int server_fd)
{
    protolcol_t msg;
    // protolcol_t reply;
    int ret = 0;
    files_info_t file_info;
    char filename[PATH_SIZE] = {0};

    memset(&msg, 0, sizeof(protolcol_t));
    memset(&file_info, 0, sizeof(files_info_t));

    // int file_size = 0;
    // int file_block_length;
    // int count = 0;
    int recv_sum = 0;
    int current = 0;
    int progress = 50;
    char recvBuf[DATA_SIZE] = {0};
    FILE *fp = NULL;

    // get file info
    ret = read(server_fd, &msg, sizeof(protolcol_t));
    if(ret < 0)
    {
        print_mcd("get msg failed!");
        goto Error;
    }
    if((msg.cmd != CMD_SENDFILES_INFO) || ( msg.state != OP_OK))
    {
        print_mcd("get msg error!");
        send_simple_msg(server_fd, CMD_SENDFILES_INFO, OP_FAILED);
        ret = -1;
        goto Error;
    }
    memcpy(&file_info, msg.data, sizeof(files_info_t));
    send_simple_msg(server_fd, CMD_SENDFILES_INFO, OP_OK);
    print_mcd("get recv file info name = %s, length = %ld", file_info.file_name, file_info.files_size);

    //get data msg
    ret = read(server_fd, &msg, sizeof(protolcol_t));
    if(ret < 0)
    {
        print_mcd("get msg failed!");
        goto Error;
    }
    if((msg.cmd != CMD_SENDFILES_DATA) || ( msg.state != OP_OK))
    {
        print_mcd("get msg error!");
        send_simple_msg(server_fd, CMD_SENDFILES_DATA, OP_FAILED);
        ret = -1;
        goto Error;
    }
    
    printf("请输入你希望保存的文件名（带扩展名）\n");
    scanf("%s", filename);
    while(access(filename, 0) != -1)
    {
        printf("文件已存在！是否覆盖Y/y N/n\n");
        if(getchar() == 'Y' || 'y')
			break;
		else {
			printf("请重新输入你希望保存的文件名（带扩展名）\n");
			scanf("%s",filename);
		}
    }
    
    fp = fopen(filename, "wb");
    if(fp == NULL)
    {
        print_mcd("fopen file failed!");
        ret = -1;
        goto Error;
    }
    memset(recvBuf, 0, sizeof(recvBuf));
    while ((ret = read(server_fd, recvBuf, sizeof(recvBuf))))
    {
        if(ret < 0)
        {
            print_mcd("current length < 0");
            goto Error;
        }
        int write_ret = fwrite(recvBuf, sizeof(char), ret, fp);
        if(write_ret < ret)
        {
            print_mcd("file: %s write failed, write length %d < length %d", filename, write_ret, ret);
            ret = -1;
            goto Error;
        }
        memset(recvBuf, 0, sizeof(recvBuf));
        recv_sum += ret;
		current = recv_sum / (file_info.files_size / progress);
		printf("\r");
		putchar('[');
		for (int i = 0; i < progress; i++) {
			putchar(i < current ? '>' : ' '); // 输出> 或者 ' '		
		}
		putchar(']');
		printf(" %8d/%ld %6.2f%%", recv_sum, file_info.files_size, (float)recv_sum / file_info.files_size * 100);

        if(recv_sum == file_info.files_size)
        {
            print_mcd("recv file finshed ...");
            break;
        }
        else if(recv_sum > file_info.files_size)
        {
            print_mcd("recv file err recv_sum[%d] ...", recv_sum);
            break;
        }
        usleep(10 * 1000);
    }
    
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
* @date  		2021-07-27-10:30
* @details		delete files
*/
static int delete_files(int server_fd, files_info_t *fileinfo)
{
    int ret = 0;
    size_t i = 0;
    files_info_t *tmp_file = (files_info_t *)&listFile.files_info;
    
    print_mcd("rm file = %d, %s", fileinfo->inedx, fileinfo->file_name);
    for (; i <= listFile.total; i++)
    {
        if((strcmp(tmp_file[i].file_name, fileinfo->file_name)) && (fileinfo->inedx == tmp_file[i].inedx))
        {
            if(!remove(fileinfo->file_name))
            {
                print_mcd("删除文件成功");
                send_simple_msg(server_fd, CMD_DELETEFILES, OP_OK);
            }
            else
            {
                print_mcd("删除文件失败");
                send_simple_msg(server_fd, CMD_DELETEFILES, OP_FAILED);
            }
        }
    }
    
    return ret; 
}

static int get_filesize(char *filepath)
{
    FILE *fp;
    
    fp = fopen(filepath, "rb");
    if(!fp) return -1;
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fclose(fp);

    return size;
}

/**
* @author  		MCD
* @date  		2021-07-27-10:30
* @details		check files
*/
static int check_files(char *file_path)
{
    // int ret = 0;
    DIR *dir;
    struct dirent *ptr;
    char base[PATH_MAX] = {0};
    int num = 0;
    files_info_t *files_info = (files_info_t *)&listFile.files_info;
    
    memset(&listFile, 0, sizeof(listFile_inof_t));

    if(file_path == NULL)
    {
        print_mcd("file path is null");
        goto Error;
    }
    dir = opendir(file_path);
    while((ptr = readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        else if(ptr->d_type == DT_DIR)  //dir
        {
            memset(base, 0, sizeof(base));
            strcpy(base, file_path);
            strcat(base, "/");
            strcat(base, ptr->d_name);
            check_files(base);
        }
        else
        {
            files_info[num].inedx = num;
            listFile.total = num;
            snprintf(files_info[num].file_name ,PATH_MAX, "%s/%s", file_path, ptr->d_name);
            int size = get_filesize(files_info[num].file_name);
            if(size < 0)
                size = 0;
            files_info[num].files_size = size;
            print_mcd("file name = %s, file size = %ld", files_info[num].file_name, files_info[num].files_size);
            num++;
        }
    }
Error:
    if(dir != NULL)
    {
        closedir(dir);
        dir = NULL;
    }

    return num;
}

/**
* @author  		MCD
* @date  		2021-08-02-15:53
* @details		send file list
*/
static int sendFilelist(int server_fd, char *file_path)
{
    // char filename[PATH_MAX] = {0};
    int num;
    int i;
    protolcol_t msg;
    protolcol_t reply;
    files_info_t *files_info = (files_info_t *)&listFile.files_info;
    
    memset(&msg, 0, sizeof(protolcol_t));
    memset(&reply, 0, sizeof(protolcol_t));
    num = check_files(file_path);
    if(num <= 0)
    {
        print_mcd("there is no file");
        return -1;
    }
    msg.cmd = CMD_CHECKFILES_NUM;
    msg.state = OP_OK;
    memcpy(msg.data, &num, sizeof(int));
    write(server_fd, &msg, sizeof(protolcol_t));
    read(server_fd, &reply, sizeof(protolcol_t));
    if(reply.cmd != CMD_CHECKFILES_NUM || reply.state != OP_OK)
    {
        print_mcd("get client msg error");
        return -1;
    }
    
    memset(&reply, 0, sizeof(protolcol_t));
    for(i = 0; i < num; i++)
    {
        msg.cmd = CMD_CHECKFILES;
        msg.state = OP_OK;
        memcpy(msg.data, &files_info[num], sizeof(files_info_t));
        write(server_fd, &msg, sizeof(protolcol_t));
    }
    send_simple_msg(server_fd, CMD_CHECKFILES_END, F_OK);
    read(server_fd, &reply, sizeof(protolcol_t));
    if(msg.cmd == CMD_CHECKFILES_END && msg.state == OP_OK)
    {
        print_mcd("send file list success");
        return 0;
    }
    else
    {
        print_mcd("send file list error"); 
        return -1;
    }
}


/**
* @author  		MCD
* @date  		2021-07-22-15:02
* @details		recv_func
*/
void *recv_func(void *arg)
{
    int new_fd, nbytes;
    // char buffer[RECV_BUFF_SIZE] = {0};
    int index = -1;
    protolcol_t msg;
    files_info_t fileinfo;
    char file_path[PATH_MAX] = {0};

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
            case CMD_LOGOUT:
                // print_mcd("index = %d, %s offline", index, online_info[index].name);
                del_user_online(index);
                close(new_fd);
                break;
            case CMD_RECVFILES:
                send_files(new_fd, "test.txt");
            break;
            case CMD_SENDFILES:
                recv_files(new_fd);
            break;
            case CMD_DELETEFILES:
                delete_files(new_fd, &fileinfo);
            break;
            case CMD_CHECKFILES:
                sendFilelist(new_fd, file_path);
            break;
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
    memset(online_info, 0, sizeof(protolcol_t) * MAX_USER_NUM);
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
        if(pconnsocket == NULL)
        {
            print_mcd("pconnsocket malloc failed!");
            exit(1);
        }
        *pconnsocket = new_fd;
        
        //这里有多clinet 所以会创建多个子线程，为了释放子线程资源，还是用分离式比较方便把？
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        ret = pthread_create(&tid, NULL, recv_func, (void *)pconnsocket);
        pthread_attr_destroy(&attr);
        // print_mcd("tid = %ld", tid);
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
    // pthread_join()
    exit(0);   
}
