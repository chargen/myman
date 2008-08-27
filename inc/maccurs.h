/* maccurs.h - Carbon/Toolbox curses emulator
 * Copyright 1997-2008, Benjamin C. Wiley Sittler <bsittler@gmail.com>
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy,
 *  modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

/*

This code is mostly written in the interoperable common subset of
Carbon and Toolbox. Because it uses Quickdraw, compiling it against
recent Mac OS X-oriented SDKs produces lots of (harmless) deprecation
warnings.

 */

#ifndef MACCURS_H_INC
#define MACCURS_H_INC 1

#ifdef macintosh

#if TARGET_API_MAC_CARBON

/* Case 1: flat Carbon headers */
#include <Carbon.h>

#else /* ! defined(TARGET_API_MAC_CARBON) */

/* Case 2: Toolbox */
#include <Errors.h>
#include <Files.h>
#include <Fonts.h>
#include <MacTypes.h>
#include <MacWindows.h>
#include <Quickdraw.h>
#include <QuickdrawText.h>
#ifdef UNICODE
#include <ATSUnicode.h>
#endif
#include <Script.h>
#include <Sound.h>
#include <TextEncodingConverter.h>
#include <TextUtils.h>
#include <Timer.h>
#include <ioctl.h>

#endif /* ! defined(TARGET_API_MAC_CARBON) */

#else /* ! defined(macintosh) */

/* Case 3: Carbon framework */
#include <Carbon/Carbon.h>

#endif /* ! defined(macintosh) */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef macintosh

#ifndef strcasecmp

#define strcasecmp(s1,s2) fake_strcasecmp(s1,s2)

int fake_strcasecmp(const char *s1, const char *s2)
{
    int i;
    int a, b;

    for (i = 0; s1[i] && s2[i] && (tolower(s1[i]) == tolower(s2[i])); i ++)
    {
        /* ... */
    }
    a = (unsigned char) s1[i];
    b = (unsigned char) s2[i];
    if (a) a = tolower(a);
    if (b) b = tolower(b);
    return (a == b) ? 0 : (a < b) ? -1 : 1;
}

#endif

#ifndef isatty

#define isatty(fd) fake_isatty(fd)

int fake_isatty(int fd)
{
    char namebuf[512];

    memset((void *) namebuf, 0, sizeof(namebuf));
#ifdef FIOFNAME
    if (ioctl(fd, FIOFNAME, (long *) (void *) namebuf) == -1)
    {
        return 0;
    }
#endif
    if ((! strcmp(namebuf, "dev:console"))
        ||
        (! strncmp(namebuf, "dev:tty", strlen("dev:tty")))
        ||
        (! strncmp(namebuf, "dev:pty", strlen("dev:pty"))))
    {
        return 1;
    }
    return 0;
}

#endif

#ifndef access

#define access(fn,type) fake_access(fn)

static int fake_access(const char *fn)
{
    FILE *fp;

    fp = fopen(fn, "rb");
    if (fp)
    {
        fclose(fp);
        return 0;
    }
    return -1;
}

#endif

#ifndef gettimeofday

#define gettimeofday(tp,tzp) fake_gettimeofday(tp,tzp)

struct timeval
{
    long tv_sec;
    long tv_usec;
};

struct timezone
{
    int tz_minuteswest;
    int tz_dsttime;
};

static int fake_gettimeofday(struct timeval *tp, struct timezone *tzp)
{
    UnsignedWide microTickCount;
    double usecs;

    Microseconds(&microTickCount);
    usecs = (4294967296.0 * microTickCount.hi + microTickCount.lo);
    if (tp)
    {
        tp->tv_sec = (long) (usecs / 1e6);
        tp->tv_usec = (long) (usecs - (1e6 * tp->tv_sec));
    }
    if (tzp)
    {
        tzp->tz_minuteswest = 0;
        tzp->tz_dsttime = 0;
    }
    return 0;
}

#endif

#ifndef usleep

#define usleep(us) fake_usleep(us)

static int fake_usleep(unsigned long us)
{
    EventRecord er;
    struct timeval tv, tv2;

    tv.tv_sec = 0;
    tv.tv_usec = 0;
    gettimeofday(&tv, NULL);
    tv2.tv_sec = tv.tv_sec + ((tv.tv_usec + us) / 1000000L);
    tv2.tv_usec = tv.tv_usec + ((tv.tv_usec + us) % 1000000L);
    while (1)
    {
        SpinCursor(32);
        if (WaitNextEvent(highLevelEventMask, &er, us / 60000, NULL))
        {
            if (er.what == kHighLevelEvent)
            {
                AEProcessAppleEvent(&er);
            }
            return -1;
        }
        gettimeofday(&tv, NULL);
        if ((tv.tv_sec != tv2.tv_sec)
            ||
            (tv.tv_usec >= tv2.tv_usec))
        {
            break;
        }
        break;
    }
    return 0;
}

#endif

#endif /* macintosh */

#define ERR -1
#define OK 0

typedef long maccurses_chtype;
typedef maccurses_chtype maccurses_attr_t;

#undef KEY_RESIZE
#define KEY_RESIZE 0x100001L

#undef KEY_UP
#define KEY_UP 0x100002L

#undef KEY_DOWN
#define KEY_DOWN 0x100003L

#undef KEY_LEFT
#define KEY_LEFT 0x100004L

#undef KEY_RIGHT
#define KEY_RIGHT 0x100005L

#undef A_REVERSE
#define A_REVERSE 0x00800000L

#undef A_UNDERLINE
#define A_UNDERLINE 0x00400000L

#undef A_BOLD
#define A_BOLD 0x00200000L

#undef A_STANDOUT
#define A_STANDOUT A_REVERSE

#undef _MACCURSES_A_COLOR
#define _MACCURSES_A_COLOR 0xff000000L

/* note that this is not a bitflag -- instead, it's a prefix for the
 * non-Unicode "plane 31" which is a copy of the BMP (plane 0) in "ACS
 * mode". */
#undef _MACCURSES_A_ALTCHARSET
#define _MACCURSES_A_ALTCHARSET 0x1f0000L

#undef _MACCURSES_A_CHARTEXT
#define _MACCURSES_A_CHARTEXT 0x1fffffL

#undef _MACCURSES_A_ATTR
#define _MACCURSES_A_ATTR (~_MACCURSES_A_CHARTEXT)

#define _PAIR_SHIFT 24

#define COLORS 16

#define COLOR_PAIRS (COLORS*COLORS)

#define COLOR_BLACK 0

#define COLOR_RED 1

#define COLOR_GREEN 2

#define COLOR_YELLOW 3

#define COLOR_BLUE 4

#define COLOR_MAGENTA 5

#define COLOR_CYAN 6

#define COLOR_WHITE 7

#define PEN_BRIGHT 8

#define _COLOR_TO_MAC(c, dfl) \
                          (((c) == COLOR_BLACK) ? blackColor : \
                          ((c) == COLOR_RED) ? redColor : \
                          ((c) == COLOR_GREEN) ? greenColor : \
                          ((c) == COLOR_YELLOW) ? yellowColor : \
                          ((c) == COLOR_BLUE) ? blueColor : \
                          ((c) == COLOR_MAGENTA) ? magentaColor : \
                          ((c) == COLOR_CYAN) ? cyanColor : \
                          ((c) == COLOR_WHITE) ? whiteColor : \
                          (dfl))

#ifdef UNICODE

#define ACS_LRCORNER (0x2518 | _MACCURSES_A_ALTCHARSET)

#define ACS_URCORNER (0x2510 | _MACCURSES_A_ALTCHARSET)

#define ACS_ULCORNER (0x250c | _MACCURSES_A_ALTCHARSET)

#define ACS_LLCORNER (0x2514 | _MACCURSES_A_ALTCHARSET)

#define ACS_PLUS (0x253c | _MACCURSES_A_ALTCHARSET)

#define ACS_HLINE (0x2500 | _MACCURSES_A_ALTCHARSET)

#define ACS_LTEE (0x251c | _MACCURSES_A_ALTCHARSET)

#define ACS_RTEE (0x2524 | _MACCURSES_A_ALTCHARSET)

#define ACS_BTEE (0x2534 | _MACCURSES_A_ALTCHARSET)

#define ACS_TTEE (0x252c | _MACCURSES_A_ALTCHARSET)

#define ACS_VLINE (0x2502 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_MIDDOT (0x00b7 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_PUCE (0x2022 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_BLACKSQUARE (0x25A0 | _MACCURSES_A_ALTCHARSET)

#define ACS_BLOCK (0x2588 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_THALF (0x2580 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_BHALF (0x2584 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_LHALF (0x258c | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_RHALF (0x2590 | _MACCURSES_A_ALTCHARSET)

#else

#define ACS_LRCORNER (0xd9 | _MACCURSES_A_ALTCHARSET)

#define ACS_URCORNER (0xbf | _MACCURSES_A_ALTCHARSET)

#define ACS_ULCORNER (0xda | _MACCURSES_A_ALTCHARSET)

#define ACS_LLCORNER (0xc0 | _MACCURSES_A_ALTCHARSET)

#define ACS_PLUS (0xc5 | _MACCURSES_A_ALTCHARSET)

#define ACS_HLINE (0xc4 | _MACCURSES_A_ALTCHARSET)

#define ACS_LTEE (0xc3 | _MACCURSES_A_ALTCHARSET)

#define ACS_RTEE (0xb4 | _MACCURSES_A_ALTCHARSET)

#define ACS_BTEE (0xc1 | _MACCURSES_A_ALTCHARSET)

#define ACS_TTEE (0xc2 | _MACCURSES_A_ALTCHARSET)

#define ACS_VLINE (0xb3 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_MIDDOT (0xf9 | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_PUCE (0xfa | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_BLACKSQUARE (0xfe | _MACCURSES_A_ALTCHARSET)

#define ACS_BLOCK (0xdb | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_THALF (0xdf | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_BHALF (0xdc | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_LHALF (0xdd | _MACCURSES_A_ALTCHARSET)

#define _MACCURSES_ACS_RHALF (0xde | _MACCURSES_A_ALTCHARSET)

#endif

#define ACS_BULLET _MACCURSES_ACS_MIDDOT

#ifndef SWAPDOTS
#define SWAPDOTS 1
#endif

/* an arbitrary integer identifying "Da Window" */
#define MACCURSES_MAINWIN 0xda317d03LU

/* somewhat arbitrary menu identifiers */
#define MACCURSES_FILEMENU 0x0a01L
#define MACCURSES_EDITMENU 0x0a02L
#define MACCURSES_FONTMENU 0x0f00L /* this and all higher codes are used by the font (sub-)menus */

static int maccurses_x = 0;
static int maccurses_y = 0;

static maccurses_attr_t maccurses_attr = 0;

static int maccurses_w = 80;
static int maccurses_h = 25;

static int maccurses_new_w = 0;
static int maccurses_new_h = 0;

static int maccurses_resize_pending = 0;

