#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include "log.h"
#include "gavin_error.h"
#if WIN32
#include<windows.h>
#else
#include <sys/time.h>
#endif
#include "err_public_def.h"

typedef struct log_ctx_s{
	int8_t m_inited;						//是否启动
	log_level_e m_eLogLevel;
	output_func_callback_t m_outptCb;			
	flush_func_callback_t m_flushCb;			
}log_ctx_t;

static const char* s_log_Level_name[LOG_LEVELS_NUM] =
{
  " TRACE",
  " DEBUG",
  " INFO",
  " NOTICE",
  " WARN",
  " ERROR",
  " CRIT",
  " ALERT",
  " FATAL",
};

static log_ctx_t s_log_ctx = {0};

static void default_output(const int8_t* pMsg, int32_t i32Len)
{
  size_t n = fwrite(pMsg, 1, i32Len, stdout);
  //FIXME check n

  (void)n;
}

static void default_flush()
{
  fflush(stdout);
}

inline int32_t get_time_str(int8_t *buf, int32_t i32BufSize)
{
#ifdef WIN32
	SYSTEMTIME tm_cur;

	GetSystemTime(&tm_cur);
	return snprintf(buf, i32BufSize, "[%04d%02d%02d %02d:%02d:%02d.%d - ", 
		tm_cur.wYear + 1900, tm_cur.wMonth + 1, tm_cur.wDay, tm_cur.wHour, tm_cur.wMinute, tm_cur.wSecond, tm_cur.wMilliseconds);
#else
	struct tm tm_cur;
	time_t curtime;
	struct  timeval tv;
	
	gettimeofday(&tv, NULL);
	curtime = tv.tv_sec;
	gmtime_r(&curtime, &tm_cur);
	return snprintf((char *)buf, i32BufSize, "[%04d%02d%02d %02d:%02d:%02d.%ld - ", tm_cur.tm_year + 1900, tm_cur.tm_mon + 1, tm_cur.tm_mday, tm_cur.tm_hour, tm_cur.tm_min, tm_cur.tm_sec, tv.tv_usec % 1000);
#endif	
}

int32_t log_startup(log_level_e eLevel)
{
	if(s_log_ctx.m_inited)
		return ERR_PUB_EINITED;
	memset(&s_log_ctx, 0, sizeof(log_ctx_t));
	s_log_ctx.m_inited = 1;
	if(eLevel >= LOG_E_TRACE && eLevel < LOG_LEVELS_NUM)
		s_log_ctx.m_eLogLevel = eLevel;
	else
		s_log_ctx.m_eLogLevel = LOG_E_DEBUG;
	s_log_ctx.m_outptCb = default_output;
	s_log_ctx.m_flushCb = default_flush;

	return ERR_PUB_OK;
}

int32_t log_shutdown()
{
	s_log_ctx.m_inited = 0;

	return ERR_PUB_OK;
}
int32_t log_get_loglevel(log_level_e *eLevel)
{
	if(!s_log_ctx.m_inited)
		return ERR_PUB_EUNINITED;
	*eLevel = s_log_ctx.m_eLogLevel;	
	return ERR_PUB_OK;
}

int32_t log_set_loglevel(log_level_e eLevel)
{
	if(!s_log_ctx.m_inited)
		return ERR_PUB_EUNINITED;
	s_log_ctx.m_eLogLevel = eLevel;	
	return ERR_PUB_OK;
}

