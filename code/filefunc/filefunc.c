#include <cJSON.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "filefunc.h"
#include "cross_platform.h"


int32_t FILE_FUNC_Get_Json_Form_File(const char *path_name, cJSON **out_json){
	int32_t file_size = 0;
	char *file_buf = NULL;
	FILE *fp;

	if (!path_name || !out_json)
	{
		return FILE_FUNC_ERR_INVALIDPARAM;
	}
	fp = fopen(path_name, "r");
	if (!fp)
	{
		return FILE_FUNC_ERR_OPENFILE;
	}
	fseek(fp, SEEK_END, 0);
	file_size = ftell(fp);
	if (!file_size)
	{
		fclose(fp);
		return FILE_FUNC_ERR_EMPTY; 
	}
	fseek(fp, SEEK_SET, 0);
	file_buf = (char *)malloc(file_size);
	if (!file_buf)
	{
		fclose(fp);
		return FILE_FUNC_ERR_OVERLOW; 
	}
	fread(file_buf, file_size, 1, fp);
	free(file_buf);
	*out_json = cJSON_Parse(file_buf);
	if (!(*out_json))
	{
		fclose(fp);
		return FILE_FUNC_ERR_PARSE_JSON;
	}
	fclose(fp);
	return FILE_FUNC_OK;
}

int32_t FILE_FUNC_Put_Json_Form_File(const char *path_name, cJSON *in_json){
	FILE *fp;
	char *ptr;

	if (!path_name || !in_json)
	{
		return FILE_FUNC_ERR_INVALIDPARAM;
	}

	ptr = cJSON_Print(in_json);
	if (!ptr)
	{
		return FILE_FUNC_ERR_PARSE_JSON;
	}
	fp = fopen(path_name, "w+");
	if (!fp)
	{
		free(ptr);
		return FILE_FUNC_ERR_OPENFILE;
	}
	fwrite(ptr, strlen(ptr), 1, fp);
	free(ptr);
	fclose(fp);

	return FILE_FUNC_OK;
}