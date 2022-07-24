/*
Copyright 2011,2012,2013 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "quantum.h"


/*         ,-----------------------------------------------.
 *         |F13|F14|F15|F16|F17|F18|F19|F20|F21|F22|F23|F24|
 * ,---.   |-----------------------------------------------|     ,-----------.     ,-----------.
 * |Esc|   |F1 |F2 |F3 |F4 |F5 |F6 |F7 |F8 |F9 |F10|F11|F12|     |PrS|ScL|Pau|     |VDn|VUp|Mut|
 * `---'   `-----------------------------------------------'     `-----------'     `-----------'
 * ,-----------------------------------------------------------. ,-----------. ,---------------.
 * |  `|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|JPY|Bsp| |Ins|Hom|PgU| |NmL|  /|  *|  -|
 * |-----------------------------------------------------------| |-----------| |---------------|
 * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|  \  | |Del|End|PgD| |  7|  8|  9|  +|
 * |-----------------------------------------------------------| `-----------' |---------------|
 * |CapsL |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '| ^a|Entr|               |  4|  5|  6|KP,|
 * |-----------------------------------------------------------|     ,---.     |---------------|
 * |Shft|  <|  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /| RO|Shift |     |Up |     |  1|  2|  3|Ent|
 * |-----------------------------------------------------------| ,-----------. |---------------|
 * |Ctl|Gui|Alt|MHEN|     Space      |HENK|KANA|Alt|Gui|App|Ctl| |Lef|Dow|Rig| |  #|  0|  .|KP=|
 * `-----------------------------------------------------------' `-----------' `---------------'
 *
 * PS/2 scan codes
 * http://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/translate.pdf
 *         ,-----------------------------------------------.
 *         | 08| 10| 18| 20| 28| 30| 38| 40| 48| 50| 57| 5F|
 * ,---.   |-----------------------------------------------|     ,-----------.     ,-----------.
 * | 76|   | 05| 06| 04| 0C| 03| 0B| 83| 0A| 01| 09| 78| 07|     | FC| 7E| FE|     | A1| B2| A3|
 * `---'   `-----------------------------------------------'     `-----------'     `-----------'
 * ,-----------------------------------------------------------. ,-----------. ,---------------.
 * | 0E| 16| 1E| 26| 25| 2E| 36| 3D| 3E| 46| 45| 4E| 55| 6A| 66| | F0| EC| FD| | 77| CA| 7C| 7B|
 * |-----------------------------------------------------------| |-----------| |---------------|
 * | 0D  | 15| 1D| 24| 2D| 2C| 35| 3C| 43| 44| 4D| 54| 5B|  5D | | F1| E9| FA| | 6C| 75| 7D| 79|
 * |-----------------------------------------------------------| `-----------' |---------------|
 * | 58   | 1C| 1B| 23| 2B| 34| 33| 3B| 42| 4B| 4C| 52| ^a| 5A |               | 6B| 73| 74| 6D|
 * |-----------------------------------------------------------|     ,---.     |---------------|
 * | 12 | 61| 1A| 22| 21| 2A| 32| 31| 3A| 41| 49| 4A| 51|  59  |     | F5|     | 69| 72| 7A| DA|
 * |-----------------------------------------------------------| ,-----------. |---------------|
 * | 14| 9F| 11| 67 |     29         | 64 | 13 | 91| A7| AF| 94| | EB| F2| F4| | 68|70 | 71| 63|
 * `-----------------------------------------------------------' `-----------' `---------------'
 * ^a ISO hash key uses identical scancode 5D to US backslash.
 * 51, 63, 68, 6D: hidden keys in IBM model M
 */
