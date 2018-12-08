#include "gavin_error.h"

typedef struct _err_info_s{
	char m_name[32];
	get_err_info_cb_t m_get_err_info_cb;
}err_info_t;

typedef struct _err_info_list_node_s{
	struct list_head m_list;
	int32_t m_err_id;
	char *m_info_ptr;
}err_info_list_node_t;

static err_info_t err_info_set[GAVIN_MODE_ID_MAX] = {};

int32_t gavin_err_init()
{

	return GAVIN_SUCCESS;
}

int32_t gavin_err_uninit()
{
	
	return GAVIN_SUCCESS;
}

int32_t gavin_err_register_output_func(int32_t module_id, const char*module_name, get_err_info_cb_t get_err_info_cb)
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

int32_t gavin_err_get_err_reason(int32_t err_value, char *out_buf, int32_t buf_size)
{
	if (!out_buf || buf_size <= 0)
	{
		return GAVIN_ERR_ILLEGAL_PARAM;
	}
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

int32_t gavin_err_info_list_add_node(struct list_head *list_head, int32_t err_id, const char *err_info)
{
	if (!list_head || !err_info)
	{
		return GAVIN_ERR_NULL_PTR;
	}
	if (err_id <= 0)
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
		err_info_list_node->m_err_id = err_id;
		list_add(&err_info_list_node->m_list, list_head);
	}

	return GAVIN_SUCCESS;
}

int32_t gavin_err_info_list_del_node(struct list_head *list_head, int32_t err_id)
{
	if (!list_head || err_id <=0)
	{
		return GAVIN_ERR_NULL_PTR;
	}
	struct list_head *head = list_head;
	err_info_list_node_t *cur, *next;
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

int32_t gavin_err_info_list_find_err_info(struct list_head *list_head, int32_t err_id, char *out_buf, int32_t buf_size)
{
	if (!list_head || !out_buf || buf_size <=0)
	{
		return GAVIN_ERR_NULL_PTR;
	}
	int32_t flag = 0;
	struct list_head *head = list_head;
	err_info_list_node_t *cur, *next;
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