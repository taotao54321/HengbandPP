﻿/*!
 * @brief enumで表現することが却って高コストになりそうなSV定義をここに格納する
 * @date 2020/05/28
 * @author Hourier
 */

#pragma once

/*** Object "sval" codes ***/
#define SV_ANY 255
#define SV_PHOTO 50

/* The "sval" codes for TV_CORPSE */
#define SV_SKELETON 0
#define SV_CORPSE 1

/* The "sval" codes for TV_SHOT/TV_ARROW/TV_BOLT */
#define SV_AMMO_LIGHT 0  /* pebbles */
#define SV_AMMO_NORMAL 1 /* shots, arrows, bolts */

#define SV_ROD_MIN_DIRECTION 12 /*!< この値以降の小項目IDを持ったロッドは使用時にターゲットを要求する / Special "sval" limit -- first "aimed" rod */
#define SV_CHEST_KANDUME 50     /*!< 箱アイテムの小項目ID: おもちゃのカンヅメ */

/* The "sval" codes for TV_FLASK */
#define SV_FLASK_OIL 0
