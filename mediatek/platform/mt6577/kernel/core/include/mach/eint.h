#ifndef __MT6577_EINT_H__
#define __MT6577_EINT_H__

/*
 * Define hardware registers.
 */

#define EINT_STA        (EINT_BASE + 0x0000)
#define EINT_INTACK     (EINT_BASE + 0x0008)
#define EINT_EEVT       (EINT_BASE + 0x0010)
#define EINT_MASK       (EINT_BASE + 0x0018)
#define EINT_MASK_SET   (EINT_BASE + 0x0020)
#define EINT_MASK_CLR   (EINT_BASE + 0x0028)
#define EINT_SENS       (EINT_BASE + 0x0030)
#define EINT_SENS_SET   (EINT_BASE + 0x0038)
#define EINT_SENS_CLR   (EINT_BASE + 0x0040)
#define EINT_D0EN       (EINT_BASE + 0x0060)
#define EINT_D1EN       (EINT_BASE + 0x0068)
#define EINT_D2EN       (EINT_BASE + 0x0070)
#define EINT_CON(n)     (EINT_BASE + 0x0080 + 4 * (n))
#define EINT_DMASK      (EINT_BASE + 0x0100)
#define EINT_DMASK_SET  (EINT_BASE + 0x0110)
#define EINT_DMASK_CLR  (EINT_BASE + 0x0120)

/*
 * Define constants.
 */

#define EINT_MAX_CHANNEL    (32)
#define MT65XX_EINT_POL_NEG (0)
#define MT65XX_EINT_POL_POS (1)

/*
 * Define function prototypes.
 */

extern void mt65xx_eint_mask(unsigned int eint_num);
extern void mt65xx_eint_unmask(unsigned int eint_num);
extern void mt65xx_deint_mask(unsigned int eint_num);
extern void mt65xx_deint_unmask(unsigned int eint_num);
extern void mt65xx_eint_set_hw_debounce(unsigned int eint_num, unsigned int ms);
extern void mt65xx_eint_set_polarity(unsigned int eint_num, unsigned int pol);
extern unsigned int mt65xx_eint_set_sens(unsigned int eint_num, unsigned int sens);
extern void mt65xx_eint_registration(unsigned int eint_num, unsigned int is_deb_en, unsigned int pol, void (EINT_FUNC_PTR)(void), unsigned int is_auto_umask);
extern void mt65xx_deint_registration(unsigned int eint_num, unsigned int pol, void (EINT_FUNC_PTR)(void), unsigned int is_auto_umask);
extern int mt65xx_eint_init(void);
extern int get_td_eint_num(char *td_name, int len);

#endif  /*!__MT6577_EINT_H__ */
