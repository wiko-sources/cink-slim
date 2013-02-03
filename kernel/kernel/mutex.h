/*
 * Mutexes: blocking mutual exclusion locks
 *
 * started by Ingo Molnar:
 *
 *  Copyright (C) 2004, 2005, 2006 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *
 * This file contains mutex debugging related internal prototypes, for the
 * !CONFIG_DEBUG_MUTEXES case. Most of them are NOPs:
 */
#include <linux/ftrace.h>
#include <linux/aee.h>

#define spin_lock_mutex(lock, flags) \
		do { spin_lock(lock); (void)(flags); } while (0)
#define spin_unlock_mutex(lock, flags) \
		do { spin_unlock(lock); (void)(flags); } while (0)
#define mutex_remove_waiter(lock, waiter, ti) \
		__list_del((waiter)->list.prev, (waiter)->list.next)

#ifdef CONFIG_SMP
static inline void mutex_set_owner(struct mutex *lock)
{
#ifdef CONFIG_MT_LOCK_DEBUG
    unsigned long flags;
    char aee_str[40];
    preempt_disable();
    local_irq_save(flags);
    if(lock->owner == current)
    {
        printk("[Warning!Recursive MutexLock!][%d:%s], lock:%s\n", current->pid, current->comm, lock->mutex_name);
        sprintf( aee_str, "Recursive MutexLock:%s\n", current->comm);
        aee_kernel_warning( aee_str,"mtlock debugger\n");
        dump_stack();
    }
    lock->owner = current;
    lock->caller = (void*)CALLER_ADDR0;
    
    if(&current->mutex_head == NULL || lock == NULL){
        printk("[%d:%s][mutex_debug] warning, 0x%x:0x%x\n", current->pid, current->comm, (unsigned long)&current->mutex_head, (unsigned long)lock);
        local_irq_restore(flags);
        preempt_enable();
        return;
    }
    /* This condition check is for kernel init, first process, maybe */
    if(current->mutex_head.prev == 0 )
        INIT_LIST_HEAD(&current->mutex_head);
    if(list_empty(&current->mutex_head))
        INIT_LIST_HEAD(&lock->mutex_list);
    list_add(&lock->mutex_list, &current->mutex_head);
    local_irq_restore(flags);
    preempt_enable();
#else
	lock->owner = current;
#endif
}

static inline void mutex_clear_owner(struct mutex *lock)
{
#ifdef CONFIG_MT_LOCK_DEBUG
    unsigned long flags;
    preempt_disable();
    local_irq_save(flags);
    lock->caller = NULL;
    lock->owner = NULL;
    list_del(&lock->mutex_list);
    INIT_LIST_HEAD(&lock->mutex_list);
    local_irq_restore(flags);
    preempt_enable();
#else
	lock->owner = NULL;
#endif
}
#else
static inline void mutex_set_owner(struct mutex *lock)
{
#ifdef CONFIG_MT_LOCK_DEBUG
    unsigned long flags;
    char aee_str[40];
    if(lock->owner == current)
    {
        printk("[Warning!Recursive Lock!][%d:%s], mutex lock:%s\n", current->pid, current->comm, lock->mutex_name);
        sprintf( aee_str, "Recursive MutexLock:%s\n", current->comm);
        aee_kernel_warning( aee_str,"mtlock debugger\n");
        dump_stack();
    }
    preempt_disable();
    local_irq_save(flags);
    lock->owner = current;
    lock->caller = CALLER_ADDR0;
    if(current->mutex_head == NULL || lock == NULL){
        printk("[%d:%s][mutex_debug] warning, 0x%x:0x%x\n", current->pid, current->comm, &current->mutex_head, &lock->mutex_list);
        local_irq_restore(flags);
        preempt_enable();
        return;
    }
    /* This condition check is for kernel init, first process, maybe */
    if(current->mutex_head.prev == 0 )
        INIT_LIST_HEAD(&current->mutex_head);

    if(list_empty(&current->mutex_head))
        INIT_LIST_HEAD(&lock->mutex_list);
    list_add(&lock->mutex_list, &current->mutex_head);
    local_irq_restore(flags);
    preempt_enable();
#endif
}

static inline void mutex_clear_owner(struct mutex *lock)
{
#ifdef CONFIG_MT_LOCK_DEBUG
    unsigned long flags;
    preempt_disable();
    local_irq_save(flags);
    lock->caller = NULL;
    lock->owner = NULL;
    list_del(&lock->mutex_list);
    INIT_LIST_HEAD(&lock->mutex_list);
    local_irq_restore(flags);
    preempt_enable();
#endif
}
#endif

#define debug_mutex_wake_waiter(lock, waiter)		do { } while (0)
#define debug_mutex_free_waiter(waiter)			do { } while (0)
#define debug_mutex_add_waiter(lock, waiter, ti)	do { } while (0)
#define debug_mutex_unlock(lock)			do { } while (0)
#define debug_mutex_init(lock, name, key)		do { } while (0)

static inline void
debug_mutex_lock_common(struct mutex *lock, struct mutex_waiter *waiter)
{
}
