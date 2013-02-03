/*
 * Copyright (C) 2010 MediaTek, Inc.
 *
 * Author: Terry Chang <terry.chang@mediatek.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/earlysuspend.h>

#include <asm/atomic.h>
#include <asm/uaccess.h>

#ifdef MT6575
#include <mach/mt6575_reg_base.h>
#include <mach/mt6575_boot.h>
#include <mt6575_kpd.h>		/* custom file */
#endif

#ifdef MT6577
#include <mach/mt6577_reg_base.h>
#include <mach/mt6577_boot.h>
#include <mtk_kpd.h>		/* custom file */
#endif

#include <mach/irqs.h>
#include <mach/eint.h>
#include <mach/mt_gpio.h>



#include <linux/aee.h>

#define KPD_AUTOTEST	KPD_YES
#define KPD_DEBUG	KPD_YES

#define KPD_NAME	"mtk-kpd"

/* Keypad registers */
#define KP_STA		(KP_BASE + 0x0000)
#define KP_MEM1		(KP_BASE + 0x0004)
#define KP_MEM2		(KP_BASE + 0x0008)
#define KP_MEM3		(KP_BASE + 0x000c)
#define KP_MEM4		(KP_BASE + 0x0010)
#define KP_MEM5		(KP_BASE + 0x0014)
#define KP_DEBOUNCE	(KP_BASE + 0x0018)

#define KPD_NUM_MEMS	5
#define KPD_MEM5_BITS	8

#define KPD_NUM_KEYS	72	/* 4 * 16 + KPD_MEM5_BITS */

#define KPD_DEBOUNCE_MASK	((1U << 14) - 1)

#define KPD_SAY		"kpd: "
#if KPD_DEBUG
#define kpd_print(fmt, arg...)	printk(KPD_SAY fmt, ##arg)
#else
#define kpd_print(fmt, arg...)	do {} while (0)
#endif

struct kpd_ledctl {
	u8 onoff;
	u8 div;		/* 0 ~ 15 */
	u8 duty;	/* 0 ~ 31 */
};

#if KPD_AUTOTEST
#define PRESS_OK_KEY		_IO('k', 1)
#define RELEASE_OK_KEY		_IO('k', 2)
#define PRESS_MENU_KEY		_IO('k', 3)
#define RELEASE_MENU_KEY	_IO('k', 4)
#define PRESS_UP_KEY		_IO('k', 5)
#define RELEASE_UP_KEY		_IO('k', 6)
#define PRESS_DOWN_KEY		_IO('k', 7)
#define RELEASE_DOWN_KEY	_IO('k', 8)
#define PRESS_LEFT_KEY		_IO('k', 9)
#define RELEASE_LEFT_KEY	_IO('k', 10)
#define PRESS_RIGHT_KEY		_IO('k', 11)
#define RELEASE_RIGHT_KEY	_IO('k', 12)
#define PRESS_HOME_KEY		_IO('k', 13)
#define RELEASE_HOME_KEY	_IO('k', 14)
#define PRESS_BACK_KEY		_IO('k', 15)
#define RELEASE_BACK_KEY	_IO('k', 16)
#define PRESS_CALL_KEY		_IO('k', 17)
#define RELEASE_CALL_KEY	_IO('k', 18)
#define PRESS_ENDCALL_KEY	_IO('k', 19)
#define RELEASE_ENDCALL_KEY	_IO('k', 20)
#define PRESS_VLUP_KEY		_IO('k', 21)
#define RELEASE_VLUP_KEY	_IO('k', 22)
#define PRESS_VLDOWN_KEY	_IO('k', 23)
#define RELEASE_VLDOWN_KEY	_IO('k', 24)
#define PRESS_FOCUS_KEY		_IO('k', 25)
#define RELEASE_FOCUS_KEY	_IO('k', 26)
#define PRESS_CAMERA_KEY	_IO('k', 27)
#define RELEASE_CAMERA_KEY	_IO('k', 28)
#endif
#define SET_KPD_BACKLIGHT	_IOW('k', 29, struct kpd_ledctl)

static struct input_dev *kpd_input_dev;
static bool kpd_suspend = false;
static int kpd_show_hw_keycode = 1;
static int kpd_show_register = 1;
static int kpd_enable_lprst = 1;

/* for AEE manual dump */
#if 0
static bool kpd_volumn_down_flag = false;
static bool kpd_volumn_up_flag = false;
static inline void check_aee_dump()
{
	if( (kpd_volumn_down_flag == true) && (kpd_volumn_up_flag == true))
	{
		printk(KPD_SAY "kpd_volumn_up+ volumn_down,will trige DB\n");
		aee_kernel_reminding("manual dump ", "Triggered by press KEY_VOLUMEUP+KEY_VOLUMEDOWN");
	}
}
#endif

