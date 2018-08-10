// #include <stdio.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <string.h>
// #include <time.h>
// #include <sys/types.h>          /* See NOTES */
// #include <sys/socket.h>
// #include <sys/select.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include "log.h"
// #include <pthread.h>
// #include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include "cross_platform.h"
#include "log.h"

typedef struct log_ctx_s{
	int8_t m_inited;						//是否启动
	int32_t m_fd;							//文件句柄
	int32_t m_sockfd;						//套接字句柄
	log_param_t m_param;				//日志参数
	pthread_mutex_t m_param_mtx;		//互斥锁
	pthread_mutex_t m_mtx;				//互斥锁
	pthread_mutex_t m_cond_mtx;			//互斥锁
	pthread_cond_t m_cond;				//条件锁
	pthread_t m_net_connect_thread;
	int8_t m_net_connect_thread_exit;
}log_ctx_t;

static log_ctx_t s_log_ctx = {0};

static int32_t log_connect_net_dst(){
	struct sockaddr_in dstaddr;
	struct timeval timeo;

	if (s_log_ctx.m_sockfd > 0)
	{
		shutdown(s_log_ctx.m_sockfd, 2);
		close(s_log_ctx.m_sockfd);
	}
	s_log_ctx.m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (s_log_ctx.m_sockfd < 0)
	{
		return -1;
	}
	pthread_mutex_lock(&s_log_ctx.m_param_mtx);
	memset(&dstaddr, 0, sizeof(dstaddr));
	dstaddr.sin_family = AF_INET;
	dstaddr.sin_addr.s_addr = htonl(s_log_ctx.m_param.m_dst_ip);
	dstaddr.sin_port = htons(s_log_ctx.m_param.m_dst_port);
	pthread_mutex_unlock(&s_log_ctx.m_param_mtx);
	socklen_t len = sizeof(struct timeval);
	//timeout
	timeo.tv_sec = 3;
	timeo.tv_usec = 0;
	setsockopt(s_log_ctx.m_sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeo, len);

	if(connect(s_log_ctx.m_sockfd, (struct sockaddr *)&dstaddr, sizeof(dstaddr)))
	{
		shutdown(s_log_ctx.m_sockfd, 2);
		close(s_log_ctx.m_sockfd);
		s_log_ctx.m_sockfd = -1;
		return -2;
	}
	pthread_mutex_lock(&s_log_ctx.m_cond_mtx);
	pthread_cond_wait(&s_log_ctx.m_cond,&s_log_ctx.m_cond_mtx);
	pthread_mutex_unlock(&s_log_ctx.m_param_mtx);
	return 0;
}

static int32_t log_disconnect_net_dst(){
	if (s_log_ctx.m_sockfd < 0)
	{
		return 0;
	}
	shutdown(s_log_ctx.m_sockfd, 2);
	close(s_log_ctx.m_sockfd);
	s_log_ctx.m_sockfd = -1;

	return 0;
}

static int32_t log_open_file(){
	if (s_log_ctx.m_fd)
	{
		close(s_log_ctx.m_fd);
	}
	s_log_ctx.m_fd = open(s_log_ctx.m_param.m_pathname, O_RDWR | O_CREAT | O_APPEND, 0666);
	if (s_log_ctx.m_fd < 0)
	{
		printf("open %s failed width:%d\n", s_log_ctx.m_param.m_pathname, errno);
		return -1;
	}

	
	return 0;
}

static int32_t log_close_file(){
	if (s_log_ctx.m_fd < 0)
	{
		return 0;
	}
	close(s_log_ctx.m_fd);
	s_log_ctx.m_fd = -1;

	return 0;
}

void *log_net_connect_thread(void *param){

	log_ctx_t *p_log_ctx = (log_ctx_t *)param;

	while (!p_log_ctx->m_net_connect_thread_exit)
	{
		if(log_connect_net_dst() < 0)
		{
			Msleep(1000);
		}
	}
	return NULL;
}

//启动日志模块
int32_t log_startup(log_param_t *log_param){
	if (!log_param)
	{
		return -1;
	}
	pthread_t thread_id;
	s_log_ctx.m_inited = 1;
	s_log_ctx.m_net_connect_thread_exit = 0;
	memset(&s_log_ctx.m_net_connect_thread, 0, sizeof(pthread_t));
	pthread_mutex_init(&s_log_ctx.m_param_mtx, NULL);
	pthread_mutex_init(&s_log_ctx.m_mtx, NULL);
	pthread_mutex_init(&s_log_ctx.m_cond_mtx, NULL);
	pthread_cond_init(&s_log_ctx.m_cond, NULL);
	memcpy(&s_log_ctx.m_param, log_param, sizeof(s_log_ctx.m_param));
	if(pthread_create(&thread_id,NULL, log_net_connect_thread, (void *)&s_log_ctx))
	{
		printf("pthread_create  log_net_connect_thread was failed.....\n");
	}
	else
	{
		s_log_ctx.m_net_connect_thread = thread_id;
	}
	log_open_file();

	return 0;
}

