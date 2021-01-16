﻿/*!
 * @file h-config.h
 * @brief 主に変愚/Zang時追加された基本事項のヘッダーファイル /
 * The most basic "include" file. This file simply includes other low level header files.
 * @date 2014/08/15
 * @author
 * 不明(変愚蛮怒スタッフ？)
 * @details
 * <pre>
 * Choose the hardware, operating system, and compiler.
 * Also, choose various "system level" compilation options.
 * A lot of these definitions take effect in "h-system.h"
 * Note that you may find it simpler to define some of these
 * options in the "Makefile", especially any options describing
 * what "system" is being used.
 * no system definitions are needed for 4.3BSD, SUN OS, DG/UX
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#ifndef INCLUDED_H_CONFIG_H
#define INCLUDED_H_CONFIG_H

/*
 * OPTION: Compile on Windows (automatic)
 */
#ifndef WINDOWS
/* #define WINDOWS */
#endif

/*
 * Extract the "WINDOWS" flag from the compiler
 */
#if defined(_Windows) || defined(__WINDOWS__) || defined(__WIN32__) || defined(WIN32) || defined(__WINNT__) || defined(__NT__)
#ifndef WINDOWS
#define WINDOWS
#endif
#endif

/*
 * OPTION: Define "L64" if a "long" is 64-bits.  See "h-types.h".
 * The only such platform that angband is ported to is currently
 * DEC Alpha AXP running OSF/1 (OpenVMS uses 32-bit longs).
 */
#if defined(__alpha) && defined(__osf__)
#define L64
#endif

/*
 * OPTION: set "SET_UID" if the machine is a "multi-user" machine.
 * This option is used to verify the use of "uids" and "gids" for
 * various "Unix" calls, and of "pids" for getting a random seed,
 * and of the "umask()" call for various reasons, and to guess if
 * the "kill()" function is available, and for permission to use
 * functions to extract user names and expand "tildes" in filenames.
 * It is also used for "locking" and "unlocking" the score file.
 * Basically, SET_UID should *only* be set for "Unix" machines,
 * or for the "Atari" platform which is Unix-like, apparently
 */
#if !defined(WINDOWS) && !defined(VM)
#define SET_UID
#endif

/*
 * Every system seems to use its own symbol as a path separator.
 * Default to the standard Unix slash, but attempt to change this
 * for various other systems.  Note that any system that uses the
 * "period" as a separator will have to pretend that it uses the
 * slash, and do its own mapping of period <-> slash.
 * Note that the VM system uses a "flat" directory, and thus uses
 * the empty string for "PATH_SEP".
 */
#undef PATH_SEP
#define PATH_SEP "/"

#if defined(WINDOWS) || defined(WINNT)
#undef PATH_SEP
#define PATH_SEP "\\"
#endif

/*
 * Linux has "stricmp()" with a different name
 */
#if defined(linux)
#define stricmp strcasecmp
#endif

/*
 * OPTION: Define "HAVE_USLEEP" only if "usleep()" exists.
 *
 * Note that this is only relevant for "SET_UID" machines.
 */
#if defined(SET_UID) && !defined(HAVE_CONFIG_H)
#if !defined(ISC)
#define HAVE_USLEEP
#endif
#endif

#ifdef JP
#if defined(EUC)
#define iskanji(x) (((unsigned char)(x) >= 0xa1 && (unsigned char)(x) <= 0xfe) || (unsigned char)(x) == 0x8e)
#define iskana(x) (0)
#elif defined(SJIS)
#define iskanji(x) ((0x81 <= (unsigned char)(x) && (unsigned char)(x) <= 0x9f) || (0xe0 <= (unsigned char)(x) && (unsigned char)(x) <= 0xfc))
#define iskana(x) (((unsigned char)(x) >= 0xA0) && ((unsigned char)(x) <= 0xDF))
#else
#error Oops! Please define "EUC" or "SJIS" for kanji-code of your system.
#endif
#endif

#endif /* INCLUDED_H_CONFIG_H */

#ifndef HAVE_CONFIG_H

#ifdef JP
#define USE_XIM
#endif

#if defined(USE_XIM)
#define USE_LOCALE
#endif

#endif /* HAVE_CONFIG_H */

/*
 * Look through the following lines, and where a comment includes the
 * tag "OPTION:", examine the associated "#define" statements, and decide
 * whether you wish to keep, comment, or uncomment them.  You should not
 * have to modify any lines not indicated by "OPTION".
 *
 * Note: Also examine the "system" configuration file "h-config.h"
 * and the variable initialization file "variable.c".  If you change
 * anything in "variable.c", you only need to recompile that file.
 *
 * And finally, remember that the "Makefile" will specify some rather
 * important compile time options, like what visual module to use.
 */