/* for backlight control */
#if KPD_DRV_CTRL_BACKLIGHT
static void kpd_switch_backlight(struct work_struct *work);
static void kpd_backlight_timeout(unsigned long data);
static DECLARE_WORK(kpd_backlight_work, kpd_switch_backlight);
static DEFINE_TIMER(kpd_backlight_timer, kpd_backlight_timeout, 0, 0);

static unsigned long kpd_wake_keybit[BITS_TO_LONGS(KEY_CNT)];
static u16 kpd_wake_key[] __initdata = KPD_BACKLIGHT_WAKE_KEY;

static volatile bool kpd_backlight_on;
static atomic_t kpd_key_pressed = ATOMIC_INIT(0);
#endif

/* for slide QWERTY */
#if KPD_HAS_SLIDE_QWERTY
static void kpd_slide_handler(unsigned long data);
static DECLARE_TASKLET(kpd_slide_tasklet, kpd_slide_handler, 0);

static u8 kpd_slide_state = !KPD_SLIDE_POLARITY;
#endif

/* for Power key using EINT */
#if KPD_PWRKEY_USE_EINT
static void kpd_pwrkey_handler(unsigned long data);
static DECLARE_TASKLET(kpd_pwrkey_tasklet, kpd_pwrkey_handler, 0);
static u8 kpd_pwrkey_state = !KPD_PWRKEY_POLARITY;
#endif

/* for Power key using PMIC */
#if KPD_PWRKEY_USE_PMIC
static void kpd_pwrkey_handler(struct work_struct *work);
static DECLARE_WORK(pwrkey_pmic_work, kpd_pwrkey_handler);
#endif

/* for keymap handling */
static void kpd_keymap_handler(unsigned long data);
static DECLARE_TASKLET(kpd_keymap_tasklet, kpd_keymap_handler, 0);

static u16 kpd_keymap[KPD_NUM_KEYS] = KPD_INIT_KEYMAP();
static u16 kpd_keymap_state[KPD_NUM_MEMS] = {
	0xffff, 0xffff, 0xffff, 0xffff, 0x00ff
};

/* for autotest */
#if KPD_AUTOTEST
static const u16 kpd_auto_keymap[] = {
	KEY_OK, KEY_MENU,
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
	KEY_HOME, KEY_BACK,
	KEY_CALL, KEY_ENDCALL,
	KEY_VOLUMEUP, KEY_VOLUMEDOWN,
	KEY_FOCUS, KEY_CAMERA,
};
#endif

/* for AEE manual dump */
#define AEE_VOLUMEUP_BIT	0
#define AEE_VOLUMEDOWN_BIT	1
#define AEE_DELAY_TIME		15
/* enable volup + voldown was pressed 5~15 s Trigger aee manual dump */
#define AEE_ENABLE_5_15		1
static struct hrtimer aee_timer;
static unsigned long  aee_pressed_keys;
static bool aee_timer_started;

#if AEE_ENABLE_5_15
#define AEE_DELAY_TIME_5S	5
static struct hrtimer aee_timer_5s;
static bool aee_timer_5s_started;
static bool flags_5s;
#endif

static inline void kpd_update_aee_state(void) {
	if(aee_pressed_keys == ((1<<AEE_VOLUMEUP_BIT) | (1<<AEE_VOLUMEDOWN_BIT))) {
		/* if volumeup and volumedown was pressed the same time then start the time of ten seconds */
		aee_timer_started = true;
		
#if AEE_ENABLE_5_15
		aee_timer_5s_started = true;
		hrtimer_start(&aee_timer_5s, 
				ktime_set(AEE_DELAY_TIME_5S, 0),
				HRTIMER_MODE_REL);
#endif
		hrtimer_start(&aee_timer, 
				ktime_set(AEE_DELAY_TIME, 0),
				HRTIMER_MODE_REL);
		kpd_print("aee_timer started\n");
	} else {
		if(aee_timer_started) {
/*
  * hrtimer_cancel - cancel a timer and wait for the handler to finish.
  * Returns:
  *	0 when the timer was not active. 
  *	1 when the timer was active.
 */
			if(hrtimer_cancel(&aee_timer))
			{
				kpd_print("try to cancel hrtimer \n");
#if AEE_ENABLE_5_15
				if(flags_5s)
				{
					printk("Pressed Volup + Voldown5s~15s then trigger aee manual dump.\n");
					aee_kernel_reminding("manual dump", "Trigger Vol Up +Vol Down 5s");
				}
#endif
					
			}
#if AEE_ENABLE_5_15
			flags_5s = false;
#endif
			aee_timer_started = false;
			kpd_print("aee_timer canceled\n");
		}

#if AEE_ENABLE_5_15
		if(aee_timer_5s_started) {
/*
  * hrtimer_cancel - cancel a timer and wait for the handler to finish.
  * Returns:
  *	0 when the timer was not active. 
  *	1 when the timer was active.
 */
			if(hrtimer_cancel(&aee_timer_5s))
			{
				kpd_print("try to cancel hrtimer (5s) \n");
			}
			aee_timer_5s_started = false;
			kpd_print("aee_timer canceled (5s)\n");
		}

#endif
	}
}

