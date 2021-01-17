﻿/* File: maid-x11.c */

/*
 * Copyright (c) 1997 Ben Harrison, and others
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.
 */

#ifdef USE_X11

#include "main/x11-gamma-builder.h"
#include <math.h>

/*
 * This file defines some "XImage" manipulation functions for X11.
 *
 * Original code by Desvignes Sebastien (desvigne@solar12.eerie.fr).
 *
 * BMP format support by Denis Eropkin (denis@dream.homepage.ru).
 *
 * Major fixes and cleanup by Ben Harrison (benh@phial.com).
 *
 * This file is designed to be "included" by "main-x11.c" or "main-xaw.c",
 * which will have already "included" several relevant header files.
 */

#ifndef IsModifierKey

/*
 * Keysym macros, used on Keysyms to test for classes of symbols
 * These were stolen from one of the X11 header files
 *
 * Also appears in "main-x11.c".
 */

#define IsKeypadKey(keysym) \
    (((unsigned)(keysym) >= XK_KP_Space) && ((unsigned)(keysym) <= XK_KP_Equal))

#define IsCursorKey(keysym) \
    (((unsigned)(keysym) >= XK_Home) && ((unsigned)(keysym) < XK_Select))

#define IsPFKey(keysym) \
    (((unsigned)(keysym) >= XK_KP_F1) && ((unsigned)(keysym) <= XK_KP_F4))

#define IsFunctionKey(keysym) \
    (((unsigned)(keysym) >= XK_F1) && ((unsigned)(keysym) <= XK_F35))

#define IsMiscFunctionKey(keysym) \
    (((unsigned)(keysym) >= XK_Select) && ((unsigned)(keysym) < XK_KP_Space))

#define IsModifierKey(keysym) \
    (((unsigned)(keysym) >= XK_Shift_L) && ((unsigned)(keysym) <= XK_Hyper_R))

#endif /* IsModifierKey */

/*
 * Checks if the keysym is a special key or a normal key
 * Assume that XK_MISCELLANY keysyms are special
 *
 * Also appears in "main-x11.c".
 */
#define IsSpecialKey(keysym) \
    ((unsigned)(keysym) >= 0xFF00)

static bool gamma_table_ready = FALSE;
static int gamma_val = 0;

/*
 * Hack -- Convert an RGB value to an X11 Pixel, or die.
 */
static XftColor create_pixel(Display* dpy, byte red, byte green, byte blue) {
    Colormap cmap = DefaultColormapOfScreen(DefaultScreenOfDisplay(dpy));
    XColor xcolour;
    if (!gamma_table_ready) {
        concptr str = getenv("ANGBAND_X11_GAMMA");
        if (str != NULL) gamma_val = atoi(str);

        gamma_table_ready = TRUE;

        /* Only need to build the table if gamma exists */
        if (gamma_val) build_gamma_table(gamma_val);
    }

    /* Hack -- Gamma Correction */
    if (gamma_val > 0) {
        red = gamma_table[red];
        green = gamma_table[green];
        blue = gamma_table[blue];
    }

    /* Build the color */

    xcolour.red = red * 255;
    xcolour.green = green * 255;
    xcolour.blue = blue * 255;
    xcolour.flags = DoRed | DoGreen | DoBlue;

    XftColor color;
    XRenderColor xcol;
    xcol.red = xcolour.red;
    xcol.green = xcolour.green;
    xcol.blue = xcolour.blue;
    if (!XftColorAllocValue(dpy, DefaultVisual(dpy, 0), cmap, &xcol, &color)) {
        quit_fmt("Couldn't allocate bitmap color '#%02x%02x%02x'\n",
            red, green, blue);
    }

    return color;
}

#endif /* USE_X11 */
