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
�ؼ���˵����
1��ģ��ID��ģ�������Ա�ڷ���ģ��ʱ����err.h�е�gavin_mode_id_eָ����ģ��ID
2������ID����err.h�е�gavin_err_level_id_eֵ�Լ�ģ�鿪����Ա���Լ���ģ������������Ĵ���ID��
3������ֵ��GAVIN_DEF_ERR

err�ӿ�ʹ�����̣�
1��û�в�ͬ��ͨ�ô��������������Ϣ�ĳ�����
| --���ݸ�����ģ��ID Ϊģ�鶨�������Ϣ���ο�err_dummy.h����
| --����gavin_err_get_err_reason���ݴ���ֵ��ȡ����Ĵ�����Ϣ��


2���в�ͬ��ͨ�ô��������������Ϣ�ĳ�����
a�������Լ�ģ�����д���ID����ʼֵ�����Ǵ���gavin_err_code_id_e�е����ֵ���ο�err_dummy.h����
b�����ݸ�����ģ��ID�ʹ���IDΪģ�鶨�����ֵ��Ϣ���ο�err_dummy.h����
c����ģ�������Ĺ����ֶ������list_head���ڹ����Լ�ģ�����еó�����
d����ģ���ʼʱ����gavin_err_info_list_add_node���Լ�ģ�����д�����뵽c�е�list_head�����У�
e�����嶨��get_custom_err_info_cb_t�ص�������������ɸ��ݴ���ID��c��list_head�л�ȡģ���Զ��������Ϣ���߼���
f������gavin_err_register_output_func��get_custom_err_info_cb_t�ص�������ģ��IDע�ᵽerr.hģ����
g������gavin_err_get_err_reason���ݴ���ֵ��ȡ����Ĵ�����Ϣ��
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

//�µ�APP��������������0x01000000L + 0x1L
#define GAVIN_ERR_IVIDEO_APPID  (0x80000000L + 0x01000000L)		//SMARTVC APP ID

#define GAVIN_DEF_ERR( module, level, errid) \
	((int32_t)( (GAVIN_ERR_IVIDEO_APPID) | ((module) << 16 ) | ((level)<<13) | (errid) ))

#define GAVIN_GET_MODID_BY_ERR(err_value) ((int32_t)( (err_value) & 0x00ff0000) >> 16)
#define GAVIN_GET_EID_BY_ERR(err_value) ((int32_t)( (err_value) & 0x00001fff))