static void kpd_aee_handler(u32 keycode, u16 pressed) {
	if(pressed) {
		if(keycode == KEY_VOLUMEUP) {
			__set_bit(AEE_VOLUMEUP_BIT, &aee_pressed_keys);
		} else if(keycode == KEY_VOLUMEDOWN) {
			__set_bit(AEE_VOLUMEDOWN_BIT, &aee_pressed_keys);
		} else {
			return;
		}
		kpd_update_aee_state();
	} else {
		if(keycode == KEY_VOLUMEUP) {
			__clear_bit(AEE_VOLUMEUP_BIT, &aee_pressed_keys);
		} else if(keycode == KEY_VOLUMEDOWN) {
			__clear_bit(AEE_VOLUMEDOWN_BIT, &aee_pressed_keys);
		} else {
			return;
		}
		kpd_update_aee_state();
	}
}

static enum hrtimer_restart aee_timer_func(struct hrtimer *timer) {
	printk("kpd: vol up+vol down AEE manual dump!\n");
	//aee_kernel_reminding("manual dump ", "Triggered by press KEY_VOLUMEUP+KEY_VOLUMEDOWN");
	aee_trigger_kdb();
	return HRTIMER_NORESTART;
}

#if AEE_ENABLE_5_15
static enum hrtimer_restart aee_timer_5s_func(struct hrtimer *timer) {
	
	printk("kpd: vol up+vol down AEE manual dump timer 5s !\n");
	flags_5s = true;
	return HRTIMER_NORESTART;
}
#endif

static inline void kpd_get_keymap_state(u16 state[])
{
	state[0] = *(volatile u16 *)KP_MEM1;
	state[1] = *(volatile u16 *)KP_MEM2;
	state[2] = *(volatile u16 *)KP_MEM3;
	state[3] = *(volatile u16 *)KP_MEM4;
	state[4] = *(volatile u16 *)KP_MEM5;
	if (kpd_show_register) {
		printk(KPD_SAY "register = %x %x %x %x %x\n",
		       state[0], state[1], state[2], state[3], state[4]);
	}
}

static inline void kpd_set_debounce(u16 val)
{
	*(volatile u16 *)KP_DEBOUNCE = (u16)(val & KPD_DEBOUNCE_MASK);
}

#if KPD_DRV_CTRL_BACKLIGHT
static void kpd_switch_backlight(struct work_struct *work)
{
	if (kpd_backlight_on) {
		kpd_enable_backlight();
		kpd_print("backlight is on\n");
	} else {
		kpd_disable_backlight();
		kpd_print("backlight is off\n");
	}
}

static void kpd_backlight_timeout(unsigned long data)
{
	if (!atomic_read(&kpd_key_pressed)) {
		kpd_backlight_on = !!atomic_read(&kpd_key_pressed);
		schedule_work(&kpd_backlight_work);
		data = 1;
	}
	kpd_print("backlight timeout%s\n", 
	          data ? ": schedule backlight work" : "");
}

void kpd_backlight_handler(bool pressed, u16 linux_keycode)
{
	if (kpd_suspend && !test_bit(linux_keycode, kpd_wake_keybit)) {
		kpd_print("Linux keycode %u is not WAKE key\n", linux_keycode);
		return;
	}

	/* not in suspend or the key pressed is WAKE key */
	if (pressed) {
		atomic_inc(&kpd_key_pressed);
		kpd_backlight_on = !!atomic_read(&kpd_key_pressed);
		schedule_work(&kpd_backlight_work);
		kpd_print("switch backlight on\n");
	} else {
		atomic_dec(&kpd_key_pressed);
		mod_timer(&kpd_backlight_timer,
		          jiffies + KPD_BACKLIGHT_TIME * HZ);
		kpd_print("activate backlight timer\n");
	}
}
#endif

#if KPD_HAS_SLIDE_QWERTY
static void kpd_slide_handler(unsigned long data)
{
	bool slid;
	u8 old_state = kpd_slide_state;

	kpd_slide_state = !kpd_slide_state;
	slid = (kpd_slide_state == !!KPD_SLIDE_POLARITY);
	/* for SW_LID, 1: lid open => slid, 0: lid shut => closed */
	input_report_switch(kpd_input_dev, SW_LID, slid);
	input_sync(kpd_input_dev);
	kpd_print("report QWERTY = %s\n", slid ? "slid" : "closed");

	if(old_state) {
		mt_set_gpio_pull_select(GPIO_QWERTYSLIDE_EINT_PIN, 0);
	} else {
		mt_set_gpio_pull_select(GPIO_QWERTYSLIDE_EINT_PIN, 1);
	}
	/* for detecting the return to old_state */
	mt65xx_eint_set_polarity(KPD_SLIDE_EINT, old_state);
	mt65xx_eint_unmask(KPD_SLIDE_EINT);
}

