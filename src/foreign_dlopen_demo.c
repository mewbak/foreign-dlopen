#include "z_utils.h"
#include "z_syscalls.h"
#include "foreign_dlopen.h"

#define RTLD_NOW 0x0002

int main(int argc, char *argv[])
{
	(void)argc;

	init_exec_elf(argv);

	init_foreign_dlopen("fdlhelper");

	z_printf("Come back: dlopen=%p\n", z_dlopen);
	void *h = z_dlopen("libc.so.6", RTLD_NOW);
	z_printf("h=%p\n", h);
	void *p = z_dlsym(h, "printf");
	int (*_printf)(const char *fmt, ...) = p;
	_printf("hello, bastards!!1\n");

	void *h2 = z_dlopen("libz.so.1", RTLD_NOW);
	z_printf("h2=%p\n", h2);

	z_exit(0);
}
