#ifndef __EXPORTS_H__
#define __EXPORTS_H__


#include <common.h>

ulong	get_version(void);
void		jumptable_init (void);


enum 
{
#define EXPORT_FUNC(x) XF_ ## x ,

EXPORT_FUNC(get_version)
EXPORT_FUNC(getc)
EXPORT_FUNC(tstc)
EXPORT_FUNC(putc)
EXPORT_FUNC(puts)
EXPORT_FUNC(printf)
EXPORT_FUNC(install_hdlr)
EXPORT_FUNC(free_hdlr)
EXPORT_FUNC(malloc)
EXPORT_FUNC(free)
EXPORT_FUNC(udelay)
EXPORT_FUNC(get_timer)
EXPORT_FUNC(vprintf)
EXPORT_FUNC(do_reset)
EXPORT_FUNC(getenv)
EXPORT_FUNC(setenv)
EXPORT_FUNC(simple_strtoul)
#undef EXPORT_FUNC

	XF_MAX
};

#define XF_VERSION	3


#endif	/* __EXPORTS_H__ */
