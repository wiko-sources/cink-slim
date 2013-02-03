/* linux/arch/arm/mach-mt6516/mt6516_rfkill.c
 *
 * (C) Copyright 2008
 * MediaTek <www.mediatek.com>
 * JinKwan Huang <jk.huang@mediatek.com>
 *
 * MT6516 RFKILL Driver -- Control bluetooth power
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/rfkill.h>
#include <linux/delay.h>

#include <mach/mt_bt.h>
#include <linux/version.h>      /* constant of kernel version */

#if defined(CONFIG_RFKILL)

extern void mt_wifi_power_on(void);
extern void mt_wifi_power_off(void);
extern int mt_wifi_suspend(pm_message_t state);
extern int mt_wifi_resume(pm_message_t state);
#ifdef BCM_GPS_SUPPORT
extern void brcm_gps_power_on(void);
extern void brcm_gps_power_off(void);
extern int brcm_gps_suspend(pm_message_t state);
extern int brcm_gps_resume(pm_message_t state);
#endif
#if defined(CONFIG_WIMAX_MT71X8)
extern void mt7118_wimax_power_on(void);
extern void mt7118_wimax_power_off(void);
extern int mt7118_wimax_suspend(pm_message_t state);
extern int mt7118_wimax_resume(pm_message_t state);
#endif

void rfkill_restore_states(void);

//#define RFKILL_PM_TEST

enum {
    RFDEV_POWER_OFF,
    RFDEV_POWER_ON,
    RFDEV_SUSPEND
};

struct mt_rfdev {
    const char *name;
    struct rfkill *rfkill;
    spinlock_t lock;
    int  state;
    int  (*suspend)(pm_message_t state);
    int  (*resume)(pm_message_t state);
    void (*power_on)(void);
    void (*power_off)(void);
};

#ifdef MTK_BT_SUPPORT
static struct mt_rfdev bt = {
    .name = "BT",
    .state = RFDEV_POWER_OFF,
    .lock = __SPIN_LOCK_UNLOCKED(bt.lock),
    .suspend = mt_bt_suspend,
    .resume = mt_bt_resume,
    .power_on = mt_bt_power_on,
    .power_off = mt_bt_power_off
};
#else //[wj]add brcm b20710 driver
#ifdef BCM_BT_SUPPORT
static struct mt_rfdev bt = {
    .name = "BT",
    .state = RFDEV_POWER_OFF,
    .lock = __SPIN_LOCK_UNLOCKED(bt.lock),
    .suspend = brcm_bt_suspend,
    .resume = brcm_bt_resume,
    .power_on = brcm_bt_power_on,
    .power_off = brcm_bt_power_off
};
#endif
#endif

#ifdef BCM_GPS_SUPPORT
static struct mt_rfdev gps = {
    .name = "GPS",
    .state = RFDEV_POWER_OFF,
    .lock = __SPIN_LOCK_UNLOCKED(gps.lock),
    .suspend = brcm_gps_suspend,
    .resume = brcm_gps_resume,
    .power_on = brcm_gps_power_on,
    .power_off = brcm_gps_power_off
};
#endif


static struct mt_rfdev wlan = {
    .name = "WLAN",
    .state = RFDEV_POWER_OFF,
    .lock = __SPIN_LOCK_UNLOCKED(wlan.lock),
    .suspend = mt_wifi_suspend,
    .resume = mt_wifi_resume,
    .power_on = mt_wifi_power_on,
    .power_off = mt_wifi_power_off
};

#if defined(CONFIG_WIMAX_MT71X8)
static struct mt_rfdev wimax = {
    .name = "WIMAX",
    .state = RFDEV_POWER_OFF,
    .lock = SPIN_LOCK_UNLOCKED,
    .suspend = mt7118_wimax_suspend,
    .resume = mt7118_wimax_resume,
    .power_on = mt7118_wimax_power_on,
    .power_off = mt7118_wimax_power_off
};
#endif

