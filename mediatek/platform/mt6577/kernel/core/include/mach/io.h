#ifndef __MT6575_IO_H__
#define __MT6575_IO_H__

#define IO_SPACE_LIMIT 0xffffffff

#define __io(a)         ((void __iomem *)((a)))
#define __mem_pci(a)    (a)
#define __mem_isa(a)    (a)


#endif  /* !__MT6575_IO_H__ */

