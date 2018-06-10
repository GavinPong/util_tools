#ifndef __FILEFUNC_H__
#define __FILEFUNC_H__
#include <cJSON.h>

#define FILE_FUNC_OK 1
#define FILE_FUNC_FALSE 0
#define FILE_FUNC_ERR_INVALIDPARAM	-1			//参数异常
#define FILE_FUNC_ERR_OPENFILE		-2			//文件打开失败
#define FILE_FUNC_ERR_EMPTY			-3			//文件为空
#define FILE_FUNC_ERR_OVERLOW		-4			//内存不够
#define FILE_FUNC_ERR_PARSE_JSON	-5			//解析json失败


#ifdef __cplusplus
extern "C"{
#endif

int32_t FILE_FUNC_Get_Json_Form_File(const char *path_name, cJSON **out_json);
int32_t FILE_FUNC_Put_Json_Form_File(const char *path_name, cJSON *in_json);

#ifdef __cplusplus
}
#endif

#endif