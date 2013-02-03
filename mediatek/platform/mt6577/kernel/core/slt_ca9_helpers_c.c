#include <asm/current.h>
#include <linux/sched.h>
#include "mach/mt6577_reg_base.h"
#include "mach/sync_write.h"
#include "mach/ca9_slt.h"

#if defined(__SLT_USE_GFP__)

//#define SLT_VA (0x70000000)

static unsigned int g_iNMRR_BAK;
static unsigned int g_iPA_section;
static struct task_struct *g_pCurrent;
extern unsigned int slt_get_ttbr0(void);
extern void slt_dsb(void);
extern void slt_isb(void);
extern void slt_invalidate_tlb(void);
extern void v7_flush_kern_cache_all(void);
extern unsigned int slt_get_nmrr(void);
extern void slt_set_nmrr(const unsigned int nmrr);
extern void slt_l1c_clean_mva_to_pou(const unsigned int mva);
extern unsigned int slt_va_to_pa(unsigned int mva);

void slt_flush_l2c(void)
{
//    mt65xx_reg_sync_writel(0xFFFF, PL310_BASE + 0x7FC);
    volatile unsigned int *pCleanInvalidateReg = (unsigned int *) (PL310_BASE + 0x7FC);
    volatile unsigned int *pCacheSyncReg = (unsigned int *) (PL310_BASE + 0x730);
    
    *pCleanInvalidateReg = 0xFFFF;
    *pCacheSyncReg = 0;
    while (*pCleanInvalidateReg != 0 || *pCacheSyncReg != 0);
}

void slt_l2_clean_pa(const unsigned int iPA)
{
    volatile unsigned int *pCleanPA = (unsigned int *) (PL310_BASE + 0x7B0);
    volatile unsigned int *pCacheSyncReg = (unsigned int *) (PL310_BASE + 0x730);
    
    *pCleanPA = iPA & ~0x1F;
    *pCacheSyncReg = 0;
    while (((*pCleanPA) & 0x1)!= 0 || *pCacheSyncReg != 0);
}

unsigned int my_get_pa(const unsigned int iVA)
{
    unsigned int iPA = slt_va_to_pa(iVA);

    iPA = (iPA & ~0xFFF) | (iVA & 0xFFF);

    return iPA;
}

int slt_get_section(const unsigned int iCP)
{
    unsigned int *pTTBR0;
    unsigned int nmrr;
    unsigned int iPA;

    nmrr = slt_get_nmrr();
    g_iNMRR_BAK = nmrr;
    nmrr &= ~((3 << 14) | (3 << 30));
    if (iCP == 0) { // L1C-WBWA, L2C-NC
        nmrr |= (1 << 14);
    }else { // L1C-NC, L2C-WBWA
        nmrr |= (1 << 30);
    }

    slt_set_nmrr(nmrr);

    g_iPA_section = 0x1FF00000;
    g_pCurrent = current;

    pTTBR0 = (unsigned int *) current->mm->pgd;
    pTTBR0[SLT_VA >> 20] = (g_iPA_section & ~((1 << 20) - 1))
                            | (0 << 19) /* NS = 0 */
                            | (0 << 17) /* nG = 1 */
                            | (0 << 16) /* S = 0 */
                            | ((0 << 15) | (3 << 10))   /* AP[2:0] = 3b011 */
                            | (2 << 5)  /* domain = 2 */
                            | (1 << 4)  /* XN = 1 */
                            | (1 << 12) /* TEX = 3b001 */
                            | (1 << 3)  /* C = 1 */
                            | (1 << 2)  /* B = 1 */
                            | (2 << 0)  /* Section type descriptor */;

#if 1
    /*
        the following sequence make sure the updated
        entry of PT cleaned to its physical memory
    */

    // clean PT entry from L1C to L2C
    slt_l1c_clean_mva_to_pou((unsigned int) &pTTBR0[SLT_VA >> 20]);
    slt_dsb();

    // L2C is physically indexd and physically tagged, must use physical address, not MVA
    iPA = my_get_pa((unsigned int) &pTTBR0[SLT_VA >> 20]);

    // clean PT entry from L2C to physical memory (MMU looks up physical memory only)
    slt_l2_clean_pa(iPA);

    // PT is updated, invalidate all TLB
    slt_invalidate_tlb();
    slt_dsb();
    slt_isb();
#else
    v7_flush_kern_cache_all();
    slt_flush_l2c();
    slt_invalidate_tlb();
    slt_dsb();
    slt_isb();
#endif

    return 1;
}

void slt_free_section(void)
{
    unsigned int *pTTBR0;
    unsigned int iPA;

    slt_set_nmrr(g_iNMRR_BAK);

    pTTBR0 = (unsigned int *) current->mm->pgd;
    pTTBR0[SLT_VA >> 20] = 0;

#if 1
    /*
        the following sequence make sure the updated
        entry of PT cleaned to its physical memory
    */

    // clean PT entry in L1C
    slt_l1c_clean_mva_to_pou((unsigned int) &pTTBR0[SLT_VA >> 20]);
    slt_dsb();

    // L2C is physically indexd and physically tagged, must use physical address, not MVA
    iPA = my_get_pa((unsigned int) &pTTBR0[SLT_VA >> 20]);

    // clean PT entry from L2C to physical memory (MMU looks up physical memory only)
    slt_l2_clean_pa(iPA);

    // PT is updated, invalidate all TLB
    slt_invalidate_tlb();
    slt_dsb();
    slt_isb();
#else
    v7_flush_kern_cache_all();
    slt_flush_l2c();
    slt_invalidate_tlb();
    slt_dsb();
    slt_isb();
#endif
}

#endif
