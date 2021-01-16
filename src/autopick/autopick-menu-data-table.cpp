﻿/*!
 * @brief 自動拾いのユーティリティ
 * @date 2020/04/25
 * @author Hourier
 */

#include "autopick/autopick-menu-data-table.h"
#include "autopick/autopick-commands-table.h"
#include "autopick/autopick-editor-table.h"
#include "autopick/autopick-keys-table.h"
#include "util/int-char-converter.h"

command_menu_type menu_data[MENU_DATA_NUM] = {
    { MN_HELP, 0, -1, EC_HELP },
    { MN_QUIT, 0, KTRL('q'), EC_QUIT },
    { MN_SAVEQUIT, 0, KTRL('w'), EC_SAVEQUIT },
    { MN_REVERT, 0, KTRL('z'), EC_REVERT },

    { MN_EDIT, 0, -1, -1 },
    { MN_CUT, 1, KTRL('x'), EC_CUT },
    { MN_COPY, 1, KTRL('c'), EC_COPY },
    { MN_PASTE, 1, KTRL('v'), EC_PASTE },
    { MN_BLOCK, 1, KTRL('g'), EC_BLOCK },
    { MN_KILL_LINE, 1, KTRL('k'), EC_KILL_LINE },
    { MN_DELETE_CHAR, 1, KTRL('d'), EC_DELETE_CHAR },
    { MN_BACKSPACE, 1, KTRL('h'), EC_BACKSPACE },
    { MN_RETURN, 1, KTRL('j'), EC_RETURN },
    { MN_RETURN, 1, KTRL('m'), EC_RETURN },

    { MN_SEARCH, 0, -1, -1 },
    { MN_SEARCH_STR, 1, KTRL('s'), EC_SEARCH_STR },
    { MN_SEARCH_FORW, 1, -1, EC_SEARCH_FORW },
    { MN_SEARCH_BACK, 1, KTRL('r'), EC_SEARCH_BACK },
    { MN_SEARCH_OBJ, 1, KTRL('y'), EC_SEARCH_OBJ },
    { MN_SEARCH_DESTROYED, 1, -1, EC_SEARCH_DESTROYED },

    { MN_MOVE, 0, -1, -1 },
    { MN_LEFT, 1, KTRL('b'), EC_LEFT },
    { MN_DOWN, 1, KTRL('n'), EC_DOWN },
    { MN_UP, 1, KTRL('p'), EC_UP },
    { MN_RIGHT, 1, KTRL('f'), EC_RIGHT },
    { MN_BOL, 1, KTRL('a'), EC_BOL },
    { MN_EOL, 1, KTRL('e'), EC_EOL },
    { MN_PGUP, 1, KTRL('o'), EC_PGUP },
    { MN_PGDOWN, 1, KTRL('l'), EC_PGDOWN },
    { MN_TOP, 1, KTRL('t'), EC_TOP },
    { MN_BOTTOM, 1, KTRL('u'), EC_BOTTOM },

    { MN_INSERT, 0, -1, -1 },
    { MN_INSERT_OBJECT, 1, KTRL('i'), EC_INSERT_OBJECT },
    { MN_INSERT_DESTROYED, 1, -1, EC_INSERT_DESTROYED },
    { MN_INSERT_BLOCK, 1, -1, EC_INSERT_BLOCK },
    { MN_INSERT_MACRO, 1, -1, EC_INSERT_MACRO },
    { MN_INSERT_KEYMAP, 1, -1, EC_INSERT_KEYMAP },

    { MN_ADJECTIVE_GEN, 0, -1, -1 },
    { KEY_UNAWARE, 1, -1, EC_IK_UNAWARE },
    { KEY_UNIDENTIFIED, 1, -1, EC_IK_UNIDENTIFIED },
    { KEY_IDENTIFIED, 1, -1, EC_IK_IDENTIFIED },
    { KEY_STAR_IDENTIFIED, 1, -1, EC_IK_STAR_IDENTIFIED },
    { KEY_COLLECTING, 1, -1, EC_OK_COLLECTING },
    { KEY_ARTIFACT, 1, -1, EC_OK_ARTIFACT },
    { KEY_EGO, 1, -1, EC_OK_EGO },
    { KEY_GOOD, 1, -1, EC_OK_GOOD },
    { KEY_NAMELESS, 1, -1, EC_OK_NAMELESS },
    { KEY_AVERAGE, 1, -1, EC_OK_AVERAGE },
    { KEY_WORTHLESS, 1, -1, EC_OK_WORTHLESS },
    { MN_RARE, 1, -1, EC_OK_RARE },
    { MN_COMMON, 1, -1, EC_OK_COMMON },

    { MN_ADJECTIVE_SPECIAL, 0, -1, -1 },
    { MN_BOOSTED, 1, -1, EC_OK_BOOSTED },
    { MN_MORE_DICE, 1, -1, EC_OK_MORE_DICE },
    { MN_MORE_BONUS, 1, -1, EC_OK_MORE_BONUS },
    { MN_WANTED, 1, -1, EC_OK_WANTED },
    { MN_UNIQUE, 1, -1, EC_OK_UNIQUE },
    { MN_HUMAN, 1, -1, EC_OK_HUMAN },
    { MN_UNREADABLE, 1, -1, EC_OK_UNREADABLE },
    { MN_REALM1, 1, -1, EC_OK_REALM1 },
    { MN_REALM2, 1, -1, EC_OK_REALM2 },
    { MN_FIRST, 1, -1, EC_OK_FIRST },
    { MN_SECOND, 1, -1, EC_OK_SECOND },
    { MN_THIRD, 1, -1, EC_OK_THIRD },
    { MN_FOURTH, 1, -1, EC_OK_FOURTH },

    { MN_NOUN, 0, -1, -1 },
    { KEY_WEAPONS, 1, -1, EC_KK_WEAPONS },
    { KEY_FAVORITE_WEAPONS, 1, -1, EC_KK_FAVORITE_WEAPONS },
    { KEY_ARMORS, 1, -1, EC_KK_ARMORS },
    { KEY_MISSILES, 1, -1, EC_KK_MISSILES },
    { KEY_DEVICES, 1, -1, EC_KK_DEVICES },
    { KEY_LIGHTS, 1, -1, EC_KK_LIGHTS },
    { KEY_JUNKS, 1, -1, EC_KK_JUNKS },
    { KEY_CORPSES, 1, -1, EC_KK_CORPSES },
    { KEY_SPELLBOOKS, 1, -1, EC_KK_SPELLBOOKS },
    { KEY_SHIELDS, 1, -1, EC_KK_SHIELDS },
    { KEY_BOWS, 1, -1, EC_KK_BOWS },
    { KEY_RINGS, 1, -1, EC_KK_RINGS },
    { KEY_AMULETS, 1, -1, EC_KK_AMULETS },
    { KEY_SUITS, 1, -1, EC_KK_SUITS },
    { KEY_CLOAKS, 1, -1, EC_KK_CLOAKS },
    { KEY_HELMS, 1, -1, EC_KK_HELMS },
    { KEY_GLOVES, 1, -1, EC_KK_GLOVES },
    { KEY_BOOTS, 1, -1, EC_KK_BOOTS },

    { MN_COMMAND_LETTER, 0, -1, -1 },
    { MN_CL_AUTOPICK, 1, -1, EC_CL_AUTOPICK },
    { MN_CL_DESTROY, 1, -1, EC_CL_DESTROY },
    { MN_CL_LEAVE, 1, -1, EC_CL_LEAVE },
    { MN_CL_QUERY, 1, -1, EC_CL_QUERY },
    { MN_CL_NO_DISP, 1, -1, EC_CL_NO_DISP },

    { MN_DELETE_CHAR, -1, 0x7F, EC_DELETE_CHAR },

    { NULL, -1, -1, 0 }
};
