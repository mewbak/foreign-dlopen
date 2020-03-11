# foreign-dlopen

(README is WIP.)

Would like to load a shared library from statically linked executable. Not
only that, would like to e.g. glibc-linked .so from musl-linked exe. Idea #1:
a (custom) ELF loader. Problem: trying to implement "full" ELF loader
(recursively load dependent .so, etc.) is prolematic, e.g. because glibc
(libc.so) is tightly coupled with dynamic loader aka interpreter (ld.so).
If you just load libc.so, and its dependendency ld.so, a lot of stuff in
ld.so will remain uninitialized, then libc.so will call into ld.so, which
will crash. To properly initialize ld.so, it must be "run", i.e. execution
should go into its entry point (and not just ELF INIT func). But when
started that way, ld.so loads an executable, which then terminates.

Idea #2: Make ld.so load an executable which will jump back into our custom
loader. This way, both ld.so will be initialized, and we get back control.
Coupled with setjmp/longjmp, this can be into a reusable library. Structure:

1. Custom ELF loader, built against any libc or lack thereof. This should be
simplified loader, without support for loading shared libs, etc. The only
thing it needs to load is a target executable and its INTERP.
2. It calls into INTERP, wrapped with setjmp. Corresponding longjmp is
wrapped into a global func, whose address (in ascii) we pass as a command-line
argument to a target binary.
3. Target binary should be linked against native libc of the target environment
whose shared libs we want to load (e.g., glibc).
4. Target binary is also linked agains target libc's libdl. The binary captures
addresses of dlopen()/dlsym()/etc. funcs into an array, and performs function
call into address passed as a command-line arg, passing array as function arg.
5. That address is, as mentioned above, is function which performs longjmp,
after storing passed function addresses.
6. After longjmp, we're back to caller.

This project is based on https://github.com/MikhailProg/elf , original
README follows.

---

# ELF loader

A small elf loader. It can load static and dynamically linked ELF EXEC and DYN (pie) binaries. The loader is PIE program that doesn't depend on libc and calls kernel services directly (z_syscall.c).

If the loader needs to load a dynamically linked ELF it places an interpreter (usually ld.so) and a requested binary into a memory and then calls the interpreter entry point.


## Build

Default build is for amd64:

```
$ make
``` 

Build for i386:

```
$ make ARCH=i386
```

Small build (exclude all messages and printf):

```
$ make SMALL=1
```

## Load binaries

Load ls:
```
$ ./loader /bin/ls
```

Load galculator:
```
$ ./loader /usr/bin/galculator
```

