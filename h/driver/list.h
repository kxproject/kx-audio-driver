// kX Driver
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#ifndef KX_LIST_H_
// linked with dsp.h
#define KX_LIST_H_

#ifndef LIST_T_
 #define LIST_T_

 // 32/64-compatible version of the list, sizeof(list) is equal for both architectures, _WIN64 and _WIN32
 struct list 
 {
    union
    {
     struct list *next;
     __int64 next_padding;
    };

    union
    {
     struct list *prev;
     __int64 prev_padding;
    };
 };
#endif

#define init_list(l) do { \
	(l)->next = (l); (l)->prev = (l); \
	} while (0)

static __inline void list_add_t(struct list *_new, struct list *prev, struct list *next)
{
	next->prev = _new;
	_new->next = next;
	_new->prev = prev;
	prev->next = _new;
}

static __inline void list_add(struct list *_new, struct list *head)
{
	list_add_t(_new, head, head->next);
}

static __inline void list_del_t(struct list *prev, struct list *next)
{
	next->prev = prev;
	prev->next = next;
}

static __inline void list_del(struct list *item)
{
	list_del_t(item->prev, item->next);
}

#define for_each_list_entry(pos, head) \
        for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_item(ptr, type, member) \
	((type *)((uintptr_t)(ptr)-(uintptr_t)(&((type *)0)->member)))

#endif