/*
 * OPTION: for multi-user machines running the game setuid to some other
 * user (like 'games') this SAFE_SETUID option allows the program to drop
 * its privileges when saving files that allow for user specified pathnames.
 * This lets the game be installed system wide without major security
 * concerns.  There should not be any side effects on any machines.
 *
 * This will handle "gids" correctly once the permissions are set right.
 */
#define SAFE_SETUID

/*
 * This flag enables the "POSIX" methods for "SAFE_SETUID".
 */
#ifdef _POSIX_SAVED_IDS
#define SAFE_SETUID_POSIX
#endif

/*
 * OPTION: Set the "default" path to the angband "lib" directory.
 *
 * See "main.c" for usage, and note that this value is only used on
 * certain machines, primarily Unix machines.  If this value is used,
 * it will be over-ridden by the "ANGBAND_PATH" environment variable,
 * if that variable is defined and accessable.  The final slash is
 * optional, but it may eventually be required.
 *
 * Using the value "./lib/" below tells Angband that, by default,
 * the user will run "angband" from the same directory that contains
 * the "lib" directory.  This is a reasonable (but imperfect) default.
 *
 * If at all possible, you should change this value to refer to the
 * actual location of the "lib" folder, for example, "/tmp/angband/lib/"
 * or "/usr/games/lib/angband/", or "/pkg/angband/lib".
 */
#ifndef DEFAULT_LIB_PATH
#define DEFAULT_LIB_PATH "./lib/"
#endif

/*
 * OPTION: Set the "default" path to the angband "var" directory.
 *
 * This is like DEFAULT_LIB_PATH, but is for files that will be
 * modified after installation.
 */
#ifndef DEFAULT_VAR_PATH
#define DEFAULT_VAR_PATH DEFAULT_LIB_PATH
#endif

/*
 * OPTION: Create and use a hidden directory in the users home directory
 * for storing pref-files and character-dumps.
 */
#ifdef SET_UID
#define PRIVATE_USER_PATH "~/.angband"
#endif /* SET_UID */

/*
 * On multiuser systems, add the "uid" to savefile names
 */
#ifdef SET_UID
#define SAVEFILE_USE_UID
#endif

/*
 * OPTION: Person to bother if something goes wrong.
 */
/* #define MAINTAINER	"rr9@angband.org" */
#define MAINTAINER "echizen@users.sourceforge.jp"

#ifdef JP
#ifdef USE_XFT
#define DEFAULT_X11_FONT "monospace-24:lang=ja:spacing=90"
#define DEFAULT_X11_FONT_SUB "sans-serif-16:lang=ja"
#else
/*
 * OPTION: Default font (when using X11).
 */
#define DEFAULT_X11_FONT                                                                                                                                       \
    "-*-*-medium-r-normal--24-*-*-*-*-*-iso8859-1"                                                                                                             \
    ",-*-*-medium-r-normal--24-*-*-*-*-*-jisx0208.1983-0"
/*	"12x24" \
        ",kanji24"*/
#define DEFAULT_X11_FONT_SUB                                                                                                                                   \
    "-*-*-medium-r-normal--16-*-*-*-*-*-iso8859-1"                                                                                                             \
    ",-*-*-medium-r-normal--16-*-*-*-*-*-jisx0208.1983-0"
#endif
/*	"8x16" \
        ",kanji16"*/

/*
 * OPTION: Default fonts (when using X11)
 */
#define DEFAULT_X11_FONT_0 DEFAULT_X11_FONT
#define DEFAULT_X11_FONT_1 DEFAULT_X11_FONT_SUB
#define DEFAULT_X11_FONT_2 DEFAULT_X11_FONT_SUB
#define DEFAULT_X11_FONT_3 DEFAULT_X11_FONT_SUB
#define DEFAULT_X11_FONT_4 DEFAULT_X11_FONT_SUB
#define DEFAULT_X11_FONT_5 DEFAULT_X11_FONT_SUB
#define DEFAULT_X11_FONT_6 DEFAULT_X11_FONT_SUB
#define DEFAULT_X11_FONT_7 DEFAULT_X11_FONT_SUB

#else
/*
 * OPTION: Default font (when using X11).
 */
#define DEFAULT_X11_FONT "9x15"

/*
 * OPTION: Default fonts (when using X11)
 */
#define DEFAULT_X11_FONT_0 "10x20"
#define DEFAULT_X11_FONT_1 "9x15"
#define DEFAULT_X11_FONT_2 "9x15"
#define DEFAULT_X11_FONT_3 "5x8"
#define DEFAULT_X11_FONT_4 "5x8"
#define DEFAULT_X11_FONT_5 "5x8"
#define DEFAULT_X11_FONT_6 "5x8"
#define DEFAULT_X11_FONT_7 "5x8"
#endif
