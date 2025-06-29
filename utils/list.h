#ifndef __LIST_H__
#define __LIST_H__

#include <stddef.h>

// 初始化节点，将 name 的前驱和后继几点都指向本身
#define LIST_HEAD_INIT(name) { &(name), &(name) }

// 定义链表头结点，新建表头 name，并把 name 的前驱后继都指向自己
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)


// 双向链表节点
struct list_head {
	struct list_head *next_ptr, *prev_ptr;
};

// 检查链表是否为空（仅包含伪头节点）
#define list_empty(list) ((list)->next_ptr == (list))

// 从链表节点获取实际的数据节点
#define list_entry(ptr, type, member) \
		(type *)((char *)ptr - offsetof(type, member))

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next_ptr, type, member)

#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev_ptr, type, member)

#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev_ptr, typeof(*(pos)), member)

#define list_next_entry(pos, member) \
	list_entry((pos)->member.next_ptr, typeof(*(pos)), member)

// iterate the list 
// 遍历链表
#define list_for_each_entry(pos, head, member) \
    for (pos = list_entry((head)->next_ptr, typeof(*pos), member); \
        	&pos->member != (head); \
        	pos = list_entry(pos->member.next_ptr, typeof(*pos), member)) 

// iterate the list safely, during which node could be added or removed in the list
#define list_for_each_entry_safe(pos, q, head, member) \
    for (pos = list_entry((head)->next_ptr, typeof(*pos), member), \
	        q = list_entry(pos->member.next_ptr, typeof(*pos), member); \
	        &pos->member != (head); \
	        pos = q, q = list_entry(pos->member.next_ptr, typeof(*q), member))


#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_last_entry(head, typeof(*pos), member);		\
	     &pos->member != (head); 					\
	     pos = list_prev_entry(pos, member))




/**
 * list_for_each_entry_continue - 继续遍历指定类型的链表
 * @pos:	类型指针，用作循环游标
 * @head:	链表头节点
 * @member:	结构体中list_head成员的名称
 *
 * 从当前位置继续遍历指定类型的链表
 */
#define list_for_each_entry_continue(pos, head, member) 		\
	for (pos = list_next_entry(pos, member);			\
	     &pos->member != (head);					\
	     pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_continue_reverse - 反向继续遍历链表
 * @pos:	类型指针，用作循环游标
 * @head:	链表头节点
 * @member:	结构体中list_head成员的名称
 *
 * 从当前位置反向继续遍历指定类型的链表
 */
#define list_for_each_entry_continue_reverse(pos, head, member)		\
	for (pos = list_prev_entry(pos, member);			\
	     &pos->member != (head);					\
	     pos = list_prev_entry(pos, member))

/**
 * list_for_each_entry_from - 从当前点开始遍历链表
 * @pos:	类型指针，用作循环游标
 * @head:	链表头节点
 * @member:	结构体中list_head成员的名称
 *
 * 从指定位置开始继续遍历链表
 */
#define list_for_each_entry_from(pos, head, member) 			\
	for (; &pos->member != (head);					\
	     pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define list_for_each_entry_safe_continue(pos, n, head, member) 		\
	for (pos = list_next_entry(pos, member), 				\
		n = list_next_entry(pos, member);				\
	     &pos->member != (head);						\
	     pos = n, n = list_next_entry(n, member))

/**
 * list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from(pos, n, head, member) 			\
	for (n = list_next_entry(pos, member);					\
	     &pos->member != (head);						\
	     pos = n, n = list_next_entry(n, member))

/**
 * list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define list_for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = list_last_entry(head, typeof(*pos), member),		\
		n = list_prev_entry(pos, member);			\
	     &pos->member != (head); 					\
	     pos = n, n = list_prev_entry(n, member))

// initialize the list head
// 初始化节点，将 list 的前驱和后继都指向自己
static inline void init_list_head(struct list_head *list)
{
	list->next_ptr = list->prev_ptr = list;
}

// insert a new_ptr node between prev_ptr and next_ptr
// 将 new_ptr 节点插入到 prev_ptr 和 next_ptr 之间
static inline void list_insert(struct list_head *new_ptr,
			      struct list_head *prev_ptr,
			      struct list_head *next_ptr)
{
	next_ptr->prev_ptr = new_ptr;
	prev_ptr->next_ptr = new_ptr;
	new_ptr->next_ptr = next_ptr;
	new_ptr->prev_ptr = prev_ptr;
}

// add a list node at the head of the list
// 添加 new_ptr 节点到链表头部
static inline void list_add_head(struct list_head *new_ptr, struct list_head *head)
{
	list_insert(new_ptr, head, head->next_ptr);
}

// add a list node at the tail of the list 
// 添加 new_ptr 节点到链表尾部
static inline void list_add_tail(struct list_head *new_ptr, struct list_head *head)
{
	list_insert(new_ptr, head->prev_ptr, head);
}

// delete the node from the list (note that it only remove the entry from 
// list, but not free allocated memory)
// 删除 entry 节点
static inline void list_delete_entry(struct list_head *entry)
{
	entry->next_ptr->prev_ptr = entry->prev_ptr;
	entry->prev_ptr->next_ptr = entry->next_ptr;
}


#endif /* __LIST_H__ */