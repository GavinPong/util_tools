#ifndef __BASE64_H__
#define __BASE64_H__

#include "cross_platform.h"

#ifdef __cplusplus
extern "C"
{
#endif

char *base64_encode(const unsigned char *bindata, char *base64, int32_t binlength);
int32_t base64_decode(const char *base64, unsigned char *bindata);

#ifdef __cplusplus
}
#endif
#endif