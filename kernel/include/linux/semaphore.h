/*
 * Copyright (c) 2008 Intel Corporation
 * Author: Matthew Wilcox <willy@linux.intel.com>
 *
 * Distributed under the terms of the GNU GPL, version 2
 *
 * Please see kernel/semaphore.c for documentation of these functions
 */
#ifndef __LINUX_SEMAPHORE_H
#define __LINUX_SEMAPHORE_H

#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/ftrace.h>
/* Please don't access any members of this structure directly */
struct semaphore {
	spinlock_t		lock;
	unsigned int		count;
	struct list_head	wait_list;
#ifdef CONFIG_MT_LOCK_DEBUG
    char *sem_name;
	struct list_head	sem_list;
	struct task_struct	*owner;
    void* caller;
#endif
};
#ifdef CONFIG_MT_LOCK_DEBUG
#define __SEMAPHORE_INITIALIZER(name, n)				\
{									\
	.lock		= __SPIN_LOCK_UNLOCKED((name).lock),		\
	.count		= n,						\
	.wait_list	= LIST_HEAD_INIT((name).wait_list),		\
	.sem_name = #name, \
	.sem_list	= LIST_HEAD_INIT((name).sem_list),		\
}
#else
#define __SEMAPHORE_INITIALIZER(name, n)				\
{									\
	.lock		= __SPIN_LOCK_UNLOCKED((name).lock),		\
	.count		= n,						\
	.wait_list	= LIST_HEAD_INIT((name).wait_list),		\
}
#endif
#define DEFINE_SEMAPHORE(name)	\
	struct semaphore name = __SEMAPHORE_INITIALIZER(name, 1)

static inline void sema_init(struct semaphore *sem, int val)
{
	static struct lock_class_key __key;
	*sem = (struct semaphore) __SEMAPHORE_INITIALIZER(*sem, val);
	lockdep_init_map(&sem->lock.dep_map, "semaphore->lock", &__key, 0);
}

extern void down(struct semaphore *sem);
extern int __must_check down_interruptible(struct semaphore *sem);
extern int __must_check down_killable(struct semaphore *sem);
extern int __must_check down_trylock(struct semaphore *sem);
extern int __must_check down_timeout(struct semaphore *sem, long jiffies);
extern void up(struct semaphore *sem);

static inline void sem_set_owner(struct semaphore *sem)
{
#ifdef CONFIG_MT_LOCK_DEBUG
    sem->owner = current;
    sem->caller = ((void *)__builtin_return_address(0));
    if(current->sem_head.prev == 0 )
        INIT_LIST_HEAD(&current->sem_head);

    if(list_empty(&current->sem_head))
        INIT_LIST_HEAD(&sem->sem_list);
    list_add(&sem->sem_list, &current->sem_head);
#endif
}

static inline void sem_clear_owner(struct semaphore *sem)
{
#ifdef CONFIG_MT_LOCK_DEBUG
    sem->owner = NULL;
    sem->caller = NULL;
    list_del(&sem->sem_list);
    INIT_LIST_HEAD(&sem->sem_list);
#endif
}
#endif /* __LINUX_SEMAPHORE_H */
