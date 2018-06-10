/******************************************************
统一平台差异化内容
******************************************************/

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <assert.h>


/****select platform*******/
#define WINDOWS_PLATFORM 0
#define LINUX_PLATFORM 1
#define IOS_PLATFORM 2

// !PLATFORM
#ifndef PLATFORM
#define PLATFORM	WINDOWS_PLATFORM
#endif 

//设备类型(硬件类型)
#define DEVICE_TYPE_IPC 0//1~999
#define DEVICE_TYPE_DEC 1//1001~1999
#define DEVICE_TYPE_HVR 2//2001~2999
#define DEVICE_TYPE_DVS 3//3001~3999
#define DEVICE_TYPE_CARD 4//4001~4999
#define DEVICE_TYPE_RB01 6//6001~6999
#define DEVICE_TYPE_RB02 7//7001~7999
#define DEVICE_TYPE_RB03 8//8001~8999
#define DEVICE_TYPE_RB04 9//9001~9999
#define DEVICE_TYPE_PC  10//10001~10999
#define GET_DEVICE_TYPE_I(type) (type / 1000)
#define GET_DEVICE_TYPE_MIN(type) (type % 100)

#define MILSEC_PER_DAY (24 * 60 * 60 * 1000)

#define MILSEC_PER_MIN (60 * 1000)
#define MILSEC_PER_HOUR (60 * 60 * 1000)
#define MILSEC_PER_DAY (24 * 60 * 60 * 1000)
#define MILSEC_PER_20DAY (20 * MILSEC_PER_DAY)//DWORD is 49.71 days
#define MILSEC_5SEC (5 * 1000)
#define TIME_STAMP_ROUND MILSEC_PER_DAY
#define GET_TIMESTAMP_ADD(curtime, oldtime) (((curtime) + (oldtime)) % TIME_STAMP_ROUND)
#define GET_TIMESTAMP_DIFF(curtime, oldtime) (((curtime) >= (oldtime)) ? ((curtime) - (oldtime)) : ((curtime) + TIME_STAMP_ROUND - (oldtime)))

#define CONFIG_PATH "/mnt/mtd/conf"

//record
#define CONFIG_PATH_RECORD "/mnt/mtd/conf/record/property.json"


#if (PLATFORM == WINDOWS_PLATFORM) 		//windows 平台
#include <typeinfo.h>	//该头文件只有在c++编译中才能用，所以没有extern "C"{中
#endif