#ifdef __cplusplus
extern "C"{
#endif

typedef enum _gavin_mode_id_e{
	GAVIN_MODE_ID_MIN = 0,

	GAVIN_MID_PUBLIC,			//ͨ������ܹ��б���
	GAVIN_MID_MODDUMMY,			//intmodule dummyģ��
	GAVIN_MID_RECMUXER,			//intmodule rec muxerģ��
	GAVIN_MID_HISI,				//intmodule hisiģ��
	GAVIN_MODE_ID_MAX,
}gavin_mode_id_e;

typedef enum _gavin_elevel_e
{
	GAVIN_ELEVEL_DEBUG = 0,  /* debug-level                                  */
	GAVIN_ELEVEL_INFO,       /* informational                                */
	GAVIN_ELEVEL_NOTICE,     /* normal but significant condition             */
	GAVIN_ELEVEL_WARNING,    /* warning conditions                           */
	GAVIN_ELEVEL_ERROR,      /* error conditions                             */
	GAVIN_ELEVEL_CRIT,       /* critical conditions                          */
	GAVIN_ELEVEL_ALERT,      /* action must be taken immediately             */
	GAVIN_ELEVEL_FATAL,      /* just for compatibility with previous version */
	GAVIN_ELEVEL_BUTT
}gavin_elevel_e;


/* NOTE! the following defined all common error code,
** all module must reserved 0~63 for their common error code
** member must be assigned for compatibility
*/
typedef enum _gavin_ecode_id_e
{
    GAVIN_EINVADEVID = 1, /* invlalid device ID                           */
    GAVIN_EINVACHNID = 2, /* invlalid channel ID                          */
    GAVIN_EILLEGALPARAM = 3, /* at lease one parameter is illagal
                               * eg, an illegal enumeration value             */
    GAVIN_EEXIST         = 4, /* resource exists                              */
    GAVIN_EUNEXIST       = 5, /* resource unexists                            */
    
    GAVIN_ENULLPTR      = 6, /* using a NULL point                           */
    
    GAVIN_ENOTCONFIG    = 7, /* try to enable or initialize system, device
                              ** or channel, before configing attribute       */

    GAVIN_ENOTSUPPORT   = 8, /* operation or type is not supported by NOW    */
    GAVIN_ENOTPERM      = 9, /* operation is not permitted
                              ** eg, try to change static attribute           */

    GAVIN_ENOMEM         = 12,/* failure caused by malloc memory              */
    GAVIN_ENOBUF         = 13,/* failure caused by malloc buffer              */

    GAVIN_EBUFEMPTY     = 14,/* no data in buffer                            */
    GAVIN_EBUFFULL      = 15,/* no buffer for new data                       */

    GAVIN_ESYSNOTREADY  = 16,/* System is not ready,maybe not initialed or 
                              ** loaded. Returning the error code when opening
                              ** a device file failed.                        */

    GAVIN_EBADADDR       = 17,/* bad address, 
                              ** eg. used for copy_from_user & copy_to_user   */

    GAVIN_EBUSY          = 18,/* resource is busy, 
                              ** eg. destroy a venc chn without unregister it */
	GAVIN_EMODULEID		= 19,/* invalid device id */

	GAVIN_EPARSEJSON	= 20,/* parse json failed */
						  
	GAVIN_EJSONOBJEMPTY = 21,/* json data is empty */
	GAVIN_EJSONSETEMPTY = 22,/* json set is empty */

	GAVIN_EMATCH			= 23,/* match failure */
	GAVIN_EINITED		= 24,/* Already initialized */
					  
	GAVIN_EMUXTYPE		= 25,/* invalid rec muxer type  */
						  
	GAVIN_EFRAMERATE	= 26,/* frame rate anomaly*/
						  

    GAVIN_EBUTT          = 63,/* maxium code, private error code of all modules
                              ** must be greater than it                      */
}gavin_ecode_id_e;

/* 
** following is an example for defining error code of VDA module
** #define GAVIN_ERR_MD_INVALID_CHNID GAVIN_DEF_ERR(GAVIN_ID_PUBLIC, GAVIN_ERR_LEVEL_ERROR, GAVIN_ERR_ILLEGAL_PARAM)
**
*/

//err info table
#define EINVALID_DEVID_STR			"invalid device id"
#define EINVALID_CHNID_STR			"invalid channel id"
#define EILLEGAL_PARAM_STR			"illegal param"
#define EEXIST_STR					"resource exist"
#define EUNEXIST_STR				"resource unexist"
#define ENULL_PTR_STR				"null point"
#define ENOT_CONFIG_STR				"not configure attribute before used"
#define ENOT_SUPPORT_STR			"operation or type is not supported by NOW"
#define ENOT_PERM_STR				"operation is not permitted"
#define ENOT_NOMEM_STR				"failure caused by malloc memory"
#define ENOT_NOBUF_STR				"failure caused by malloc buffer"
#define EBUF_EMPTY_STR				"no data in buffer"
#define EBUF_FULLE_STR				"no buffer for new data"
#define ESYS_NOTREADY_STR			"System is not ready,maybe not initialed or loaded"
#define EBADADDR_STR				" bad address"
#define EBUSY_STR					"resource is busy"
#define EMODULE_ID_STR				"invalid module id"
#define EPARSE_JSON_STR				"parse json failed"
#define EJSON_OBJ_EMPTY_STR			"json data is empty"
#define EJSON_SET_EMPTY_STR			"json set is empty"
#define EMATCH_STR					"match failure"
#define EINITED_STR					"Already initialized"
#define EINVALID_MUXTYPE_STR		"invalid rec muxer type"
#define EFRAME_RATE_STR				"frame rate anomaly"

int32_t gavin_err_init();
int32_t gavin_err_uninit();
typedef int32_t (*get_custom_err_info_cb_t)(int32_t err_value, int8_t *out_buf, int32_t buf_size);
int32_t gavin_err_get_err_reason(int32_t err_value, int8_t *out_buf, int32_t buf_size);

//**********һ�׸��ⲿ�������ID��������ϸ�����Ľӿ�start***************/
int32_t gavin_err_unregister_output_func(int32_t module_id);
int32_t gavin_err_register_output_func(int32_t module_id, const int8_t *module_name, get_custom_err_info_cb_t get_err_info_cb);
int32_t gavin_err_info_list_add_node(struct list_head *list_head, int32_t err_id, const int8_t *err_info);
int32_t gavin_err_info_list_del_node(struct list_head *list_head, int32_t err_id);
int32_t gavin_err_info_list_find_err_info(struct list_head *list_head, int32_t err_id, int8_t *out_buf, int32_t buf_size);
int32_t gavin_err_info_list_init(struct list_head *list_head);
int32_t gavin_err_info_list_uninit(struct list_head *list_head);

//**********һ�׸��ⲿ�������ID��������ϸ�����Ľӿ� end***************/
#ifdef __cplusplus
};
#endif

#endif