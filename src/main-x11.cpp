#include <algorithm>
#include <string>

#include "cmd-io/macro-util.h"
#include "game-option/runtime-arguments.h"
#include "game-option/special-options.h"
#include "io/files-util.h"
#include "main/sound-definitions-table.h"
#include "main/sound-of-music.h"
#include "main/x11-type-string.h"
#include "system/angband.h"
#include "system/system-variables.h"
#include "term/gameterm.h"
#include "term/term-color-types.h"
#include "util/angband-files.h"
#include "util/int-char-converter.h"
#include "util/string-processor.h"

/*
 * Available graphic modes
 */
#define GRAPHICS_NONE 0
#define GRAPHICS_ORIGINAL 1
#define GRAPHICS_ADAM_BOLT 2
#define GRAPHICS_HENGBAND 3

#ifndef __MAKEDEPEND__
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xlocale.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#endif /* __MAKEDEPEND__ */

#include <X11/Xft/Xft.h>

#include <iconv.h>

#include "prelude.hpp"

/*
 * Hack -- Convert an RGB value to an X11 Pixel, or die.
 */
static XftColor create_pixel(Display* dpy, byte red, byte green, byte blue) {
    Colormap cmap = DefaultColormapOfScreen(DefaultScreenOfDisplay(dpy));
    XColor xcolour;

    xcolour.red = red * 255;
    xcolour.green = green * 255;
    xcolour.blue = blue * 255;
    xcolour.flags = DoRed | DoGreen | DoBlue;

    XftColor color;
    XRenderColor xcol;
    xcol.red = xcolour.red;
    xcol.green = xcolour.green;
    xcol.blue = xcolour.blue;
    if (XftColorAllocValue(dpy, DefaultVisual(dpy, 0), cmap, &xcol, &color) == 0) {
        quit_fmt("Couldn't allocate bitmap color '#%02x%02x%02x'\n",
            red, green, blue);
    }

    return color;
}

static bool is_modifier_key(KeySym ks) {
    return (XK_Shift_L <= ks) && (ks <= XK_Hyper_R);
}

static bool is_special_key(KeySym ks) {
    return ks >= 0xFF00;
}

using Pixell = XftColor;

struct metadpy {
    Display* dpy;   // The Display itself
    Screen* screen; // The default Screen for the display
    Window root;    // The virtual root (usually just the root)
    Colormap cmap;  // The default colormap (from a macro)
    XIM xim;

    Pixell black; // The black Pixell (from a macro)
    Pixell white; // The white Pixell (from a macro)

    Pixell bg; // The background Pixell (default: black)
    Pixell fg; // The foreground Pixell (default: white)
};

/*
 * A Structure summarizing Window Information.
 *
 * I assume that a window is at most 30000 pixels on a side.
 * I assume that the root windw is also at most 30000 square.
 */
struct infowin {
    Window win;

    XIC xic;
    long xic_mask;

    XftDraw* draw;

    long mask;

    s16b ox, oy;

    s16b w, h;
};

struct infoclr {
    Pixell fg;
    Pixell bg;
};

/*
 * A Structure to Hold Font Information
 */
struct infofnt {
    XftFont* info;

    s16b wid;
    s16b twid;
    s16b hgt;
    s16b asc;
};

/*
 * The "default" values
 */
static metadpy metadpy_default;

/*
 * The "current" variables
 */
static metadpy* Metadpy = &metadpy_default;
static infowin* Infowin = nullptr;
static infowin* Focuswin = nullptr;
static infoclr* Infoclr = nullptr;
static infofnt* Infofnt = nullptr;

static errr Metadpy_init(const char* name) {
    Display* dpy = XOpenDisplay(name);
    if (!dpy) return -1;

    metadpy* m = Metadpy;
    m->dpy = dpy;
    m->screen = DefaultScreenOfDisplay(dpy);
    m->root = RootWindowOfScreen(m->screen);
    m->cmap = DefaultColormapOfScreen(m->screen);

    Visual* vis = DefaultVisual(dpy, 0);
    XftColorAllocName(dpy, vis, m->cmap, "black", &m->black);
    XftColorAllocName(dpy, vis, m->cmap, "white", &m->white);

    m->bg = m->black;
    m->fg = m->white;

    return 0;
}

/*
 * General Flush/ Sync/ Discard routine
 */
static errr Metadpy_update(bool flush, bool sync, bool discard) {
    if (flush)
        XFlush(Metadpy->dpy);
    if (sync)
        XSync(Metadpy->dpy, Bool(discard));

    return (0);
}

/*
 * Make a simple beep
 */
static errr Metadpy_do_beep() {
    XBell(Metadpy->dpy, 100);
    return (0);
}

/*
 * Set the name (in the title bar) of Infowin
 */
static void Infowin_set_name(const std::string& name) {
    XTextProperty tp;
    const auto* ptr = name.c_str();
    if (XStringListToTextProperty(const_cast<char**>(&ptr), 1, &tp) != 0)
        XSetWMName(Metadpy->dpy, Infowin->win, &tp);
}

/*
 * Prepare a new 'infowin'.
 */
static errr Infowin_prepare(Window xid) {
    infowin* iwin = Infowin;
    Window tmp_win;
    XWindowAttributes xwa;
    int x, y;
    unsigned int w, h, b, d;
    iwin->win = xid;
    XGetGeometry(Metadpy->dpy, xid, &tmp_win, &x, &y, &w, &h, &b, &d);

    Visual* vis = DefaultVisual(Metadpy->dpy, 0);
    if (vis->c_class != TrueColor) {
        quit_fmt("Display does not support truecolor.\n");
    }
    iwin->draw = XftDrawCreate(Metadpy->dpy, iwin->win, vis, Metadpy->cmap);

    iwin->w = w;
    iwin->h = h;

    XGetWindowAttributes(Metadpy->dpy, xid, &xwa);
    iwin->mask = xwa.your_event_mask;
    return (0);
}

