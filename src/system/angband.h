﻿#pragma once
/*!
 * @file angband.h
 * @brief Angband(変愚蛮怒)メインヘッダファイル
 * Main "Angband" header file
 * @date 2014/08/08
 * @author
 * Copyright (c) 1989 James E. Wilson
 * @details *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 * This file has been modified for use with "Angband 2.8.2"
 */

#ifndef INCLUDED_ANGBAND_H
#define INCLUDED_ANGBAND_H

/*
 * First, include the low-level includes.  Be sure to edit "h-config.h"
 * to reflect any hardware, operating system, or compiler nuances.
 */
#include "system/h-basic.h"

/*
 * Then, include the header files for the low-level code
 */
#include "locale/language-switcher.h"
#include "term/z-form.h"
#include "term/z-rand.h"
#include "term/z-term.h"
#include "term/z-util.h"
#include "term/z-virt.h"

/*
 * Include the "Angband" configuration header
 */
#include "player/player-status.h"
#include "system/gamevalue.h"

/***** Some copyright messages follow below *****/

/*
 * Note that these copyright messages apply to an ancient version
 * of Angband, as in, from pre-2.4.frog-knows days, and thus the
 * reference to "5.0" is rather misleading...
 */

/*
 * UNIX ANGBAND Version 5.0
 */

/* Original copyright message follows. */

/*
 * ANGBAND Version 4.8	COPYRIGHT (c) Robert Alan Koeneke
 *
 *	 I lovingly dedicate this game to hackers and adventurers
 *	 everywhere...
 *
 *	 Designer and Programmer:
 *		Robert Alan Koeneke
 *		University of Oklahoma
 *
 *	 Assistant Programmer:
 *		Jimmey Wayne Todd
 *		University of Oklahoma
 *
 *	 Assistant Programmer:
 *		Gary D. McAdoo
 *		University of Oklahoma
 *
 *	 UNIX Port:
 *		James E. Wilson
 *		UC Berkeley
 *		wilson@ernie.Berkeley.EDU
 *		ucbvax!ucbernie!wilson
 */

/*
 *	 ANGBAND may be copied and modified freely as long as the above
 *	 credits are retained.	No one who-so-ever may sell or market
 *	 this software in any form without the expressed written consent
 *	 of the author Robert Alan Koeneke.
 */

#endif