static int mt_rfkill_power(struct mt_rfdev *dev, int on)
{
    BUG_ON(dev->state == RFDEV_SUSPEND);

    printk(KERN_INFO "[%s] Power %s\n", dev->name, on ? "on" : "off");

    spin_lock(&dev->lock);
    dev->state = on ? RFDEV_POWER_ON : RFDEV_POWER_OFF;
    spin_unlock(&dev->lock);

    if (on) {
        if(NULL != dev->power_on)
            dev->power_on();
    } else {
        if(NULL != dev->power_off)
            dev->power_off();
    }

    return 0;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
static int mt_rfkill_toggle_radio(void *data, enum rfkill_state state)
{
    struct mt_rfdev *dev = (struct mt_rfdev *)data;
    BUG_ON(!data);
    BUG_ON(!dev);

    printk(KERN_INFO "[%s] rfkill_toggle_radio(state %d)\n", dev->name, state);

    switch (state)
    {
    case RFKILL_STATE_UNBLOCKED:
        mt_rfkill_power(dev, 1);
        break;
    case RFKILL_STATE_SOFT_BLOCKED:
        mt_rfkill_power(dev, 0);
        break;
    default:
        printk(KERN_ERR "[%s] bad state %d\n", dev->name, state);
    }
    return 0;
}

#else

static int mt_rfkill_set_block(void *data, bool blocked)
{
    struct mt_rfdev *dev = (struct mt_rfdev *)data;
    printk(KERN_INFO "[%s] rfkill_set_block(blocked %d)\n", dev->name, blocked);

    if(blocked == false)
    {
        mt_rfkill_power(dev, 1);
    } else {
        mt_rfkill_power(dev, 0);
    }

    return 0;
}
#endif

#ifdef CONFIG_PM
    #if defined(CONFIG_WIMAX_MT71X8) || defined(RFKILL_PM_TEST)
static int mt_rfkill_suspend_dev(struct mt_rfdev *dev, pm_message_t state)
{
    printk(KERN_INFO "[%s] PM Suspend\n", dev->name);

    spin_lock(&dev->lock);

    if (dev->state != RFDEV_POWER_ON)
        goto err;

    if (dev->suspend && dev->suspend(state) == 0) {
        dev->state = RFDEV_SUSPEND;
        spin_unlock(&dev->lock);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
        return rfkill_force_state(dev->rfkill, RFKILL_STATE_SOFT_BLOCKED);
#else
        rfkill_set_sw_state(dev->rfkill, true);
        return 0;
#endif
    }
err:
    spin_unlock(&dev->lock);

    return -1;
}

static int mt_rfkill_resume_dev(struct mt_rfdev *dev, pm_message_t state)
{
    printk(KERN_INFO "[%s] PM Resume\n", dev->name);

    spin_lock(&dev->lock);

    if (dev->state != RFDEV_SUSPEND)
        goto err;

    if (dev->resume && dev->resume(state) == 0) {
        dev->state = RFDEV_POWER_ON;
        spin_unlock(&dev->lock);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
        return rfkill_force_state(dev->rfkill, RFKILL_STATE_UNBLOCKED);
#else
        rfkill_set_sw_state(dev->rfkill, false);
        return 0;
#endif
    }
err:
    spin_unlock(&dev->lock);

    return -1;
}
    #endif // end of defined(CONFIG_WIMAX_MT71X8) || defined(RFKILL_PM_TEST)

static int mt_rfkill_suspend(struct platform_device *pdev, pm_message_t state)
{
    printk(KERN_INFO "[RFKill] mt_rfkill_suspend\n");

    if (state.event == PM_EVENT_SUSPEND) {
#if defined(CONFIG_WIMAX_MT71X8)
        (void)mt_rfkill_suspend_dev(&wimax, state);
#endif
    }
    return 0;
}
static int mt_rfkill_resume(struct platform_device *pdev)
{
    pm_message_t state;

    printk(KERN_INFO "[RFKill] mt_rfkill_resume\n");
    state.event = PM_EVENT_RESUME;
#if defined(CONFIG_WIMAX_MT71X8)
    (void)mt_rfkill_resume_dev(&wimax, state);
#endif
    return 0;
}
#endif

#ifdef RFKILL_PM_TEST
static ssize_t mt_rfkill_pm_test(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
    struct rfkill *rfk = container_of(dev, struct rfkill, dev);
    struct mt_rfdev *rfdev = (struct mt_rfdev *)rfk->data;

    if (rfk->type == RFKILL_TYPE_WLAN) {
        pm_message_t s;

        printk(KERN_INFO "==== [WIFI] Power on ====\n");
        mt_rfkill_power(rfdev, 1);
        msleep(10);
        printk(KERN_INFO "==== [WIFI] Suspend ====\n");
        s.event = PM_EVENT_SUSPEND;
        mt_rfkill_suspend_dev(rfdev, s);
        msleep(10);
        printk(KERN_INFO "==== [WIFI] Resume ====\n");
        s.event = PM_EVENT_RESUME;
        mt_rfkill_resume_dev(rfdev, s);
        msleep(10);
        printk(KERN_INFO "==== [WIFI] Power off ====\n");
        mt_rfkill_power(rfdev, 0);
        msleep(10);
        printk(KERN_INFO "==== [WIFI] Suspend ====\n");
        s.event = PM_EVENT_SUSPEND;
        mt_rfkill_suspend_dev(rfdev, s);
        msleep(10);
        printk(KERN_INFO "==== [WIFI] Resume ====\n");
        s.event = PM_EVENT_RESUME;
        mt_rfkill_resume_dev(rfdev, s);
        msleep(10);
        printk(KERN_INFO "==== [WIFI] Power on ====\n");
        mt_rfkill_power(rfdev, 1);
        msleep(10);
        printk(KERN_INFO "==== [WIFI] Resume ====\n");
        s.event = PM_EVENT_RESUME;
        mt_rfkill_resume_dev(rfdev, s);
        msleep(10);
        printk(KERN_INFO "==== [WIFI] Power off ====\n");
        mt_rfkill_power(rfdev, 0);
        msleep(10);
        printk(KERN_INFO "==== [WIFI] Resume ====\n");
        s.event = PM_EVENT_RESUME;
        mt_rfkill_resume_dev(rfdev, s);
        msleep(10);
        printk(KERN_INFO "==== [ BT ] Power on ====\n");
        mt_rfkill_power(&bt, 1);
        msleep(10);
        printk(KERN_INFO "==== [WIFI] Power on ====\n");
        mt_rfkill_power(rfdev, 1);
        msleep(10);
        printk(KERN_INFO "==== [ BT ] Power off ====\n");
        mt_rfkill_power(&bt, 0);
        msleep(10);
    }
	return count;
}

DEVICE_ATTR(pmtst, S_IWUSR | S_IRUGO, NULL, mt_rfkill_pm_test);
#endif

//for Android 2.2 porting
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,31))
struct rfkill_ops mt_rfkill_ops = {
.poll = NULL,
.query = NULL,
.set_block = mt_rfkill_set_block
};
#endif