static errr Infowin_init(int x, int y, int w, int h, int b, Pixell fg, Pixell bg) {
    (void)WIPE(Infowin, infowin);

    Window xid = XCreateSimpleWindow(Metadpy->dpy, Metadpy->root, x, y, w, h, b, fg.pixel, bg.pixel);

    XSelectInput(Metadpy->dpy, xid, 0L);
    return (Infowin_prepare(xid));
}

/*
 * Modify the event mask of an Infowin
 */
static errr Infowin_set_mask(long mask) {
    Infowin->mask = mask;
    XSelectInput(Metadpy->dpy, Infowin->win, Infowin->mask);
    return (0);
}

/*
 * Request that Infowin be mapped
 */
static errr Infowin_map() {
    XMapWindow(Metadpy->dpy, Infowin->win);
    return (0);
}

/*
 * Request that Infowin be raised
 */
static errr Infowin_raise() {
    XRaiseWindow(Metadpy->dpy, Infowin->win);
    return (0);
}

/*
 * Request that Infowin be moved to a new location
 */
static errr Infowin_impell(int x, int y) {
    XMoveWindow(Metadpy->dpy, Infowin->win, x, y);
    return (0);
}

/*
 * Resize an infowin
 */
static errr Infowin_resize(int w, int h) {
    XResizeWindow(Metadpy->dpy, Infowin->win, w, h);
    return (0);
}

/*
 * Visually clear Infowin
 */
static errr Infowin_wipe() {
    XClearWindow(Metadpy->dpy, Infowin->win);
    return (0);
}

static errr Infoclr_init(Pixell fg, Pixell bg) {
    infoclr* iclr = Infoclr;
    (void)WIPE(iclr, infoclr);

    iclr->fg = fg;
    iclr->bg = bg;
    return 0;
}

/*
 * Change the 'fg' for an infoclr
 *
 * Inputs:
 *	fg:   The Pixell for the requested Foreground (see above)
 */
static errr Infoclr_change_fg(Pixell fg) {
    infoclr* iclr = Infoclr;

    iclr->fg = fg;

    return (0);
}

/*
 * Prepare a new 'infofnt'
 */
static errr Infofnt_prepare(XftFont* info) {
    infofnt* ifnt = Infofnt;

    ifnt->info = info;

    ifnt->asc = info->ascent;
    ifnt->hgt = info->ascent + info->descent;
    const char* text = "A";
    XGlyphInfo extent;
    XftTextExtentsUtf8(Metadpy->dpy, info, (FcChar8*)text, strlen(text), &extent);
    ifnt->wid = extent.xOff;

    if (use_bigtile)
        ifnt->twid = 2 * ifnt->wid;
    else
        ifnt->twid = ifnt->wid;

    return (0);
}

/*
 * Init an infofnt by its Name
 *
 * Inputs:
 *	name: The name of the requested Font
 */
static void Infofnt_init_data(const std::string& name)

{
    if (name.empty())
        quit("Missing font!");

    XftFont* info = XftFontOpenName(Metadpy->dpy, 0, name.c_str());
    /* TODO: error handling */

    if (!info)
        quit_fmt("Failed to find font:\"%s\"", name.c_str());

    (void)WIPE(Infofnt, infofnt);
    if (Infofnt_prepare(info)) {
        XftFontClose(Metadpy->dpy, info);
        quit_fmt("Failed to prepare font:\"%s\"", name.c_str());
    }
}

/*
 * Standard Text
 */
static errr Infofnt_text_std(int x, int y, concptr str, int len) {
    if (!str || !*str)
        return (-1);

    if (len < 0)
        len = strlen(str);

    y = (y * Infofnt->hgt) + Infofnt->asc + Infowin->oy;
    x = (x * Infofnt->wid) + Infowin->ox;

    iconv_t cd = iconv_open("UTF-8", "EUC-JP");
    size_t inlen = len;
    size_t outlen = len * 2;
    char* kanji = (char*)malloc(outlen);
    char* sp;
    char* kp = kanji;
    char sbuf[1024];
    angband_strcpy(sbuf, str, sizeof(sbuf));
    sp = sbuf;
    iconv(cd, &sp, &inlen, &kp, &outlen);
    iconv_close(cd);

    XftDraw* draw = Infowin->draw;

    XRectangle r;
    r.x = 0;
    r.y = 0;
    r.width = Infofnt->wid * len;
    r.height = Infofnt->hgt;
    XftDrawSetClipRectangles(draw, x, y - Infofnt->asc, &r, 1);
    XftDrawRect(draw, &Infoclr->bg, x, y - Infofnt->asc, Infofnt->wid * len, Infofnt->hgt);
    XftDrawStringUtf8(draw, &Infoclr->fg, Infofnt->info, x, y, (FcChar8*)kanji, kp - kanji);
    XftDrawSetClip(draw, 0);

    free(kanji);

    return (0);
}

/*
 * Painting where text would be
 */
static errr Infofnt_text_non(int x, int y, concptr str, int len) {
    int w, h;
    if (len < 0)
        len = strlen(str);

    w = len * Infofnt->wid;
    x = x * Infofnt->wid + Infowin->ox;
    h = Infofnt->hgt;
    y = y * h + Infowin->oy;

    XftDrawRect(Infowin->draw, &Infoclr->fg, x, y, w, h);

    return (0);
}

/*
 * Angband specific code follows... (ANGBAND)
 */

/*
 * Hack -- cursor color
 */
static infoclr* xor_;

/*
 * Actual color table
 */
static infoclr* clr[256];

/*
 * Color info (unused, red, green, blue).
 */
static byte color_table[256][4];

/*
 * A structure for each "term"
 */
struct term_data {
    term_type t;
    infofnt* fnt;
    infowin* win;
};

constexpr int TERM_DATA_COUNT = 8;

static term_data data[TERM_DATA_COUNT];

/* Use short names for the most commonly used elements of various structures. */
#define DPY (Metadpy->dpy)
#define WIN (Infowin->win)