static void kpd_slide_eint_handler(void)
{
	tasklet_schedule(&kpd_slide_tasklet);
}
#endif

#if KPD_PWRKEY_USE_EINT
static void kpd_pwrkey_handler(unsigned long data)
{
	bool pressed;
	u8 old_state = kpd_pwrkey_state;

	kpd_pwrkey_state = !kpd_pwrkey_state;
	pressed = (kpd_pwrkey_state == !!KPD_PWRKEY_POLARITY);
	if (kpd_show_hw_keycode) {
		printk(KPD_SAY "(%s) HW keycode = using EINT\n",
		       pressed ? "pressed" : "released");
	}
	kpd_backlight_handler(pressed, KPD_PWRKEY_MAP);
	input_report_key(kpd_input_dev, KPD_PWRKEY_MAP, pressed);
	input_sync(kpd_input_dev);
	kpd_print("report Linux keycode = %u\n", KPD_PWRKEY_MAP);

	/* for detecting the return to old_state */
	mt65xx_eint_set_polarity(KPD_PWRKEY_EINT, old_state);
	mt65xx_eint_unmask(KPD_PWRKEY_EINT);
}

static void kpd_pwrkey_eint_handler(void)
{
	tasklet_schedule(&kpd_pwrkey_tasklet);
}
#endif

#if KPD_PWRKEY_USE_PMIC
static void kpd_pwrkey_handler(struct work_struct *work)
{
	int pressed = 1;
	/* report  Press*/
	input_report_key(kpd_input_dev, KPD_PWRKEY_MAP, pressed);
	input_sync(kpd_input_dev);
	if (kpd_show_hw_keycode) {
		printk(KPD_SAY "(%s) HW keycode = using PMIC\n",
		       pressed ? "pressed" : "released");
	}
	while(1)
	{
		if(upmu_status_chr_pwrkey_deb() == 1)
		{
			pressed = 0;
			/* Report Release */
			input_report_key(kpd_input_dev, KPD_PWRKEY_MAP, pressed);
			input_sync(kpd_input_dev);
	    if (kpd_show_hw_keycode) {
		       printk(KPD_SAY "(%s) HW keycode = using PMIC\n",
		         pressed ? "pressed" : "released");
	    }			
			break;
		}
		msleep(10);
	}		
}

void kpd_pwrkey_pmic_handler(unsigned long pressed)
{
	printk(KPD_SAY "Power Key generate, pressed=%ld\n", pressed);
	if(!kpd_input_dev) {
		printk("KPD input device not ready\n");
		return;
	}
	if(get_chip_eco_ver() == CHIP_E2) {
		input_report_key(kpd_input_dev, KPD_PWRKEY_MAP, pressed);
		input_sync(kpd_input_dev);
		if (kpd_show_hw_keycode) {
			printk(KPD_SAY "(%s) HW keycode =%d using PMIC\n",
			       pressed ? "pressed" : "released", KPD_PWRKEY_MAP);
		}
	} else {
		schedule_work(&pwrkey_pmic_work);
	}
}
#endif

void kpd_pmic_rstkey_handler(unsigned long pressed)
{
	printk(KPD_SAY "PMIC reset Key generate, pressed=%ld\n", pressed);
	if(!kpd_input_dev) {
		printk("KPD input device not ready\n");
		return;
	}
#ifdef KPD_PMIC_RSTKEY_MAP
	if(get_chip_eco_ver() == CHIP_E2) {
		input_report_key(kpd_input_dev, KPD_PMIC_RSTKEY_MAP, pressed);
		input_sync(kpd_input_dev);
		if (kpd_show_hw_keycode) {
			printk(KPD_SAY "(%s) HW keycode =%d using PMIC\n",
			       pressed ? "pressed" : "released", KPD_PMIC_RSTKEY_MAP);
		}
		kpd_aee_handler(KPD_PMIC_RSTKEY_MAP, pressed);
	}
#endif
}

static void kpd_keymap_handler(unsigned long data)
{
	int i, j;
	bool pressed;
	u16 new_state[KPD_NUM_MEMS], change, mask;
	u16 hw_keycode, linux_keycode;
	kpd_get_keymap_state(new_state);

	for (i = 0; i < KPD_NUM_MEMS; i++) {
		change = new_state[i] ^ kpd_keymap_state[i];
		if (!change)
			continue;

		for (j = 0; j < 16; j++) {
			mask = 1U << j;
			if (!(change & mask))
				continue;

			hw_keycode = (i << 4) + j;
			/* bit is 1: not pressed, 0: pressed */
			pressed = !(new_state[i] & mask);
			if (kpd_show_hw_keycode) {
				printk(KPD_SAY "(%s) HW keycode = %u\n",
				       pressed ? "pressed" : "released",
				       hw_keycode);
			}
			BUG_ON(hw_keycode >= KPD_NUM_KEYS);
			linux_keycode = kpd_keymap[hw_keycode];			
			if (unlikely(linux_keycode == 0)) {
				kpd_print("Linux keycode = 0\n");
				continue;
			}		
			kpd_aee_handler(linux_keycode, pressed);
			
			kpd_backlight_handler(pressed, linux_keycode);
			input_report_key(kpd_input_dev, linux_keycode, pressed);
			input_sync(kpd_input_dev);
			kpd_print("report Linux keycode = %u\n", linux_keycode);
		}
	}
	
	memcpy(kpd_keymap_state, new_state, sizeof(new_state));
	kpd_print("save new keymap state\n");
	enable_irq(MT6577_KP_IRQ_ID);
}