//关闭日志模块
int32_t log_shutdown(){
	if (!s_log_ctx.m_inited)
	{
		return -1;
	}
	pthread_mutex_lock(&s_log_ctx.m_mtx);
	s_log_ctx.m_net_connect_thread_exit = 1;

	if (PTHREAD_IS_NULL(s_log_ctx.m_net_connect_thread))
	{
		pthread_join(s_log_ctx.m_net_connect_thread, NULL);
		memset(&s_log_ctx.m_net_connect_thread, 0, sizeof(s_log_ctx.m_net_connect_thread));
	}	
	log_disconnect_net_dst();
	log_close_file();
	pthread_mutex_destroy(&s_log_ctx.m_param_mtx);
	pthread_mutex_destroy(&s_log_ctx.m_cond_mtx);
	pthread_cond_destroy(&s_log_ctx.m_cond);
	s_log_ctx.m_inited = 0;
	pthread_mutex_unlock(&s_log_ctx.m_mtx);
	pthread_mutex_destroy(&s_log_ctx.m_mtx);
	return 0;
}

//支持运行时调整日志输出目的地
int32_t log_modify_param(log_param_t log_param){
	pthread_mutex_lock(&s_log_ctx.m_param_mtx);
	if (strcmp(s_log_ctx.m_param.m_pathname, log_param.m_pathname))
	{
		log_close_file();
		log_open_file();
	}
	if (s_log_ctx.m_param.m_dst_ip != log_param.m_dst_ip || \
		s_log_ctx.m_param.m_dst_port != log_param.m_dst_port)
	{
		log_disconnect_net_dst();//先断开，再唤醒重连接线程
		memcpy(&s_log_ctx.m_param, &log_param, sizeof(s_log_ctx.m_param));
		pthread_mutex_lock(&s_log_ctx.m_cond_mtx);
		pthread_cond_signal(&s_log_ctx.m_cond);
		pthread_mutex_unlock(&s_log_ctx.m_param_mtx);
	}
	pthread_mutex_unlock(&s_log_ctx.m_param_mtx);
	return 0;
}

