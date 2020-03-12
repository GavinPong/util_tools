#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"
#include "gavin_error.h"



#define get_comm_err_info(i32ErrValue, i32ErrID) do \
{\
	i32ErrID = GAVIN_GET_EID_BY_ERR((i32ErrValue));\
	if (i32ErrID <= GAVIN_EFRAMERATE)\
	{\
		int32_t i32Len = strlen(g_common_err_info_set[i32ErrID].m_pInfoPtr);\
		i32Len = (i32Len+1) > i32BufSize ? i32BufSize : i32Len;\
		strncpy(pOutBuf, g_common_err_info_set[i32ErrID].m_pInfoPtr, i32Len);\
		pOutBuf[i32Len] = '\0';\
		return GAVIN_SUCCESS;\
	}\
} while (0)

typedef struct _err_info_s{
	int8_t m_name[32];
	get_custom_err_info_cb_t m_getErrInfoCb;
}err_info_t;

typedef struct _err_info_list_node_s{
	list_head_t m_list;
	int32_t m_i32ErrId;
	int8_t *m_pInfoPtr;
}err_info_list_node_t;

static err_info_t err_info_set[GAVIN_MODID_MAX] = {0};
static err_info_list_node_t g_common_err_info_set[GAVIN_EFRAMERATE] = {0};