struct co_ord {
    int x;
    int y;
};

/*
 * A special structure to store information about the text currently
 * selected.
 */
struct x11_selection_type {
    bool select;  /* The selection is currently in use. */
    bool drawn;   /* The selection is currently displayed. */
    term_type* t; /* The window where the selection is found. */
    co_ord init;  /* The starting co-ordinates. */
    co_ord cur;   /* The end co-ordinates (the current ones if still copying). */
    co_ord old;   /* The previous end co-ordinates. */
    Time time;    /* The time at which the selection was finalised. */
};

static x11_selection_type s_ptr[1];

/*
 * Convert to EUC-JP
 */
static void convert_to_euc(char* buf) {
    size_t inlen = strlen(buf);
    size_t outlen = inlen + 1;
    char tmp[outlen];

    iconv_t iconvd = iconv_open("EUC-JP", "UTF-8");
    char* inbuf = buf;
    char* outbuf = tmp;
    iconv(iconvd, &inbuf, &inlen, &outbuf, &outlen);
    iconv_close(iconvd);

    int i, l = strlen(tmp);
    for (i = 0; i < l; i++)
        buf[i] = tmp[i];
    buf[l] = '\0';
}

/*
 * Push multiple keys reversal
 */
static void term_string_push(char* buf) {
    int i, l = strlen(buf);
    for (i = l; i >= 0; i--)
        term_key_push(buf[i]);
}

/*
 * Process a keypress event
 */
static void react_keypress(XKeyEvent* ev) {
    int n;
    KeySym ks;
    char buf[128];

    bool valid_keysym = true;

    if (Focuswin && Focuswin->xic) {
        Status status;
        n = XmbLookupString(Focuswin->xic, ev, buf, 125, &ks, &status);
        if (status == XBufferOverflow) {
            printf("Input is too long, and dropped\n");
            return;
        }
        if (status != XLookupKeySym && status != XLookupBoth) {
            valid_keysym = false;
        }
    }
    else {
        n = XLookupString(ev, buf, 125, &ks, NULL);
    }
    buf[n] = '\0';

    if (!valid_keysym) { /* XIMからの入力時のみ false になる */
        convert_to_euc(buf);
        term_string_push(buf);
        return;
    }

    if (is_modifier_key(ks))
        return;

    const auto ks1 = uint(ks);

    const auto mc = bool(ev->state & ControlMask);
    const auto ms = bool(ev->state & ShiftMask);
    const auto mo = bool(ev->state & Mod1Mask);
    const auto mx = bool(ev->state & Mod2Mask);

    if (n && !mo && !mx && !is_special_key(ks)) {
        term_string_push(buf);
        return;
    }

    switch (ks1) {
    case XK_Escape: {
        term_key_push(ESCAPE);
        return;
    }

    case XK_Return: {
        term_key_push('\r');
        return;
    }

    case XK_Tab: {
        term_key_push('\t');
        return;
    }

    case XK_Delete: {
        term_key_push(0x7f);
        return;
    }
    case XK_BackSpace: {
        term_key_push('\010');
        return;
    }
    }

    char msg[128];
    if (ks) {
        sprintf(msg, "%c%s%s%s%s_%lX%c", 31, mc ? "N" : "", ms ? "S" : "", mo ? "O" : "", mx ? "M" : "", (unsigned long)(ks), 13);
    }
    else {
        sprintf(msg, "%c%s%s%s%sK_%X%c", 31, mc ? "N" : "", ms ? "S" : "", mo ? "O" : "", mx ? "M" : "", ev->keycode, 13);
    }

    term_string_push(msg);

    if (n && (macro_find_exact(msg) < 0)) {
        macro_add(msg, buf);
    }
}

/*
 * Find the square a particular pixel is part of.
 */
static void pixel_to_square(int* const x, int* const y, const int ox, const int oy) {
    (*x) = (ox - Infowin->ox) / Infofnt->wid;
    (*y) = (oy - Infowin->oy) / Infofnt->hgt;
}

/*
 * Find the pixel at the top-left corner of a square.
 */
static void square_to_pixel(int* const x, int* const y, const int ox, const int oy) {
    (*x) = ox * Infofnt->wid + Infowin->ox;
    (*y) = oy * Infofnt->hgt + Infowin->oy;
}

/*
 * Convert co-ordinates from starting corner/opposite corner to minimum/maximum.
 */
static void sort_co_ord(co_ord* min, co_ord* max, const co_ord* b, const co_ord* a) {
    min->x = MIN(a->x, b->x);
    min->y = MIN(a->y, b->y);
    max->x = MAX(a->x, b->x);
    max->y = MAX(a->y, b->y);
}

/*
 * Remove the selection by redrawing it.
 */
static void mark_selection_clear(int x1, int y1, int x2, int y2) { term_redraw_section(x1, y1, x2, y2); }

/*
 * Select an area by drawing a grey box around it.
 * NB. These two functions can cause flicker as the selection is modified,
 * as the game redraws the entire marked section.
 */
static void mark_selection_mark(int x1, int y1, int x2, int y2) {
    square_to_pixel(&x1, &y1, x1, y1);
    square_to_pixel(&x2, &y2, x2, y2);
    XftDrawRect(Infowin->draw, &clr[2]->fg, x1, y1, x2 - x1 + Infofnt->wid - 1, y2 - y1 + Infofnt->hgt - 1);
}

/*
 * Mark a selection by drawing boxes around it (for now).
 */
static void mark_selection() {
    co_ord min, max;
    term_type* old = Term;
    bool draw = s_ptr->select;
    bool clear = s_ptr->drawn;
    if (s_ptr->t != old)
        term_activate(s_ptr->t);

    if (clear) {
        sort_co_ord(&min, &max, &s_ptr->init, &s_ptr->old);
        mark_selection_clear(min.x, min.y, max.x, max.y);
    }
    if (draw) {
        sort_co_ord(&min, &max, &s_ptr->init, &s_ptr->cur);
        mark_selection_mark(min.x, min.y, max.x, max.y);
    }

    if (s_ptr->t != old)
        term_activate(old);

    s_ptr->old.x = s_ptr->cur.x;
    s_ptr->old.y = s_ptr->cur.y;
    s_ptr->drawn = s_ptr->select;
}

