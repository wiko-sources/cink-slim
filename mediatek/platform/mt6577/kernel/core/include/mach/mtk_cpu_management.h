#ifndef _MTK_CPU_MANAGEMENT
#define _MTK_CPU_MANAGEMENT

#define DVFS_OPP1 (1)
#define DVFS_OPP2 (2)
#define DVFS_OPP3 (3)
#define DVFS_OPP4 (4)
#define DVFS_OPP5 (5)
#define DVFS_OPP6 (6)
/*
typedef enum {
	EVENT_THERMAL	= 0,
	EVENT_VIDEO,
	EVENT_SCREEN_ON_OFF,
	EVENT_COUNT
} mtk_cpu_management_event;
*/

struct module_info {
	char module_name[12];
	int mask[10];
	struct list_head link;
};

extern int cpu_opp_mask(char *name, int *mask, bool flag);
//extern int cpu_opp_limit(mtk_cpu_management_event event, int limited_freq, int limited_cpu, bool flag);


#endif //enf of #ifndef _MTK_CPU_MANAGEMENT