int32_t gavin_err_init()
{
	//load err ionfo for commom error
	int32_t i = 0;
	int32_t total_cnt = sizeof(g_common_err_info_set)/sizeof(g_common_err_info_set[0]);//Currently defined maximum error value
	for (; i < total_cnt; i++)
	{
		g_common_err_info_set[i].m_i32ErrId = i;
		if (GAVIN_OK == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)"success";
		if (GAVIN_FALSE == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)"failure";
		if (GAVIN_EINVADEVID == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EINVADEVIDSTR;
		else if(GAVIN_EINVACHNID == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EINVACHNIDSTR;
		else if (GAVIN_EILLEGALPARAM == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EILLEGALPARAMSTR;
		else if (GAVIN_EEXIST == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EEXISTSTR;
		else if (GAVIN_EUNEXIST == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EUNEXISTSTR;
		else if (GAVIN_ENULLPTR == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)ENULLPTRSTR;
		else if (GAVIN_ENOTCONFIG == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)ENOTCONFIGSTR;
		else if (GAVIN_ENOTSUPPORT == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)ENOTSUPPORTSTR;
		else if (GAVIN_ENOTPERM == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)ENOTPERMSTR;
		else if (GAVIN_ENOMEM == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)ENOTNOMEMSTR;
		else if (GAVIN_ENOBUF == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)ENOTNOBUFSTR;
		else if (GAVIN_EBUFEMPTY == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EBUFEMPTYSTR;
		else if (GAVIN_EBUFFULL == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EBUFFULLESTR;
		else if (GAVIN_ESYSNOTREADY == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)ESYSNOTREADYSTR;
		else if (GAVIN_EBADADDR == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EBADADDRSTR;
		else if (GAVIN_EBUSY == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EBUSYSTR;
		else if (GAVIN_EMODULEID == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EMODULEIDSTR;
		else if (GAVIN_EPARSEJSON == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EPARSEJSONSTR;
		else if (GAVIN_EJSONOBJEMPTY == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EJSONOBJEMPTYSTR;
		else if (GAVIN_EJSONSETEMPTY == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EJSONSETEMPTYSTR;
		else if (GAVIN_EMATCH == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EMATCHSTR;
		else if (GAVIN_EUNINITED == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EUNINITEDSTR;
		else if (GAVIN_EINITED == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EINITEDSTR;
		else if (GAVIN_EMUXTYPE == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EINVAMUXTYPESTR;
		else if (GAVIN_EFRAMERATE == i)
			g_common_err_info_set[i].m_pInfoPtr = (int8_t *)EFRAMERATESTR;
	}

	return GAVIN_SUCCESS;
}

int32_t gavin_err_uninit()
{
	
	return GAVIN_SUCCESS;
}

int32_t gavin_err_register_output_func(int32_t i32ModuleID, const int8_t *strModuleName, get_custom_err_info_cb_t getErrInfoCb)
{
	err_info_t *pstErrInfo;	
	
	if (i32ModuleID >= GAVIN_MODID_MAX || i32ModuleID <= GAVIN_MODID_MIN \
		|| !strModuleName || !strlen(strModuleName)\
		|| !getErrInfoCb)
		return GAVIN_EILLEGALPARAM;
	pstErrInfo = &err_info_set[i32ModuleID];
	if (pstErrInfo->m_getErrInfoCb)
		return GAVIN_EEXIST;
//	if (strlen(pstErrInfo->m_name))
//		return GAVIN_EEXIST;
	strncpy(pstErrInfo->m_name, strModuleName, sizeof(pstErrInfo->m_name));
	pstErrInfo->m_getErrInfoCb = getErrInfoCb;
	return GAVIN_SUCCESS;
}

int32_t gavin_err_unregister_output_func(int32_t i32ErrValue)
{
	int32_t i32ErrID = GAVIN_GET_EID_BY_ERR(i32ErrValue);
	int32_t i32ModuleID = GAVIN_GET_MODID_BY_ERR(i32ErrValue);

	if(i32ErrID < GAVIN_EBUTT)	//通用错误值，不用注册新的错误说明
		return GAVIN_EILLEGALPARAM;
	if (i32ModuleID >= GAVIN_MODID_MAX || i32ModuleID <= GAVIN_MODID_MIN)
		return GAVIN_EILLEGALPARAM;
	err_info_t *pstErrInfo = &err_info_set[i32ModuleID];
	memset(pstErrInfo->m_name, 0, sizeof(pstErrInfo->m_name));
	pstErrInfo->m_getErrInfoCb = NULL;

	return GAVIN_SUCCESS;
}

int32_t gavin_err_get_err_reason(int32_t i32ErrValue, int8_t *pOutBuf, int32_t i32BufSize)
{
	if (!pOutBuf || i32BufSize <= 0)
	{
		return GAVIN_EILLEGALPARAM;
	}
	int32_t i32ErrID;
	get_comm_err_info(i32ErrValue, i32ErrID);
	int32_t i32ModuleID = GAVIN_GET_MODID_BY_ERR(i32ErrValue);
	if (i32ModuleID >= GAVIN_MODID_MAX || i32ModuleID <= GAVIN_MODID_MIN)
	{
		return GAVIN_EUNEXIST;
	}
	err_info_t *p_err_info = &err_info_set[i32ModuleID];
	if (p_err_info->m_getErrInfoCb)
	{
		p_err_info->m_getErrInfoCb(i32ErrValue, pOutBuf, i32BufSize);
	}
	return GAVIN_SUCCESS;
}

static int32_t gavin_err_is_exist(list_head_t *pstListHead, int32_t i32ErrID)
{
	if (!pstListHead || i32ErrID <= 0)
	{printf("%s->%d:------\n", __FILE__, __LINE__);
		return GAVIN_EILLEGALPARAM;
	}
	list_head_t *pHead = pstListHead;
	err_info_list_node_t *cur, *next;
	list_for_each_entry_safe(err_info_list_node_t, cur, next, pHead, m_list)
	{
		if (cur->m_i32ErrId == i32ErrID)
		{
			return GAVIN_SUCCESS;
		}	
	}printf("%s->%d:------\n", __FILE__, __LINE__);
	return GAVIN_FAILURE;
}

int32_t gavin_err_info_list_add_node(list_head_t *pstListHead, int32_t i32ErrValue, const int8_t *pstrErrInfo)
{
 	int32_t	i32ErrID = GAVIN_GET_EID_BY_ERR(i32ErrValue);
	if (!pstListHead || !pstrErrInfo)
		return GAVIN_ENULLPTR;
	if (i32ErrID <= GAVIN_EBUTT)
		return GAVIN_EILLEGALPARAM;
	if (GAVIN_SUCCESS == gavin_err_is_exist(pstListHead, i32ErrID))
		return GAVIN_EEXIST;
	err_info_list_node_t *pstErrInfoListNode = (err_info_list_node_t *)calloc(1, sizeof(err_info_list_node_t));
	if (pstErrInfoListNode)
	{
		pstErrInfoListNode->m_pInfoPtr = (int8_t *)calloc(1,strlen(pstrErrInfo)+1);
		if (!pstErrInfoListNode->m_pInfoPtr)
		{
			return GAVIN_ENOMEM;
		}
		memcpy(pstErrInfoListNode->m_pInfoPtr, pstrErrInfo, strlen(pstrErrInfo));
		pstErrInfoListNode->m_i32ErrId = i32ErrID;
		list_add(&pstErrInfoListNode->m_list, pstListHead);
	}

	return GAVIN_SUCCESS;
}

int32_t gavin_err_info_list_del_node(list_head_t *pstListHead, int32_t i32ErrID)
{
	list_head_t *phead = pstListHead;
	err_info_list_node_t *pcur, *pnext;

	if (!pstListHead || i32ErrID <= 0)
		return GAVIN_ENULLPTR;
	if (i32ErrID <= GAVIN_EBUTT)
		return GAVIN_EILLEGALPARAM;
	list_for_each_entry_safe(err_info_list_node_t, pcur, pnext, phead, m_list)
	{
		if (pcur->m_i32ErrId == i32ErrID)
		{
			list_del(&pcur->m_list);
			if (pcur->m_pInfoPtr)
			{
				free(pcur->m_pInfoPtr);
			}
			free(pcur);
			return GAVIN_SUCCESS;
		}	
	}

	return GAVIN_EUNEXIST;
}

int32_t gavin_err_info_list_find_err_info(list_head_t *pstListHead, int32_t i32ErrValue, int8_t *pOutBuf, int32_t i32BufSize)
{
	int32_t flag = 0;
	list_head_t *phead = pstListHead;
	err_info_list_node_t *pcur, *pnext;
	int32_t	i32ErrID;

	if (!pstListHead || !pOutBuf || i32BufSize <=0)
		return GAVIN_ENULLPTR;

	get_comm_err_info(i32ErrValue, i32ErrID);//err_id 可以替代err_value，因为只是提取错误值

	list_for_each_entry_safe(err_info_list_node_t, pcur, pnext, phead, m_list)
	{
		if (pcur->m_i32ErrId == i32ErrID)
			flag = 1;
		if (1 == flag && pcur->m_pInfoPtr)
		{
			strncpy(pOutBuf, pcur->m_pInfoPtr, i32BufSize);
			break;
		}

	}
	if (!flag)
		return GAVIN_FAILURE;

	return GAVIN_SUCCESS;
}

int32_t gavin_err_info_list_init(list_head_t *pstListHead)
{
	if (!pstListHead)
		return GAVIN_ENULLPTR;
	init_list_head(pstListHead);

	return GAVIN_SUCCESS;
}

int32_t gavin_err_info_list_uninit(list_head_t *pstListHead)
{
	if (!pstListHead)
		return GAVIN_ENULLPTR;

	list_head_t *phead = pstListHead;
	err_info_list_node_t *pcur, *pnext;
	list_for_each_entry_safe(err_info_list_node_t, pcur, pnext, phead, m_list)
	{
		list_del(&pcur->m_list);
		if (pcur->m_pInfoPtr)
		{
			free(pcur->m_pInfoPtr);
		}
		free(pcur);
	}

	return GAVIN_SUCCESS;
}
