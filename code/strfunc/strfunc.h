/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : strfunc.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2005/7/27
  Last Modified :
  Description   : strfunc.c header file
  Function List :
  History       :
  1.Date        : 2005/7/27
    Author      : T41030
    Modification: Created file

******************************************************************************/

#ifndef __STRFUNC_H__
#define __STRFUNC_H__

#include "cross_platform.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define STRFMT_ADDR32    "%#010lX"
#define STRFMT_ADDR32_2  "0x%08lX"

extern int32_t StrToNumber(char *str , uint32_t * ulValue);
//保证dstbuf一定有\0结尾
int32_t strncpy_ex(char *dstbuf, const char *srcbuf, int32_t dst_size);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __STRFUNC_H__ */
