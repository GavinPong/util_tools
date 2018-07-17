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
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	if (!file_size)
	{
		fclose(fp);
		return FILE_FUNC_ERR_EMPTY; 
	}
	fseek(fp, 0, SEEK_SET);
	file_buf = (char *)malloc(file_size);
	if (!file_buf)
	{
		fclose(fp);
		return FILE_FUNC_ERR_OVERLOW; 
	}
	fread(file_buf, file_size, 1, fp);
	*out_json = cJSON_Parse(file_buf);
	if (!(*out_json))
	{
		free(file_buf);
		fclose(fp);
		return FILE_FUNC_ERR_PARSE_JSON;
	}
	free(file_buf);
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