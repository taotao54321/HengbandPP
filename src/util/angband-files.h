﻿#pragma once

#include "system/angband.h"

/* Force definitions -- see fd_seek() */
#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

/* Force definitions -- see fd_lock() */
#ifndef F_UNLCK
#define F_UNLCK 0
#endif
#ifndef F_RDLCK
#define F_RDLCK 1
#endif
#ifndef F_WRLCK
#define F_WRLCK 2
#endif

#ifdef SET_UID
extern void user_name(char* buf, int id);
#ifndef HAVE_USLEEP
extern int usleep(huge usecs);
#endif
#endif

errr path_parse(char* buf, int max, concptr file);
errr path_build(char* buf, int max, concptr path, concptr file);
FILE* angband_fopen(concptr file, concptr mode);
FILE* angband_fopen_temp(char* buf, int max);
errr angband_fgets(FILE* fff, char* buf, huge n);
errr angband_fputs(FILE* fff, concptr buf, huge n);
errr angband_fclose(FILE* fff);
errr fd_kill(concptr file);
errr fd_move(concptr file, concptr what);
errr fd_copy(concptr file, concptr what);
int fd_make(concptr file, BIT_FLAGS mode);
int fd_open(concptr file, int flags);
errr fd_lock(int fd, int what);
errr fd_seek(int fd, huge n);
errr fd_chop(int fd, huge n);
errr fd_read(int fd, char* buf, huge n);
errr fd_write(int fd, concptr buf, huge n);
errr fd_close(int fd);