/*
 * Forget a selection for one reason or another.
 */
static void copy_x11_release(void) {
    s_ptr->select = FALSE;
    mark_selection();
}

/*
 * Start to select some text on the screen.
 */
static void copy_x11_start(int x, int y) {
    if (s_ptr->select)
        copy_x11_release();

    s_ptr->t = Term;
    s_ptr->init.x = s_ptr->cur.x = s_ptr->old.x = x;
    s_ptr->init.y = s_ptr->cur.y = s_ptr->old.y = y;
}

/*
 * Respond to movement of the mouse when selecting text.
 */
static void copy_x11_cont(int x, int y, unsigned int buttons) {
    x = MIN(MAX(x, 0), Term->wid - 1);
    y = MIN(MAX(y, 0), Term->hgt - 1);
    if (~buttons & Button1Mask)
        return;
    if (s_ptr->t != Term)
        return;
    if (x == s_ptr->old.x && y == s_ptr->old.y && s_ptr->select)
        return;

    s_ptr->select = TRUE;
    s_ptr->cur.x = x;
    s_ptr->cur.y = y;
    mark_selection();
}

/*
 * Respond to release of the left mouse button by putting the selected text in
 * the primary buffer.
 */
static void copy_x11_end(const Time time) {
    if (!s_ptr->select)
        return;
    if (s_ptr->t != Term)
        return;

    s_ptr->time = time;
    XSetSelectionOwner(Metadpy->dpy, XA_PRIMARY, Infowin->win, time);
    if (XGetSelectionOwner(Metadpy->dpy, XA_PRIMARY) != Infowin->win) {
        s_ptr->select = FALSE;
        mark_selection();
    }
}

static Atom xa_targets, xa_timestamp, xa_text, xa_compound_text;

/*
 * Set the required variable atoms at start-up to avoid errors later.
 */
static void set_atoms() {
    xa_targets = XInternAtom(DPY, "TARGETS", False);
    xa_timestamp = XInternAtom(DPY, "TIMESTAMP", False);
    xa_text = XInternAtom(DPY, "TEXT", False);
    xa_compound_text = XInternAtom(DPY, "COMPOUND_TEXT", False);
}

static Atom request_target = 0;

/*
 * Send a message to request that the PRIMARY buffer be sent here.
 */
static void paste_x11_request(Atom target, const Time time) {
    Atom property = XInternAtom(DPY, "__COPY_TEXT", False);
    if (XGetSelectionOwner(DPY, XA_PRIMARY) == None) {
        /* No selection. */
        /* bell("No selection found."); */
        return;
    }

    request_target = target;
    XConvertSelection(DPY, XA_PRIMARY, target, property, WIN, time);
}

/*
 * Add the contents of the PRIMARY buffer to the input queue.
 *
 * Hack - This doesn't use the "time" of the event, and so accepts anything a
 * client tries to send it.
 */
static void paste_x11_accept(const XSelectionEvent* ptr) {
    unsigned long left;
    const long offset = 0;
    const long length = 32000;
    XTextProperty xtextproperty;
    errr err = 0;
    Atom property = XInternAtom(DPY, "__COPY_TEXT", False);
    if (ptr->property == None) {
        if (request_target == xa_compound_text) {
            paste_x11_request(XA_STRING, ptr->time);
        }
        else {
            request_target = 0;
            plog("Paste failure (remote client could not send).");
        }

        return;
    }

    if (ptr->selection != XA_PRIMARY) {
        plog("Paste failure (remote client did not send primary selection).");
        return;
    }

    if (ptr->target != request_target) {
        plog("Paste failure (selection in unknown format).");
        return;
    }

    if (XGetWindowProperty(Metadpy->dpy, Infowin->win, property, offset, length, TRUE, request_target, &xtextproperty.encoding, &xtextproperty.format,
            &xtextproperty.nitems, &left, &xtextproperty.value)
        != Success) {
        return;
    }

    if (request_target == xa_compound_text) {
        char** list;
        int count;

        XmbTextPropertyToTextList(DPY, &xtextproperty, &list, &count);

        if (list) {
            int i;

            for (i = 0; i < count; i++) {
                err = type_string(list[i], 0);
                if (err)
                    break;
            }

            XFreeStringList(list);
        }
    }
    else {
        err = type_string((char*)xtextproperty.value, xtextproperty.nitems);
    }

    XFree(xtextproperty.value);
    if (err) {
        plog("Paste failure (too much text selected).");
    }
}

/*
 * Add a character to a string in preparation for sending it to another
 * client as a STRING.
 * This doesn't change anything, as clients tend not to have difficulty in
 * receiving this format (although the standard specifies a restricted set).
 * Strings do not have a colour.
 */