static irqreturn_t kpd_irq_handler(int irq, void *dev_id)
{
	/* use _nosync to avoid deadlock */
	disable_irq_nosync(MT6577_KP_IRQ_ID);

	tasklet_schedule(&kpd_keymap_tasklet);
	return IRQ_HANDLED;
}

static long kpd_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	void __user *uarg = (void __user *)arg;
	struct kpd_ledctl ledctl;

	switch (cmd) {
#if KPD_AUTOTEST
	case PRESS_OK_KEY:
		printk("[AUTOTEST] PRESS OK KEY!!\n");
		input_report_key(kpd_input_dev, KEY_OK, 1);
		input_sync(kpd_input_dev);
		break;
	case RELEASE_OK_KEY:
		printk("[AUTOTEST] RELEASE OK KEY!!\n");
		input_report_key(kpd_input_dev, KEY_OK, 0);
		input_sync(kpd_input_dev);
		break;
	case PRESS_MENU_KEY:
		printk("[AUTOTEST] PRESS MENU KEY!!\n");
		input_report_key(kpd_input_dev, KEY_MENU, 1);
		input_sync(kpd_input_dev);
		break;
	case RELEASE_MENU_KEY:
		printk("[AUTOTEST] RELEASE MENU KEY!!\n");
		input_report_key(kpd_input_dev, KEY_MENU, 0);
		input_sync(kpd_input_dev);
		break;
	case PRESS_UP_KEY:
		printk("[AUTOTEST] PRESS UP KEY!!\n");
		input_report_key(kpd_input_dev, KEY_UP, 1);
		input_sync(kpd_input_dev);
		break;
	case RELEASE_UP_KEY:
		printk("[AUTOTEST] RELEASE UP KEY!!\n");
		input_report_key(kpd_input_dev, KEY_UP, 0);
		input_sync(kpd_input_dev);
		break;
	case PRESS_DOWN_KEY:
		printk("[AUTOTEST] PRESS DOWN KEY!!\n");
		input_report_key(kpd_input_dev, KEY_DOWN, 1);
		input_sync(kpd_input_dev);
		break;
	case RELEASE_DOWN_KEY:
		printk("[AUTOTEST] RELEASE DOWN KEY!!\n");
		input_report_key(kpd_input_dev, KEY_DOWN, 0);
		input_sync(kpd_input_dev);
		break;
	case PRESS_LEFT_KEY:
		printk("[AUTOTEST] PRESS LEFT KEY!!\n");
		input_report_key(kpd_input_dev, KEY_LEFT, 1);
		input_sync(kpd_input_dev);
		break;
	case RELEASE_LEFT_KEY:
		printk("[AUTOTEST] RELEASE LEFT KEY!!\n");
		input_report_key(kpd_input_dev, KEY_LEFT, 0);
		input_sync(kpd_input_dev);
		break;
	case PRESS_RIGHT_KEY:
		printk("[AUTOTEST] PRESS RIGHT KEY!!\n");
		input_report_key(kpd_input_dev, KEY_RIGHT, 1);
		input_sync(kpd_input_dev);
		break;
	case RELEASE_RIGHT_KEY:
		printk("[AUTOTEST] RELEASE RIGHT KEY!!\n");
		input_report_key(kpd_input_dev, KEY_RIGHT, 0);
		input_sync(kpd_input_dev);
		break;
	case PRESS_HOME_KEY:
		printk("[AUTOTEST] PRESS HOME KEY!!\n");
		input_report_key(kpd_input_dev, KEY_HOME, 1);
		input_sync(kpd_input_dev);
		break;
	case RELEASE_HOME_KEY:
		printk("[AUTOTEST] RELEASE HOME KEY!!\n");
		input_report_key(kpd_input_dev, KEY_HOME, 0);
		input_sync(kpd_input_dev);
		break;
	case PRESS_BACK_KEY:
		printk("[AUTOTEST] PRESS BACK KEY!!\n");
		input_report_key(kpd_input_dev, KEY_BACK, 1);
		input_sync(kpd_input_dev);
		break;
	case RELEASE_BACK_KEY:
		printk("[AUTOTEST] RELEASE BACK KEY!!\n");
		input_report_key(kpd_input_dev, KEY_BACK, 0);
		input_sync(kpd_input_dev);
		break;
	case PRESS_CALL_KEY:
		printk("[AUTOTEST] PRESS CALL KEY!!\n");
		input_report_key(kpd_input_dev, KEY_CALL, 1);
		input_sync(kpd_input_dev);
		break;
	case RELEASE_CALL_KEY:
		printk("[AUTOTEST] RELEASE CALL KEY!!\n");
		input_report_key(kpd_input_dev, KEY_CALL, 0);
		input_sync(kpd_input_dev);
		break;
	case PRESS_ENDCALL_KEY:
		printk("[AUTOTEST] PRESS ENDCALL KEY!!\n");
		input_report_key(kpd_input_dev, KEY_ENDCALL, 1);
		input_sync(kpd_input_dev);
		break;
	case RELEASE_ENDCALL_KEY:
		printk("[AUTOTEST] RELEASE ENDCALL KEY!!\n");
		input_report_key(kpd_input_dev, KEY_ENDCALL, 0);
		input_sync(kpd_input_dev);
		break;
	case PRESS_VLUP_KEY:
		printk("[AUTOTEST] PRESS VOLUMEUP KEY!!\n");
		input_report_key(kpd_input_dev, KEY_VOLUMEUP, 1);
		input_sync(kpd_input_dev);
		break;
	case RELEASE_VLUP_KEY:
		printk("[AUTOTEST] RELEASE VOLUMEUP KEY!!\n");
		input_report_key(kpd_input_dev, KEY_VOLUMEUP, 0);
		input_sync(kpd_input_dev);
		break;
	case PRESS_VLDOWN_KEY:
		printk("[AUTOTEST] PRESS VOLUMEDOWN KEY!!\n");
		input_report_key(kpd_input_dev, KEY_VOLUMEDOWN, 1);
		input_sync(kpd_input_dev);
		break;
	case RELEASE_VLDOWN_KEY:
		printk("[AUTOTEST] RELEASE VOLUMEDOWN KEY!!\n");
		input_report_key(kpd_input_dev, KEY_VOLUMEDOWN, 0);
		input_sync(kpd_input_dev);
		break;
	case PRESS_FOCUS_KEY:
		printk("[AUTOTEST] PRESS FOCUS KEY!!\n");
		input_report_key(kpd_input_dev, KEY_FOCUS, 1);
		input_sync(kpd_input_dev);
		break;
	case RELEASE_FOCUS_KEY:
		printk("[AUTOTEST] RELEASE FOCUS KEY!!\n");
		input_report_key(kpd_input_dev, KEY_FOCUS, 0);
		input_sync(kpd_input_dev);
		break;
	case PRESS_CAMERA_KEY:
		printk("[AUTOTEST] PRESS CAMERA KEY!!\n");
		input_report_key(kpd_input_dev, KEY_CAMERA, 1);
		input_sync(kpd_input_dev);
		break;
	case RELEASE_CAMERA_KEY:
		printk("[AUTOTEST] RELEASE CAMERA KEY!!\n");
		input_report_key(kpd_input_dev, KEY_CAMERA, 0);
		input_sync(kpd_input_dev);
		break;
#endif

	case SET_KPD_BACKLIGHT:
		if (copy_from_user(&ledctl, uarg, sizeof(struct kpd_ledctl)))
			return -EFAULT;

		//kpd_set_backlight(ledctl.onoff, &ledctl.div, &ledctl.duty);
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static int kpd_dev_open(struct inode *inode, struct file *file)
{
	return 0;
}

static struct file_operations kpd_dev_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl	= kpd_dev_ioctl,
	.open		= kpd_dev_open,
};