/* All keys */
#define KEYMAP_ALL( \
        K08,K10,K18,K20,K28,K30,K38,K40,K48,K50,K57,K5F,                                        \
    K76,K05,K06,K04,K0C,K03,K0B,K83,K0A,K01,K09,K78,K07,         KFC,K7E,KFE,      KA1,KB2,KA3, \
    K0E,K16,K1E,K26,K25,K2E,K36,K3D,K3E,K46,K45,K4E,K55,K6A,K66, KF0,KEC,KFD,  K77,KCA,K7C,K7B, \
    K0D,K15,K1D,K24,K2D,K2C,K35,K3C,K43,K44,K4D,K54,K5B,    K5D, KF1,KE9,KFA,  K6C,K75,K7D,K79, \
    K58,K1C,K1B,K23,K2B,K34,K33,K3B,K42,K4B,K4C,K52,        K5A,               K6B,K73,K74,K6D, \
    K12,K61,K1A,K22,K21,K2A,K32,K31,K3A,K41,K49,K4A,    K51,K59,     KF5,      K69,K72,K7A,KDA, \
    K14,K9F,K11,K67,    K29,            K64,K13,K91,KA7,KAF,K94, KEB,KF2,KF4,  K68,K70,K71,K63, \
                                                                                            \
    KB7, KBF, KDE,           /* System Power, Sleep, Wake */                                \
    KCD, K95, KBB, KB4, KD0, /* Next, Previous, Stop, Pause, Media Select */                \
    KC8, KAB, KC0,           /* Mail, Calculator, My Computer */                            \
    K90, KBA, KB8, KB0,      /* WWW Search, Home, Back, Forward */                          \
    KA8, KA0, K98            /* WWW Stop, Refresh, Favorites */                             \
) { \
    { KC_NO,    KC_##K01, KC_NO,    KC_##K03, KC_##K04, KC_##K05, KC_##K06, KC_##K07 }, \
    { KC_##K08, KC_##K09, KC_##K0A, KC_##K0B, KC_##K0C, KC_##K0D, KC_##K0E, KC_NO    }, \
    { KC_##K10, KC_##K11, KC_##K12, KC_##K13, KC_##K14, KC_##K15, KC_##K16, KC_NO    }, \
    { KC_##K18, KC_NO,    KC_##K1A, KC_##K1B, KC_##K1C, KC_##K1D, KC_##K1E, KC_NO    }, \
    { KC_##K20, KC_##K21, KC_##K22, KC_##K23, KC_##K24, KC_##K25, KC_##K26, KC_NO    }, \
    { KC_##K28, KC_##K29, KC_##K2A, KC_##K2B, KC_##K2C, KC_##K2D, KC_##K2E, KC_NO    }, \
    { KC_##K30, KC_##K31, KC_##K32, KC_##K33, KC_##K34, KC_##K35, KC_##K36, KC_NO    }, \
    { KC_##K38, KC_NO,    KC_##K3A, KC_##K3B, KC_##K3C, KC_##K3D, KC_##K3E, KC_NO    }, \
    { KC_##K40, KC_##K41, KC_##K42, KC_##K43, KC_##K44, KC_##K45, KC_##K46, KC_NO    }, \
    { KC_##K48, KC_##K49, KC_##K4A, KC_##K4B, KC_##K4C, KC_##K4D, KC_##K4E, KC_NO    }, \
    { KC_##K50, KC_##K51, KC_##K52, KC_NO,    KC_##K54, KC_##K55, KC_NO,    KC_##K57 }, \
    { KC_##K58, KC_##K59, KC_##K5A, KC_##K5B, KC_NO,    KC_##K5D, KC_NO,    KC_##K5F }, \
    { KC_NO,    KC_##K61, KC_NO,    KC_##K63, KC_##K64, KC_NO,    KC_##K66, KC_##K67 }, \
    { KC_##K68, KC_##K69, KC_##K6A, KC_##K6B, KC_##K6C, KC_##K6D, KC_NO,    KC_NO    }, \
    { KC_##K70, KC_##K71, KC_##K72, KC_##K73, KC_##K74, KC_##K75, KC_##K76, KC_##K77 }, \
    { KC_##K78, KC_##K79, KC_##K7A, KC_##K7B, KC_##K7C, KC_##K7D, KC_##K7E, KC_NO    }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_##K83, KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_##K90, KC_##K91, KC_NO,    KC_NO,    KC_##K94, KC_##K95, KC_NO,    KC_NO    }, \
    { KC_##K98, KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_##K9F }, \
    { KC_##KA0, KC_##KA1, KC_NO,    KC_##KA3, KC_NO,    KC_NO,    KC_NO,    KC_##KA7 }, \
    { KC_##KA8, KC_NO,    KC_NO,    KC_##KAB, KC_NO,    KC_NO,    KC_NO,    KC_##KAF }, \
    { KC_##KB0, KC_NO,    KC_##KB2, KC_NO,    KC_##KB4, KC_NO,    KC_NO,    KC_##KB7 }, \
    { KC_##KB8, KC_NO,    KC_##KBA, KC_##KBB, KC_NO,    KC_NO,    KC_NO,    KC_##KBF }, \
    { KC_##KC0, KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_##KC8, KC_NO,    KC_##KCA, KC_NO,    KC_NO,    KC_##KCD, KC_NO,    KC_NO    }, \
    { KC_##KD0, KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_NO,    KC_##KDA, KC_NO,    KC_NO,    KC_NO,    KC_##KDE, KC_NO    }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_##KE9, KC_NO,    KC_##KEB, KC_##KEC, KC_NO,    KC_NO,    KC_NO    }, \
    { KC_##KF0, KC_##KF1, KC_##KF2, KC_NO,    KC_##KF4, KC_##KF5, KC_NO,    KC_NO    }, \
    { KC_NO,    KC_NO,    KC_##KFA, KC_NO,    KC_##KFC, KC_##KFD, KC_##KFE, KC_NO    }, \
}

#define KEYMAP_FULL( \
        K08,K10,K18,K20,K28,K30,K38,K40,K48,K50,K57,K5F,                                        \
    K76,K05,K06,K04,K0C,K03,K0B,K83,K0A,K01,K09,K78,K07,         KFC,K7E,KFE,      KA1,KB2,KA3, \
    K0E,K16,K1E,K26,K25,K2E,K36,K3D,K3E,K46,K45,K4E,K55,K6A,K66, KF0,KEC,KFD,  K77,KCA,K7C,K7B, \
    K0D,K15,K1D,K24,K2D,K2C,K35,K3C,K43,K44,K4D,K54,K5B,    K5D, KF1,KE9,KFA,  K6C,K75,K7D,K79, \
    K58,K1C,K1B,K23,K2B,K34,K33,K3B,K42,K4B,K4C,K52,        K5A,               K6B,K73,K74,K6D, \
    K12,K61,K1A,K22,K21,K2A,K32,K31,K3A,K41,K49,K4A,    K51,K59,     KF5,      K69,K72,K7A,KDA, \
    K14,K9F,K11,K67,    K29,            K64,K13,K91,KA7,KAF,K94, KEB,KF2,KF4,  K68,K70,K71,K63  \
) \
KEYMAP_ALL( \
        K08,K10,K18,K20,K28,K30,K38,K40,K48,K50,K57,K5F,                                        \
    K76,K05,K06,K04,K0C,K03,K0B,K83,K0A,K01,K09,K78,K07,         KFC,K7E,KFE,      KA1,KB2,KA3, \
    K0E,K16,K1E,K26,K25,K2E,K36,K3D,K3E,K46,K45,K4E,K55,K6A,K66, KF0,KEC,KFD,  K77,KCA,K7C,K7B, \
    K0D,K15,K1D,K24,K2D,K2C,K35,K3C,K43,K44,K4D,K54,K5B,    K5D, KF1,KE9,KFA,  K6C,K75,K7D,K79, \
    K58,K1C,K1B,K23,K2B,K34,K33,K3B,K42,K4B,K4C,K52,        K5A,               K6B,K73,K74,K6D, \
    K12,K61,K1A,K22,K21,K2A,K32,K31,K3A,K41,K49,K4A,    K51,K59,     KF5,      K69,K72,K7A,KDA, \
    K14,K9F,K11,K67,    K29,            K64,K13,K91,KA7,KAF,K94, KEB,KF2,KF4,  K68,K70,K71,K63, \
                                                                                                \
    SYSTEM_POWER, SYSTEM_SLEEP, SYSTEM_WAKE,                                                    \
    MEDIA_NEXT_TRACK, MEDIA_PREV_TRACK, MEDIA_STOP, MEDIA_PLAY_PAUSE, MEDIA_SELECT,             \
    MAIL, CALCULATOR, MY_COMPUTER,                                                              \
    WWW_SEARCH, WWW_HOME, WWW_BACK, WWW_FORWARD,                                                \
    WWW_STOP, WWW_REFRESH, WWW_FAVORITES                                                        \
)

/* US layout */
#define KEYMAP( \
    K76,K05,K06,K04,K0C,K03,K0B,K83,K0A,K01,K09,K78,K07,     KFC,K7E,KFE,                   \
    K0E,K16,K1E,K26,K25,K2E,K36,K3D,K3E,K46,K45,K4E,K55,K66, KF0,KEC,KFD,  K77,KCA,K7C,K7B, \
    K0D,K15,K1D,K24,K2D,K2C,K35,K3C,K43,K44,K4D,K54,K5B,K5D, KF1,KE9,KFA,  K6C,K75,K7D,     \
    K58,K1C,K1B,K23,K2B,K34,K33,K3B,K42,K4B,K4C,K52,    K5A,               K6B,K73,K74,K79, \
    K12,K1A,K22,K21,K2A,K32,K31,K3A,K41,K49,K4A,        K59,     KF5,      K69,K72,K7A,     \
    K14,K9F,K11,        K29,                K91,KA7,KAF,K94, KEB,KF2,KF4,  K70,    K71,KDA  \
) \
KEYMAP_FULL( \
        F13,F14,F15,F16,F17,F18,F19,F20,F21,F22,F23,F24,                                        \
    K76,K05,K06,K04,K0C,K03,K0B,K83,K0A,K01,K09,K78,K07,         KFC,K7E,KFE,  VOLD,VOLU,MUTE,  \
    K0E,K16,K1E,K26,K25,K2E,K36,K3D,K3E,K46,K45,K4E,K55,JPY,K66, KF0,KEC,KFD,  K77,KCA,K7C,K7B, \
    K0D,K15,K1D,K24,K2D,K2C,K35,K3C,K43,K44,K4D,K54,K5B,    K5D, KF1,KE9,KFA,  K6C,K75,K7D,K79, \
    K58,K1C,K1B,K23,K2B,K34,K33,K3B,K42,K4B,K4C,K52,        K5A,               K6B,K73,K74,PCMM,\
    K12,NUBS,K1A,K22,K21,K2A,K32,K31,K3A,K41,K49,K4A,   RO, K59,     KF5,      K69,K72,K7A,KDA, \
    K14,K9F,K11,MHEN,   K29,          HENK,KANA,K91,KA7,KAF,K94, KEB,KF2,KF4,  PWR,K70,K71,PEQL \
)

/*
 * IBM Terminal keyboard 1399625, 101-key
 */
#define LAYOUT_101( \
    k08,      k07, k0F, k17, k1F, k27, k2F, k37, k3F, k47, k4F, k56, k5E,  k57, k5F, k62, \
\
    k0E, k16, k1E, k26, k25, k2E, k36, k3D, k3E, k46, k45, k4E, k55, k66,  k67, k6E, k6F,  k76, k77, k7E, k84, \
    k0D, k15, k1D, k24, k2D, k2C, k35, k3C, k43, k44, k4D, k54, k5B, k5C,  k64, k65, k6D,  k6C, k75, k7D, \
    k14, k1C, k1B, k23, k2B, k34, k33, k3B, k42, k4B, k4C, k52,      k5A,                  k6B, k73, k74, k7C, \
    k12,      k1A, k22, k21, k2A, k32, k31, k3A, k41, k49, k4A,      k59,       k63,       k69, k72, k7A, \
    k11,      k19,                k29,                     k39,      k58,  k61, k60, k6A,  k70,      k71, k79 \
) { \
    { XXX, XXX, XXX, XXX, XXX, XXX, XXX, k07 }, \
    { k08, XXX, XXX, XXX, XXX, k0D, k0E, k0F }, \
    { XXX, k11, k12, XXX, k14, k15, k16, k17 }, \
    { XXX, k19, k1A, k1B, k1C, k1D, k1E, k1F }, \
    { XXX, k21, k22, k23, k24, k25, k26, k27 }, \
    { XXX, k29, k2A, k2B, k2C, k2D, k2E, k2F }, \
    { XXX, k31, k32, k33, k34, k35, k36, k37 }, \
    { XXX, k39, k3A, k3B, k3C, k3D, k3E, k3F }, \
    { XXX, k41, k42, k43, k44, k45, k46, k47 }, \
    { XXX, k49, k4A, k4B, k4C, k4D, k4E, k4F }, \
    { XXX, XXX, k52, XXX, k54, k55, k56, k57 }, \
    { k58, k59, k5A, k5B, k5C, XXX, k5E, k5F }, \
    { k60, k61, k62, k63, k64, k65, k66, k67 }, \
    { XXX, k69, k6A, k6B, k6C, k6D, k6E, k6F }, \
    { k70, k71, k72, k73, k74, k75, k76, k77 }, \
    { XXX, k79, k7A, XXX, k7C, k7D, k7E, XXX }, \
    { XXX, XXX, XXX, XXX, k84, XXX, XXX, XXX } \
}