static bool paste_x11_send_text(XSelectionRequestEvent* rq) {
    char buf[1024];
    char* list[1000];
    co_ord max, min;
    TERM_LEN x, y;
    int l, n;
    TERM_COLOR a;
    char c;

    if (rq->time < s_ptr->time)
        return FALSE;

    sort_co_ord(&min, &max, &s_ptr->init, &s_ptr->cur);
    if (XGetSelectionOwner(DPY, XA_PRIMARY) != WIN) {
        return FALSE;
    }

    XDeleteProperty(DPY, rq->requestor, rq->property);

    for (n = 0, y = 0; y < Term->hgt; y++) {
#ifdef JP
        int kanji = 0;
#endif
        if (y < min.y)
            continue;
        if (y > max.y)
            break;

        for (l = 0, x = 0; x < Term->wid; x++) {
#ifdef JP
            if (x > max.x)
                break;

            term_what(x, y, &a, &c);
            if (1 == kanji)
                kanji = 2;
            else if (iskanji(c))
                kanji = 1;
            else
                kanji = 0;

            if (x < min.x)
                continue;

            /*
             * A single kanji character was divided in two...
             * Delete the garbage.
             */
            if ((2 == kanji && x == min.x) || (1 == kanji && x == max.x))
                c = ' ';
#else
            if (x > max.x)
                break;
            if (x < min.x)
                continue;

            term_what(x, y, &a, &c);
#endif

            buf[l] = c;
            l++;
        }

        while (buf[l - 1] == ' ')
            l--;

        if (min.y != max.y) {
            buf[l] = '\n';
            l++;
        }

        buf[l] = '\0';
        list[n++] = (char*)string_make(buf);
    }

    list[n] = NULL;
    if (rq->target == XA_STRING) {
        for (n = 0; list[n]; n++) {
            XChangeProperty(DPY, rq->requestor, rq->property, rq->target, 8, PropModeAppend, (unsigned char*)list[n], strlen(list[n]));
        }
    }
    else if (rq->target == xa_text || rq->target == xa_compound_text) {
        XTextProperty text_prop;
        XICCEncodingStyle style;

        if (rq->target == xa_text)
            style = XStdICCTextStyle;
        else
            style = XCompoundTextStyle;

        if (Success == XmbTextListToTextProperty(DPY, list, n, style, &text_prop)) {
            XChangeProperty(DPY, rq->requestor, rq->property, text_prop.encoding, text_prop.format, PropModeAppend, text_prop.value, text_prop.nitems);
            XFree(text_prop.value);
        }
    }

    for (n = 0; list[n]; n++) {
        string_free(list[n]);
    }

    return TRUE;
}

/*
 * Send some text requested by another X client.
 */
static void paste_x11_send(XSelectionRequestEvent* rq) {
    XEvent event;
    XSelectionEvent* ptr = &(event.xselection);

    ptr->type = SelectionNotify;
    ptr->property = rq->property;
    ptr->display = rq->display;
    ptr->requestor = rq->requestor;
    ptr->selection = rq->selection;
    ptr->target = rq->target;
    ptr->time = rq->time;

    /*
     * Paste the appropriate information for each target type.
     * Note that this currently rejects MULTIPLE targets.
     */

    if (rq->target == XA_STRING || rq->target == xa_text || rq->target == xa_compound_text) {
        if (!paste_x11_send_text(rq))
            ptr->property = None;
    }
    else if (rq->target == xa_targets) {
        Atom target_list[4];
        target_list[0] = XA_STRING;
        target_list[1] = xa_text;
        target_list[2] = xa_compound_text;
        target_list[3] = xa_targets;
        XChangeProperty(DPY, rq->requestor, rq->property, rq->target, (8 * sizeof(target_list[0])), PropModeReplace, (unsigned char*)target_list,
            (sizeof(target_list) / sizeof(target_list[0])));
    }
    else if (rq->target == xa_timestamp) {
        XChangeProperty(DPY, rq->requestor, rq->property, rq->target, (8 * sizeof(Time)), PropModeReplace, (unsigned char*)s_ptr->time, 1);
    }
    else {
        ptr->property = None;
    }

    XSendEvent(DPY, rq->requestor, FALSE, NoEventMask, &event);
}

/*
 * Handle various events conditional on presses of a mouse button.
 */
static void handle_button(Time time, int x, int y, int button, bool press) {
    pixel_to_square(&x, &y, x, y);

    if (press && button == 1)
        copy_x11_start(x, y);
    if (!press && button == 1)
        copy_x11_end(time);
    if (!press && button == 2)
        paste_x11_request(xa_compound_text, time);
}

/*
 * Process events
 *
 * イベントがあったら 0 を、なかったら 1 を返す。
 */
static errr CheckEvent(bool wait) {
    auto* old_td = static_cast<term_data*>(Term->data);

    XEvent xev_body;
    XEvent* xev = &xev_body;

    term_data* td = nullptr;
    infowin* iwin = nullptr;

    do {
        if (!wait && XPending(Metadpy->dpy) == 0)
            return 1;

        if (s_ptr->select && !s_ptr->drawn)
            mark_selection();

        XNextEvent(Metadpy->dpy, xev);
    } while (bool(XFilterEvent(xev, xev->xany.window)));

    if (xev->type == MappingNotify) {
        XRefreshKeyboardMapping(&xev->xmapping);
        return 0;
    }

    // イベントが発生したウィンドウと、それに対応する端末を得る
    for (int i = 0; i < TERM_DATA_COUNT; i++) {
        if (!data[i].win) continue;
        if (xev->xany.window == data[i].win->win) {
            td = &data[i];
            iwin = td->win;
            break;
        }
    }

    // ウィンドウまたは端末がなければイベントを無視
    if (!td || !iwin) return 0;

    term_activate(&td->t);
    Infowin = iwin;

    switch (xev->type) {
    case ButtonPress:
    case ButtonRelease: {
        bool press = (xev->type == ButtonPress);
        int x = xev->xbutton.x;
        int y = xev->xbutton.y;
        int z;
        if (xev->xbutton.button == Button1)
            z = 1;
        else if (xev->xbutton.button == Button2)
            z = 2;
        else if (xev->xbutton.button == Button3)
            z = 3;
        else if (xev->xbutton.button == Button4)
            z = 4;
        else if (xev->xbutton.button == Button5)
            z = 5;
        else
            z = 0;

        handle_button(xev->xbutton.time, x, y, z, press);
        break;
    }
    case EnterNotify:
    case LeaveNotify: {
        break;
    }
    case MotionNotify: {
        int x = xev->xmotion.x;
        int y = xev->xmotion.y;
        unsigned int z = xev->xmotion.state;
        pixel_to_square(&x, &y, x, y);
        copy_x11_cont(x, y, z);
        break;
    }
    case SelectionNotify: {
        paste_x11_accept(&(xev->xselection));
        break;
    }
    case SelectionRequest: {
        paste_x11_send(&(xev->xselectionrequest));
        break;
    }
    case SelectionClear: {
        s_ptr->select = FALSE;
        mark_selection();
        break;
    }
    case KeyRelease: {
        break;
    }
    case KeyPress: {
        term_activate(&old_td->t);
        react_keypress(&(xev->xkey));
        break;
    }
    case Expose: {
        term_redraw();
        break;
    }
    case MapNotify: {
        Term->mapped_flag = true;
        break;
    }
    case UnmapNotify: {
        Term->mapped_flag = false;
        break;
    }
    case ConfigureNotify: {
        int ox = Infowin->ox;
        int oy = Infowin->oy;

        Infowin->w = xev->xconfigure.width;
        Infowin->h = xev->xconfigure.height;

        int cols = ((Infowin->w - (ox + ox)) / td->fnt->wid);
        int rows = ((Infowin->h - (oy + oy)) / td->fnt->hgt);
        chmax(cols, 1);
        chmax(rows, 1);

        if (td == &data[0]) {
            chmax(cols, 80);
            chmax(rows, 24);
        }

        int wid = cols * td->fnt->wid + (ox + ox);
        int hgt = rows * td->fnt->hgt + (oy + oy);
        term_resize(cols, rows);
        if ((Infowin->w != wid) || (Infowin->h != hgt))
            Infowin_resize(wid, hgt);

        break;
    }
    case FocusIn: {
        if (iwin->xic) {
            XSetICFocus(iwin->xic);
        }
        Focuswin = iwin;
        break;
    }
    case FocusOut: {
        if (iwin->xic) {
            XUnsetICFocus(iwin->xic);
        }
        break;
    }
    }

    term_activate(&old_td->t);
    Infowin = old_td->win;

    return 0;
}

