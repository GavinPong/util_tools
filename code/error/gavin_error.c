#include "gavin_error.h"
#include <stdint.h>

#define get_comm_err_info(err_value) do \
{\
	int32_t err_id = GAVIN_GET_EID_BY_ERR((err_value));\
	if (err_id <= GAVIN_EFRAMERATE)\
	{\
		strncpy(out_buf, g_common_err_info_set[err_id].m_info_ptr, buf_size - 1);\
		out_buf[buf_size - 1] = '\0';\
		return GAVIN_SUCCESS;\
	}\
} while (0)

typedef struct _err_info_s{
	int8_t m_name[32];
	get_custom_err_info_cb_t m_get_err_info_cb;
}err_info_t;

typedef struct _err_info_list_node_s{
	struct list_head m_list;
	int32_t m_err_id;
	int8_t *m_info_ptr;
}err_info_list_node_t;

static err_info_t err_info_set[GAVIN_MODE_ID_MAX] = {};
static err_info_list_node_t g_common_err_info_set[GAVIN_EFRAMERATE - 1] = {};

int32_t gavin_err_init()
{
	//load err ionfo for commom error
	int32_t i = 0;
	int32_t total_cnt = sizeof(g_common_err_info_set);//Currently defined maximum error value
	for (; i < total_cnt; i++)
	{
		g_common_err_info_set[i].m_err_id = i + 1;
		if (GAVIN_EINVADEVID == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EINVALID_DEVID_STR;
		else if(GAVIN_EINVACHNID == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EINVALID_CHNID_STR;
		else if (GAVIN_EILLEGALPARAM == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EILLEGAL_PARAM_STR;
		else if (GAVIN_EEXIST == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EEXIST_STR;
		else if (GAVIN_EUNEXIST == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EUNEXIST_STR;
		else if (GAVIN_ENULLPTR == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ENULL_PTR_STR;
		else if (GAVIN_ENOTCONFIG == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ENOT_CONFIG_STR;
		else if (GAVIN_ENOTSUPPORT == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ENOT_SUPPORT_STR;
		else if (GAVIN_ENOTPERM == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ENOT_PERM_STR;
		else if (GAVIN_ENOMEM == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ENOT_NOMEM_STR;
		else if (GAVIN_ENOBUF == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ENOT_NOBUF_STR;
		else if (GAVIN_EBUFEMPTY == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EBUF_EMPTY_STR;
		else if (GAVIN_EBUFFULL == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EBUF_FULLE_STR;
		else if (GAVIN_ESYSNOTREADY == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ESYS_NOTREADY_STR;
		else if (GAVIN_EBADADDR == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EBADADDR_STR;
		else if (GAVIN_EBUSY == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EBUSY_STR;
		else if (GAVIN_EMODULEID == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EMODULE_ID_STR;
		else if (GAVIN_EPARSEJSON == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EPARSE_JSON_STR;
		else if (GAVIN_EJSONOBJEMPTY == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EJSON_OBJ_EMPTY_STR;
		else if (GAVIN_EJSONSETEMPTY == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EJSON_SET_EMPTY_STR;
		else if (GAVIN_EMATCH == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EMATCH_STR;
		else if (GAVIN_EINITED == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EINITED_STR;
		else if (GAVIN_EMUXTYPE == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EINVALID_MUXTYPE_STR;
		else if (GAVIN_EFRAMERATE == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)EFRAME_RATE_STR;
	}

	return GAVIN_SUCCESS;
}

int32_t gavin_err_uninit()
{
	
	return GAVIN_SUCCESS;
}

int32_t gavin_err_register_output_func(int32_t module_id, const int8_t *module_name, get_custom_err_info_cb_t get_err_info_cb)
{
	if (module_id >= GAVIN_MODE_ID_MAX || module_id <= GAVIN_MODE_ID_MIN \
		|| !module_name || !strlen(module_name)\
		|| !get_err_info_cb)
	{
		return GAVIN_EILLEGALPARAM;
	}
	err_info_t *p_err_info = &err_info_set[module_id];
	if (strlen(p_err_info->m_name))
	{
		return GAVIN_EEXIST;
	}
	strncpy(p_err_info->m_name, module_name, sizeof(p_err_info->m_name));
	p_err_info->m_get_err_info_cb = get_err_info_cb;
	return GAVIN_SUCCESS;
}

int32_t gavin_err_unregister_output_func(int32_t module_id)
{
	if (module_id >= GAVIN_MODE_ID_MAX || module_id <= GAVIN_MODE_ID_MIN)
	{
		return GAVIN_EILLEGALPARAM;
	}
	err_info_t *p_err_info = &err_info_set[module_id];
	memset(p_err_info->m_name, 0, sizeof(p_err_info->m_name));
	p_err_info->m_get_err_info_cb = NULL;

	return GAVIN_SUCCESS;
}

int32_t gavin_err_get_err_reason(int32_t err_value, int8_t *out_buf, int32_t buf_size)
{
	if (!out_buf || buf_size <= 0)
	{
		return GAVIN_EILLEGALPARAM;
	}
	get_comm_err_info(err_value);
	int32_t module_id = GAVIN_GET_MODID_BY_ERR(err_value);
	if (module_id >= GAVIN_MODE_ID_MAX || module_id <= GAVIN_MODE_ID_MIN)
	{
		return GAVIN_EUNEXIST;
	}
	err_info_t *p_err_info = &err_info_set[module_id];

	if (p_err_info->m_get_err_info_cb)
	{
		p_err_info->m_get_err_info_cb(err_value, out_buf, buf_size);
	}
	return GAVIN_SUCCESS;
}

static int32_t gavin_err_is_exist(struct list_head *list_head, int32_t err_id)
{
	if (!list_head || err_id <= 0)
	{
		return GAVIN_EILLEGALPARAM;
	}
	struct list_head *head = list_head;
	err_info_list_node_t *cur, *next;
	list_for_each_entry_safe(err_info_list_node_t, cur, next, head, m_list)
	{
		if (cur->m_err_id == err_id)
		{
			return GAVIN_SUCCESS;
		}	
	}
	return GAVIN_FAILURE;
}

int32_t gavin_err_info_list_add_node(struct list_head *list_head, int32_t err_id, const int8_t *err_info)
{
	if (!list_head || !err_info)
	{
		return GAVIN_ENULLPTR;
	}
	if (err_id <= 0 || err_id <= GAVIN_EBUTT)
	{
		return GAVIN_EILLEGALPARAM;
	}
	if (gavin_err_is_exist(list_head, err_id))
	{
		return GAVIN_EEXIST;
	}
	err_info_list_node_t *err_info_list_node = (err_info_list_node_t *)calloc(1, sizeof(err_info_list_node_t));
	if (err_info_list_node)
	{
		err_info_list_node->m_info_ptr = (char *)calloc(1,strlen(err_info)+1);
		if (!err_info_list_node->m_info_ptr)
		{
			return GAVIN_ENOMEM;
		}
		memcpy(err_info_list_node->m_info_ptr, err_info, strlen(err_info));
		err_info_list_node->m_err_id = err_id;
		list_add(&err_info_list_node->m_list, list_head);
	}

	return GAVIN_SUCCESS;
}

int32_t gavin_err_info_list_del_node(struct list_head *list_head, int32_t err_id)
{
	struct list_head *head = list_head;
	err_info_list_node_t *cur, *next;

	if (!list_head || err_id <= 0)
	{
		return GAVIN_ENULLPTR;
	}
	if (err_id <= 0 || err_id <= GAVIN_EBUTT)
	{
		return GAVIN_EILLEGALPARAM;
	}
	list_for_each_entry_safe(err_info_list_node_t, cur, next, head, m_list)
	{
		if (cur->m_err_id == err_id)
		{
			list_del(&cur->m_list);
			if (cur->m_info_ptr)
			{
				free(cur->m_info_ptr);
			}
			free(cur);
			return GAVIN_SUCCESS;
		}	
	}

	return GAVIN_EUNEXIST;
}

int32_t gavin_err_info_list_find_err_info(struct list_head *list_head, int32_t err_id, int8_t *out_buf, int32_t buf_size)
{
	if (!list_head || !out_buf || buf_size <=0)
	{
		return GAVIN_ENULLPTR;
	}
	int32_t flag = 0;
	struct list_head *head = list_head;
	err_info_list_node_t *cur, *next;

	get_comm_err_info(err_id);//err_id 可以替代err_value，因为只是提取错误值

	list_for_each_entry_safe(err_info_list_node_t, cur, next, head, m_list)
	{
		if (cur->m_err_id == err_id)
		{
			flag = 1;
		}	
		if (1 == flag && cur->m_info_ptr)
		{
			strncpy(out_buf, cur->m_info_ptr, buf_size);
			break;
		}

	}
	if (!flag)
	{
		return GAVIN_FAILURE;
	}

	return GAVIN_SUCCESS;
}

int32_t gavin_err_info_list_init(struct list_head *list_head)
{
	if (!list_head)
	{
		return GAVIN_ENULLPTR;
	}
	init_list_head(list_head);

	return GAVIN_SUCCESS;
}

int32_t gavin_err_info_list_uninit(struct list_head *list_head)
{
	if (!list_head)
	{
		return GAVIN_ENULLPTR;
	}

	struct list_head *head = list_head;
	err_info_list_node_t *cur, *next;
	list_for_each_entry_safe(err_info_list_node_t, cur, next, head, m_list)
	{
		list_del(&cur->m_list);
		if (cur->m_info_ptr)
		{
			free(cur->m_info_ptr);
		}
		free(cur);
	}

	return GAVIN_SUCCESS;
}