void log_output(log_level_e eLevel, const char *fmt, va_list *ap)
{
	if(s_log_ctx.m_eLogLevel > eLevel) //过滤不需要输出的日志
		return;
	
#define  LOGSTR_BUF_MAX 4000
	const char* p = fmt;
	char log_str[LOGSTR_BUF_MAX];
	int32_t log_str_indx = 0;
	int lenTotal = sizeof(log_str);

	log_str_indx = get_time_str((int8_t *)log_str + log_str_indx, LOGSTR_BUF_MAX - 2);
	while (*p && log_str_indx < LOGSTR_BUF_MAX - 2)
	{
		if ('%' == *p)
		{
			p++;
			if ('d' == *(p))	//十进制
			{
				log_str_indx += snprintf(log_str + log_str_indx, lenTotal - log_str_indx, "%d", va_arg(*ap, int32_t));
			}
			else if ('s' == *(p))
			{
				log_str_indx += snprintf(log_str + log_str_indx, lenTotal - log_str_indx, "%s", va_arg(*ap, char *));
			}
			else if ('x' == *(p))
			{
				log_str_indx += snprintf(log_str + log_str_indx, lenTotal - log_str_indx, "%x", va_arg(*ap, int32_t));
			}
			else if ('f' == *(p))
			{
				log_str_indx += snprintf(log_str + log_str_indx, lenTotal - log_str_indx, "%f", va_arg(*ap, double));
			}
			else if ('i' == *(p))
			{
				log_str_indx += snprintf(log_str + log_str_indx, lenTotal - log_str_indx, "%i", va_arg(*ap, int32_t));
			}
			else if ('u' == *(p))
			{
				log_str_indx += snprintf(log_str + log_str_indx, lenTotal - log_str_indx, "%u", va_arg(*ap, uint32_t));
			}
			else if ('p' == *(p))
			{
				log_str_indx += snprintf(log_str + log_str_indx, lenTotal - log_str_indx, "%p", va_arg(*ap, int32_t *));
			}
			else if ('c' == *(p))
			{
				log_str[log_str_indx++] = va_arg(*ap, int32_t);
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
	}
	if(log_str_indx < LOGSTR_BUF_MAX - 2)
		log_str_indx += snprintf(log_str + log_str_indx, \
			lenTotal - log_str_indx, "%s", s_log_Level_name[eLevel]);
	log_str[++log_str_indx] = '\n';
	log_str[++log_str_indx] = '\0';
	s_log_ctx.m_outptCb((int8_t *)log_str, log_str_indx);
	s_log_ctx.m_flushCb();
	if(eLevel == LOG_E_FATAL)
	{
		abort();
	}
}

void log_output_trace(const char *fmt, ...)
{
	va_list ap;

	if(s_log_ctx.m_eLogLevel > LOG_E_TRACE)
		return;
	va_start(ap, fmt);
	log_output(LOG_E_TRACE, fmt, &ap);
	va_end(ap);
}
void log_output_debug(const char *fmt, ...)
{
	va_list ap;

	if(s_log_ctx.m_eLogLevel > LOG_E_DEBUG)
		return;
	va_start(ap, fmt);
	log_output(LOG_E_DEBUG, fmt, &ap);
	va_end(ap);
}

void log_output_info(const char *fmt, ...)
{
	va_list ap;

	if(s_log_ctx.m_eLogLevel > LOG_E_INFO)
		return;
	va_start(ap, fmt);
	log_output(LOG_E_INFO, fmt, &ap);
	va_end(ap);
}

void log_output_notice(const char *fmt, ...)
{
	va_list ap;

	if(s_log_ctx.m_eLogLevel > LOG_E_NOTICE)
		return;
	va_start(ap, fmt);
	log_output(LOG_E_NOTICE, fmt, &ap);
	va_end(ap);
}

void log_output_warn(const char *fmt, ...)
{
	va_list ap;

	if(s_log_ctx.m_eLogLevel > LOG_E_WARNING)
		return;
	va_start(ap, fmt);
	log_output(LOG_E_WARNING, fmt, &ap);
	va_end(ap);
}

void log_output_err(const char *fmt, ...)
{
	va_list ap;

	if(s_log_ctx.m_eLogLevel > LOG_E_ERROR)
		return;
	va_start(ap, fmt);
	log_output(LOG_E_ERROR, fmt, &ap);
	va_end(ap);
}

void log_output_crit(const char *fmt, ...)
{
	va_list ap;

	if(s_log_ctx.m_eLogLevel > LOG_E_CRIT)
		return;
	va_start(ap, fmt);
	log_output(LOG_E_CRIT, fmt, &ap);
	va_end(ap);
}

void log_output_alert(const char *fmt, ...)
{
	va_list ap;

	if(s_log_ctx.m_eLogLevel > LOG_E_ALERT)
		return;
	va_start(ap, fmt);
	log_output(LOG_E_ALERT, fmt, &ap);
	va_end(ap);
}

void log_output_fatal(const char *fmt, ...)
{
	va_list ap;

	if(s_log_ctx.m_eLogLevel > LOG_E_FATAL)
		return;
	va_start(ap, fmt);
	log_output(LOG_E_FATAL, fmt, &ap);
	va_end(ap);
}


int32_t log_set_output_callback(output_func_callback_t pCb)
{
	if(!s_log_ctx.m_inited)
		return ERR_PUB_EUNINITED;
	s_log_ctx.m_outptCb = pCb;
	return ERR_PUB_OK;	
}

int32_t log_set_flush_callback(flush_func_callback_t pCb)
{
	if(!s_log_ctx.m_inited)
		return ERR_PUB_EUNINITED;
	s_log_ctx.m_flushCb = pCb;
	return ERR_PUB_OK;
}
