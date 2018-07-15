/******************************************************************************

Copyright (C), 2018-2035, xxxxx Tech. Co., Ltd.

******************************************************************************
File Name     : gavin_error.h.h
Version       : Initial Draft
Author        : xxxxx multimedia software group
Created       : 2018/5/13
Last Modified :
Description   :
Function List :
History       :
1.Date        : 2018/5/13
  Author      : gavinpong
  Modification: Created file

******************************************************************************/

#ifndef __GAVIN_ERROR_H__
#define __GAVIN_ERROR_H__

#include "cross_platform.h"
#include "list.h"

/******************************************************************************
|----------------------------------------------------------------|
| 1 |   APP_ID   |   MOD_ID    | ERR_LEVEL |   ERR_ID            |
|----------------------------------------------------------------|
|<--><--7bits----><----8bits---><--3bits---><------13bits------->|
******************************************************************************/

#define GAVIN_SUCCESS  0
#define GAVIN_FAILURE  (-1)

//新的APP都依次往下增加0x01000000L + 0x1L
#define GAVIN_ERR_SMARTVC_APPID  (0x80000000L + 0x01000000L)		//SMARTVC APP ID

#define GAVIN_DEF_ERR( module, level, errid) \
	((int32_t)( (GAVIN_ERR_SMARTVC_APPID) | ((module) << 16 ) | ((level)<<13) | (errid) ))

#define GAVIN_GET_MODULE_ID_BY_ERR(err_value) ((int32_t)( (err_value) & 0x00ff0000) >> 16)
#define GAVIN_GET_ERR_ID_BY_ERR(err_value) ((int32_t)( (err_value) & 0x00001fff))

#ifdef __cplusplus
extern "C"{
#endif

typedef enum _gavin_mode_id_e{
	GAVIN_MODE_ID_MIN = 0,

	GAVIN_MODE_ID_PUBLIC,			//通用软件架构中报错
	GAVIN_MODE_ID_INT_MOD_CTR,		//intmodule ctrl模块
	GAVIN_MODE_ID_INT_MOD_DUMMY,		//intmodule dummy模块
	
	GAVIN_MODE_ID_MAX,
}gavin_mode_id_e;

typedef enum _gavin_err_level_id_e
{
	GAVIN_ERR_LEVEL_DEBUG = 0,  /* debug-level                                  */
	GAVIN_ERR_LEVEL_INFO,       /* informational                                */
	GAVIN_ERR_LEVEL_NOTICE,     /* normal but significant condition             */
	GAVIN_ERR_LEVEL_WARNING,    /* warning conditions                           */
	GAVIN_ERR_LEVEL_ERROR,      /* error conditions                             */
	GAVIN_ERR_LEVEL_CRIT,       /* critical conditions                          */
	GAVIN_ERR_LEVEL_ALERT,      /* action must be taken immediately             */
	GAVIN_ERR_LEVEL_FATAL,      /* just for compatibility with previous version */
	GAVIN_ERR_LEVEL_BUTT
}gavin_err_level_id_e;


/* NOTE! the following defined all common error code,
** all module must reserved 0~63 for their common error code
*/
typedef enum _gavin_err_code_id_e
{
    GAVIN_ERR_INVALID_DEVID = 1, /* invlalid device ID                           */
    GAVIN_ERR_INVALID_CHNID = 2, /* invlalid channel ID                          */
    GAVIN_ERR_ILLEGAL_PARAM = 3, /* at lease one parameter is illagal
                               * eg, an illegal enumeration value             */
    GAVIN_ERR_EXIST         = 4, /* resource exists                              */
    GAVIN_ERR_UNEXIST       = 5, /* resource unexists                            */
    
    GAVIN_ERR_NULL_PTR      = 6, /* using a NULL point                           */
    
    GAVIN_ERR_NOT_CONFIG    = 7, /* try to enable or initialize system, device
                              ** or channel, before configing attribute       */

    GAVIN_ERR_NOT_SUPPORT   = 8, /* operation or type is not supported by NOW    */
    GAVIN_ERR_NOT_PERM      = 9, /* operation is not permitted
                              ** eg, try to change static attribute           */

    GAVIN_ERR_NOMEM         = 12,/* failure caused by malloc memory              */
    GAVIN_ERR_NOBUF         = 13,/* failure caused by malloc buffer              */

    GAVIN_ERR_BUF_EMPTY     = 14,/* no data in buffer                            */
    GAVIN_ERR_BUF_FULL      = 15,/* no buffer for new data                       */

    GAVIN_ERR_SYS_NOTREADY  = 16,/* System is not ready,maybe not initialed or 
                              ** loaded. Returning the error code when opening
                              ** a device file failed.                        */

    GAVIN_ERR_BADADDR       = 17,/* bad address, 
                              ** eg. used for copy_from_user & copy_to_user   */

    GAVIN_ERR_BUSY          = 18,/* resource is busy, 
                              ** eg. destroy a venc chn without unregister it */

    GAVIN_ERR_BUTT          = 63,/* maxium code, private error code of all modules
                              ** must be greater than it                      */
}gavin_err_code_id_e;

/* 
** following is an example for defining error code of VDA module
** #define GAVIN_ERR_MD_INVALID_CHNID GAVIN_DEF_ERR(GAVIN_ID_PUBLIC, GAVIN_ERR_LEVEL_ERROR, GAVIN_ERR_ILLEGAL_PARAM)
**
*/

//err info table
#define INVALID_DEVID		"invalid device id"
#define INVALID_CHNID		"invalid channel id"
#define ILLEGAL_PARAM		"illegal param"
#define EXIST				"resource exist"
#define UNEXIST				"resource unexist"
#define NULL_PTR			"null point"
#define NOT_CONFIG			"not configure attribute before used"
#define NOT_SUPPORT			"operation or type is not supported by NOW"
#define NOT_PERM			"operation is not permitted"
#define NOT_NOMEM			"failure caused by malloc memory"
#define NOT_NOBUF			"failure caused by malloc buffer"
#define BUF_EMPTY			"no data in buffer"
#define BUF_FULLE			"no buffer for new data"
#define SYS_NOTREADY		"System is not ready,maybe not initialed or loaded"
#define BADADDR				" bad address"
#define BUSY				"resource is busy"
#define ERR_MODULE_ID		"invalid device id"
#define ERR_PARSE_JSON		"parse json failed"
#define JSON_OBJ_EMPTY		"json data is empty"
#define SET_EMPTY			"json set is empty"
#define MATCH				"match failure"
#define INITED				"Already initialized"

typedef int (*get_err_info_cb_t)(int err_value, char *out_buf, int buf_size);
int32_t gavin_err_get_err_reason(int32_t err_value, char *out_buf, int32_t buf_size);
int32_t gavin_err_unregister_output_func(int32_t module_id);
int32_t gavin_err_register_output_func(int32_t module_id, const char*module_name, get_err_info_cb_t get_err_info_cb);

//**********一套给外部管理错误ID跟错误详细描述的接口start***************/
int32_t gavin_err_info_list_add_node(struct list_head *list_head, int32_t err_id, const char *err_info);
int32_t gavin_err_info_list_del_node(struct list_head *list_head, int32_t err_id);
int32_t gavin_err_info_list_find_err_info(struct list_head *list_head, int32_t err_id, char *out_buf, int32_t buf_size);
int32_t gavin_err_info_list_init(struct list_head *list_head);
int32_t gavin_err_info_list_uninit(struct list_head *list_head);

//**********一套给外部管理错误ID跟错误详细描述的接口 end***************/
#ifdef __cplusplus
};
#endif

#endif