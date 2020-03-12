#ifndef __LIST_H__
#define __LIST_H__

#include <stddef.h>

#ifdef _cplusplus
extern "C"{
#endif

typedef struct _list_head_t{
	struct _list_head_t *next, *prev;
}list_head_t, *plist_head;

#define container_of(ptr, type, member)		(type *)((char *)ptr - offsetof(type, member))

#define LIST_HEAD_INIT(name) {&(name), &(name)}

static inline void init_list_head(plist_head list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(plist_head cur, plist_head prev, plist_head next)
{
	cur->prev = prev;
	cur->next = next;
	next->prev = cur;
	prev->next = cur;
}

static inline void list_add(plist_head cur, plist_head head)
{
	__list_add(cur, head, head->next);
}

static inline void list_add_tail(plist_head cur, plist_head head)
{
	__list_add(cur, head->prev, head);
}

static inline void _list_del(plist_head prev, plist_head next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(plist_head entry)
{
	_list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}

static inline int list_empty(const plist_head head)
{
	return head->next == head;
}

static inline int list_empty_careful(const plist_head head)
{
	plist_head next = head->next;
	return (next == head) && (next == head->prev);
}

static inline plist_head list_get_tail(plist_head head)
{
	return head->prev;
}

#define list_entry(ptr, type, member)	\
	container_of(ptr, type, member)

#define list_get_entry(type, pos, head, member)	\
	pos = list_entry((head)->next, type, member);

#define list_for_each(pos, head)	\
	for(pos = (head)->next; pos != (head); pos = pos->next)

#define __list_for_each(pos, head)	\
	for(pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_prev(pos, head)	\
	for(pos = (head)->prev; pos != (head); pos = pos->prev)

#define list_for_each_safe(pos, n, head)	\
	for(pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

#define list_for_each_entry_safe(type, pos, n, head, member)	\
	for(pos = list_entry((head)->next, type, member), n = list_entry(pos->member.next, type, member);	\
	&pos->member != (head);	\
	pos = n, n = list_entry(n->member.next,type, member))

#define list_for_each_entry(type, pos, head, member)	\
	for(pos = list_entry((head)->next, type, member);	\
		&pos->member != (head);	\
		pos = list_entry(pos->member.next, type, member))

#define list_for_each_entry_reverse(type, pos, head, member)	\
	for(pos = list_entry((head)->prev, type, member);	\
		pos->member != (head);	\
		pos = list_entry(pos->member.prev, type, member))


#ifdef _cplusplus
}
#endif

#endif