#ifndef _LINUX_STDDEF_H
#define _LINUX_STDDEF_H

#include <types.h>



#if defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#endif