static int __init mt_rfkill_probe(struct platform_device *pdev)
{
    int rc = 0;
    struct rfkill *rfk;
	printk(KERN_INFO "mt_rfkill_probe\n");

#ifdef MTK_BT_SUPPORT
    rfkill_restore_states();

    /* default to bluetooth off */
    mt_rfkill_power(&bt, 0);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
    rfkill_set_default(RFKILL_TYPE_BLUETOOTH, RFKILL_STATE_SOFT_BLOCKED);

    rfk = rfkill_allocate(&pdev->dev, RFKILL_TYPE_BLUETOOTH);

    if (!rfk)
        return -ENOMEM;

    rfk->name  = "mt6611";
    rfk->state = RFKILL_STATE_SOFT_BLOCKED;
    /* userspace cannot take exclusive control */
    rfk->user_claim_unsupported = 1;
    rfk->user_claim = 0;
    rfk->data = &bt;  // user data
    rfk->toggle_radio = mt_rfkill_toggle_radio;

#else
    rfk = rfkill_alloc("mt661x", &pdev->dev, RFKILL_TYPE_BLUETOOTH, &mt_rfkill_ops, &bt);
    if (!rfk)
    {
        printk(KERN_ALERT "rfkill_alloc failed for BT\n");
        return -ENOMEM;
    }

    rfkill_init_sw_state(rfk, true);
#endif

    rc = rfkill_register(rfk);

    if (rc == 0) {
        bt.rfkill = rfk;
        #ifdef RFKILL_PM_TEST
        rc = device_create_file(&rfk->dev, &dev_attr_pmtst);
        #endif
    } else {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
        rfkill_free(rfk);
#else
        rfkill_destroy(rfk);
#endif
    }
 #else
//[wj]add brcm b20710 driver
#ifdef BCM_BT_SUPPORT
    rfkill_restore_states();

    /* default to bluetooth off */
    mt_rfkill_power(&bt, 0);


    rfk = rfkill_alloc("bcm4330_BT", &pdev->dev, RFKILL_TYPE_BLUETOOTH, &mt_rfkill_ops, &bt);
    if (!rfk)
    {
        printk(KERN_ALERT "rfkill_alloc failed for BT\n");
        return -ENOMEM;
    }

    rfkill_init_sw_state(rfk, true);

    rc = rfkill_register(rfk);

    if (rc == 0) {
        bt.rfkill = rfk;
    } else {

        rfkill_destroy(rfk);
    }
#endif       
#endif

#if defined(MTK_WLAN_SUPPORT)
    /* default to wlan off */
    mt_rfkill_power(&wlan, 0);
 #if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
    rfkill_set_default(RFKILL_TYPE_WLAN, RFKILL_STATE_SOFT_BLOCKED);

    rfk = rfkill_allocate(&pdev->dev, RFKILL_TYPE_WLAN);
    if (!rfk)
        return -ENOMEM;

    rfk->name   = "mtk_wifi";
    rfk->state  = RFKILL_STATE_SOFT_BLOCKED;
    /* userspace cannot take exclusive control */
    rfk->user_claim_unsupported = 1;
    rfk->user_claim = 0;
    rfk->data = &wlan;  // user data
    rfk->toggle_radio = mt_rfkill_toggle_radio;
#else

    rfk = rfkill_alloc("mtk_wifi", &pdev->dev, RFKILL_TYPE_WLAN, &mt_rfkill_ops, &wlan);
    if(!rfk)
    {
        printk(KERN_ALERT "rfkill_alloc failed for wlan\n");
        return -ENOMEM;
    }

    rfkill_init_sw_state(rfk, false);
#endif
    rc = rfkill_register(rfk);
    if (rc == 0) {
        wlan.rfkill = rfk;
        #ifdef RFKILL_PM_TEST
        rc = device_create_file(&rfk->dev, &dev_attr_pmtst);
        #endif
    } else {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
        rfkill_free(rfk);
#else
        rfkill_destroy(rfk);
#endif
    }
#endif // end of defined(MTK_WLAN_SUPPORT)

#if defined(CONFIG_WIMAX_MT71X8)
    /* default to wimax off */
    mt_rfkill_power(&wimax, 0);
    //rfkill_switch_all(RFKILL_TYPE_WIMAX, true);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
    rfkill_set_default(RFKILL_TYPE_WIMAX, RFKILL_STATE_SOFT_BLOCKED);

    rfk = rfkill_allocate(&pdev->dev, RFKILL_TYPE_WIMAX);
    if (!rfk)
        return -ENOMEM;

    rfk->name   = "mt7118";
    rfk->state  = RFKILL_STATE_SOFT_BLOCKED;
    /* userspace cannot take exclusive control */
    rfk->user_claim_unsupported = 1;
    rfk->user_claim = 0;
    rfk->data = &wimax;  // user data
    rfk->toggle_radio = mt_rfkill_toggle_radio;
#else

    rfk = rfkill_alloc("mt7118", &pdev->dev, RFKILL_TYPE_WIMAX, &mt_rfkill_ops, &wimax);
    if(!rfk)
    {
        printk(KERN_INFO "rfkill_alloc failed for wimax\n");
        return -ENOMEM;
    }

    rfkill_init_sw_state(rfk, true);
#endif

    rc = rfkill_register(rfk);
    if (rc == 0) {
        wimax.rfkill = rfk;
        #ifdef RFKILL_PM_TEST
        rc = device_create_file(&rfk->dev, &dev_attr_pmtst);
        #endif
    } else {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
        rfkill_free(rfk);
#else
        rfkill_destroy(rfk);
#endif
    }
#endif

    rfkill_restore_states();

#ifdef BCM_GPS_SUPPORT

    /* default to bluetooth off */
    mt_rfkill_power(&gps, 0);

    rfk = rfkill_alloc("bcm4751", &pdev->dev, RFKILL_TYPE_GPS, &mt_rfkill_ops, &gps);
    if (!rfk)
    {
        printk(KERN_ALERT "rfkill_alloc failed for GPS\n");
        return -ENOMEM;
    }

    rfkill_init_sw_state(rfk, true);

    rc = rfkill_register(rfk);

    if (rc == 0) {
        gps.rfkill = rfk;
        #ifdef RFKILL_PM_TEST
        rc = device_create_file(&rfk->dev, &dev_attr_pmtst);
        #endif
    } else {

        rfkill_destroy(rfk);
    }
#endif
    return rc;
}

static struct platform_driver mt_rfkill_driver =
{
    .probe    = mt_rfkill_probe,
#ifdef CONFIG_PM
    .suspend  = mt_rfkill_suspend,
    .resume   = mt_rfkill_resume,
#endif
    .driver =
    {
        .name = "mt-rfkill",
        .owner = THIS_MODULE,
    },
};

static int __init mt_rfkill_init(void)
{
    int iResult = 0;
    printk(KERN_INFO "---->mt_rfkill_init\n");
    iResult = platform_driver_register(&mt_rfkill_driver);
    printk(KERN_INFO "mt_rfkill_init = [%d]\n", iResult);
    return iResult;
}

module_init(mt_rfkill_init);
MODULE_DESCRIPTION("mt-rfkill");
MODULE_AUTHOR("JinKwan <jk.huang@mediatek.com>");
MODULE_LICENSE("GPL");

#endif