/*
int32_t log_output(log_level_e log_level, const char *log_str, int32_t str_size){
	if (!log_str || !str_size)
	{
		return -1;
	}
	pthread_mutex_lock(&s_log_ctx.m_mtx);
	time_t cur_time;
	char datetime_str[128] = "";

	time(&cur_time);
	plat_sprintf(datetime_str, sizeof(datetime_str), "%s", ctime(&cur_time));
	if (strlen(datetime_str) > 1)
	{
			datetime_str[strlen(datetime_str) - 1] = '\0';
	}

	switch (log_level)
	{
	case LOG_LEVEL_SCREEN:
		printf("[%s]", datetime_str);
		printf("%s\r\n", log_str);
		break;
	case LOG_LEVEL_FILE:
		if (s_log_ctx.m_fd > 0)
		{
			write(s_log_ctx.m_fd, "[", 1);
			write(s_log_ctx.m_fd, datetime_str, strlen(datetime_str));
			write(s_log_ctx.m_fd, "]", 1);
			write(s_log_ctx.m_fd,  log_str, str_size);
			write(s_log_ctx.m_fd,  "\r\n", strlen("\r\n"));
		}
		break;
	case LOG_LEVEL_NET:
		if (s_log_ctx.m_sockfd > 0)
		{
			send(s_log_ctx.m_sockfd, "[", 1, 0);
			send(s_log_ctx.m_sockfd, datetime_str, strlen(datetime_str), 0);
			send(s_log_ctx.m_sockfd, "]", 1, 0);
			send(s_log_ctx.m_sockfd, datetime_str, strlen(datetime_str), 0);
			send(s_log_ctx.m_sockfd, "\r\n", strlen("\r\n"), 0);
		}
		break;
	case LOG_LEVEL_FILE_SCREEN:
		printf("[%s]", datetime_str);
		printf("%s\r\n", log_str);
		if (s_log_ctx.m_fd > 0)
		{
			write(s_log_ctx.m_fd, "[", 1);
			write(s_log_ctx.m_fd, datetime_str, strlen(datetime_str));
			write(s_log_ctx.m_fd, "]", 1);
			write(s_log_ctx.m_fd,  log_str, strlen(log_str));
			write(s_log_ctx.m_fd,  "\r\n", strlen("\r\n"));
		}
		break;
	case LOG_LEVEL_NET_SCREEN:
		printf("[%s]", datetime_str);
		printf("%s\r\n", log_str);
		if (s_log_ctx.m_sockfd)
		{
			send(s_log_ctx.m_sockfd, "[", 1, 0);
			send(s_log_ctx.m_sockfd, datetime_str, strlen(datetime_str), 0);
			send(s_log_ctx.m_sockfd, "]", 1, 0);
			send(s_log_ctx.m_sockfd, datetime_str, strlen(datetime_str), 0);
		}
		break;
	case LOG_LEVEL_NET_FILE_SCREEN:
		printf("%s", datetime_str);
		printf("%s\r\n", log_str);
		if (s_log_ctx.m_fd > 0)
		{
			write(s_log_ctx.m_fd, "[", 1);
			write(s_log_ctx.m_fd, datetime_str, strlen(datetime_str));
			write(s_log_ctx.m_fd, "]", 1);
			write(s_log_ctx.m_fd,  log_str, strlen(log_str));
			write(s_log_ctx.m_fd,  "\r\n", strlen("\r\n"));
		}
		if (s_log_ctx.m_sockfd)
		{
			send(s_log_ctx.m_sockfd, "[", 1, 0);
			send(s_log_ctx.m_sockfd, datetime_str, strlen(datetime_str), 0);
			send(s_log_ctx.m_sockfd, "]", 1, 0);
			send(s_log_ctx.m_sockfd, datetime_str, strlen(datetime_str), 0);
			send(s_log_ctx.m_sockfd, "\r\n", strlen("\r\n"), 0);
		}
		break;
	default:
		printf("%s", datetime_str);
		printf("%s\r\n", log_str);
		break;
	}
	pthread_mutex_unlock(&s_log_ctx.m_mtx);
	return 0;
}
*/
int32_t log_output(log_level_e log_level, const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
#define  LOGSTR_BUF_MAX 2048
	const char* p = fmt;
	char log_str[LOGSTR_BUF_MAX];
	char tmp_str[1024];
	int32_t log_str_indx = 0;
	int length = 0, i = 0;
	
	while(*p && log_str_indx < LOGSTR_BUF_MAX - 1)
	{
		memset(tmp_str, 0, length);
		if ('%' == *p)
		{
			p++;
			if ('d' == *(p))	//十进制
			{
				plat_sprintf(tmp_str, sizeof(tmp_str),"%d",va_arg(ap, int32_t));
				length = strlen(tmp_str);
				for(i = 0;i < length;i++)
				{
					log_str[log_str_indx++] = tmp_str[i];
				}
			}
			else if('s' == *(p))
			{
				plat_sprintf(tmp_str, sizeof(tmp_str),"%s",va_arg(ap, char *));
				length = strlen(tmp_str);
				for(i = 0;i < length;i++)
				{
					log_str[log_str_indx++] = tmp_str[i];
				}
			}
			else if('x' == *(p))
			{
				plat_sprintf(tmp_str, sizeof(tmp_str),"%x",va_arg(ap, int32_t));
				length = strlen(tmp_str);
				for(i = 0;i < length;i++)
				{
					log_str[log_str_indx++] = tmp_str[i];
				}
			}
			else if('f' == *(p))
			{
				plat_sprintf(tmp_str, sizeof(tmp_str),"%f",va_arg(ap, double));
				length = strlen(tmp_str);
				for(i = 0;i < length;i++)
				{
					log_str[log_str_indx++] = tmp_str[i];
				}
			}
			else if('i' == *(p))
			{
				plat_sprintf(tmp_str, sizeof(tmp_str),"%i",va_arg(ap, int32_t));
				length = strlen(tmp_str);
				for(i = 0;i < length;i++)
				{
					log_str[log_str_indx++] = tmp_str[i];
				}
			}
			else if('u' == *(p))
			{
				plat_sprintf(tmp_str, sizeof(tmp_str),"%u",va_arg(ap, uint32_t));
				length = strlen(tmp_str);
				for(i = 0;i < length;i++)
				{
					log_str[log_str_indx++] = tmp_str[i];
				}
			}
			else if('p' == *(p))
			{
				plat_sprintf(tmp_str, sizeof(tmp_str),"%p",va_arg(ap, int32_t *));
				length = strlen(tmp_str);
				for(i = 0;i < length;i++)
				{
					log_str[log_str_indx++] = tmp_str[i];
				}
			}
			else if('c' == *(p))
			{
				log_str[log_str_indx++] = va_arg(ap, int32_t);
			}
			else
			{
				log_str[log_str_indx++] = *p;
			}
			p++;
		}
		else
		{
			log_str[log_str_indx++] = *p;
			p++;
		}
		va_end(ap);
	}
	if (log_str_indx < LOGSTR_BUF_MAX - 1)
	{
		log_str[log_str_indx++] = '\0';
	}
	pthread_mutex_lock(&s_log_ctx.m_mtx);
	time_t cur_time;
	char datetime_str[128] = "";

	time(&cur_time);
	plat_sprintf(datetime_str, sizeof(datetime_str), "%s", ctime(&cur_time));
	if (strlen(datetime_str) > 1)
	{
		datetime_str[strlen(datetime_str) - 1] = '\0';
	}

	switch (log_level)
	{
	case LOG_LEVEL_SCREEN:
		printf("[%s]", datetime_str);
		printf("%s\r\n", log_str);
		break;
	case LOG_LEVEL_FILE:
		if (s_log_ctx.m_fd > 0)
		{
			write(s_log_ctx.m_fd, "[", 1);
			write(s_log_ctx.m_fd, datetime_str, strlen(datetime_str));
			write(s_log_ctx.m_fd, "]", 1);
			write(s_log_ctx.m_fd,  log_str, strlen(log_str));
			write(s_log_ctx.m_fd,  "\r\n", strlen("\r\n"));
		}
		break;
	case LOG_LEVEL_NET:
		if (s_log_ctx.m_sockfd > 0)
		{
			send(s_log_ctx.m_sockfd, "[", 1, 0);
			send(s_log_ctx.m_sockfd, datetime_str, strlen(datetime_str), 0);
			send(s_log_ctx.m_sockfd, "]", 1, 0);
			send(s_log_ctx.m_sockfd, datetime_str, strlen(datetime_str), 0);
			send(s_log_ctx.m_sockfd, "\r\n", strlen("\r\n"), 0);
		}
		break;
	case LOG_LEVEL_FILE_SCREEN:
		printf("[%s]", datetime_str);
		printf("%s\r\n", log_str);
		if (s_log_ctx.m_fd > 0)
		{
			write(s_log_ctx.m_fd, "[", 1);
			write(s_log_ctx.m_fd, datetime_str, strlen(datetime_str));
			write(s_log_ctx.m_fd, "]", 1);
			write(s_log_ctx.m_fd,  log_str, strlen(log_str));
			write(s_log_ctx.m_fd,  "\r\n", strlen("\r\n"));
		}
		break;
	case LOG_LEVEL_NET_SCREEN:
		printf("[%s]", datetime_str);
		printf("%s\r\n", log_str);
		if (s_log_ctx.m_sockfd)
		{
			send(s_log_ctx.m_sockfd, "[", 1, 0);
			send(s_log_ctx.m_sockfd, datetime_str, strlen(datetime_str), 0);
			send(s_log_ctx.m_sockfd, "]", 1, 0);
			send(s_log_ctx.m_sockfd, datetime_str, strlen(datetime_str), 0);
		}
		break;
	case LOG_LEVEL_NET_FILE_SCREEN:
		printf("%s", datetime_str);
		printf("%s\r\n", log_str);
		if (s_log_ctx.m_fd > 0)
		{
			write(s_log_ctx.m_fd, "[", 1);
			write(s_log_ctx.m_fd, datetime_str, strlen(datetime_str));
			write(s_log_ctx.m_fd, "]", 1);
			write(s_log_ctx.m_fd,  log_str, strlen(log_str));
			write(s_log_ctx.m_fd,  "\r\n", strlen("\r\n"));
		}
		if (s_log_ctx.m_sockfd)
		{
			send(s_log_ctx.m_sockfd, "[", 1, 0);
			send(s_log_ctx.m_sockfd, datetime_str, strlen(datetime_str), 0);
			send(s_log_ctx.m_sockfd, "]", 1, 0);
			send(s_log_ctx.m_sockfd, datetime_str, strlen(datetime_str), 0);
			send(s_log_ctx.m_sockfd, "\r\n", strlen("\r\n"), 0);
		}
		break;
	default:
		printf("%s", datetime_str);
		printf("%s\r\n", log_str);
		break;
	}
	pthread_mutex_unlock(&s_log_ctx.m_mtx);
	return 0;
}