/*
 * An array of sound file names
 */
static concptr sound_file[SOUND_MAX];

/*
 * Check for existance of a file
 */
static bool check_file(concptr s) {
    FILE* fff;

    fff = fopen(s, "r");
    if (!fff)
        return (FALSE);

    fclose(fff);
    return (TRUE);
}

/*
 * Initialize sound
 */
static void init_sound() {
    char wav[128];
    char buf[1024];
    char dir_xtra_sound[1024];
    path_build(dir_xtra_sound, sizeof(dir_xtra_sound), ANGBAND_DIR_XTRA, "sound");
    for (int i = 1; i < SOUND_MAX; i++) {
        sprintf(wav, "%s.wav", angband_sound_name[i]);
        path_build(buf, sizeof(buf), dir_xtra_sound, wav);
        if (check_file(buf))
            sound_file[i] = string_make(buf);
    }

    use_sound = TRUE;
}

/*
 * Hack -- make a sound
 */
static errr Term_xtra_x11_sound(int v) {
    char buf[1024];
    if (!use_sound)
        return (1);
    if ((v < 0) || (v >= SOUND_MAX))
        return (1);
    if (!sound_file[v])
        return (1);

    sprintf(buf, "./playwave.sh %s\n", sound_file[v]);
    return (system(buf) < 0);
}

/*
 * Handle "activation" of a term
 */
static errr Term_xtra_x11_level(int v) {
    term_data* td = (term_data*)(Term->data);
    if (v) {
        Infowin = td->win;
        Infofnt = td->fnt;
    }

    return (0);
}

/*
 * React to changes
 */
static errr Term_xtra_x11_react(void) {
    int i;

    for (i = 0; i < 256; i++) {
        if ((color_table[i][0] != angband_color_table[i][0]) || (color_table[i][1] != angband_color_table[i][1])
            || (color_table[i][2] != angband_color_table[i][2]) || (color_table[i][3] != angband_color_table[i][3])) {
            Pixell pixel;
            color_table[i][0] = angband_color_table[i][0];
            color_table[i][1] = angband_color_table[i][1];
            color_table[i][2] = angband_color_table[i][2];
            color_table[i][3] = angband_color_table[i][3];
            pixel = create_pixel(Metadpy->dpy, color_table[i][1], color_table[i][2], color_table[i][3]);
            Infoclr = clr[i];
            Infoclr_change_fg(pixel);
        }
    }

    return (0);
}

/*
 * Handle a "special request"
 */
static errr Term_xtra_x11(int n, int v) {
    switch (n) {
    case TERM_XTRA_NOISE:
        Metadpy_do_beep();
        return (0);
    case TERM_XTRA_SOUND:
        return (Term_xtra_x11_sound(v));
    case TERM_XTRA_FRESH:
        Metadpy_update(true, true, false);
        return (0);
    case TERM_XTRA_BORED:
        return (CheckEvent(false));
    case TERM_XTRA_EVENT:
        return (CheckEvent(bool(v)));
    case TERM_XTRA_FLUSH:
        while (CheckEvent(false) == 0)
            ;
        return (0);
    case TERM_XTRA_LEVEL:
        return (Term_xtra_x11_level(v));
    case TERM_XTRA_CLEAR:
        Infowin_wipe();
        s_ptr->drawn = FALSE;
        return (0);
    case TERM_XTRA_DELAY:
        usleep(1000 * v);
        return (0);
    case TERM_XTRA_REACT:
        return (Term_xtra_x11_react());
    }

    return (1);
}

/*
 * Draw the cursor as an inverted rectangle.
 *
 * Consider a rectangular outline like "main-mac.c".  XXX XXX
 */
static errr Term_curs_x11(int x, int y) {
    if (use_graphics) {
        XftDrawRect(Infowin->draw, &xor_->fg, x * Infofnt->wid + Infowin->ox, y * Infofnt->hgt + Infowin->oy, Infofnt->wid - 1, Infofnt->hgt - 1);
        XftDrawRect(Infowin->draw, &xor_->fg, x * Infofnt->wid + Infowin->ox + 1, y * Infofnt->hgt + Infowin->oy + 1, Infofnt->wid - 3, Infofnt->hgt - 3);
    }
    else {
        Infoclr = xor_;
        Infofnt_text_non(x, y, " ", 1);
    }

    return (0);
}

