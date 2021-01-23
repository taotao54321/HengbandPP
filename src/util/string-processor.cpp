#include "util/string-processor.h"
#include "util/int-char-converter.h"

/*!
 * 10進数から16進数への変換テーブル /
 * Global array for converting numbers to uppercase hecidecimal digit
 * This array can also be used to convert a number to an octal digit
 */
const char hexsym[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

int max_macrotrigger = 0;                         /*!< 現在登録中のマクロ(トリガー)の数 */
concptr macro_template = NULL;                    /*!< Angband設定ファイルのT: タグ情報から読み込んだ長いTコードを処理するために利用する文字列ポインタ */
concptr macro_modifier_chr;                       /*!< &x# で指定されるマクロトリガーに関する情報を記録する文字列ポインタ */
concptr macro_modifier_name[MAX_MACRO_MOD];       /*!< マクロ上で取り扱う特殊キーを文字列上で表現するためのフォーマットを記録した文字列ポインタ配列 */
concptr macro_trigger_name[MAX_MACRO_TRIG];       /*!< マクロのトリガーコード */
concptr macro_trigger_keycode[2][MAX_MACRO_TRIG]; /*!< マクロの内容 */

/*
 * Convert a decimal to a single digit octal number
 */
static char octify(uint i) { return (hexsym[i % 8]); }

/*
 * Convert a decimal to a single digit hex number
 */
static char hexify(uint i) { return (hexsym[i % 16]); }

/*
 * Convert a octal-digit into a decimal
 */
static int deoct(char c) {
    if (isdigit(c))
        return (D2I(c));
    return 0;
}

/*
 * Convert a hexidecimal-digit into a decimal
 */
static int dehex(char c) {
    if (isdigit(c))
        return (D2I(c));
    if (islower(c))
        return (A2I(c) + 10);
    if (isupper(c))
        return (A2I(tolower(c)) + 10);
    return 0;
}

static char force_upper(char a) { return (islower(a)) ? toupper(a) : a; }

static int angband_stricmp(concptr a, concptr b) {
    for (concptr s1 = a, s2 = b; TRUE; s1++, s2++) {
        char z1 = force_upper(*s1);
        char z2 = force_upper(*s2);
        if (z1 < z2)
            return -1;
        if (z1 > z2)
            return 1;
        if (!z1)
            return 0;
    }
}

static int angband_strnicmp(concptr a, concptr b, int n) {
    for (concptr s1 = a, s2 = b; n > 0; s1++, s2++, n--) {
        char z1 = force_upper(*s1);
        char z2 = force_upper(*s2);
        if (z1 < z2)
            return -1;
        if (z1 > z2)
            return 1;
        if (!z1)
            return 0;
    }

    return 0;
}

// text_to_ascii() 下請け
// "\[F1]" などの処理
static void trigger_text_to_ascii(char** bufptr, concptr* strptr) {
    char* s = *bufptr;
    concptr str = *strptr;
    bool mod_status[MAX_MACRO_MOD];

    int i, len = 0;
    int shiftstatus = 0;
    concptr key_code;

    // マクロテンプレートがなければ無視
    if (macro_template == NULL)
        return;

    // 修飾キーフラグ初期化
    for (i = 0; macro_modifier_chr[i]; i++)
        mod_status[i] = FALSE;

    // '[' を読み飛ばす
    str++;

    /* Examine modifier keys */
    while (TRUE) {
        // 修飾キー文字列 ("shift-" など) と一致するか調べる(case insensitive)
        for (i = 0; macro_modifier_chr[i]; i++) {
            len = strlen(macro_modifier_name[i]);

            if (!angband_strnicmp(str, macro_modifier_name[i], len))
                break; // 一致した
        }
        // 一致がなければ次の処理へ
        if (!macro_modifier_chr[i])
            break;

        // 修飾キー文字列部分の次まで読み進め、修飾キーフラグを立てる
        str += len;
        mod_status[i] = TRUE;

        // Shift キーの場合、shift フラグを立てる
        if ('S' == macro_modifier_chr[i])
            shiftstatus = 1;
    }

    // 一致するマクロトリガーを探す
    // 例えば "F1]" なら F1 に一致
    for (i = 0; i < max_macrotrigger; i++) {
        len = strlen(macro_trigger_name[i]);
        if (!angband_strnicmp(str, macro_trigger_name[i], len) && ']' == str[len]) {
            break;
        }
    }

    // 一致するマクロトリガーが見つからなかった場合
    if (i == max_macrotrigger) {
        str = angband_strchr(str, ']');
        if (str) {
            // 後続の ']' がある場合
            // マクロトリガー開始文字 '\x31' と終端文字 '\r' を書き込む
            // "[...]" の中身は読み飛ばす (次は ']' から読む)
            *s++ = (char)31;
            *s++ = '\r';
            *bufptr = s;
            *strptr = str; /* where **strptr == ']' */
        }
        // 後続の ']' がない場合、単に無視する

        return;
    }

    // 一致するマクロトリガーが見つかった

    // 対応するキーコードを取得 (pref 内の "T:<trigger>:<keycode>:<shift-keycode>")
    key_code = macro_trigger_keycode[shiftstatus][i];

    // ']' まで読み進める
    str += len;

    // マクロトリガー開始文字 '\x31' を書き込む
    *s++ = (char)31;

    // マクロテンプレート展開
    for (i = 0; macro_template[i]; i++) {
        char ch = macro_template[i];
        switch (ch) {
        case '&':
            // '&' は修飾キーたちに置換
            for (int j = 0; macro_modifier_chr[j]; j++) {
                if (mod_status[j])
                    *s++ = macro_modifier_chr[j];
            }
            break;
        case '#':
            // '#' はキーコードに置換
            strcpy(s, key_code);
            s += strlen(key_code);
            break;
        default:
            // その他の文字はそのまま
            *s++ = ch;
            break;
        }
    }

    // マクロトリガー終端文字 '\r' を書き込む
    *s++ = '\r';

    *bufptr = s;
    *strptr = str; /* where **strptr == ']' */
    return;
}

/*
 * Hack -- convert a printable string into real ascii
 *
 * I have no clue if this function correctly handles, for example,
 * parsing "\xFF" into a (signed) char.  Whoever thought of making
 * the "sign" of a "char" undefined is a complete moron.  Oh well.
 *
 * buf: 変換先
 * str: 変換元
 */
void text_to_ascii(char* buf, concptr str) {
    char* s = buf;
    while (*str) {
        if (*str == '\\') {
            // バックスラッシュで始まる文字列の処理
            str++;

            // バックスラッシュの後に何もなければ無視
            if (!(*str))
                break;

            if (*str == '[') {
                // "\[F1]" などの処理
                trigger_text_to_ascii(&s, &str);
            }
            else {
                if (*str == 'x') {
                    // "\xFF" などの処理 (16進)
                    *s = 16 * (char)dehex(*++str);
                    *s++ += (char)dehex(*++str);
                }
                else if (*str == '\\') {
                    // "\\" は単一のバックスラッシュ
                    *s++ = '\\';
                }
                else if (*str == '^') {
                    // "\^" は "^" (生の "^" はコントロール文字を表すためエスケープが必要)
                    *s++ = '^';
                }
                else if (*str == 's') {
                    // "\s" はスペース
                    *s++ = ' ';
                }
                else if (*str == 'e') {
                    // "\e" はエスケープ
                    *s++ = ESCAPE;
                }
                else if (*str == 'b') {
                    // "\b" はバックスペース
                    *s++ = '\b';
                }
                else if (*str == 'n') {
                    *s++ = '\n';
                }
                else if (*str == 'r') {
                    *s++ = '\r';
                }
                else if (*str == 't') {
                    *s++ = '\t';
                }
                // ここから "\377" などの処理 (8進)
                else if (*str == '0') {
                    *s = 8 * (char)deoct(*++str);
                    *s++ += (char)deoct(*++str);
                }
                else if (*str == '1') {
                    *s = 64 + 8 * (char)deoct(*++str);
                    *s++ += (char)deoct(*++str);
                }
                else if (*str == '2') {
                    *s = 64 * 2 + 8 * (char)deoct(*++str);
                    *s++ += (char)deoct(*++str);
                }
                else if (*str == '3') {
                    *s = 64 * 3 + 8 * (char)deoct(*++str);
                    *s++ += (char)deoct(*++str);
                }
                // ここまで8進の処理
            }

            str++;
        }
        else if (*str == '^') {
            // "^a" などの処理 (コントロール文字)
            // 下位 5bit のみを取り出す
            str++;
            *s++ = (*str++ & 037);
        }
        else {
            *s++ = *str++;
        }
    }

    *s = '\0';
}

static bool trigger_ascii_to_text(char** bufptr, concptr* strptr) {
    char* s = *bufptr;
    concptr str = *strptr;
    char key_code[100];
    int i;
    if (macro_template == NULL)
        return FALSE;

    *s++ = '\\';
    *s++ = '[';

    concptr tmp;
    for (i = 0; macro_template[i]; i++) {
        char ch = macro_template[i];

        switch (ch) {
        case '&':
            while ((tmp = angband_strchr(macro_modifier_chr, *str)) != 0) {
                int j = (int)(tmp - macro_modifier_chr);
                tmp = macro_modifier_name[j];
                while (*tmp)
                    *s++ = *tmp++;
                str++;
            }

            break;
        case '#': {
            int j;
            for (j = 0; *str && *str != '\r'; j++)
                key_code[j] = *str++;
            key_code[j] = '\0';
            break;
        }
        default:
            if (ch != *str)
                return FALSE;
            str++;
        }
    }

    if (*str++ != '\r')
        return FALSE;

    for (i = 0; i < max_macrotrigger; i++) {
        if (!angband_stricmp(key_code, macro_trigger_keycode[0][i]) || !angband_stricmp(key_code, macro_trigger_keycode[1][i]))
            break;
    }

    if (i == max_macrotrigger)
        return FALSE;

    tmp = macro_trigger_name[i];
    while (*tmp)
        *s++ = *tmp++;

    *s++ = ']';

    *bufptr = s;
    *strptr = str;
    return TRUE;
}

/*
 * Hack -- convert a string into a printable form
 */
void ascii_to_text(char* buf, concptr str) {
    char* s = buf;
    while (*str) {
        byte i = (byte)(*str++);
        if (i == 31) {
            if (!trigger_ascii_to_text(&s, &str)) {
                *s++ = '^';
                *s++ = '_';
            }
        }
        else {
            if (i == ESCAPE) {
                *s++ = '\\';
                *s++ = 'e';
            }
            else if (i == ' ') {
                *s++ = '\\';
                *s++ = 's';
            }
            else if (i == '\b') {
                *s++ = '\\';
                *s++ = 'b';
            }
            else if (i == '\t') {
                *s++ = '\\';
                *s++ = 't';
            }
            else if (i == '\n') {
                *s++ = '\\';
                *s++ = 'n';
            }
            else if (i == '\r') {
                *s++ = '\\';
                *s++ = 'r';
            }
            else if (i == '^') {
                *s++ = '\\';
                *s++ = '^';
            }
            else if (i == '\\') {
                *s++ = '\\';
                *s++ = '\\';
            }
            else if (i < 32) {
                *s++ = '^';
                *s++ = i + 64;
            }
            else if (i < 127) {
                *s++ = i;
            }
            else if (i < 64) {
                *s++ = '\\';
                *s++ = '0';
                *s++ = octify(i / 8);
                *s++ = octify(i % 8);
            }
            else {
                *s++ = '\\';
                *s++ = 'x';
                *s++ = hexify(i / 16);
                *s++ = hexify(i % 16);
            }
        }
    }

    *s = '\0';
}

/*
 * The angband_strcpy() function copies up to 'bufsize'-1 characters from 'src'
 * to 'buf' and NUL-terminates the result.  The 'buf' and 'src' strings may
 * not overlap.
 *
 * angband_strcpy() returns strlen(src).  This makes checking for truncation
 * easy.  Example: if (angband_strcpy(buf, src, sizeof(buf)) >= sizeof(buf)) ...;
 *
 * This function should be equivalent to the strlcpy() function in BSD.
 */
size_t angband_strcpy(char* buf, concptr src, size_t bufsize) {
#ifdef JP
    char* d = buf;
    concptr s = src;
    size_t len = 0;

    if (bufsize > 0) {
        /* reserve for NUL termination */
        bufsize--;

        /* Copy as many bytes as will fit */
        while (*s && (len < bufsize)) {
            if (iskanji(*s)) {
                if (len + 1 >= bufsize || !*(s + 1))
                    break;
                *d++ = *s++;
                *d++ = *s++;
                len += 2;
            }
            else {
                *d++ = *s++;
                len++;
            }
        }
        *d = '\0';
    }

    while (*s++)
        len++;
    return len;

#else
    size_t len = strlen(src);
    size_t ret = len;
    if (bufsize == 0)
        return ret;

    if (len >= bufsize)
        len = bufsize - 1;

    (void)memcpy(buf, src, len);
    buf[len] = '\0';
    return ret;
#endif
}

/*
 * The angband_strcat() tries to append a string to an existing NUL-terminated string.
 * It never writes more characters into the buffer than indicated by 'bufsize' and
 * NUL-terminates the buffer.  The 'buf' and 'src' strings may not overlap.
 *
 * angband_strcat() returns strlen(buf) + strlen(src).  This makes checking for
 * truncation easy.  Example:
 * if (angband_strcat(buf, src, sizeof(buf)) >= sizeof(buf)) ...;
 *
 * This function should be equivalent to the strlcat() function in BSD.
 */
size_t angband_strcat(char* buf, concptr src, size_t bufsize) {
    size_t dlen = strlen(buf);
    if (dlen < bufsize - 1) {
        return (dlen + angband_strcpy(buf + dlen, src, bufsize - dlen));
    }
    else {
        return (dlen + strlen(src));
    }
}

/*
 * A copy of ANSI strstr()
 *
 * angband_strstr() can handle Kanji strings correctly.
 */
char* angband_strstr(concptr haystack, concptr needle) {
    int l1 = strlen(haystack);
    int l2 = strlen(needle);

    if (l1 >= l2) {
        for (int i = 0; i <= l1 - l2; i++) {
            if (!strncmp(haystack + i, needle, l2))
                return (char*)haystack + i;

#ifdef JP
            if (iskanji(*(haystack + i)))
                i++;
#endif
        }
    }

    return NULL;
}

/*
 * A copy of ANSI strchr()
 *
 * angband_strchr() can handle Kanji strings correctly.
 */
char* angband_strchr(concptr ptr, char ch) {
    for (; *ptr != '\0'; ptr++) {
        if (*ptr == ch)
            return (char*)ptr;

#ifdef JP
        if (iskanji(*ptr))
            ptr++;
#endif
    }

    return NULL;
}
