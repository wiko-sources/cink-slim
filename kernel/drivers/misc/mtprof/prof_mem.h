#ifndef __PROF_ALLOCPAGES_H__
#define __PROF_ALLOCPAGES_H__

// alloc_pages profile

extern unsigned long num_good_pages;
extern unsigned long num_bad_pages;
extern unsigned long long good_pages_time;
extern unsigned long long bad_pages_time;
extern int mt_memprof_enabled;

extern void showing_memprof(struct seq_file *m, void *v);
#endif