static struct miscdevice kpd_dev = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= KPD_NAME,
	.fops	= &kpd_dev_fops,
};

static int kpd_open(struct input_dev *dev)
{
#if KPD_HAS_SLIDE_QWERTY
	bool evdev_flag=false;
	bool power_op=false;
	struct input_handler *handler;
	struct input_handle *handle;
	handle = rcu_dereference(dev->grab);
	if (handle)
	{
		handler = handle->handler;
		if(strcmp(handler->name, "evdev")==0) 
		{
			return -1;
		}	
	}
	else 
	{
		list_for_each_entry_rcu(handle, &dev->h_list, d_node) {
			handler = handle->handler;
			if(strcmp(handler->name, "evdev")==0) 
			{
				evdev_flag=true;
				break;
			}
		}
		if(evdev_flag==false)
		{
			return -1;	
		}	
	}

	power_op = powerOn_slidePin_interface();
	if(!power_op) {
		printk(KPD_SAY "Qwerty slide pin interface power on fail\n");
	} else {
		kpd_print("Qwerty slide pin interface power on success\n");
	}
		
	mt65xx_eint_set_sens(KPD_SLIDE_EINT, KPD_SLIDE_SENSITIVE);
	mt65xx_eint_set_hw_debounce(KPD_SLIDE_EINT, KPD_SLIDE_DEBOUNCE);
	mt65xx_eint_registration(KPD_SLIDE_EINT, true, KPD_SLIDE_POLARITY,
	                         kpd_slide_eint_handler, false);
	                         
	power_op = powerOff_slidePin_interface();
	if(!power_op) {
		printk(KPD_SAY "Qwerty slide pin interface power off fail\n");
	} else {
		kpd_print("Qwerty slide pin interface power off success\n");
	}

#if 0
	/*qwerty slide: GPIO 214. input, mode=EINT, pull enbale, pull select high*/
	mt_set_gpio_mode(214, 2);
	mt_set_gpio_dir(214, 0);
	mt_set_gpio_pull_enable(214, 1);
	mt_set_gpio_pull_select(214, 0);
#endif
#endif	
	return 0;
}

