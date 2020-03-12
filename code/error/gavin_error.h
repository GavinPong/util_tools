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
#define GAVIN_ERR_IVIDEO_APPID  (0x80000000L + 0x01000000L)		//SMARTVC APP ID

#define GAVIN_DEF_ERR( module, level, errid) \
	((int32_t)( (GAVIN_ERR_IVIDEO_APPID) | ((module) << 16 ) | ((level)<<13) | (errid) ))

#define GAVIN_GET_MODID_BY_ERR(err_value) ((int32_t)( (err_value) & 0x00ff0000) >> 16)
#define GAVIN_GET_EID_BY_ERR(err_value) ((int32_t)( (err_value) & 0x00001fff))

#ifdef __cplusplus
extern "C"{
#endif

typedef enum _gavin_mode_id_e{
	GAVIN_MODID_MIN = 0,

	GAVIN_MODID_PUBLIC,			//通用软件架构中报错
	GAVIN_MODID_MODCTR,		//intmodule ctrl模块
	GAVIN_MODID_DUMMY,		//intmodule dummy模块
	GAVIN_MODID_RECMUXER,		//intmodule rec muxer模块
	GAVIN_MODID_HISI,		//intmodule hisi模块
	
	GAVIN_MODID_MAX,
}gavin_mode_id_e;


/* NOTE! the following defined all common error code,
** all module must reserved 0~63 for their common error code
** member must be assigned for compatibility
*/
typedef enum _gavin_err_code_id_e
{
	GAVIN_OK 				= 0,
	GAVIN_FALSE 			= 1,
	GAVIN_EINVADEVID 		= 2, /* invlalid device ID                           */
    GAVIN_EINVACHNID 		= 3, /* invlalid channel ID                          */
    GAVIN_EILLEGALPARAM 	= 4, /* at lease one parameter is illagal
                               * eg, an illegal enumeration value             */
    GAVIN_EEXIST         	= 5, /* resource exists                              */
    GAVIN_EUNEXIST       	= 6, /* resource unexists                            */
    
    GAVIN_ENULLPTR      	= 7, /* using a NULL point                           */
    
    GAVIN_ENOTCONFIG    	= 8, /* try to enable or initialize system, device
                              ** or channel, before configing attribute       */

    GAVIN_ENOTSUPPORT   	= 9, /* operation or type is not supported by NOW    */
    GAVIN_ENOTPERM      	= 10, /* operation is not permitted
                              ** eg, try to change static attribute           */

    GAVIN_ENOMEM         	= 11,/* failure caused by malloc memory              */
    GAVIN_ENOBUF         	= 12,/* failure caused by malloc buffer              */

    GAVIN_EBUFEMPTY     	= 13,/* no data in buffer                            */
    GAVIN_EBUFFULL      	= 14,/* no buffer for new data                       */

    GAVIN_ESYSNOTREADY  	= 15,/* System is not ready,maybe not initialed or 
                              ** loaded. Returning the error code when opening
                              ** a device file failed.                        */

    GAVIN_EBADADDR       	= 16,/* bad address, 
                              ** eg. used for copy_from_user & copy_to_user   */

    GAVIN_EBUSY          	= 17,/* resource is busy, 
                              ** eg. destroy a venc chn without unregister it */
	GAVIN_EMODULEID			= 18,/* invalid device id */

	GAVIN_EPARSEJSON		= 19,/* parse json failed */
						  
	GAVIN_EJSONOBJEMPTY 	= 20,/* json data is empty */
	GAVIN_EJSONSETEMPTY 	= 21,/* json set is empty */

	GAVIN_EMATCH			= 22,/* match failure */
	GAVIN_EUNINITED			= 23,/* no initialized */
	GAVIN_EINITED			= 23,/* Already initialized */
					  
	GAVIN_EMUXTYPE			= 24,/* invalid rec muxer type  */
						  
	GAVIN_EFRAMERATE		= 25,/* frame rate anomaly*/
						  

    GAVIN_EBUTT          = 63,/* maxium code, private error code of all modules
                              ** must be greater than it                      */
}gavin_err_code_id_e;

/* 
** following is an example for defining error code of VDA module
** #define GAVIN_ERR_MD_INVALID_CHNID GAVIN_DEF_ERR(GAVIN_ID_PUBLIC, GAVIN_ERR_LEVEL_ERROR, GAVIN_ERR_ILLEGAL_PARAM)
**
*/

//err info table
#define EINVADEVIDSTR			"invalid device id"
#define EINVACHNIDSTR			"invalid channel id"
#define EILLEGALPARAMSTR			"illegal param"
#define EEXISTSTR					"resource exist"
#define EUNEXISTSTR					"resource unexist"
#define ENULLPTRSTR				"null point"
#define ENOTCONFIGSTR				"not configure attribute before used"
#define ENOTSUPPORTSTR				"operation or type is not supported by NOW"
#define ENOTPERMSTR				"operation is not permitted"
#define ENOTNOMEMSTR				"failure caused by malloc memory"
#define ENOTNOBUFSTR				"failure caused by malloc buffer"
#define EBUFEMPTYSTR				"no data in buffer"
#define EBUFFULLESTR				"no buffer for new data"
#define ESYSNOTREADYSTR			"System is not ready,maybe not initialed or loaded"
#define EBADADDRSTR					" bad address"
#define EBUSYSTR					"resource is busy"
#define EMODULEIDSTR				"invalid module id"
#define EPARSEJSONSTR				"parse json failed"
#define EJSONOBJEMPTYSTR			"json data is empty"
#define EJSONSETEMPTYSTR				"json set is empty"
#define EMATCHSTR					"match failure"
#define EUNINITEDSTR					"No initialized"
#define EINITEDSTR					"Already initialized"
#define EINVAMUXTYPESTR			"invalid rec muxer type"
#define EFRAMERATESTR				"frame rate anomaly"

int32_t gavin_err_init();
int32_t gavin_err_uninit();
typedef int32_t (*get_custom_err_info_cb_t)(int32_t i32ErrValue, int8_t *pOutBuf, int32_t i32BufSize);
int32_t gavin_err_get_err_reason(int32_t i32ErrValue, int8_t *pOutBuf, int32_t i32BufSize);

//**********一套给外部管理错误ID跟错误详细描述的接口start***************/
int32_t gavin_err_unregister_output_func(int32_t i32ErrValue);
int32_t gavin_err_register_output_func(int32_t i32ModuleID, const int8_t *pstrModuleName, get_custom_err_info_cb_t getErrInfoCb);
int32_t gavin_err_info_list_add_node(list_head_t *pstListHead, int32_t i32ErrID, const int8_t *pErrInfo);
int32_t gavin_err_info_list_del_node(list_head_t *pstListHead, int32_t i32ErrID);
int32_t gavin_err_info_list_find_err_info(list_head_t *pstListHead, int32_t i32ErrID, int8_t *pOutBuf, int32_t i32BufSize);
int32_t gavin_err_info_list_init(list_head_t *pstListHead);
int32_t gavin_err_info_list_uninit(list_head_t *pstListHead);

//**********一套给外部管理错误ID跟错误详细描述的接口 end***************/
#ifdef __cplusplus
};
#endif

#endif