#ifndef _MACCURSES_RESIZE_TIMER
#define _MACCURSES_RESIZE_TIMER 10
#endif

static int maccurses_clearok_flag = FALSE;

static int maccurses_nodelay_flag = FALSE;

static int maccurses_intrflush_flag = FALSE;

static int maccurses_idlok_flag = FALSE;

static int maccurses_leaveok_flag = FALSE;

#ifndef UNICODE

static unsigned short *maccurses_cvtt = 0;

static unsigned short maccurses_macvt100[] = {
    0x80, 0x82, 0x81, 0x9f, 0x82, 0x8e, 0x83, 0x89,
    0x84, 0x8a, 0x85, 0x88, 0x86, 0x8c, 0x87, 0x8d,
    0x88, 0x90, 0x89, 0x91, 0x8a, 0x8f, 0x8b, 0x95,
    0x8c, 0x94, 0x8d, 0x93, 0x8e, 0x80, 0x8f, 0x81,
    0x90, 0x83, 0x91, 0xbe, 0x92, 0xae, 0x93, 0x99,
    0x94, 0x9a, 0x95, 0x98, 0x96, 0x9e, 0x97, 0x9d,
    0x98, 0xd8, 0x99, 0x85, 0x9a, 0x86, 0x9b, 0xa2,
    0x9c, 0xa3, 0x9d, 0xb4, 0x9f, 0xc4, 0xa0, 0x87,
    0xa1, 0x92, 0xa2, 0x97, 0xa3, 0x9c, 0xa4, 0x96,
    0xa5, 0x84, 0xa6, 0xbb, 0xa7, 0xbc, 0xa8, 0xc0,
    0xaa, 0xc2, 0xab, 0xc3, 0xac, 0xc5, 0xad, 0xc1,
    0xae, 0xc7, 0xaf, 0xc8, 0xb1, 0xbd, 0xb3, 0xfb,
    0xb4, 0xf8, 0xbf, 0xd3, 0xc0, 0xd5, 0xc1, 0xf9,
    0xc2, 0xfa, 0xc3, 0xf7, 0xc4, 0xe4, 0xc5, 0xda,
    0xd9, 0xd2, 0xda, 0xd4, 0xe1, 0xa7, 0xe3, 0xb9,
    0xe6, 0xb5, 0xf1, 0xb1, 0xf2, 0xb3, 0xf3, 0xb2,
    0xf6, 0xd6, 0xf8, 0xa1, 0xfa, 0xe1, 0xfd, 0xb7,
    0, 0
};
static unsigned short maccurses_macjapanese[] = {
    0x5c,   0x80, 0x9b, 0x8191, 0x9c, 0x8192, 0xaa, 0x81ca,
    0xb3, 0x84a0, 0xb4, 0x84a7, 0xbf, 0x84a2, 0xc0, 0x84a4,
    0xc1, 0x84a8, 0xc2, 0x84a6, 0xc3, 0x84a5, 0xc4, 0x849f,
    0xc5, 0x84a9, 0xd9, 0x84a3, 0xda, 0x84a1, 0xe0, 0x83bf,
    0xe2, 0x83a1, 0xe3, 0x83ce, 0xe4, 0x83b0, 0xe5, 0x83d0,
    0xe6, 0x83ca, 0xe7, 0x83d1, 0xe8, 0x83b3, 0xe9, 0x83a6,
    0xea, 0x83b6, 0xeb, 0x83c2, 0xec, 0x8187, 0xed, 0x83d3,
    0xee, 0x83c3, 0xef, 0x81bf, 0xf0, 0x81df, 0xf2, 0x8186,
    0xf3, 0x8185, 0xf6, 0x8180, 0xf8, 0x818b, 0xfb, 0x81e3,
    0xfe, 0x81a1, 0xff,   0xa0,
    0, 0
};
static unsigned short maccurses_macroman[] = {
    0x80, 0x82, 0x81, 0x9f, 0x82, 0x8e, 0x83, 0x89,
    0x84, 0x8a, 0x85, 0x88, 0x86, 0x8c, 0x87, 0x8d,
    0x88, 0x90, 0x89, 0x91, 0x8a, 0x8f, 0x8b, 0x95,
    0x8c, 0x94, 0x8d, 0x93, 0x8e, 0x80, 0x8f, 0x81,
    0x90, 0x83, 0x91, 0xbe, 0x92, 0xae, 0x93, 0x99,
    0x94, 0x9a, 0x95, 0x98, 0x96, 0x9e, 0x97, 0x9d,
    0x98, 0xd8, 0x99, 0x85, 0x9a, 0x86, 0x9b, 0xa2,
    0x9c, 0xa3, 0x9d, 0xb4, 0x9f, 0xc4, 0xa0, 0x87,
    0xa1, 0x92, 0xa2, 0x97, 0xa3, 0x9c, 0xa4, 0x96,
    0xa5, 0x84, 0xa6, 0xbb, 0xa7, 0xbc, 0xa8, 0xc0,
    0xaa, 0xc2, 0xad, 0xc1, 0xae, 0xc7, 0xaf, 0xc8,
    0xe1, 0xa7, 0xe3, 0xb9, 0xe6, 0xb5, 0xea, 0xbd,
    0xec, 0xb0, 0xf1, 0xb1, 0xf2, 0xb3, 0xf3, 0xb2,
    0xf6, 0xd6, 0xf7, 0xc5, 0xf8, 0xa1,
    0, 0
};
static unsigned short maccurses_cp437[] = {
    0, 0
};

#endif /* defined(UNICODE) */

static int maccurses_valid = FALSE;
static int maccurses_gotQuitEvent = FALSE;

static WindowRef maccurses_hwnd;
static Point maccurses_size;
static int maccurses_ideal_w = 0;
static int maccurses_ideal_h = 0;
static Rect maccurses_ideal_rect;
static Boolean maccurses_maximized = false;
static CGrafPtr maccurses_port;

static Str255 maccurses_title = { 0 };

static int maccurses_zoom = 0;

static short maccurses_widMax = 0;
static FontInfo maccurses_fontinfo;
static int maccurses_fontsize = 12;
static FMFontFamily maccurses_fontfamily = kInvalidFontFamily;
static FMFontFamily maccurses_custom_fontfamily = kInvalidFontFamily;
static FMFontStyle maccurses_custom_fontstyle = 0;

static short maccurses_pairs[COLOR_PAIRS][2];

static RGBColor maccurses_palette[COLORS + COLORS * COLORS + 1];

#define COLOR_PAIR(p) ((p) << _PAIR_SHIFT)

#define PAIR_NUMBER(a) (((a) & _MACCURSES_A_COLOR) >> _PAIR_SHIFT)

static void maccurses_cleanup(void)
{
    if (maccurses_valid != FALSE)
    {
        maccurses_valid = FALSE;
        DisposeWindow(maccurses_hwnd);
    }
}

static int maccurses_wcwidth(wchar_t wc)
{
    switch ((wc & _MACCURSES_A_ALTCHARSET)
            ?
            wc
            :
            (wc | _MACCURSES_A_ALTCHARSET))
    {
        case ACS_BLOCK:
        case _MACCURSES_ACS_THALF:
        case _MACCURSES_ACS_BHALF:
        case _MACCURSES_ACS_LHALF:
        case _MACCURSES_ACS_RHALF:
            return 1;
    }
    if ((wc >= 0x20) && (wc < 0x7f))
    {
        return 1;
    }
    if ((wc & _MACCURSES_A_ALTCHARSET) == _MACCURSES_A_ALTCHARSET)
    {
        wc ^= _MACCURSES_A_ALTCHARSET;
    }
    else
    {
#ifndef UNICODE
        if (maccurses_cvtt)
        {
            int j;

            if (! maccurses_cvtt[0])
            {
                return 1;
            }
            for (j = 0; maccurses_cvtt[j]; j += 2)
            {
                if (maccurses_cvtt[j] == wc)
                {
                    if (maccurses_cvtt[j + 1] > 0xff)
                    {
                        return 2;
                    }
                    else
                    {
                        return 1;
                    }
                }
            }
        }
#endif
#ifdef UNICODE
        if (((wc & _MACCURSES_A_ALTCHARSET) != _MACCURSES_A_ALTCHARSET)
            &&
            maccurses_valid)
        {
            ATSUStyle sty;
            Boolean atsuok = false;
            Rect dimen;
            UniChar uc[3];
            UniCharCount unilen;
            maccurses_chtype chuni;
            ATSUTextLayout lyo;
            ATSUAttributeTag fontSizeAttrTag, fontAttrTag;
            Fixed fontSizeAttrValue;
            ATSUFontID fontAttrValue;
            FMFontStyle whatwegot;
            ByteCount fontSizeAttrValueSize, fontAttrValueSize;
            ATSUAttributeValuePtr fontSizeAttrValuePtr, fontAttrValuePtr;

            chuni = (((wc & _MACCURSES_A_ALTCHARSET) == _MACCURSES_A_ALTCHARSET)
                     ?
                     (wc ^ _MACCURSES_A_ALTCHARSET)
                     :
                     wc);
            if ((chuni < 0) || (chuni > 0x10fffdL) || ((chuni & 0xfffeL) == 0xfffeL))
            {
                chuni = 0xfffd;
            }
            unilen = 1 + (chuni > 0xffffL);
            uc[0] = (chuni > 0xffffL) ? (((chuni - 0x10000L) >> 10) | 0xd800L) : chuni;
            uc[1] = (chuni > 0xffffL) ? (((chuni - 0x10000L) & 0x3ffL) | 0xd800L) : 0;
            uc[2] = 0;
            if (noErr == ATSUCreateStyle(&sty))
            {
                fontSizeAttrTag = kATSUSizeTag;
                fontSizeAttrValueSize = sizeof(fontSizeAttrValue);
                fontSizeAttrValuePtr = &fontSizeAttrValue;
                fontSizeAttrValue = maccurses_fontsize << 16;
                fontAttrTag = kATSUFontTag;
                fontAttrValueSize = sizeof(fontAttrValue);
                fontAttrValuePtr = &fontAttrValue;
                if ((noErr == FMGetFontFromFontFamilyInstance(maccurses_fontfamily,
                                                              maccurses_custom_fontstyle
                                                              |
                                                              ((maccurses_attr & A_UNDERLINE) ? underline : 0)
                                                              |
                                                              ((maccurses_attr & A_BOLD) ? bold : 0),
                                                              &fontAttrValue,
                                                              &whatwegot))
                    &&
                    (noErr == ATSUSetAttributes(sty, 1, &fontAttrTag, &fontAttrValueSize, &fontAttrValuePtr))
                    &&
                    (noErr == ATSUSetAttributes(sty, 1, &fontSizeAttrTag, &fontSizeAttrValueSize, &fontSizeAttrValuePtr))
                    &&
                    (noErr == ATSUCreateTextLayoutWithTextPtr(uc,
                                                              kATSUFromTextBeginning,
                                                              kATSUToTextEnd,
                                                              unilen,
                                                              1,
                                                              &unilen,
                                                              &sty,
                                                              &lyo)))
                {
                    ATSUSetTransientFontMatching(lyo, true);
                    if (noErr == ATSUMeasureTextImage(lyo,
                                                      kATSUFromTextBeginning,
                                                      kATSUToTextEnd,
                                                      0,
                                                      0,
                                                      &dimen))
                    {
                        /* woot. */
                        atsuok = true;
                    }
                    ATSUDisposeTextLayout(lyo);
                }
                ATSUDisposeStyle(sty);
            }
            if (atsuok != false)
            {
                if ((dimen.right - dimen.left) > (maccurses_widMax + (maccurses_widMax / 2)))
                {
                    return 2;
                }
                /* FIXME: this is a horrible hack! */
                if ((chuni >= 0x0020) && (chuni < 0x3000)) return 1;
                return 2;
            }
        }
#endif /* defined(UNICODE) */
    }
    if (! (wc & _MACCURSES_A_ALTCHARSET))
    {
        switch (wc | _MACCURSES_A_ALTCHARSET)
        {
        case ACS_LRCORNER:
        case ACS_URCORNER:
        case ACS_ULCORNER:
        case ACS_LLCORNER:
        case ACS_PLUS:
        case ACS_LTEE:
        case ACS_RTEE:
        case ACS_BTEE:
        case ACS_TTEE:
        case ACS_HLINE:
        case ACS_VLINE:
        case _MACCURSES_ACS_MIDDOT:
        case _MACCURSES_ACS_PUCE:
        case _MACCURSES_ACS_BLACKSQUARE:
        case ACS_BLOCK:
        case _MACCURSES_ACS_THALF:
        case _MACCURSES_ACS_BHALF:
        case _MACCURSES_ACS_LHALF:
        case _MACCURSES_ACS_RHALF:
            return 1;
        }
    }
    return -1;
}

