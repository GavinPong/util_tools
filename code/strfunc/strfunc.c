/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : strfunc.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2005/7/27
  Last Modified :
  Description   : String functions
  Function List :
  History       :
  1.Date        : 2005/7/27
    Author      : T41030
    Modification: Created file

******************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "strfunc.h"
#include "cross_platform.h"

static int32_t atoul(char *str,uint32_t * pulValue);
static int32_t atoulx(char *str,uint32_t * pulValue);


/*****************************************************************************
 Prototype    : StrToNumber
 Description  : 10/16 �����ַ���ת��Ϊ�޷������֡�
 Input  args  : IN CHAR *str 
                   10�����ַ���, �����ܷ���
                   16�����ַ���, ������ǰ׺0x. ��ABCDE
                            
 Output args  : U32* pulValue, ת���������
 Return value : HI_RET  HI_SUCCESS ת���ɹ�
                        HI_FAILURE ת��ʧ��
 Calls        : isdigit
                
 Called  By   : 
                
 History        : 
 1.Date         : 2005��7��10��
   Author       : t41030
   Modification : Created function
*****************************************************************************/

int32_t StrToNumber(char *str , uint32_t * pulValue)
{
    /*�ж��Ƿ�16���Ƶ��ַ���*/
    if ( *str == '0' && (*(str+1) == 'x' || *(str+1) == 'X') )
    {
        if (*(str+2) == '\0')
        {
            return -1;
        }
        else
        {
            return atoulx(str+2,pulValue);
        }
    }
    else
    {
        return atoul(str,pulValue);
    }
}

/*****************************************************************************
 Prototype    : atoul
 Description  : 10�����ַ���ת��Ϊ�޷������֡�
 Input  args  : IN CHAR *str 10�����ַ���
                �����ܷ���
 Output args  : U32* pulValue, ת���������
 Return value : HI_RET  HI_SUCCESS ת���ɹ�
                        HI_FAILURE ת��ʧ��
 Calls        : isdigit
                
 Called  By   : 
                
 History        : 
 1.Date         : 2005��7��10��
   Author       : t41030
   Modification : Created function
*****************************************************************************/
static int32_t atoul(char *str,uint32_t * pulValue)
{
    uint32_t ulResult=0;

    while (*str)
    {
        if (isdigit((int32_t)*str))
        {
            /*���֧�ֵ�0xFFFFFFFF(4294967295), 
               X * 10 + (*str)-48 <= 4294967295
               ���ԣ� X = 429496729 */
            if ((ulResult<429496729) || ((ulResult==429496729) && (*str<'6')))
            {
                ulResult = ulResult*10 + (*str)-48;
            }
            else
            {
                *pulValue = ulResult;
                return -1;
            }
        }
        else
        {
            *pulValue=ulResult;
            return -1;
        }
        str++;
    }
    *pulValue=ulResult;
    return 0;
}



/*****************************************************************************
 Prototype    : atoulx
 Description  : 16�����ַ���ת��Ϊ�޷������֡������16�����ַ���������ǰ׺0x
 Input  args  : IN CHAR *str 16�����ַ���, ������ǰ׺0x. ��ABCDE
 Output args  : U32* pulValue, ת���������
 Return value : HI_RET  HI_SUCCESS ת���ɹ�
                        HI_FAILURE ת��ʧ��
 Calls        : toupper
                isdigit
                
 Called  By   : 
                
 History        : 
 1.Date         : 2005��7��10��
   Author       : t41030
   Modification : Created function
*****************************************************************************/
#define ASC2NUM(ch) (ch - '0')
#define HEXASC2NUM(ch) (ch - 'A' + 10)

int32_t  atoulx(char *str,uint32_t * pulValue)
{
    uint32_t   ulResult=0;
    int32_t ch;

    while (*str)
    {
        ch=toupper(*str);
        if (isdigit(ch) || ((ch >= 'A') && (ch <= 'F' )))
        {
            if (ulResult < 0x10000000)
            {
                ulResult = (ulResult << 4) + ((ch<='9')?(ASC2NUM(ch)):(HEXASC2NUM(ch)));
            }
            else
            {
                *pulValue=ulResult;
                return -1;
            }
        }
        else
        {
            *pulValue=ulResult;
            return -1;
        }
        str++;
    }
    
    *pulValue=ulResult;
    return 0;
}

int32_t strncpy_ex(char *dstbuf, const char *srcbuf, int32_t dst_size)
{
	if (!dstbuf || !srcbuf || !dst_size)
	{
		return -1;
	}
	int32_t cpy_cnt = 0;
	char *pRet = 0;
	
	cpy_cnt = dst_size > strlen(srcbuf) + 1?strlen(srcbuf):dst_size - 1;
	pRet = strncpy(dstbuf, srcbuf, cpy_cnt);
	dstbuf[cpy_cnt] = '\0';

	if (pRet)
	{
		return 0;
	}
	return -2;
}