#ifdef __cplusplus
extern "C"{
#endif

//////////windows platform
#if (PLATFORM == WINDOWS_PLATFORM) 		//windows 平台

#include <WinSock2.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include<ws2tcpip.h>
#include <stdint.h>
#include "pthread.h"
#pragma comment(lib, "ws2_32.lib")

#define FDSET FD_SET
#define Msleep(x) Sleep(x)
#define PACKED_ALIGN
#define av_always_inline __inline
#define inline __inline
#define plat_sprintf sprintf_s
#define  gettid() GetCurrentThreadId()
#define fsync 
#define lseek _lseek
#define open _open
#define write _write
#define read _read
#define close _close
#define F_OK 0
#define DEBUG_VERSION
#ifdef DEBUG_VERSION
	//#define DBG_PRT printf
#define DBG_PRT(format, ...) printf("[DEBUG:%s -> %s : %d->] " format , __FILE__, \
	__FUNCTION__, __LINE__, ##__VA_ARGS__);
#define ERR_PRT(format, ...) printf("[ERR:%s -> %s : %d->] " format , __FILE__, \
	__FUNCTION__, __LINE__, ##__VA_ARGS__);
#define WAR_PRT(format, ...) printf("[WAR:%s -> %s : %d->] " format , __FILE__, \
	__FUNCTION__, __LINE__, ##__VA_ARGS__);
#else
#define DBG_PRT 
#endif

#define IMPORT_PRT(format, ...) printf("[WAR:%s -> %s : %d->] " format , __FILE__, \
	__FUNCTION__, __LINE__, ##__VA_ARGS__ );

#define SYS_SIGNAL_PRT(format, ...) printf("[SYS_INFO:%s -> %s : %d->" format "]",__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ );

#define PTHREAD_IS_NULL(x) (x).p

typedef HMODULE  MODULE_HANDLE; 
typedef unsigned long long __uint64;
//安全的执行system函数
int32_t sys_cmd_safe(char *cmd);
int32_t get_cur_datetime(struct tm *curtime);
//ms
uint32_t GetCurTimeStamp();
#endif

/******linux platform*******/
#if (PLATFORM == LINUX_PLATFORM)
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>  
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <dirent.h>
#include <error.h>
#include <stdbool.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <linux/route.h>
#include <string.h>
#include <errno.h>
#include <linux/if_ether.h> 
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <bits/wordsize.h>


#define FDSET fd_set
#define Msleep(x) usleep((x) * 1000)
#define PACKED_ALIGN __attribute__((packed))
#define plat_sprintf snprintf
#define  gettid() syscall(__NR_gettid)

#define TRUE 1
#define FALSE 0

#define DEBUG_VERSION
#ifdef DEBUG_VERSION
//#define DBG_PRT printf
#define DBG_PRT(format, args...) printf("\033[36m[DEBUG:%s -> %s : %d->] " format "\033[0m", __FILE__, \
	__FUNCTION__, __LINE__, ##args );
#define ERR_PRT(format, args...) printf("\033[31m[ERR:%s -> %s : %d->] " format "\033[0m", __FILE__, \
	__FUNCTION__, __LINE__, ##args );
#define WAR_PRT(format, args...) printf("\033[35m[WAR:%s -> %s : %d->] " format "\033[0m", __FILE__, \
	__FUNCTION__, __LINE__, ##args );
#else
#define DBG_PRT 
#endif

#define IMPORT_PRT(format, args...) printf("\033[35m[WAR:%s -> %s : %d->] " format "\033[0m", __FILE__, \
	__FUNCTION__, __LINE__, ##args );
#define SYS_SIGNAL_PRT(format, args...) printf("\033[35m[SYS_INFO:%s -> %s : %d->]" format "\033[0m",__FILE__, \
	__FUNCTION__, __LINE__, ##args );

#define OutputDebugString(x) DBG_PRT("%s\n", (x))

//============================================
typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef signed char        int_least8_t;
typedef short              int_least16_t;
typedef int                int_least32_t;
typedef long long          int_least64_t;
typedef unsigned char      uint_least8_t;
typedef unsigned short     uint_least16_t;
typedef unsigned int       uint_least32_t;
typedef unsigned long long uint_least64_t;

typedef signed char        int_fast8_t;
typedef int                int_fast16_t;
typedef int                int_fast32_t;
typedef long long          int_fast64_t;
typedef unsigned char      uint_fast8_t;
typedef unsigned int       uint_fast16_t;
typedef unsigned int       uint_fast32_t;
typedef unsigned long long uint_fast64_t;

#ifndef _INTPTR_T_DEFINED
#define _INTPTR_T_DEFINED
#if (__WORDSIZE == 64)
typedef long long          intptr_t;
#else /* _WIN64 */
typedef int           intptr_t;
#endif /* _WIN64 */
#endif /* _INTPTR_T_DEFINED */


#ifndef _UINTPTR_T_DEFINED
#define _UINTPTR_T_DEFINED
#if (__WORDSIZE == 64)
typedef unsigned long long uintptr_t;
#else /* __WORDSIZE */
typedef  unsigned int  uintptr_t;
#endif /* __WORDSIZE */
#endif /* _UINTPTR_T_DEFINED */

typedef long long          intmax_t;
typedef unsigned long long uintmax_t;
//============================================
#define PTHREAD_IS_NULL(x) (x)
//安全的执行system函数
int32_t sys_cmd_safe(char *cmd);
int32_t get_cur_datetime(struct tm *curtime);
//ms
uint32_t GetCurTimeStamp();
#endif

/*****ios platform**********/
#if (PLATFORM == IOS_PLATFORM)
#endif

typedef struct platform_info_ctx_s
{
	uint32_t m_device_type_major;		//设备主类型
	uint32_t m_device_type_minor;		//设备子类型
	uint32_t m_devive_id;				//设备唯一ID
}platform_info_ctx_t;

extern platform_info_ctx_t g_platform_info_ctx;

#ifdef __cplusplus
}
#endif

#endif