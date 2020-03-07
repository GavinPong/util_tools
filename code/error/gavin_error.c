#include "gavin_error.h"
#include <stdint.h>

#define get_comm_err_info(err_value) do \
{\
	int32_t err_id = GAVIN_GET_ERR_ID_BY_ERR((err_value));\
	if (err_id <= GAVIN_ERR_FRAME_RATE)\
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
static err_info_list_node_t g_common_err_info_set[GAVIN_ERR_FRAME_RATE - 1] = {};

int32_t gavin_err_init()
{
	//load err ionfo for commom error
	int32_t i = 0;
	int32_t total_cnt = sizeof(g_common_err_info_set);//Currently defined maximum error value
	for (; i < total_cnt; i++)
	{
		g_common_err_info_set[i].m_err_id = i + 1;
		if (GAVIN_ERR_INVALID_DEVID == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_INVALID_DEVID_STR;
		else if(GAVIN_ERR_INVALID_CHNID == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_INVALID_CHNID_STR;
		else if (GAVIN_ERR_ILLEGAL_PARAM == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_ILLEGAL_PARAM_STR;
		else if (GAVIN_ERR_EXIST == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_EXIST_STR;
		else if (GAVIN_ERR_UNEXIST == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_UNEXIST_STR;
		else if (GAVIN_ERR_NULL_PTR == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_NULL_PTR_STR;
		else if (GAVIN_ERR_NOT_CONFIG == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_NOT_CONFIG_STR;
		else if (GAVIN_ERR_NOT_SUPPORT == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_NOT_SUPPORT_STR;
		else if (GAVIN_ERR_NOT_PERM == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_NOT_PERM_STR;
		else if (GAVIN_ERR_NOMEM == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_NOT_NOMEM_STR;
		else if (GAVIN_ERR_NOBUF == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_NOT_NOBUF_STR;
		else if (GAVIN_ERR_BUF_EMPTY == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_BUF_EMPTY_STR;
		else if (GAVIN_ERR_BUF_FULL == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_BUF_FULLE_STR;
		else if (GAVIN_ERR_SYS_NOTREADY == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_SYS_NOTREADY_STR;
		else if (GAVIN_ERR_BADADDR == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_BADADDR_STR;
		else if (GAVIN_ERR_BUSY == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_BUSY_STR;
		else if (GAVIN_ERR_MODULE_ID == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_MODULE_ID_STR;
		else if (GAVIN_ERR_PARSE_JSON == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_PARSE_JSON_STR;
		else if (GAVIN_ERR_JSON_OBJ_EMPTY == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_JSON_OBJ_EMPTY_STR;
		else if (GAVIN_ERR_JSON_SET_EMPTY == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_JSON_SET_EMPTY_STR;
		else if (GAVIN_ERR_MATCH == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_MATCH_STR;
		else if (GAVIN_ERR_INITED == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_INITED_STR;
		else if (GAVIN_ERR_MUXTYPE == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_INVALID_MUXTYPE_STR;
		else if (GAVIN_ERR_FRAME_RATE == i + 1)
			g_common_err_info_set[i].m_info_ptr = (int8_t *)ERR_FRAME_RATE_STR;
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
		return GAVIN_ERR_ILLEGAL_PARAM;
	}
	err_info_t *p_err_info = &err_info_set[module_id];
	if (strlen(p_err_info->m_name))
	{
		return GAVIN_ERR_EXIST;
	}
	strncpy(p_err_info->m_name, module_name, sizeof(p_err_info->m_name));
	p_err_info->m_get_err_info_cb = get_err_info_cb;
	return GAVIN_SUCCESS;
}

int32_t gavin_err_unregister_output_func(int32_t module_id)
{
	if (module_id >= GAVIN_MODE_ID_MAX || module_id <= GAVIN_MODE_ID_MIN)
	{
		return GAVIN_ERR_ILLEGAL_PARAM;
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
		return GAVIN_ERR_ILLEGAL_PARAM;
	}
	get_comm_err_info(err_value);
	int32_t module_id = GAVIN_GET_MODULE_ID_BY_ERR(err_value);
	if (module_id >= GAVIN_MODE_ID_MAX || module_id <= GAVIN_MODE_ID_MIN)
	{
		return GAVIN_ERR_UNEXIST;
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
		return GAVIN_ERR_ILLEGAL_PARAM;
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
		return GAVIN_ERR_NULL_PTR;
	}
	if (err_id <= 0 || err_id <= GAVIN_ERR_BUTT)
	{
		return GAVIN_ERR_ILLEGAL_PARAM;
	}
	if (gavin_err_is_exist(list_head, err_id))
	{
		return GAVIN_ERR_EXIST;
	}
	err_info_list_node_t *err_info_list_node = (err_info_list_node_t *)calloc(1, sizeof(err_info_list_node_t));
	if (err_info_list_node)
	{
		err_info_list_node->m_info_ptr = (char *)calloc(1,strlen(err_info)+1);
		if (!err_info_list_node->m_info_ptr)
		{
			return GAVIN_ERR_NOMEM;
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
		return GAVIN_ERR_NULL_PTR;
	}
	if (err_id <= 0 || err_id <= GAVIN_ERR_BUTT)
	{
		return GAVIN_ERR_ILLEGAL_PARAM;
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

	return GAVIN_ERR_UNEXIST;
}

int32_t gavin_err_info_list_find_err_info(struct list_head *list_head, int32_t err_id, int8_t *out_buf, int32_t buf_size)
{
	if (!list_head || !out_buf || buf_size <=0)
	{
		return GAVIN_ERR_NULL_PTR;
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
		return GAVIN_ERR_NULL_PTR;
	}
	INIT_LIST_HEAD(list_head);

	return GAVIN_SUCCESS;
}

int32_t gavin_err_info_list_uninit(struct list_head *list_head)
{
	if (!list_head)
	{
		return GAVIN_ERR_NULL_PTR;
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