/*
 * Draw the double width cursor
 */
static errr Term_bigcurs_x11(int x, int y) {
    if (use_graphics) {
        XftDrawRect(Infowin->draw, &xor_->fg, x * Infofnt->wid + Infowin->ox, y * Infofnt->hgt + Infowin->oy, Infofnt->twid - 1, Infofnt->hgt - 1);
        XftDrawRect(Infowin->draw, &xor_->fg, x * Infofnt->wid + Infowin->ox + 1, y * Infofnt->hgt + Infowin->oy + 1, Infofnt->twid - 3, Infofnt->hgt - 3);
    }
    else {
        Infoclr = xor_;
        Infofnt_text_non(x, y, "  ", 2);
    }

    return (0);
}

/*
 * Erase some characters.
 */
static errr Term_wipe_x11(int x, int y, int n) {
    Infoclr = clr[TERM_DARK];
    Infofnt_text_non(x, y, "", n);
    s_ptr->drawn = FALSE;
    return (0);
}

/*
 * Draw some textual characters.
 */
static errr Term_text_x11(TERM_LEN x, TERM_LEN y, int n, TERM_COLOR a, concptr s) {
    Infoclr = clr[a];
    Infofnt_text_std(x, y, s, n);
    s_ptr->drawn = FALSE;
    return (0);
}

static void IMDestroyCallback(XIM, XPointer, XPointer);

static void IMInstantiateCallback(Display* display, XPointer unused1, XPointer unused2) {
    XIM xim;
    XIMCallback ximcallback;
    XIMStyles* xim_styles = NULL;
    int i;

    (void)unused1;
    (void)unused2;

    xim = XOpenIM(display, NULL, NULL, NULL);
    if (!xim) {
        printf("can't open IM\n");
        return;
    }

    ximcallback.callback = IMDestroyCallback;
    ximcallback.client_data = NULL;
    XSetIMValues(xim, XNDestroyCallback, &ximcallback, NULL);
    XGetIMValues(xim, XNQueryInputStyle, &xim_styles, NULL);
    for (i = 0; i < xim_styles->count_styles; i++) {
        if (xim_styles->supported_styles[i] == (XIMPreeditNothing | XIMStatusNothing))
            break;
    }
    if (i >= xim_styles->count_styles) {
        printf("Sorry, your IM does not support 'Root' preedit style...\n");
        XCloseIM(xim);
        return;
    }
    XFree(xim_styles);

    Metadpy->xim = xim;

    for (i = 0; i < TERM_DATA_COUNT; i++) {
        infowin* iwin = data[i].win;
        if (!iwin)
            continue;
        iwin->xic = XCreateIC(xim, XNInputStyle, (XIMPreeditNothing | XIMStatusNothing), XNClientWindow, iwin->win, XNFocusWindow, iwin->win, NULL);
        if (!iwin->xic) {
            printf("Can't create input context for Term%d\n", i);
            continue;
        }

        if (XGetICValues(iwin->xic, XNFilterEvents, &iwin->xic_mask, NULL) != NULL) {
            iwin->xic_mask = 0L;
        }

        XSelectInput(Metadpy->dpy, iwin->win, iwin->mask | iwin->xic_mask);
    }

    return;
}

static void IMDestroyCallback(XIM xim, XPointer client_data, XPointer call_data) {
    int i;
    (void)xim;
    (void)client_data;

    if (call_data == NULL) {
        XRegisterIMInstantiateCallback(Metadpy->dpy, NULL, NULL, NULL, IMInstantiateCallback, NULL);
    }

    for (i = 0; i < TERM_DATA_COUNT; i++) {
        infowin* iwin = data[i].win;
        if (!iwin)
            continue;
        if (iwin->xic_mask) {
            XSelectInput(Metadpy->dpy, iwin->win, iwin->mask);
            iwin->xic_mask = 0L;
        }
        iwin->xic = NULL;
    }

    Metadpy->xim = NULL;
}

static char force_lower(char a) { return ((isupper((a))) ? tolower((a)) : (a)); }

/*
 * Initialize a term_data
 */
