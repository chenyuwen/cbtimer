#ifndef __CPU_H__
#define __CPU_H__

#define _alwaysinline_ __inline 

#ifndef NULL
#define NULL ((void *)0x0)
#endif /*NULL*/

typedef unsigned long psr_t;

_alwaysinline_ psr_t ENTER_CRITICAL(void);
_alwaysinline_ void EXIT_CRITICAL(psr_t psr);


#endif /*__CPU_H__*/