static int kpd_pdrv_probe(struct platform_device *pdev)
{
	int i, r;

	/* initialize and register input device (/dev/input/eventX) */
	kpd_input_dev = input_allocate_device();
	if (!kpd_input_dev)
		return -ENOMEM;

	kpd_input_dev->name = KPD_NAME;
	kpd_input_dev->id.bustype = BUS_HOST;
	kpd_input_dev->id.vendor = 0x2454;
	kpd_input_dev->id.product = 0x6575;
	kpd_input_dev->id.version = 0x0010;
	kpd_input_dev->open = kpd_open;

	__set_bit(EV_KEY, kpd_input_dev->evbit);

#if (KPD_PWRKEY_USE_EINT||KPD_PWRKEY_USE_PMIC)
	__set_bit(KPD_PWRKEY_MAP, kpd_input_dev->keybit);
	kpd_keymap[8] = 0;
#endif
	for (i = 17; i < KPD_NUM_KEYS; i += 9)	/* only [8] works for Power key */
		kpd_keymap[i] = 0;

	for (i = 0; i < KPD_NUM_KEYS; i++) {
		if (kpd_keymap[i] != 0)
			__set_bit(kpd_keymap[i], kpd_input_dev->keybit);
	}

#if KPD_AUTOTEST
	for (i = 0; i < ARRAY_SIZE(kpd_auto_keymap); i++)
		__set_bit(kpd_auto_keymap[i], kpd_input_dev->keybit);
#endif

#if KPD_HAS_SLIDE_QWERTY
	__set_bit(EV_SW, kpd_input_dev->evbit);
	__set_bit(SW_LID, kpd_input_dev->swbit);
#endif

#ifdef KPD_PMIC_RSTKEY_MAP
	__set_bit(KPD_PMIC_RSTKEY_MAP, kpd_input_dev->keybit);
#endif

	kpd_input_dev->dev.parent = &pdev->dev;
	r = input_register_device(kpd_input_dev);
	if (r) {
		printk(KPD_SAY "register input device failed (%d)\n", r);
		input_free_device(kpd_input_dev);
		return r;
	}

	/* register device (/dev/mt6575-kpd) */
	kpd_dev.parent = &pdev->dev;
	r = misc_register(&kpd_dev);
	if (r) {
		printk(KPD_SAY "register device failed (%d)\n", r);
		input_unregister_device(kpd_input_dev);
		return r;
	}

	/* register IRQ and EINT */
	kpd_set_debounce(KPD_KEY_DEBOUNCE);
	mt65xx_irq_set_sens(MT6577_KP_IRQ_ID, MT65xx_EDGE_SENSITIVE);
	mt65xx_irq_set_polarity(MT6577_KP_IRQ_ID, MT65xx_POLARITY_LOW);
	r = request_irq(MT6577_KP_IRQ_ID, kpd_irq_handler, 0, KPD_NAME, NULL);
	if (r) {
		printk(KPD_SAY "register IRQ failed (%d)\n", r);
		misc_deregister(&kpd_dev);
		input_unregister_device(kpd_input_dev);
		return r;
	}

#if KPD_PWRKEY_USE_EINT
	mt65xx_eint_set_sens(KPD_PWRKEY_EINT, KPD_PWRKEY_SENSITIVE);
	mt65xx_eint_set_hw_debounce(KPD_PWRKEY_EINT, KPD_PWRKEY_DEBOUNCE);
	mt65xx_eint_registration(KPD_PWRKEY_EINT, true, KPD_PWRKEY_POLARITY,
	                         kpd_pwrkey_eint_handler, false);
#endif

#ifndef KPD_EARLY_PORTING /*add for avoid early porting build err the macro is defined in custom file*/
	if(kpd_enable_lprst && get_boot_mode() == NORMAL_BOOT) {
		kpd_print("Normal Boot\n");
#ifdef KPD_PMIC_LPRST_TD
		kpd_print("Enable LPRST\n");
		upmu_testmode_pwrkey_rst_en(0x01);
		upmu_testmode_homekey_rst_en(0x01);
		upmu_testmode_pwrkey_rst_td(KPD_PMIC_LPRST_TD);
#endif
	} else {
		kpd_print("Disable LPRST %d\n", kpd_enable_lprst);
	}
#endif	
	hrtimer_init(&aee_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	aee_timer.function = aee_timer_func;

#if AEE_ENABLE_5_15
    hrtimer_init(&aee_timer_5s, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    aee_timer_5s.function = aee_timer_5s_func;


#endif

	
#if 0
	/* KCOL0: GPIO103: KCOL1: GPIO108, KCOL2: GPIO105 input + pull enable + pull up */
	mt_set_gpio_mode(103, 1);
	mt_set_gpio_dir(103, 0);
	mt_set_gpio_pull_enable(103, 1);
	mt_set_gpio_pull_select(103, 1);
	
	
	mt_set_gpio_mode(108, 1);
	mt_set_gpio_dir(108, 0);
	mt_set_gpio_pull_enable(108, 1);
	mt_set_gpio_pull_select(108, 1);
	
	mt_set_gpio_mode(105, 1);
	mt_set_gpio_dir(105, 0);
	mt_set_gpio_pull_enable(105, 1);
	mt_set_gpio_pull_select(105, 1);
	/* KROW0: GPIO98, KROW1: GPIO97: KROW2: GPIO95 output + pull disable + pull down */
	mt_set_gpio_mode(98, 1);
	mt_set_gpio_dir(98, 1);
	mt_set_gpio_pull_enable(98, 0);	
	mt_set_gpio_pull_select(98, 0);
	
	mt_set_gpio_mode(97, 1);
	mt_set_gpio_dir(97, 1);
	mt_set_gpio_pull_enable(97, 0);	
	mt_set_gpio_pull_select(97, 0);
	
	mt_set_gpio_mode(95, 1);
	mt_set_gpio_dir(95, 1);
	mt_set_gpio_pull_enable(95, 0);		
	mt_set_gpio_pull_select(95, 0);
#endif
	return 0;
}

/* should never be called */
static int kpd_pdrv_remove(struct platform_device *pdev)
{
	return 0;
}

#ifndef CONFIG_HAS_EARLYSUSPEND
static int kpd_pdrv_suspend(struct platform_device *pdev, pm_message_t state)
{
	kpd_suspend = true;
	kpd_disable_backlight();
	kpd_print("suspend!! (%d)\n", kpd_suspend);
	return 0;
}

static int kpd_pdrv_resume(struct platform_device *pdev)
{
	kpd_suspend = false;
	//kpd_enable_backlight();
	kpd_print("resume!! (%d)\n", kpd_suspend);
	return 0;
}
#else
#define kpd_pdrv_suspend	NULL
#define kpd_pdrv_resume		NULL
#endif

static struct platform_driver kpd_pdrv = {
	.probe		= kpd_pdrv_probe,
	.remove		= kpd_pdrv_remove,
	.suspend	= kpd_pdrv_suspend,
	.resume		= kpd_pdrv_resume,
	.driver		= {
		.name	= KPD_NAME,
		.owner	= THIS_MODULE,
	},
};

#ifdef CONFIG_HAS_EARLYSUSPEND
static void kpd_early_suspend(struct early_suspend *h)
{
	kpd_suspend = true;
	kpd_disable_backlight();
	kpd_print("early suspend!! (%d)\n", kpd_suspend);
}

static void kpd_early_resume(struct early_suspend *h)
{
	kpd_suspend = false;
	//kpd_enable_backlight();
	kpd_print("early resume!! (%d)\n", kpd_suspend);
}

static struct early_suspend kpd_early_suspend_desc = {
	.level		= EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1,
	.suspend	= kpd_early_suspend,
	.resume		= kpd_early_resume,
};
#endif

static int __init kpd_mod_init(void)
{
	int r;

#if KPD_DRV_CTRL_BACKLIGHT
	for (r = 0; r < ARRAY_SIZE(kpd_wake_key); r++)
		__set_bit(kpd_wake_key[r], kpd_wake_keybit);
#endif

	r = platform_driver_register(&kpd_pdrv);
	if (r) {
		printk(KPD_SAY "register driver failed (%d)\n", r);
		return r;
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	register_early_suspend(&kpd_early_suspend_desc);
#endif
	return 0;
}

/* should never be called */
static void __exit kpd_mod_exit(void)
{
}

module_init(kpd_mod_init);
module_exit(kpd_mod_exit);

module_param(kpd_show_hw_keycode, bool, 0644);
module_param(kpd_show_register, bool, 0644);
module_param(kpd_enable_lprst, bool, 0644);

MODULE_AUTHOR("Terry Chang <terry.chang@mediatek.com>");
MODULE_DESCRIPTION("MT6575 Keypad (KPD) Driver v0.3");
MODULE_LICENSE("GPL");
