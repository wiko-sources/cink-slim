#include <linux/module.h>
#include <linux/slab.h>

#include <linux/mutex.h>
#include <linux/semaphore.h>


/*mutex*/
struct mutex * mt_mutex_init(void)
{
    struct mutex *lock;
    lock = kmalloc(sizeof(struct mutex), GFP_KERNEL);
    if(!lock){
        printk("[mt mutex init]: kmalloc fail\n");
    }
    else
        mutex_init(lock);
    return lock;
}
void mt_mutex_lock_nested(struct mutex *lock, unsigned int subclass)
{
    mutex_lock_nested(lock,subclass);
}
void mt_mutex_lock_nest_lock(struct mutex *lock, struct rw_semaphore *nest_lock)
{
    mutex_lock_nest_lock(lock, nest_lock);
}
int mt_mutex_lock_interruptible_nested(struct mutex *lock,unsigned int subclass)
{
    return mutex_lock_interruptible_nested(lock,subclass);
}
int mt_mutex_lock_killable_nested(struct mutex *lock,unsigned int subclass)
{
    return mutex_lock_killable_nested(lock, subclass);
}
void mt_mutex_lock(struct mutex *lock)
{
    mutex_lock(lock);
}
int mt_mutex_lock_interruptible(struct mutex *lock)
{
    return mutex_lock_interruptible(lock);
}
int mt_mutex_lock_killable(struct mutex *lock)
{
    return mutex_lock_killable(lock);
}
int mt_mutex_trylock(struct mutex *lock)
{
    return mutex_trylock(lock);
}
void mt_mutex_unlock(struct mutex *lock)
{
    mutex_unlock(lock);
}
EXPORT_SYMBOL(mt_mutex_init);
EXPORT_SYMBOL(mt_mutex_lock_nested);
EXPORT_SYMBOL(mt_mutex_lock_nest_lock);
EXPORT_SYMBOL(mt_mutex_lock_interruptible_nested);
EXPORT_SYMBOL(mt_mutex_lock_killable_nested);
EXPORT_SYMBOL(mt_mutex_lock);
EXPORT_SYMBOL(mt_mutex_lock_interruptible);
EXPORT_SYMBOL(mt_mutex_lock_killable);
EXPORT_SYMBOL(mt_mutex_trylock);
EXPORT_SYMBOL(mt_mutex_unlock);


/* semaphore */
struct semaphore* mt_sema_init(int val)
{
    struct semaphore *sem; 
    sem = kmalloc(sizeof(struct semaphore), GFP_KERNEL);
    if(!sem){
        printk("[mt sema init]: kmalloc fail\n");
    }else
        sema_init(sem, val);
    return sem;
}
void mt_down(struct semaphore *sem)
{
    down(sem);
}
int mt_down_interruptible(struct semaphore *sem)
{
    return down_interruptible(sem);
}
int mt_down_killable(struct semaphore *sem)
{
    return down_killable(sem);
}
int mt_down_trylock(struct semaphore *sem)
{
    return down_trylock(sem);
}
int mt_down_timeout(struct semaphore *sem, long jiffies)
{
    return down_timeout(sem,jiffies);
}
void mt_up(struct semaphore *sem)
{
    up(sem);
}
EXPORT_SYMBOL(mt_sema_init);
EXPORT_SYMBOL(mt_down);
EXPORT_SYMBOL(mt_down_interruptible);
EXPORT_SYMBOL(mt_down_killable);
EXPORT_SYMBOL(mt_down_trylock);
EXPORT_SYMBOL(mt_down_timeout);
EXPORT_SYMBOL(mt_up);

