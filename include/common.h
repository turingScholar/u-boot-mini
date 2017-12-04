#ifndef __COMMON_H_
#define __COMMON_H_	1


#ifdef	DEBUG
#define	debug(fmt,args...)	printf (fmt ,##args)
#define	debugX(level,fmt,args...) if (DEBUG>=level) printf(fmt,##args);
#else
#define	debug(fmt,args...)
#define	debugX(level,fmt,args...)
#endif	/* DEBUG */


#include <config.h>
#include <types.h>
#include <string.h>
#include <stdarg.h>

#include <asm/global_data.h>

typedef void (interrupt_handler_t)(void *);


/* for the following variables, see start.S */

extern uint32_t _armboot_start;	/* code start */
extern uint32_t _bss_start;	/* code + data end == BSS start */
extern uint32_t _bss_end;		/* BSS end */
extern uint32_t IRQ_STACK_START;	/* top of IRQ stack */
extern uint32_t FIQ_STACK_START;	/* top of FIQ stack */

extern ulong monitor_flash_len;
extern ulong load_addr;		/* Default Load Address */


#define	stdin		0
#define	stdout		1
#define	stderr		2

#define	MAX_FILES	3


#define min(X, Y)				\
	({ typeof (X) __x = (X), __y = (Y);	\
		(__x < __y) ? __x : __y; })

#define max(X, Y)				\
	({ typeof (X) __x = (X), __y = (Y);	\
		(__x > __y) ? __x : __y; })


void	hang(void) __attribute__ ((noreturn));


/* dlmalloc.c */
void *	malloc(size_t);
void		free(void*);


/* cpu.c */
int		cpu_init(void);
int		icache_status(void);
void		icache_enable (void);
void		icache_disable(void);
int		dcache_status(void);
void		dcache_enable(void);
void		dcache_disable(void);
void		reset_cpu(ulong addr);
int		print_cpuinfo(void);


/* $(BOARD)/$(BOARD).c */
int		board_late_init (void);
int		board_init(void);
int		dram_init(void);
int		checkboard(void);


/* interrupts.c */
void		reset_timer_masked(void);
ulong	get_timer_masked(void);
ulong	get_tbclk(void);
int		interrupt_init(void);

void		reset_timer(void);
ulong	get_timer(ulong base);
void		set_timer(ulong t);
void		enable_interrupts  (void);
int		disable_interrupts (void);

unsigned long long get_ticks(void);
void		udelay(ulong);



/* console.c */
int		display_options (void);
void		print_size(ulong, const char *);
void		serial_printf (const char *fmt, ...);
int		getc(void);
int		tstc(void);

void		putc(const char c);
void		puts(const char *s);
void		printf(const char *fmt, ...);
void		vprintf(const char *fmt, va_list args);

void		fprintf(int file, const char *fmt, ...);
void		fputs(int file, const char *s);
void		fputc(int file, const char c);
int		ftstc(int file);
int		fgetc(int file);

int		console_init_f(void);
int		console_init_r(void);
int		console_assign (int file, char *devname);
int		ctrlc(void);
int		had_ctrlc(void);
void		clear_ctrlc(void);
int		disable_ctrlc(int);


/* common/main.c */
void		main_loop(void);
int		run_command(const char *cmd, int flag);
int		readline(const char *const prompt);


/* common/cmd_nvedit.c */
int		env_init(void);
void		env_relocate (void);
char	*	getenv(char *);
int		getenv_r(char *name, char *buf, unsigned len);
int		saveenv(void);
void		setenv(char *, char *);


/* common/exports.c */
void		jumptable_init(void);


/* $(CPU)/serial.c */
int		serial_init(void);
void		serial_setbrg(void);
int		serial_getc(void);
void		serial_putc(const char);
void		serial_puts(const char *);
int		serial_tstc(void);


/* lib_generic/vsprintf.c */
ulong	simple_strtoul(const char *cp,char **endp,unsigned int base);
long		simple_strtol(const char *cp,char **endp,unsigned int base);
void		panic(const char *fmt, ...);
int		sprintf(char * buf, const char *fmt, ...);
int		vsprintf(char *buf, const char *fmt, va_list args);


/* lib_generic/crc32.c */
ushort	cyg_crc16(unsigned char *s, int len);
ulong	crc32 (ulong, const unsigned char *, unsigned int);
ulong	crc32_no_comp (ulong, const unsigned char *, unsigned int);


#endif	/* __COMMON_H_ */