static int maccurses_endwin(void);

static int maccurses_init_pair(short pair, short f, short b);

static void maccurses_initscrWithHints(int h, int w, const char *title, const char *shortname);

static int maccurses_has_colors(void)
{
    if (! maccurses_valid)
    {
        maccurses_initscrWithHints(maccurses_h, maccurses_w, 0, 0);
    }
    return IsPortColor(maccurses_port) ? TRUE : FALSE;
}

static int maccurses_can_change_color(void)
{
    if (! maccurses_has_colors())
    {
        return FALSE;
    }
    return TRUE;
}

static int maccurses_erase(void);

/* should be in <math.h> but sometimes it's missing... */

#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880168872420969808
#endif

static unsigned short maccurses_mix(unsigned short a, unsigned short b)
{
    unsigned long r;

    r = 0xffffL * sqrt(1.0 * a / 0xffffL * a / 0xffffL + 1.0 * b / 0xffffL * b / 0xffffL) / M_SQRT2;
    if (r > 0xffffL)
    {
        r = 0xffffL;
    }
    return (unsigned short) r;
}

static void maccurses_updateFontCheckMarks(MenuRef mr)
{
    int i;
    UInt16 count;

    count = CountMenuItems(mr);
    for (i = 0;  i <= count; i ++)
    {
        FMFontFamily fam;
        FMFontStyle sty;
        CharParameter ch;

        GetItemMark(mr, i, &ch);
        if ((ch == noMark) || (ch == checkMark))
        {
            GetFontFamilyFromMenuSelection(mr, i, &fam, &sty);
            MacCheckMenuItem(mr, i,
                             ((fam == maccurses_fontfamily) && (sty == maccurses_custom_fontstyle)) ? true : false);
        }
        else
        {
            MenuRef submr;

            if (noErr == GetMenuItemHierarchicalMenu(mr, i, &submr))
            {
                maccurses_updateFontCheckMarks(submr);
            }
            else
            {
                maccurses_updateFontCheckMarks(GetMenuRef(ch));
            }
        }
    }
}

static void maccurses_updateFontMenu(void)
{
#ifdef __CARBON__
    MenuRef fontmenu;

    fontmenu = GetMenuRef(MACCURSES_FONTMENU);
    if (fontmenu)
    {
        UpdateStandardFontMenu(fontmenu, NULL);
        maccurses_updateFontCheckMarks(fontmenu);
    }
#endif /* defined(__CARBON__) */
}

static pascal OSErr maccurses_onQuitEvent(const AppleEvent *ev, AppleEvent *reply, long handlerRefcon)
{
    maccurses_gotQuitEvent = TRUE;
    return noErr;
}

static MenuBarHandle maccurses_oldMainMenu;