static errr term_data_init(term_data* td, int i) {
    term_type* t = &td->t;

    concptr name = angband_term_name[i];

    int cols = 80;
    int rows = 24;

    int ox = 1;
    int oy = 1;

    int wid, hgt, num;

    char buf[80];

    concptr str;

    int val;

    XClassHint* ch;

    char res_name[20];
    char res_class[20];

    XSizeHints* sh;
    XWMHints* wh;

    const char* font = getenv(FORMAT("ANGBAND_X11_FONT_{}", i).c_str());
    if (!font)
        font = getenv("ANGBAND_X11_FONT");
    if (!font)
        font = DEFAULT_X11_FONTS[i];

    sprintf(buf, "ANGBAND_X11_AT_X_%d", i);
    str = getenv(buf);
    int x = (str != NULL) ? atoi(str) : -1;

    sprintf(buf, "ANGBAND_X11_AT_Y_%d", i);
    str = getenv(buf);
    int y = (str != NULL) ? atoi(str) : -1;

    sprintf(buf, "ANGBAND_X11_COLS_%d", i);
    str = getenv(buf);
    val = (str != NULL) ? atoi(str) : -1;
    if (val > 0)
        cols = val;

    sprintf(buf, "ANGBAND_X11_ROWS_%d", i);
    str = getenv(buf);
    val = (str != NULL) ? atoi(str) : -1;
    if (val > 0)
        rows = val;

    if (!i) {
        if (cols < 80)
            cols = 80;
        if (rows < 24)
            rows = 24;
    }

    sprintf(buf, "ANGBAND_X11_IBOX_%d", i);
    str = getenv(buf);
    val = (str != NULL) ? atoi(str) : -1;
    if (val > 0)
        ox = val;

    sprintf(buf, "ANGBAND_X11_IBOY_%d", i);
    str = getenv(buf);
    val = (str != NULL) ? atoi(str) : -1;
    if (val > 0)
        oy = val;

    MAKE(td->fnt, infofnt);
    Infofnt = td->fnt;
    Infofnt_init_data(font);

    num = ((i == 0) ? 1024 : 16);
    wid = cols * td->fnt->wid + (ox + ox);
    hgt = rows * td->fnt->hgt + (oy + oy);
    MAKE(td->win, infowin);
    Infowin = td->win;
    Infowin_init(x, y, wid, hgt, 0, Metadpy->fg, Metadpy->bg);

    Infowin_set_mask(ExposureMask | StructureNotifyMask | KeyPressMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | FocusChangeMask);

    Infowin_set_name(name);
    Infowin->ox = ox;
    Infowin->oy = oy;
    ch = XAllocClassHint();

    if (ch == NULL)
        quit("XAllocClassHint failed");

    strcpy(res_name, name);
    res_name[0] = force_lower(res_name[0]);
    ch->res_name = res_name;

    strcpy(res_class, "Angband");
    ch->res_class = res_class;

    XSetClassHint(Metadpy->dpy, Infowin->win, ch);
    sh = XAllocSizeHints();
    if (sh == NULL)
        quit("XAllocSizeHints failed");

    if (i == 0) {
        sh->flags = PMinSize | PMaxSize;
        sh->min_width = 80 * td->fnt->wid + (ox + ox);
        sh->min_height = 24 * td->fnt->hgt + (oy + oy);
        sh->max_width = 255 * td->fnt->wid + (ox + ox);
        sh->max_height = 255 * td->fnt->hgt + (oy + oy);
    }
    else {
        sh->flags = PMinSize | PMaxSize;
        sh->min_width = td->fnt->wid + (ox + ox);
        sh->min_height = td->fnt->hgt + (oy + oy);
        sh->max_width = 256 * td->fnt->wid + (ox + ox);
        sh->max_height = 256 * td->fnt->hgt + (oy + oy);
    }

    sh->flags |= PResizeInc;
    sh->width_inc = td->fnt->wid;
    sh->height_inc = td->fnt->hgt;
    sh->flags |= PBaseSize;
    sh->base_width = (ox + ox);
    sh->base_height = (oy + oy);
    XSetWMNormalHints(Metadpy->dpy, Infowin->win, sh);
    Infowin_map();

    wh = XAllocWMHints();
    if (wh == NULL)
        quit("XAllocWMHints failed");
    wh->flags = InputHint;
    wh->input = True;
    XSetWMHints(Metadpy->dpy, Infowin->win, wh);

    if ((x >= 0) && (y >= 0))
        Infowin_impell(x, y);

    term_init(t, cols, rows, num);
    t->attr_blank = TERM_WHITE;
    t->char_blank = ' ';
    t->xtra_hook = Term_xtra_x11;
    t->curs_hook = Term_curs_x11;
    t->bigcurs_hook = Term_bigcurs_x11;
    t->wipe_hook = Term_wipe_x11;
    t->text_hook = Term_text_x11;
    t->data = td;
    term_activate(t);

    return 0;
}

/*
 * Initialization function for an "X11" module to Angband
 */
errr init_x11(int argc, char* argv[]) {
    concptr dpy_name = "";
    int num_term = 3;

    for (int i = 1; i < argc; i++) {
        if (prefix(argv[i], "-d")) {
            dpy_name = &argv[i][2];
            continue;
        }

        if (prefix(argv[i], "-b")) {
            arg_bigtile = use_bigtile = TRUE;
            continue;
        }

        if (prefix(argv[i], "-n")) {
            num_term = atoi(&argv[i][2]);
            num_term = std::clamp(num_term, 1, TERM_DATA_COUNT);
            continue;
        }

        if (prefix(argv[i], "--")) {
            continue;
        }

        plog_fmt("Ignoring option: %s", argv[i]);
    }

#ifdef JP
    setlocale(LC_ALL, "");

#ifdef DEFAULT_LOCALE
    if (!strcmp(setlocale(LC_ALL, NULL), "C")) {
        printf("try default locale \"%s\"\n", DEFAULT_LOCALE);
        setlocale(LC_ALL, DEFAULT_LOCALE);
    }
#endif

    if (!strcmp(setlocale(LC_ALL, NULL), "C")) {
        printf("WARNING: Locale is not supported. Non-english font may be displayed incorrectly.\n");
    }

    if (!XSupportsLocale()) {
        printf("can't support locale in X\n");
        setlocale(LC_ALL, "C");
    }
#else
    setlocale(LC_ALL, "C");
#endif /* JP */

    if (Metadpy_init(dpy_name))
        return (-1);

    MAKE(xor_, infoclr);
    Infoclr = xor_;
    Infoclr_init(Metadpy->fg, Metadpy->bg);
    for (int i = 0; i < 256; ++i) {
        Pixell pixel;
        MAKE(clr[i], infoclr);
        Infoclr = clr[i];
        color_table[i][0] = angband_color_table[i][0];
        color_table[i][1] = angband_color_table[i][1];
        color_table[i][2] = angband_color_table[i][2];
        color_table[i][3] = angband_color_table[i][3];
        pixel = create_pixel(Metadpy->dpy, color_table[i][1], color_table[i][2], color_table[i][3]);

        Infoclr_init(pixel, Metadpy->bg);
    }

    set_atoms();
    for (int i = 0; i < num_term; i++) {
        term_data* td = &data[i];
        term_data_init(td, i);
        angband_term[i] = Term;
    }

    Infowin = data[0].win;
    Infowin_raise();
    term_activate(&data[0].t);

    {
        char* p;
        p = XSetLocaleModifiers("");
        if (!p || !*p) {
            p = XSetLocaleModifiers("@im=");
        }
    }
    XRegisterIMInstantiateCallback(Metadpy->dpy, NULL, NULL, NULL, IMInstantiateCallback, NULL);

    if (arg_sound)
        init_sound();

    return (0);
}
