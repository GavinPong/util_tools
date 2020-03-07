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
/*
关键字说明：
1、模块ID：模块分配人员在分配模块时，在err.h中的gavin_mode_id_e指定的模块ID
2、错误ID：在err.h中的gavin_err_level_id_e值以及模块开发人员在自己的模块代码中新增的错误ID；
3、错误值：GAVIN_DEF_ERR

err接口使用流程：
1、没有不同于通用错误的其他错误信息的场景：
| --根据给定的模块ID 为模块定义错误信息（参考err_dummy.h）；
| --调用gavin_err_get_err_reason根据错误值获取具体的错误信息；


2、有不同于通用错误的其他错误信息的场景：
a、定义自己模块特有错误ID，起始值必须是大于gavin_err_code_id_e中的最大值（参考err_dummy.h）；
b、根据给定的模块ID和错误ID为模块定义错误值信息（参考err_dummy.h）；
c、在模块上下文管理字段中添加list_head用于关联自己模块特有得出错误
d、在模块初始时调用gavin_err_info_list_add_node将自己模块特有错误加入到c中的list_head链表中；
e、定义定义get_custom_err_info_cb_t回调函数，里面完成根据错误ID在c的list_head中获取模块自定义错误信息的逻辑；
f、调用gavin_err_register_output_func将get_custom_err_info_cb_t回调函数和模块ID注册到err.h模块中
g、调用gavin_err_get_err_reason根据错误值获取具体的错误信息；
*/


#ifndef __GAVIN_ERROR_H__
#define __GAVIN_ERROR_H__

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
	GAVIN_MODE_ID_INT_MOD_REC_MUXER,		//intmodule rec muxer模块
	GAVIN_MODE_ID_INT_MOD_HISI,		//intmodule hisi模块
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
** member must be assigned for compatibility
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
	GAVIN_ERR_MODULE_ID		= 19,/* invalid device id */

	GAVIN_ERR_PARSE_JSON	= 20,/* parse json failed */
						  
	GAVIN_ERR_JSON_OBJ_EMPTY = 21,/* json data is empty */
	GAVIN_ERR_JSON_SET_EMPTY = 22,/* json set is empty */

	GAVIN_ERR_MATCH			= 23,/* match failure */
	GAVIN_ERR_INITED		= 24,/* Already initialized */
					  
	GAVIN_ERR_MUXTYPE		= 25,/* invalid rec muxer type  */
						  
	GAVIN_ERR_FRAME_RATE	= 26,/* frame rate anomaly*/
						  

    GAVIN_ERR_BUTT          = 63,/* maxium code, private error code of all modules
                              ** must be greater than it                      */
}gavin_err_code_id_e;

/* 
** following is an example for defining error code of VDA module
** #define GAVIN_ERR_MD_INVALID_CHNID GAVIN_DEF_ERR(GAVIN_ID_PUBLIC, GAVIN_ERR_LEVEL_ERROR, GAVIN_ERR_ILLEGAL_PARAM)
**
*/

//err info table
#define ERR_INVALID_DEVID_STR			"invalid device id"
#define ERR_INVALID_CHNID_STR			"invalid channel id"
#define ERR_ILLEGAL_PARAM_STR			"illegal param"
#define ERR_EXIST_STR					"resource exist"
#define ERR_UNEXIST_STR					"resource unexist"
#define ERR_NULL_PTR_STR				"null point"
#define ERR_NOT_CONFIG_STR				"not configure attribute before used"
#define ERR_NOT_SUPPORT_STR				"operation or type is not supported by NOW"
#define ERR_NOT_PERM_STR				"operation is not permitted"
#define ERR_NOT_NOMEM_STR				"failure caused by malloc memory"
#define ERR_NOT_NOBUF_STR				"failure caused by malloc buffer"
#define ERR_BUF_EMPTY_STR				"no data in buffer"
#define ERR_BUF_FULLE_STR				"no buffer for new data"
#define ERR_SYS_NOTREADY_STR			"System is not ready,maybe not initialed or loaded"
#define ERR_BADADDR_STR					" bad address"
#define ERR_BUSY_STR					"resource is busy"
#define ERR_MODULE_ID_STR				"invalid module id"
#define ERR_PARSE_JSON_STR				"parse json failed"
#define ERR_JSON_OBJ_EMPTY_STR			"json data is empty"
#define ERR_JSON_SET_EMPTY_STR				"json set is empty"
#define ERR_MATCH_STR					"match failure"
#define ERR_INITED_STR					"Already initialized"
#define ERR_INVALID_MUXTYPE_STR			"invalid rec muxer type"
#define ERR_FRAME_RATE_STR				"frame rate anomaly"

typedef int32_t (*get_custom_err_info_cb_t)(int32_t err_value, int8_t *out_buf, int32_t buf_size);
int32_t gavin_err_get_err_reason(int32_t err_value, int8_t *out_buf, int32_t buf_size);
int32_t gavin_err_unregister_output_func(int32_t module_id);
int32_t gavin_err_register_output_func(int32_t module_id, const int8_t *module_name, get_custom_err_info_cb_t get_err_info_cb);

//**********一套给外部管理错误ID跟错误详细描述的接口start***************/
int32_t gavin_err_info_list_add_node(struct list_head *list_head, int32_t err_id, const int8_t *err_info);
int32_t gavin_err_info_list_del_node(struct list_head *list_head, int32_t err_id);
int32_t gavin_err_info_list_find_err_info(struct list_head *list_head, int32_t err_id, int8_t *out_buf, int32_t buf_size);
int32_t gavin_err_info_list_init(struct list_head *list_head);
int32_t gavin_err_info_list_uninit(struct list_head *list_head);

//**********一套给外部管理错误ID跟错误详细描述的接口 end***************/
#ifdef __cplusplus
};
#endif

#endif