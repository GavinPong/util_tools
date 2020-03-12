#ifndef __LOG_H__
#define __LOG_H__

#include <stdint.h>

typedef enum log_level_e{
    LOG_E_TRACE = 0,	/* trace-level                                  */
	LOG_E_DEBUG,  	/* debug-level                                  */
	LOG_E_INFO,       /* informational                                */
	LOG_E_NOTICE,     /* normal but significant condition             */
	LOG_E_WARNING,    /* warning conditions                           */
	LOG_E_ERROR,      /* error conditions                             */
	LOG_E_CRIT,       /* critical conditions                          */
	LOG_E_ALERT,      /* action must be taken immediately             */
	LOG_E_FATAL,      /* just for compatibility with previous version */
	LOG_LEVELS_NUM
}log_level_e;

#ifdef __cplusplus
extern "C"{
#endif

int32_t log_startup(log_level_e eLevel);
int32_t log_shutdown();
int32_t log_get_loglevel(log_level_e *eLevel);
int32_t log_set_loglevel(log_level_e eLevel);
void log_output_trace(const char *fmt, ...);
void log_output_debug(const char *fmt, ...);
void log_output_info(const char *fmt, ...);
void log_output_notice(const char *fmt, ...);
void log_output_warn(const char *fmt, ...);
void log_output_err(const char *fmt, ...);
void log_output_crit(const char *fmt, ...);
void log_output_alert(const char *fmt, ...);
void log_output_fatal(const char *fmt, ...);

typedef void (*output_func_callback_t)(const int8_t* strMsg, int32_t i32Len);
typedef void (*flush_func_callback_t)();
int32_t log_set_output_callback(output_func_callback_t pCb);
int32_t log_set_flush_callback(flush_func_callback_t pCb);

#ifdef WIN32
#define SPLIT_FILENAME strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\')+1:""
#else
#define SPLIT_FILENAME  strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1:""
#endif

#define LOG_TRACE(fmt, ...) 	log_output_trace("%s:%d] " fmt , SPLIT_FILENAME, __LINE__, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)  	log_output_debug("%s:%d] " fmt , SPLIT_FILENAME, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  	log_output_info("%s:%d] " fmt , SPLIT_FILENAME, __LINE__, ##__VA_ARGS__)
#define LOG_NOTICE(fmt, ...)  	log_output_notice("%s:%d] " fmt , SPLIT_FILENAME, __LINE__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  	log_output_warn("%s:%d] " fmt , SPLIT_FILENAME, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)  	log_output_err("%s:%d] " fmt , SPLIT_FILENAME, __LINE__, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...)  	log_output_fatal("%s:%d] " fmt , SPLIT_FILENAME, __LINE__, ##__VA_ARGS__)
#define LOG_CRIT(fmt, ...)  	log_output_crit("%s:%d] " fmt , SPLIT_FILENAME, __LINE__, ##__VA_ARGS__)
#define LOG_ALERT(fmt, ...)  	log_output_alert("%s:%d] " fmt , SPLIT_FILENAME, __LINE__, ##__VA_ARGS__)
#define LOG_SYSERR(fmt, ...)  	log_output_err("%s:%d] " fmt , SPLIT_FILENAME, __LINE__, ##__VA_ARGS__)
#define LOG_SYSFATAL(fmt, ...)  log_output_fatal("%s:%d] " fmt , SPLIT_FILENAME, __LINE__, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif