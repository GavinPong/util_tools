/**
 *  base64编码、解码实现
 *       C语言源代码
 *
 *   注意：请使用gcc编译
 *
 *             叶剑飞
 *
 * 
 *
 *  使用说明：
 *      命令行参数说明：若有“-d”参数，则为base64解码，否则为base64编码。
 *                      若有“-o”参数，后接文件名，则输出到标准输出文件。
 *      输入来自标准输入stdin，输出为标准输出stdout。可重定向输入输出流。
 *
 *        base64编码：输入任意二进制流，读取到文件读完了为止（键盘输入则遇到文件结尾符为止）。
 *                    输出纯文本的base64编码。
 *
 *        base64解码：输入纯文本的base64编码，读取到文件读完了为止（键盘输入则遇到文件结尾符为止）。
 *                    输出原来的二进制流。
 *
 */

#include "cross_platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base64.h"


const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *base64_encode(const unsigned char *bindata, char *base64, int32_t binlength)
{
    int32_t i, j;
    uint8_t current;

    for ( i = 0, j = 0 ; i < binlength ; i += 3 )
    {
        current = (bindata[i] >> 2) ;
        current &= (uint8_t)0x3F;
        base64[j++] = base64char[(int32_t)current];

        current = ( (uint8_t)(bindata[i] << 4 ) ) & ( (uint8_t)0x30 ) ;
        if ( i + 1 >= binlength )
        {
            base64[j++] = base64char[(int32_t)current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ( (uint8_t)(bindata[i+1] >> 4) ) & ( (uint8_t) 0x0F );
        base64[j++] = base64char[(int32_t)current];

        current = ( (uint8_t)(bindata[i+1] << 2) ) & ( (uint8_t)0x3C ) ;
        if ( i + 2 >= binlength )
        {
            base64[j++] = base64char[(int32_t)current];
            base64[j++] = '=';
            break;
        }
        current |= ( (uint8_t)(bindata[i+2] >> 6) ) & ( (uint8_t) 0x03 );
        base64[j++] = base64char[(int32_t)current];

        current = ( (uint8_t)bindata[i+2] ) & ( (uint8_t)0x3F ) ;
        base64[j++] = base64char[(int32_t)current];
    }
    base64[j] = '\0';
    return base64;
}

int32_t base64_decode(const char *base64, unsigned char *bindata)
{
    int32_t i, j;
    uint8_t k;
    uint8_t temp[4];
    for ( i = 0, j = 0; base64[i] != '\0' ; i += 4 )
    {
        memset( temp, 0xFF, sizeof(temp) );
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i] )
                temp[0]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+1] )
                temp[1]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+2] )
                temp[2]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+3] )
                temp[3]= k;
        }

        bindata[j++] = ((uint8_t)(((uint8_t)(temp[0] << 2))&0xFC)) |
                ((uint8_t)((uint8_t)(temp[1]>>4)&0x03));
        if ( base64[i+2] == '=' )
            break;

        bindata[j++] = ((uint8_t)(((uint8_t)(temp[1] << 4))&0xF0)) |
                ((uint8_t)((uint8_t)(temp[2]>>2)&0x0F));
        if ( base64[i+3] == '=' )
            break;

        bindata[j++] = ((uint8_t)(((uint8_t)(temp[2] << 6))&0xF0)) |
                ((uint8_t)(temp[3]&0x3F));
    }
    return j;
}