#ifndef _WD_KICKER_H_
#define _WD_KICKER_H_

enum wk_wdt_mode {
	WK_WDT_NORMAL_MODE,
	WK_WDT_EXP_MODE
};

#ifdef CONFIG_LOCAL_WDT
enum wk_wdt_type {
	WK_WDT_LOC_TYPE,
	WK_WDT_EXT_TYPE
};

struct wk_wdt {
	int (*config)(enum wk_wdt_type, enum wk_wdt_mode, int timeout);
	void (*kick_wdt)(enum wk_wdt_type);
};
#else
struct wk_wdt {
	int (*config)(enum wk_wdt_mode, int timeout);
	void (*kick_wdt)(void);
};
#endif
int wk_register_wdt(struct wk_wdt *wk_wdt);

#endif