static void maccurses_initscrWithHints(int h, int w, const char *title, const char *shortname)
{
    char *font;
    const char *encoding_name;
    Str255 font_pascal;
    Rect coords;
    TextEncoding encoding;
    Boolean encoding_determined = false;
    ScriptCode scriptID;
    int i;
    int j;
    int firsttime;

    if (maccurses_valid == FALSE)
    {
        maccurses_valid = TRUE;
        firsttime = 1;
    }
    else
    {
        firsttime = 0;
    }
    if (maccurses_new_w && maccurses_new_h)
    {
        w = maccurses_new_w;
        h = maccurses_new_h;
        maccurses_new_w = 0;
        maccurses_new_h = 0;
    }
    if (! w)
    {
        w = maccurses_w;
    }
    if (! h)
    {
        h = maccurses_h;
    }
    if (! w)
    {
        w = 80;
    }
    if (! h)
    {
        h = 25;
    }
    if (w < 8)
    {
        w = 8;
    }
    if (h < 4)
    {
        h = 4;
    }
    maccurses_x = 0;
    maccurses_y = 0;
    maccurses_attr = 0;
    if (title
        ||
        (! maccurses_title[0]))
    {
        if ((! title)
            ||
            (! *title))
        {
            title = "Untitled";
        }
        maccurses_title[0] = 0;
        for (i = 0; (i < 256) && title && title[i]; i ++)
        {
            maccurses_title[0] = i + 1;
            maccurses_title[i + 1] = title[i];
        }
    }
    if (firsttime)
    {
        ProcessSerialNumber psn;

        atexit(maccurses_cleanup);
        if (noErr == MacGetCurrentProcess(&psn))
        {
#ifndef macintosh
            TransformProcessType(&psn, kProcessTransformToForegroundApplication);
#endif /* ! defined(macintosh) */
            SetFrontProcess(&psn);
        }
#ifndef __CARBON__
        InitGraf((void *) &maccurses_port);
        InitFonts();
        InitWindows();
        InitMenus();
        TEInit();
#endif /* ! defined(__CARBON__) */

        InitCursor();
    }

    maccurses_oldMainMenu = GetMenuBar();

    {
        int i;
        const char *filemenu_title = "File";
        const char *editmenu_title = "Edit";
        const char *fontmenu_title = "Font";
        Str255 filemenu_title_pascal = { 0 };
        Str255 editmenu_title_pascal = { 0 };
        Str255 fontmenu_title_pascal = { 0 };
        MenuRef filemenu = NULL;
        MenuRef editmenu = NULL;
        MenuRef fontmenu = NULL;

        filemenu_title_pascal[0] = 0;
        for (i = 0; (i < 255) && filemenu_title[i]; i ++)
        {
            filemenu_title_pascal[i + 1] = filemenu_title[i];
            filemenu_title_pascal[0] = i + 1;
        }
        filemenu = NewMenu(MACCURSES_FILEMENU, filemenu_title_pascal);
        if (filemenu)
        {
            const char *filemenu_items = "Help/?;-;Quit/Q";
            Str255 filemenu_items_pascal = { 0 };

            filemenu_items_pascal[0] = 0;
            for (i = 0; (i < 255) && filemenu_items[i]; i ++)
            {
                filemenu_items_pascal[i + 1] = filemenu_items[i];
                filemenu_items_pascal[0] = i + 1;
            }
            AppendMenu(filemenu, filemenu_items_pascal);
            InsertMenu(filemenu, 0);
        }
        editmenu_title_pascal[0] = 0;
        for (i = 0; (i < 255) && editmenu_title[i]; i ++)
        {
            editmenu_title_pascal[i + 1] = editmenu_title[i];
            editmenu_title_pascal[0] = i + 1;
        }
        editmenu = NewMenu(MACCURSES_EDITMENU, editmenu_title_pascal);
        if (editmenu)
        {
            const char *editmenu_items = "Zoom/Z;-;Redraw/R";
            Str255 editmenu_items_pascal = { 0 };

            editmenu_items_pascal[0] = 0;
            for (i = 0; (i < 255) && editmenu_items[i]; i ++)
            {
                editmenu_items_pascal[i + 1] = editmenu_items[i];
                editmenu_items_pascal[0] = i + 1;
            }
            AppendMenu(editmenu, editmenu_items_pascal);
            InsertMenu(editmenu, 0);
        }
        fontmenu_title_pascal[0] = 0;
        for (i = 0; (i < 255) && fontmenu_title[i]; i ++)
        {
            fontmenu_title_pascal[i + 1] = fontmenu_title[i];
            fontmenu_title_pascal[0] = i + 1;
        }
        fontmenu = NewMenu(MACCURSES_FONTMENU, fontmenu_title_pascal);
        if (fontmenu)
        {
            const char *fontmenu_items = "Bigger Font/+;Smaller Font/-;Original Font Size/=";
            Str255 fontmenu_items_pascal = { 0 };

            fontmenu_items_pascal[0] = 0;
            for (i = 0; (i < 255) && fontmenu_items[i]; i ++)
            {
                fontmenu_items_pascal[i + 1] = fontmenu_items[i];
                fontmenu_items_pascal[0] = i + 1;
            }
            AppendMenu(fontmenu, fontmenu_items_pascal);
#ifdef __CARBON__
            {
                fontmenu_items = "-;Original Font/T;-";
                fontmenu_items_pascal[0] = 0;
                for (i = 0; (i < 255) && fontmenu_items[i]; i ++)
                {
                    fontmenu_items_pascal[i + 1] = fontmenu_items[i];
                    fontmenu_items_pascal[0] = i + 1;
                }
                AppendMenu(fontmenu, fontmenu_items_pascal);
                CreateStandardFontMenu(fontmenu,
                                       CountMenuItems(fontmenu) + 1,
                                       MACCURSES_FONTMENU + 1,
                                       kHierarchicalFontMenuOption,
                                       NULL);
            }
#endif /* defined(__CARBON__) */
            InsertMenu(fontmenu, 0);
        }
    }

    maccurses_fontsize = (GetDefFontSize() >= 11) ? 12 : 9;
    if (getenv("MACCURSES_FONTSIZE")
        &&
        *getenv("MACCURSES_FONTSIZE")
        &&
        atoi(getenv("MACCURSES_FONTSIZE")))
    {
        maccurses_fontsize = atoi(getenv("MACCURSES_FONTSIZE"));
        if (maccurses_fontsize < 1) maccurses_fontsize = 1;
        if (maccurses_fontsize > 72) maccurses_fontsize = 72;
    }
    for (i = 0; i < maccurses_zoom; i ++)
    {
        if (maccurses_fontsize < 3)
        {
            maccurses_fontsize = maccurses_fontsize + 1;
        }
        else if (maccurses_fontsize < 72)
        {
            maccurses_fontsize = maccurses_fontsize * 4 / 3;
        }
        else
        {
            maccurses_zoom --;
        }
    }
    for (i = 0; i > maccurses_zoom; i --)
    {
        if (maccurses_fontsize > 1)
        {
            maccurses_fontsize = maccurses_fontsize * 3 / 4;
        }
        else
        {
            maccurses_zoom ++;
        }
    }
    if (maccurses_fontsize < 1) maccurses_fontsize = 1;
    if (maccurses_fontsize > 72) maccurses_fontsize = 72;
    {
        SetRect(&coords, 100, 100,
                100 + (w * maccurses_fontsize / 2), 100 + (h * maccurses_fontsize));
    }
    if (firsttime)
    {
        maccurses_hwnd = NewCWindow(0, &coords, maccurses_title, true, zoomDocProc, (WindowRef)-1L, true, MACCURSES_MAINWIN);
        SetEventMask(keyDownMask | autoKeyMask | activMask | mDownMask | osMask | highLevelEventMask);
        maccurses_port = GetWindowPort(maccurses_hwnd);
        SetPort((GrafPtr) maccurses_port);
    }
    encoding_name = getenv("MACCURSES_FONTENCODING");
    maccurses_fontfamily = maccurses_custom_fontfamily;
    font_pascal[0] = 0;
    font = getenv("MACCURSES_FONT");
    if (maccurses_custom_fontfamily != kInvalidFontFamily)
    {
        if (noErr != FMGetFontFamilyName(maccurses_custom_fontfamily, font_pascal))
        {
            GetFNum(font_pascal, &maccurses_fontfamily);
        }
    }
    if (((! maccurses_fontfamily) || (maccurses_fontfamily == kInvalidFontFamily))
        &&
        font
        &&
        font[0])
    {
        font_pascal[0] = 0;
        for (i = 1; (font[i - 1]) && (i < 256); i ++)
        {
            font_pascal[i] = font[i - 1];
            font_pascal[0] = i;
        }
        maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
        if (maccurses_fontfamily == kInvalidFontFamily)
        {
            GetFNum(font_pascal, &maccurses_fontfamily);
            if (! maccurses_fontfamily)
            {
                encoding_name = 0;
            }
        }
    }
#if UNICODE
    if ((! maccurses_fontfamily) || (maccurses_fontfamily == kInvalidFontFamily))
    {
        font = "DejaVu Sans Mono";
        font_pascal[0] = 0;
        for (i = 1; (font[i - 1]) && (i < 256); i ++)
        {
            font_pascal[i] = font[i - 1];
            font_pascal[0] = i;
        }
        maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
        if (maccurses_fontfamily == kInvalidFontFamily)
        {
            GetFNum(font_pascal, &maccurses_fontfamily);
        }
    }
    if ((! maccurses_fontfamily) || (maccurses_fontfamily == kInvalidFontFamily))
    {
        font = "Liberation Mono";
        font_pascal[0] = 0;
        for (i = 1; (font[i - 1]) && (i < 256); i ++)
        {
            font_pascal[i] = font[i - 1];
            font_pascal[0] = i;
        }
        maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
        if (maccurses_fontfamily == kInvalidFontFamily)
        {
            GetFNum(font_pascal, &maccurses_fontfamily);
        }
    }
#endif
    if (((! maccurses_fontfamily) || (maccurses_fontfamily == kInvalidFontFamily))
        &&
        ((maccurses_fontsize == 9)
         ||
         (maccurses_fontsize == 12)))
    {
        font = "VT100";
        font_pascal[0] = 0;
        for (i = 1; (font[i - 1]) && (i < 256); i ++)
        {
            font_pascal[i] = font[i - 1];
            font_pascal[0] = i;
        }
        maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
        if (maccurses_fontfamily == kInvalidFontFamily)
        {
            GetFNum(font_pascal, &maccurses_fontfamily);
        }
    }
#if 0
    if ((! maccurses_fontfamily) || (maccurses_fontfamily == kInvalidFontFamily))
    {
        /* Osaka Mono (Shift_JIS/MacJapanese) */
        font = "Osaka\201\174\223\231\225\235";
        font_pascal[0] = 0;
        for (i = 1; (font[i - 1]) && (i < 256); i ++)
        {
            font_pascal[i] = font[i - 1];
            font_pascal[0] = i;
        }
        maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
        if (maccurses_fontfamily == kInvalidFontFamily)
        {
            GetFNum(font_pascal, &maccurses_fontfamily);
        }
    }
#endif
    if ((! maccurses_fontfamily) || (maccurses_fontfamily == kInvalidFontFamily))
    {
        font = "Monaco";
        font_pascal[0] = 0;
        for (i = 1; (font[i - 1]) && (i < 256); i ++)
        {
            font_pascal[i] = font[i - 1];
            font_pascal[0] = i;
        }
        maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
        if (maccurses_fontfamily == kInvalidFontFamily)
        {
            GetFNum(font_pascal, &maccurses_fontfamily);
        }
    }
    if ((! maccurses_fontfamily) || (maccurses_fontfamily == kInvalidFontFamily))
    {
        font = "Courier";
        font_pascal[0] = 0;
        for (i = 1; (font[i - 1]) && (i < 256); i ++)
        {
            font_pascal[i] = font[i - 1];
            font_pascal[0] = i;
        }
        maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
        if (maccurses_fontfamily == kInvalidFontFamily)
        {
            GetFNum(font_pascal, &maccurses_fontfamily);
        }
    }
    maccurses_fontfamily = FMGetFontFamilyFromName(font_pascal);
    if (maccurses_fontfamily == kInvalidFontFamily)
    {
        GetFNum(font_pascal, &maccurses_fontfamily);
    }
    if (noErr != FMGetFontFamilyName(maccurses_fontfamily, font_pascal))
    {
        GetFontName(maccurses_fontfamily, font_pascal);
    }
    encoding = kTextEncodingMacRoman;
    GetFontName(maccurses_fontfamily, font_pascal);
    if ((noErr != FMGetFontFamilyTextEncoding(maccurses_fontfamily, &encoding))
        ||
        (noErr != RevertTextEncodingToScriptInfo(encoding, &scriptID, NULL, NULL)))
    {
        scriptID = FontToScript(maccurses_fontfamily);
    }
    if (font_pascal[0] < 255) font_pascal[font_pascal[0] + 1] = '\0';
    if (scriptID == smJapanese)
    {
        encoding = kTextEncodingMacJapanese;
    }
    else if (scriptID == smRoman)
    {
        encoding = kTextEncodingMacRoman;
        if ((scriptID == smRoman)
            &&
            (! memcmp((void *) (font_pascal + 1), (void *) "VT100", strlen("VT100"))))
        {
            encoding = kTextEncodingMacVT100;
        }
    }
    else if ((scriptID == smUninterp)
             &&
             (! memcmp((void *) (font_pascal + 1), (void *) "VT100", strlen("VT100"))))
    {
        encoding = kTextEncodingMacVT100;
    }
    else
    {
        encoding = kTextEncodingUS_ASCII;
    }
    if (encoding_name && *encoding_name)
    {
        Str255 encoding_name_pascal;

        encoding_determined = true;
        encoding_name_pascal[0] = 0;
        for (i = 0; (i < 255) && encoding_name[i]; i ++)
        {
            encoding_name_pascal[0] = i + 1;
            encoding_name_pascal[i + 1] = encoding_name[i];
        }
        if (noErr == TECGetTextEncodingFromInternetName(&encoding, encoding_name_pascal))
        {
        }
        else
        {
            if ((! strcasecmp(encoding_name, "MacRoman"))
                ||
                (! strcasecmp(encoding_name, "Macintosh"))
                ||
                (! strcasecmp(encoding_name, "csMacintosh"))
                ||
                (! strcasecmp(encoding_name, "Mac")))
            {
                encoding = kTextEncodingMacRoman;
            }
            else if (! strcasecmp(encoding_name, "MacJapanese"))
            {
                encoding = kTextEncodingMacJapanese;
            }
            else if (! strcasecmp(encoding_name, "MacVT100"))
            {
                encoding = kTextEncodingMacVT100;
            }
            else if ((! strcasecmp(encoding_name, "cp437"))
                     ||
                     (! strcasecmp(encoding_name, "437"))
                     ||
                     (! strcasecmp(encoding_name, "ibm437"))
                     ||
                     (! strcasecmp(encoding_name, "DOSLatinUS")))
            {
                encoding = kTextEncodingDOSLatinUS;
            }
            else
            {
                encoding = kTextEncodingUS_ASCII;
            }
        }
    }
#ifndef UNICODE
    if (encoding == kTextEncodingMacRoman)
    {
        maccurses_cvtt = maccurses_macroman;
    }
    else if (encoding == kTextEncodingMacJapanese)
    {
        maccurses_cvtt = maccurses_macjapanese;
    }
    else if (encoding == kTextEncodingMacVT100)
    {
        maccurses_cvtt = maccurses_macvt100;
    }
    else if (encoding == kTextEncodingDOSLatinUS)
    {
        maccurses_cvtt = maccurses_cp437;
    }
    else
    {
        maccurses_cvtt = 0;
    }

    /* We can only rely on TextEncodingConverter where
     * Carbon is also present. We have a few hardwired
     * conversion tables to help us in the non-Carbon
     * case. */
    {
        if ((encoding_determined == true)
            ||
            (noErr == FMGetFontFamilyTextEncoding(maccurses_fontfamily, &encoding))
            ||
            (noErr == UpgradeScriptInfoToTextEncoding(scriptID, kTextLanguageDontCare, kTextRegionDontCare, font_pascal, &encoding)))
        {
            TextEncoding cp437;
            TECObjectRef cvt;
            OSStatus ret;
            static unsigned short cvtd[256 * 2];

            encoding_determined = true;
            cp437 = CreateTextEncoding(kTextEncodingDOSLatinUS, kTextEncodingDefaultVariant, kTextEncodingDefaultFormat);
            ret = TECCreateConverter(&cvt, cp437, encoding);
            if (noErr == ret)
            {
                UInt8 ibyte;
                ByteCount nread = 0;
                UInt8 obytes[32];
                ByteCount nwritten = 0;
                int cvtu = 0;

                for (i = 0x20; i <= 0xff; i ++)
                {
                    if (i != 0x7f)
                    {
                        ibyte = i;
                        if (noErr == TECConvertText(cvt, &ibyte, 1, &nread, obytes, sizeof(obytes), &nwritten))
                        {
                            ByteCount moar = 0;

                            if (noErr == TECFlushText(cvt, obytes + nwritten, sizeof(obytes) - nwritten, &moar))
                            {
                                nwritten += moar;
                            }
                            obytes[nwritten] = 0;
                            if ((nwritten == 1)
                                &&
                                (obytes[0] > 0x7f))
                            {
                                cvtd[2 * cvtu] = i;
                                cvtd[2 * cvtu + 1] = obytes[0];
                                cvtu ++;
                            }
                            else if ((nwritten == 2)
                                     &&
                                     (obytes[0] > 0x7f))
                            {
                                cvtd[2 * cvtu] = i;
                                cvtd[2 * cvtu + 1] = (obytes[0] << 8) | obytes[1];
                                cvtu ++;
                            }
                        }
                    }
                }
                TECDisposeConverter(cvt);
                if (cvtu || maccurses_cvtt)
                {
                    cvtd[cvtu * 2] = 0;
                    cvtd[cvtu * 2 + 1] = 0;
                    maccurses_cvtt = cvtd;
                }
            }
            else
            {
            }
        }
    }
#endif /* ! defined(UNICODE) */
    TextFont(maccurses_fontfamily);
    /*TextMode(transparent);*/
    TextMode(srcCopy);
    TextFace(0);
    TextSize(maccurses_fontsize);
    GetFontInfo(&maccurses_fontinfo);
    maccurses_widMax = 0;
    for (i = 0x20; i < 0x7f; i ++)
    {
        Str255 si;
        short wid;

        si[0] = 1;
        si[1] = i;
        wid = StringWidth(si);
        if (wid > maccurses_widMax) maccurses_widMax = wid;
    }
#ifndef UNICODE
    if (maccurses_cvtt)
    {
        if (! maccurses_cvtt[0])
        {
            for (j = 0x80; j <= 0xff; j ++)
            {
                Str255 si;
                short wid;

                si[0] = 1;
                si[1] = j;
                wid = StringWidth(si);
                if (wid > maccurses_widMax)
                {
                    maccurses_widMax = wid;
                }
            }
        }
        for (j = 0; maccurses_cvtt[j]; j += 2)
        {
            if (maccurses_cvtt[j + 1] <= 0xff)
            {
                Str255 si;
                short wid;

                si[0] = 1;
                si[1] = maccurses_cvtt[j + 1];
                wid = StringWidth(si);
                if (wid > maccurses_widMax)
                {
                    maccurses_widMax = wid;
                }
            }
        }
    }
#endif /* ! defined(UNICODE) */
    if (! maccurses_widMax) maccurses_widMax = maccurses_fontinfo.widMax;
    if ((w * maccurses_widMax) > 4096)
    {
        w = 4096 / maccurses_widMax;
    }
    if ((h * (maccurses_fontinfo.ascent + maccurses_fontinfo.descent)) > 2048)
    {
        h = 2048 / (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
    }
    maccurses_size.h = w * maccurses_widMax;
    maccurses_size.v = h * (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
#ifndef __CARBON__
    if (1)
    {
        Rect chrome;

        GetWindowStructureWidths(maccurses_hwnd, &chrome);
        GetRegionBounds(GetGrayRgn(), &coords);
        if (maccurses_size.h > (coords.right - coords.left - chrome.left - chrome.right))
        {
            maccurses_size.h = coords.right - coords.left - chrome.left - chrome.right;
        }
        if (maccurses_size.v > (coords.bottom - coords.top - chrome.top - chrome.bottom))
        {
            maccurses_size.v = coords.bottom - coords.top - chrome.bottom - chrome.bottom;
        }
    }
#endif /* ! defined(__CARBON__) */
    SizeWindow(maccurses_hwnd,
               maccurses_size.h,
               maccurses_size.v,
               true);
#ifdef __CARBON__
    ConstrainWindowToScreen(maccurses_hwnd,
                            kWindowStructureRgn,
                            kWindowConstrainMayResize,
                            NULL,
                            NULL);
#endif /* ! defined(__CARBON__) */
    GetWindowBounds(maccurses_hwnd,
                    kWindowContentRgn,
                    &coords);
    maccurses_size.v = coords.bottom - coords.top;
    maccurses_size.h = coords.right - coords.left;
    maccurses_w = maccurses_size.h / maccurses_widMax;
    maccurses_h = maccurses_size.v / (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
    for (i = 0; i < COLORS; i ++)
        for (j = 0; j < COLORS; j ++)
        {
            maccurses_init_pair((i * COLORS) + j, i, j);
        }
    maccurses_init_pair(0, COLOR_BLACK, COLOR_WHITE);
    for (i = 0; i < COLORS; i ++)
    {
        maccurses_palette[i].red = (i & 1) ? 0xffff : 0;
        maccurses_palette[i].green = (i & 2) ? 0xffff : 0;
        maccurses_palette[i].blue = (i & 4) ? 0xffff : 0;
        if (i & PEN_BRIGHT)
        {
            maccurses_palette[i].red |= 0x4000;
            maccurses_palette[i].green |= 0x4000;
            maccurses_palette[i].blue |= 0x4000;
        }
        else
        {
            maccurses_palette[i].red -= maccurses_palette[i].red / 3;
            maccurses_palette[i].green -= maccurses_palette[i].green / 3;
            maccurses_palette[i].blue -= maccurses_palette[i].blue / 3;
        }
    }
    for (i = 0; i < COLORS; i ++)
    {
        for (j = 0; j < COLORS; j ++)
        {
            maccurses_palette[COLORS + (i * COLORS) + j].red = maccurses_mix(maccurses_palette[i].red, maccurses_palette[j].red);
            maccurses_palette[COLORS + (i * COLORS) + j].green = maccurses_mix(maccurses_palette[i].green, maccurses_palette[j].green);
            maccurses_palette[COLORS + (i * COLORS) + j].blue = maccurses_mix(maccurses_palette[i].blue, maccurses_palette[j].blue);
        }
    }
    maccurses_palette[COLORS + COLORS * COLORS].red = maccurses_mix(0xffffU, 0);
    maccurses_palette[COLORS + COLORS * COLORS].green = maccurses_mix(0xffffU, 0);
    maccurses_palette[COLORS + COLORS * COLORS].blue = maccurses_mix(0xffffU, 0);
    SetWTitle(maccurses_hwnd, maccurses_title);
    AEInstallEventHandler(kCoreEventClass, kAEQuitApplication,
                          NewAEEventHandlerUPP(maccurses_onQuitEvent), 0, false);
    maccurses_updateFontMenu();
    maccurses_erase();
}

static int maccurses_endwin(void)
{
    SetMenuBar(maccurses_oldMainMenu);
    DisposeHandle(maccurses_oldMainMenu);
    return OK;
}

static int maccurses_clearok(int flag)
{
    maccurses_clearok_flag = flag;
    return OK;
}

static int maccurses_nodelay(int flag)
{
    maccurses_nodelay_flag = flag;
    return OK;
}

static int maccurses_intrflush(int flag)
{
    maccurses_intrflush_flag = flag;
    return OK;
}

static int maccurses_idlok(int flag)
{
    maccurses_idlok_flag = flag;
    return OK;
}

static int maccurses_leaveok(int flag)
{
    maccurses_leaveok_flag = flag;
    return OK;
}

static int maccurses_getyx(int *y, int *x)
{
    *y = maccurses_y;
    *x = maccurses_x;
    return OK;
}

static int maccurses_refresh(void)
{
    return OK;
}

static int maccurses_beep(void)
{
    SysBeep(30);
    return OK;
}

static int maccurses_doMenuItem(long msr)
{
    CharParameter ch;
    MenuRef mr;
    MenuItemIndex item;

    mr = GetMenuRef(msr >> 16);
    item = msr & 0xffffL;
    GetItemCmd(mr, item, &ch);
    switch (ch)
    {
    case '-':
        maccurses_zoom --;
        maccurses_new_w = maccurses_w;
        maccurses_new_h = maccurses_h;
        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
        break;
    case '+':
        maccurses_zoom ++;
        maccurses_new_w = maccurses_w;
        maccurses_new_h = maccurses_h;
        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
        break;
    case '=':
        maccurses_zoom = 0;
        maccurses_new_w = maccurses_w;
        maccurses_new_h = maccurses_h;
        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
        break;
    case 'r':
    case 'R':
        maccurses_new_w = maccurses_w;
        maccurses_new_h = maccurses_h;
        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
        break;
    case '?':
        HiliteMenu(0);
        return '?';
    case 'q':
    case 'Q':
        /* CTRL('C') */
        HiliteMenu(0);
        return 0x03;
    case 't':
    case 'T':
        maccurses_custom_fontfamily = kInvalidFontFamily;
        maccurses_custom_fontstyle = 0;
        maccurses_new_w = maccurses_w;
        maccurses_new_h = maccurses_h;
        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
        break;
    case 'z':
    case 'Z':
    case '\r':
    case '\n':
        if (1)
        {
            if (maccurses_maximized)
            {
                maccurses_maximized = false;
                maccurses_new_w = maccurses_ideal_w;
                maccurses_new_h = maccurses_ideal_h;
                SetWindowBounds(maccurses_hwnd,
                                kWindowContentRgn,
                                &maccurses_ideal_rect);
            }
            else
            {
                maccurses_maximized = true;
                maccurses_ideal_w = maccurses_w;
                maccurses_ideal_h = maccurses_h;
                maccurses_new_w = 4096 / maccurses_widMax;
                maccurses_new_h = 2048 / (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
                GetWindowBounds(maccurses_hwnd,
                                kWindowContentRgn,
                                &maccurses_ideal_rect);
            }
            maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
            break;
        }
    default:
#ifdef __CARBON__
        if ((msr >> 16) >= MACCURSES_FONTMENU)
        {
            maccurses_custom_fontfamily = kInvalidFontFamily;
            maccurses_custom_fontstyle = 0;
            GetFontFamilyFromMenuSelection(mr, item, &maccurses_custom_fontfamily, &maccurses_custom_fontstyle);
            maccurses_new_w = maccurses_w;
            maccurses_new_h = maccurses_h;
            maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
        }
        else
#endif /* defined(__CARBON__) */
        {
            maccurses_beep();
        }
        break;
    }
    HiliteMenu(0);
    return ERR;
}

static int maccurses_getch(void)
{
    EventRecord er;
    int ret = ERR;

    if (maccurses_gotQuitEvent == TRUE)
    {
        maccurses_gotQuitEvent = FALSE;
        return 'q';
    }
    if (maccurses_resize_pending
        &&
        (! (-- maccurses_resize_pending)))
    {
        ret = KEY_RESIZE;
    }
    if (! maccurses_valid)
    {
        return ret;
    }
    if (IsWindowCollapsed(maccurses_hwnd)
        &&
        (! maccurses_resize_pending))
    {
        maccurses_new_w = maccurses_w;
        maccurses_new_h = maccurses_h;
        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
    }
    while (WaitNextEvent(keyDownMask | autoKeyMask | activMask | mDownMask | mUpMask | osMask | highLevelEventMask,
                         &er, IsWindowCollapsed(maccurses_hwnd) ? 30 : 0, NULL)
           ||
           IsWindowCollapsed(maccurses_hwnd))
    {
        if (er.what == kHighLevelEvent)
        {
            AEProcessAppleEvent(&er);
            if (maccurses_gotQuitEvent == TRUE)
            {
                maccurses_gotQuitEvent = FALSE;
                return 'q';
            }
            continue;
        }
        if (IsWindowCollapsed(maccurses_hwnd)
            &&
            er.what == nullEvent)
        {
            continue;
        }
        if ((er.what == osEvt)
            &&
            (! (er.message & resumeFlag)))
        {
            while (1)
            {
                WaitNextEvent(osMask | activMask | mDownMask | highLevelEventMask, &er, 60, NULL);
                if (er.what == kHighLevelEvent)
                {
                    AEProcessAppleEvent(&er);
                    if (maccurses_gotQuitEvent == TRUE)
                    {
                        maccurses_gotQuitEvent = FALSE;
                        return 'q';
                    }
                    continue;
                }
                if (((er.what == osEvt)
                     &&
                     (er.message & resumeFlag))
                    ||
                    (((er.what == activateEvt) || (er.what == mouseDown))
                     &&
                     (er.modifiers & activeFlag)))
                {
                    maccurses_new_w = maccurses_w;
                    maccurses_new_h = maccurses_h;
                    maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
                    maccurses_updateFontMenu();
                    break;
                }
            }
        }
        if (er.what == mouseDown)
        {
            WindowPartCode wpc;
            WindowRef win;

            win = 0;
            wpc = FindWindow(er.where, &win);
            if (win != maccurses_hwnd)
            {
                switch (wpc)
                {
                case inMenuBar:
                {
                    long msr;

                    msr = MenuSelect(er.where);
                    if (msr >> 16)
                    {
                        ret = maccurses_doMenuItem(msr);
                        if (ret != ERR)
                        {
                            return ret;
                        }
                    }
                    break;
                }
                }
            }
            else
            {
                switch (wpc)
                {
                case inDesk:
                    break;
                case inMenuBar:
                {
                    long msr;

                    msr = MenuSelect(er.where);
                    if (msr >> 16)
                    {
                        ret = maccurses_doMenuItem(msr);
                        if (ret != ERR)
                        {
                            return ret;
                        }
                    }
                    break;
                }
                case inSysWindow:
                    break;
                case inContent:
                    SelectWindow(maccurses_hwnd);
                    ShowWindow(maccurses_hwnd);
                    break;
                case inDrag:
                    if (1)
                    {
                        Rect dragbounds;

                        SetRect(&dragbounds,
                                0, 0,
                                4096, 2048);
                        DragWindow(win, er.where, &dragbounds);
                    }
                    break;
                case inGrow:
                    if (1)
                    {
                        Rect resizebounds;

                        if (ResizeWindow(win, er.where, 0, &resizebounds))
                        {
                            maccurses_size.h = resizebounds.right - resizebounds.left;
                            maccurses_size.v = resizebounds.bottom - resizebounds.top;
                            maccurses_new_w = maccurses_size.h / maccurses_widMax;
                            maccurses_new_h = maccurses_size.v / (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
                            if ((maccurses_new_w != maccurses_w)
                                ||
                                (maccurses_new_h != maccurses_h))
                            {
                                maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
                                continue;
                            }
                        }
                    }
                    break;
                case inGoAway:
                    if (TrackGoAway(win, er.where))
                    {
                        return 0x3;
                    }
                    break;
                case inZoomIn:
                case inZoomOut:
                    if (TrackBox(win, er.where, wpc))
                    {
                        if (maccurses_maximized)
                        {
                            maccurses_maximized = false;
                            maccurses_new_w = maccurses_ideal_w;
                            maccurses_new_h = maccurses_ideal_h;
                            SetWindowBounds(maccurses_hwnd,
                                            kWindowContentRgn,
                                            &maccurses_ideal_rect);
                        }
                        else
                        {
                            maccurses_maximized = true;
                            maccurses_ideal_w = maccurses_w;
                            maccurses_ideal_h = maccurses_h;
                            maccurses_new_w = 4096 / maccurses_widMax;
                            maccurses_new_h = 2048 / (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
                            GetWindowBounds(maccurses_hwnd,
                                            kWindowContentRgn,
                                            &maccurses_ideal_rect);
                        }
                        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
                        continue;
                    }
                    break;
                case inCollapseBox:
                    if (TrackBox(win, er.where, wpc))
                    {
                        CollapseWindow(win, IsWindowCollapsed(win) ? false : true);
                        maccurses_new_w = maccurses_w;
                        maccurses_new_h = maccurses_h;
                        maccurses_resize_pending = _MACCURSES_RESIZE_TIMER;
                        continue;
                    }
                    break;
                case inProxyIcon:
                    break;
                }
            }
        }
        else if ((er.what == keyDown)
                 ||
                 (er.what == autoKey))
        {
            if (er.modifiers & cmdKey)
            {
                long msr;

                msr = MenuEvent(&er);
                if (msr >> 16)
                {
                    ret = maccurses_doMenuItem(msr);
                    if (ret != ERR)
                    {
                        return ret;
                    }
                }
                continue;
            }
            switch (er.message & charCodeMask)
            {
            case kLeftArrowCharCode: return KEY_LEFT;
            case kRightArrowCharCode: return KEY_RIGHT;
            case kUpArrowCharCode: return KEY_UP;
            case kDownArrowCharCode: return KEY_DOWN;
            case kDeleteCharCode: return 0x7f;
            default:
                return er.message & charCodeMask;
            }
        }
    }
    return ret;
}

static int maccurses_resizeterm(int lines, int columns)
{
    maccurses_h = lines;
    maccurses_w = columns;
    return OK;
}

static int maccurses_move(int y, int x)
{
    maccurses_y = y;
    maccurses_x = x;
    return OK;
}

static int maccurses_init_pair(short pair, short f, short b)
{
    if (! maccurses_has_colors())
    {
        return ERR;
    }
    if ((pair < 0)
        ||
        (pair >= COLOR_PAIRS)
        ||
        (f < 0)
        ||
        (f >= COLORS)
        ||
        (b < 0)
        ||
        (b >= COLORS))
    {
        return ERR;
    }
    maccurses_pairs[pair][0] = f;
    maccurses_pairs[pair][1] = b;
    return OK;
}

static int maccurses_pair_content(short pair, short *fp, short *bp)
{
    if (! maccurses_has_colors())
    {
        return ERR;
    }
    if ((pair < 0)
        ||
        (pair >= COLOR_PAIRS))
    {
        return ERR;
    }
    *fp = maccurses_pairs[pair][0];
    *bp = maccurses_pairs[pair][1];
    return OK;
}

static int maccurses_addch(maccurses_chtype ch)
{
    Rect cell;
    int j;
    Str255 si;
    short fg, bg;
    int wcw;

    if (ch == '\n')
    {
        maccurses_move(maccurses_y + 1, 0);
        return OK;
    }
    if (ch == '\r')
    {
        maccurses_move(maccurses_y, 0);
        return OK;
    }
    if (ch == '\t')
    {
        do
        {
            maccurses_addch(' ');
        }
        while (maccurses_x % 8);
        return OK;
    }
    if (maccurses_x >= maccurses_w)
    {
        maccurses_x = 0;
        maccurses_y ++;
    }
    if (maccurses_y >= maccurses_h)
    {
        maccurses_y = maccurses_h - 1;
    }
    if (maccurses_x < 0)
    {
        maccurses_x = maccurses_w - 1;
        maccurses_y --;
    }
    if (maccurses_y < 0)
    {
        maccurses_y = 0;
    }
    wcw = maccurses_wcwidth(ch);
    if (wcw < 1)
    {
        return ERR;
    }
    if (maccurses_valid)
    {
        Boolean substituted = false;
        Boolean usefake = false;
        maccurses_attr_t attr, attr_extra;
        Pattern black;

        GetQDGlobalsBlack(&black);
        attr = maccurses_attr | (ch & _MACCURSES_A_ATTR);
        ch &= _MACCURSES_A_CHARTEXT;
        attr_extra = 0;
        si[0] = 1;
        if ((ch >= 0x20) && (ch < 0x7f))
        {
            si[1] = ch;
            if (ch == ' ')
            {
                usefake = true;
            }
        }
        else
        {
            substituted = true;
            si[1] = '?';
            switch ((ch & _MACCURSES_A_ALTCHARSET) ? ch : (ch | _MACCURSES_A_ALTCHARSET))
            {
            case ACS_LRCORNER:
            case ACS_URCORNER:
            case ACS_ULCORNER:
            case ACS_LLCORNER:
            case ACS_PLUS:
            case ACS_LTEE:
            case ACS_RTEE:
            case ACS_BTEE:
            case ACS_TTEE:
                si[1] = '+';
                break;
            case ACS_HLINE:
                si[1] = '-';
                break;
            case ACS_VLINE:
                si[1] = '|';
                break;
            case _MACCURSES_ACS_MIDDOT:
                si[1] = '.';
                break;
            case _MACCURSES_ACS_PUCE:
            case _MACCURSES_ACS_BLACKSQUARE:
                si[1] = 'o';
                break;
            case ACS_BLOCK:
                si[1] = ' ';
                attr_extra = A_REVERSE;
                usefake = true;
                break;
            case _MACCURSES_ACS_THALF:
                si[1] = '\"';
                usefake = true;
                break;
            case _MACCURSES_ACS_BHALF:
                si[1] = ',';
                usefake = true;
                break;
            case _MACCURSES_ACS_LHALF:
            case _MACCURSES_ACS_RHALF:
                usefake = true;
                si[1] = '#';
                break;
            }
        }
#ifndef UNICODE
        if ((((ch & _MACCURSES_A_ALTCHARSET) != _MACCURSES_A_ALTCHARSET)
             ||
             (maccurses_wcwidth(ch ^ _MACCURSES_A_ALTCHARSET) == 1))
            &&
            maccurses_cvtt)
        {
            if (! maccurses_cvtt[0])
            {
                substituted = false;
                attr_extra = 0;
                si[1] = ch;
            }
            for (j = 0; maccurses_cvtt[j]; j += 2)
            {
                if (maccurses_cvtt[j]
                    ==
                    (((ch & _MACCURSES_A_ALTCHARSET) == _MACCURSES_A_ALTCHARSET)
                     ?
                     (ch ^ _MACCURSES_A_ALTCHARSET)
                     :
                     ch))
                {
                    if ((maccurses_cvtt[j + 1] > 0xff)
                        &&
                        (ch & _MACCURSES_A_ALTCHARSET) != _MACCURSES_A_ALTCHARSET)
                    {
                        substituted = false;
                        attr_extra = 0;
                        si[0] = 2;
                        si[1] = maccurses_cvtt[j + 1] >> 8;
                        si[2] = maccurses_cvtt[j + 1] & 0xff;
                    }
                    else if (maccurses_cvtt[j + 1] <= 0xff)
                    {
                        substituted = false;
                        attr_extra = 0;
                        si[1] = maccurses_cvtt[j + 1];
                    }
                    break;
                }
            }
        }
#endif /* ! defined(UNICODE) */
        attr ^= attr_extra;
        fg = COLOR_WHITE | PEN_BRIGHT;
        bg = COLOR_BLACK;
        if ((attr & _MACCURSES_A_COLOR) && maccurses_has_colors())
        {
            short pair;

            pair = PAIR_NUMBER(attr);
            maccurses_pair_content(pair, &fg, &bg);
        }
        if (attr & A_REVERSE)
        {
            int tmp;

            tmp = fg;
            fg = bg;
            bg = tmp;
        }
        if (fg == bg)
        {
            fg = COLOR_BLACK;
        }
        if (fg == bg)
        {
            bg = COLOR_WHITE;
        }
        PenPat(&black);
        ForeColor(_COLOR_TO_MAC(fg & 7, whiteColor));
        BackColor(_COLOR_TO_MAC(bg & 7, blackColor));
        if ((attr & _MACCURSES_A_COLOR) && maccurses_has_colors())
        {
            RGBForeColor(maccurses_palette + fg);
            RGBBackColor(maccurses_palette + bg);
        }
        TextFace(maccurses_custom_fontstyle
                 |
                 ((attr & A_UNDERLINE) ? underline : 0)
                 |
                 ((attr & A_BOLD) ? bold : 0));
        SetRect(&cell,
                maccurses_x * maccurses_widMax,
                maccurses_y * (maccurses_fontinfo.ascent + maccurses_fontinfo.descent),
                (maccurses_x + wcw) * maccurses_widMax,
                (maccurses_y + 1) * (maccurses_fontinfo.ascent + maccurses_fontinfo.descent));
        EraseRect(&cell);
#ifdef UNICODE
        if (((ch & _MACCURSES_A_ALTCHARSET) != _MACCURSES_A_ALTCHARSET)
            &&
            (! usefake))
        {
            ATSUStyle sty;
            Boolean atsuok = false;
            UniChar uc[3];
            UniCharCount unilen;
            maccurses_chtype chuni;
            ATSUTextLayout lyo;
            ATSUAttributeTag fontSizeAttrTag, fontAttrTag;
            Fixed fontSizeAttrValue;
            ATSUFontID fontAttrValue;
            FMFontStyle whatwegot;
            ByteCount fontSizeAttrValueSize, fontAttrValueSize;
            ATSUAttributeValuePtr fontSizeAttrValuePtr, fontAttrValuePtr;
            Rect dimen;

            if (noErr == ATSUCreateStyle(&sty))
            {
                chuni = (((ch & _MACCURSES_A_ALTCHARSET) == _MACCURSES_A_ALTCHARSET)
                         ?
                         (ch ^ _MACCURSES_A_ALTCHARSET)
                         :
                         ch);
                if ((chuni < 0) || (chuni > 0x10fffdL) || ((chuni & 0xfffeL) == 0xfffeL))
                {
                    chuni = 0xfffd;
                }
                unilen = 1 + (chuni > 0xffffL);
                uc[0] = (chuni > 0xffffL) ? (((chuni - 0x10000L) >> 10) | 0xd800L) : chuni;
                uc[1] = (chuni > 0xffffL) ? (((chuni - 0x10000L) & 0x3ffL) | 0xd800L) : 0;
                uc[2] = 0;
                fontSizeAttrTag = kATSUSizeTag;
                fontSizeAttrValueSize = sizeof(fontSizeAttrValue);
                fontSizeAttrValuePtr = &fontSizeAttrValue;
                fontSizeAttrValue = maccurses_fontsize << 16;
                fontAttrTag = kATSUFontTag;
                fontAttrValueSize = sizeof(fontAttrValue);
                fontAttrValuePtr = &fontAttrValue;
                if ((noErr == FMGetFontFromFontFamilyInstance(maccurses_fontfamily,
                                                              maccurses_custom_fontstyle
                                                              |
                                                              ((attr & A_UNDERLINE) ? underline : 0)
                                                              |
                                                              ((attr & A_BOLD) ? bold : 0),
                                                              &fontAttrValue,
                                                              &whatwegot))
                    &&
                    (noErr == ATSUSetAttributes(sty, 1, &fontAttrTag, &fontAttrValueSize, &fontAttrValuePtr))
                    &&
                    (noErr == ATSUSetAttributes(sty, 1, &fontSizeAttrTag, &fontSizeAttrValueSize, &fontSizeAttrValuePtr))
                    &&
                    (noErr == ATSUCreateTextLayoutWithTextPtr(uc,
                                                              kATSUFromTextBeginning,
                                                              kATSUToTextEnd,
                                                              unilen,
                                                              1,
                                                              &unilen,
                                                              &sty,
                                                              &lyo)))
                {
                    ATSUSetTransientFontMatching(lyo, true);
                    if (noErr == ATSUMeasureTextImage(lyo,
                                                      kATSUFromTextBeginning,
                                                      kATSUToTextEnd,
                                                      0,
                                                      0,
                                                      &dimen))
                    {
                        MoveTo(maccurses_x * maccurses_widMax /* + (wcw * maccurses_widMax - (dimen.right - dimen.left)) / 2 */,
                               maccurses_y * (maccurses_fontinfo.ascent + maccurses_fontinfo.descent) + maccurses_fontinfo.ascent);
                        if (noErr == ATSUDrawText(lyo,
                                                  kATSUFromTextBeginning,
                                                  kATSUToTextEnd,
                                                  kATSUUseGrafPortPenLoc,
                                                  kATSUUseGrafPortPenLoc))
                        {
                            /* woot. */
                            atsuok = true;
                        }
                        ATSUDisposeTextLayout(lyo);
                    }
                }
                ATSUDisposeStyle(sty);
            }
            if (atsuok != false)
            {
                maccurses_move(maccurses_y, maccurses_x + wcw);
                return OK;
            }
        }
#endif /* defined(UNICODE) */
        MoveTo(maccurses_x * maccurses_widMax + (wcw * maccurses_widMax - StringWidth(si)) / 2,
               maccurses_y * (maccurses_fontinfo.ascent + maccurses_fontinfo.descent) + maccurses_fontinfo.ascent);
        if ((substituted == true)
            ||
            usefake
            ||
            (maccurses_widMax < 2)
            ||
            ((maccurses_fontinfo.ascent + maccurses_fontinfo.descent) < 2))
        {
            int x0, x1, x2, y0, y1, y2;
            maccurses_chtype chacs;

            if ((maccurses_widMax < 2)
                ||
                ((maccurses_fontinfo.ascent + maccurses_fontinfo.descent) < 2))
            {
                switch (ch)
                {
                case '\"':
                    ch = _MACCURSES_ACS_THALF;
                    break;
                case ',':
                    ch = _MACCURSES_ACS_BHALF;
                    break;
                case 'o':
                    ch = _MACCURSES_ACS_PUCE;
                    break;
                case '.':
                    ch = _MACCURSES_ACS_MIDDOT;
                    break;
                case '+':
                    ch = ACS_PLUS;
                    break;
                case '-':
                    ch = ACS_HLINE;
                    break;
                case '|':
                    ch = ACS_VLINE;
                    break;
                }
            }
            x0 = maccurses_x * maccurses_widMax;
            x2 = x0 + wcw * maccurses_widMax;
            x1 = (x0 + x2) / 2;
            y0 = maccurses_y * (maccurses_fontinfo.ascent + maccurses_fontinfo.descent);
            y2 = y0 + maccurses_fontinfo.ascent + maccurses_fontinfo.descent;
            y1 = (y0 + y2) / 2;
            chacs = ((ch & _MACCURSES_A_ALTCHARSET)
                     ?
                     ch
                     :
                     (ch | _MACCURSES_A_ALTCHARSET));
            switch (chacs)
            {
            case (' ' | _MACCURSES_A_ALTCHARSET):
                break;
            case ACS_LRCORNER:
            case ACS_URCORNER:
            case ACS_ULCORNER:
            case ACS_LLCORNER:
            case ACS_PLUS:
            case ACS_LTEE:
            case ACS_RTEE:
            case ACS_BTEE:
            case ACS_TTEE:
            case ACS_HLINE:
            case ACS_VLINE:
                if (1)
                {
                    int u, d, l, r;

                    u = ((chacs == ACS_LRCORNER)
                         ||
                         (chacs == ACS_LLCORNER)
                         ||
                         (chacs == ACS_PLUS)
                         ||
                         (chacs == ACS_BTEE)
                         ||
                         (chacs == ACS_LTEE)
                         ||
                         (chacs == ACS_RTEE)
                         ||
                         (chacs == ACS_VLINE));
                    d = ((chacs == ACS_URCORNER)
                         ||
                         (chacs == ACS_ULCORNER)
                         ||
                         (chacs == ACS_PLUS)
                         ||
                         (chacs == ACS_TTEE)
                         ||
                         (chacs == ACS_LTEE)
                         ||
                         (chacs == ACS_RTEE)
                         ||
                         (chacs == ACS_VLINE));
                    l = ((chacs == ACS_LRCORNER)
                         ||
                         (chacs == ACS_URCORNER)
                         ||
                         (chacs == ACS_PLUS)
                         ||
                         (chacs == ACS_TTEE)
                         ||
                         (chacs == ACS_BTEE)
                         ||
                         (chacs == ACS_RTEE)
                         ||
                         (chacs == ACS_HLINE));
                    r = ((chacs == ACS_LLCORNER)
                         ||
                         (chacs == ACS_ULCORNER)
                         ||
                         (chacs == ACS_PLUS)
                         ||
                         (chacs == ACS_TTEE)
                         ||
                         (chacs == ACS_BTEE)
                         ||
                         (chacs == ACS_LTEE)
                         ||
                         (chacs == ACS_HLINE));
                    if (u)
                    {
                        MoveTo(x1, y0);
                        LineTo(x1, y1);
                    }
                    if (d)
                    {
                        MoveTo(x1, y1);
                        LineTo(x1, y2 - 1);
                    }
                    if (l)
                    {
                        MoveTo(x0, y1);
                        LineTo(x1, y1);
                    }
                    if (r)
                    {
                        MoveTo(x1, y1);
                        LineTo(x2 - 1, y1);
                    }
                }
                break;
            case _MACCURSES_ACS_MIDDOT:
                if (1)
                {
                    int bullet_radius;

                    bullet_radius = (x1 - x0) / 2;
                    if (((y1 - y0) / 2) < bullet_radius)
                    {
                        bullet_radius = (y1 - y0) / 2;
                    }
                    if (bullet_radius > 1) bullet_radius = 1;
                    if (bullet_radius)
                    {
                        SetRect(&cell, x1 - bullet_radius, y1 - bullet_radius, x1 + bullet_radius, y1 + bullet_radius);
                    }
                    else
                    {
                        SetRect(&cell, x1, y1, x1 + 1, y1 + 1);
                    }
                    PaintOval(&cell);
                }
                break;
            case _MACCURSES_ACS_PUCE:
            case _MACCURSES_ACS_BLACKSQUARE:
                if (1)
                {
                    int puce_radius;

                    puce_radius = x1 - x0;
                    if ((y1 - y0) < puce_radius)
                    {
                        puce_radius = y1 - y0;
                    }
                    if (puce_radius)
                    {
                        SetRect(&cell, x1 - puce_radius, y1 - puce_radius, x1 + puce_radius, y1 + puce_radius);
                    }
                    else
                    {
                        SetRect(&cell, x1, y1, x1 + 1, y1 + 1);
                    }
                    PaintOval(&cell);
                }
                break;
            case _MACCURSES_ACS_THALF:
            case _MACCURSES_ACS_BHALF:
                if (y2 == (y0 + 1))
                {
                    if ((attr & _MACCURSES_A_COLOR) && maccurses_has_colors())
                    {
                        RGBForeColor(maccurses_palette + COLORS + fg + COLORS * bg);
                        RGBBackColor(maccurses_palette + COLORS + fg + COLORS * bg);
                    }
                    else if (maccurses_has_colors())
                    {
                        RGBForeColor(maccurses_palette + COLORS + COLORS * COLORS);
                        RGBBackColor(maccurses_palette + COLORS + COLORS * COLORS);
                    }
                    EraseRect(&cell);
                }
                if (chacs == _MACCURSES_ACS_THALF)
                {
                    SetRect(&cell, x0, y0, x2, y1 + (y0 == y1));
                }
                else if (chacs == _MACCURSES_ACS_BHALF)
                {
                    SetRect(&cell, x0, y1, x2, y2);
                }
                PaintRect(&cell);
                break;
            case _MACCURSES_ACS_LHALF:
            case _MACCURSES_ACS_RHALF:
                if (x2 == (x0 + 1))
                {
                    if ((attr & _MACCURSES_A_COLOR) && maccurses_has_colors())
                    {
                        RGBForeColor(maccurses_palette + COLORS + fg + COLORS * bg);
                        RGBBackColor(maccurses_palette + COLORS + fg + COLORS * bg);
                    }
                    else if (maccurses_has_colors())
                    {
                        RGBForeColor(maccurses_palette + COLORS + COLORS * COLORS);
                        RGBBackColor(maccurses_palette + COLORS + COLORS * COLORS);
                    }
                    EraseRect(&cell);
                }
                if (chacs == _MACCURSES_ACS_LHALF)
                {
                    SetRect(&cell, x0, y0, x1 + (x0 == x1), y2);
                }
                else if (chacs == _MACCURSES_ACS_RHALF)
                {
                    SetRect(&cell, x1, y0, x2, y2);
                }
                PaintRect(&cell);
                break;
            case ACS_BLOCK:
                if (! substituted)
                {
                    PaintRect(&cell);
                }
                break;
            default:
                if ((maccurses_widMax < 2)
                    ||
                    ((maccurses_fontinfo.ascent + maccurses_fontinfo.descent) < 2))
                {
                    if ((ch != ' ')
                        &&
                        (si[1] != ' '))
                    {
                        PaintRect(&cell);
                    }
                }
                else
                {
                    DrawString(si);
                }
            }
        }
        else
        {
            DrawString(si);
        }
    }
    maccurses_move(maccurses_y, maccurses_x + wcw);
    return OK;
}

static int maccurses_addstr(const char *s)
{
    while (*s)
    {
        maccurses_addch((maccurses_chtype) *s);
        s ++;
    }
    return OK;
}

static int maccurses_mvprintw(int y, int x, const char *s)
{
    maccurses_move(y, x);
    maccurses_addstr(s);
    return OK;
}

static int maccurses_clrtoeol(void)
{
    int ox, oy;
    int x;

    maccurses_getyx(&oy, &ox);
    for (x = ox; x < maccurses_w; x ++)
    {
        maccurses_move(oy, x);
        maccurses_addch(' ');
    }
    maccurses_move(oy, ox);
    return OK;
}

static int maccurses_clrtobot(void)
{
    int ox, oy;
    int y;

    maccurses_clrtoeol();
    maccurses_getyx(&oy, &ox);
    for (y = oy + 1; y < maccurses_h; y ++)
    {
        maccurses_move(y, 0);
        maccurses_clrtoeol();
    }
    maccurses_move(oy, ox);
    return OK;
}

static int maccurses_erase(void)
{
    Rect coords;

    maccurses_clrtoeol();
    GetWindowBounds(maccurses_hwnd,
                    kWindowContentRgn,
                    &coords);
    coords.right -= coords.left;
    coords.left -= coords.left;
    coords.bottom -= coords.top;
    coords.top -= coords.top;
    EraseRect(&coords);
    return OK;
}

static int maccurses_clear(void)
{
    return maccurses_erase();
}

static int maccurses_attrset(maccurses_attr_t a)
{
    maccurses_attr = a;
    return OK;
}

static int maccurses_insch(maccurses_chtype ch)
{
    return ERR;
}

static int maccurses_init_color(short i, short r, short g, short b)
{
    int j;

    if (! maccurses_can_change_color())
    {
        return ERR;
    }
    if ((i < 0)
        ||
        (i >= COLORS))
    {
        return ERR;
    }
    if (r < 0) r = 0;
    if (g < 0) g = 0;
    if (b < 0) b = 0;
    if (r > 1000) r = 1000;
    if (g > 1000) g = 1000;
    if (b > 1000) b = 1000;
    maccurses_palette[i].red = (unsigned short) (65535UL * (unsigned long) r / 1000UL);
    maccurses_palette[i].green = (unsigned short) (65535UL * (unsigned long) g / 1000UL);
    maccurses_palette[i].blue = (unsigned short) (65535UL * (unsigned long) b / 1000UL);
    for (j = 0; j < COLORS; j ++)
    {
        maccurses_palette[COLORS + (i * COLORS) + j].red = maccurses_mix(maccurses_palette[i].red, maccurses_palette[j].red);
        maccurses_palette[COLORS + (i * COLORS) + j].green = maccurses_mix(maccurses_palette[i].green, maccurses_palette[j].green);
        maccurses_palette[COLORS + (i * COLORS) + j].blue = maccurses_mix(maccurses_palette[i].blue, maccurses_palette[j].blue);
        maccurses_palette[COLORS + (j * COLORS) + i].red = maccurses_mix(maccurses_palette[i].red, maccurses_palette[j].red);
        maccurses_palette[COLORS + (j * COLORS) + i].green = maccurses_mix(maccurses_palette[i].green, maccurses_palette[j].green);
        maccurses_palette[COLORS + (j * COLORS) + i].blue = maccurses_mix(maccurses_palette[i].blue, maccurses_palette[j].blue);
    }
    return OK;
}

static int maccurses_color_content(short i, short *rp, short *gp, short *bp)
{
    if (! maccurses_can_change_color())
    {
        return ERR;
    }
    if ((i < 0)
        ||
        (i >= COLORS))
    {
        return ERR;
    }
    *rp = (short) (1000UL * (unsigned long) maccurses_palette[i].red / 65535UL);
    *gp = (short) (1000UL * (unsigned long) maccurses_palette[i].green / 65535UL);
    *bp = (short) (1000UL * (unsigned long) maccurses_palette[i].blue / 65535UL);
    return OK;
}

static int maccurses_start_color(void)
{
    return maccurses_has_colors() ? OK : ERR;
}

#undef has_colors
#define has_colors() (maccurses_has_colors() ? TRUE : FALSE)

#undef start_color
#define start_color() maccurses_start_color()

#undef can_change_color
#define can_change_color() (maccurses_can_change_color() ? TRUE : FALSE)

#undef color_content
#define color_content(i,rp,gp,bp) maccurses_color_content(i,rp,gp,bp)

#undef init_pair
#define init_pair(p,f,b) maccurses_init_pair(p,f,b)

#undef pair_content
#define pair_content(p,fp,bp) maccurses_pair_content(p,fp,bp)

#undef clear
#define clear() maccurses_clear()

#undef clrtobot
#define clrtobot() maccurses_clrtobot()

#undef clrtoeol
#define clrtoeol() maccurses_clrtoeol()

#undef erase
#define erase() maccurses_erase()

#undef attrset
#define attrset(a) maccurses_attrset(a)

#undef standout
#define standout() maccurses_attrset(A_STANDOUT)

#undef standend
#define standend() maccurses_attrset(0)

#undef resizeterm
#define resizeterm(h,w) maccurses_resizeterm(h,w)

#undef use_env
#define use_env(flag)

#undef keypad
#define keypad(stdscr,flag)

#undef addstr
#define addstr(s) maccurses_addstr(s)

#undef addch
#define addch(ch) maccurses_addch(ch)

#undef insch
#define insch(ch) maccurses_insch(ch)

#undef move
#define move(y,x) maccurses_move(y,x)

#undef wcwidth
#define wcwidth(wc) maccurses_wcwidth(wc)

#undef getyx
#define getyx(stdscr,y,x) maccurses_getyx(&(y),&(x))

#undef clearok
#define clearok(stdscr,flag) maccurses_clearok(flag)

#undef nodelay
#define nodelay(stdscr,flag) maccurses_nodelay(flag)

#undef intrflush
#define intrflush(stdscr,flag) maccurses_intrflush(flag)

#undef idlok
#define idlok(stdscr,flag) maccurses_idlok(flag)

#undef leaveok
#define leaveok(stdscr,flag) maccurses_leaveok(flag)

#undef wrefresh
#define wrefresh(stdscr) maccurses_refresh()

#undef refresh
#define refresh() maccurses_refresh()

#undef cbreak
#define cbreak()

#undef noecho
#define noecho()

#undef echo
#define echo()

#undef nonl
#define nonl()

#undef curs_set
#define curs_set(vis)

#undef beep
#define beep() maccurses_beep()

#undef getch
#define getch() maccurses_getch()

#undef mvprintw
#define mvprintw(y,x,s) maccurses_mvprintw(y,x,s)

#undef init_color
#define init_color(i,r,g,b) maccurses_init_color(i,r,g,b)

#define INITSCR_WITH_HINTS 1

#undef initscrWithHints
#define initscrWithHints maccurses_initscrWithHints

#undef endwin
#define endwin() maccurses_endwin()

#undef chtype
#define chtype maccurses_chtype

#undef attr_t
#define attr_t maccurses_attr_t

#undef COLS
#define COLS maccurses_w

#undef LINES
#define LINES maccurses_h

#ifndef USE_IOCTL
#define USE_IOCTL 0
#endif

#ifndef USE_WCWIDTH
#define USE_WCWIDTH 1
#endif

#ifndef USE_ICONV
#define USE_ICONV 0
#endif

#ifndef USE_SIGWINCH
#define USE_SIGWINCH 0
#endif

#ifndef USE_RAW_UCS
#ifdef UNICODE
#define USE_RAW_UCS 1
#else
#define USE_RAW_UCS 0
#endif
#endif

#endif /* ! defined(MACCURS_H_INC) */
