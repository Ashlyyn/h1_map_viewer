#include <stddef.h>
#include <limits.h>

#define countof(a) (sizeof((a)) / sizeof(*(a))) 

typedef struct str_s {
    const char* __data;
    size_t      __len;
} str_t;

typedef struct string_s {
    char*  __data;
    size_t __len;
} string_t;

#define A_NPOS SIZE_MAX

str_t A_str_Literal(const char* s, size_t c);
str_t A_str_String(string_t s);

#define A_str(s) (_Generic((s), \
    const char*: A_str_Literal(s, countof(s)), \
    str_t:       A_str_Str(s), \
    string_t:    A_str_String(s) \
))

string_t A_string_Empty(void);
string_t A_string_Str(str_t s);
string_t A_string_String(string_t s);
string_t A_string_SizeT(size_t c) {

#define A_string(s) (_Generic((s), \
    void:     A_str_Empty(), \
    str_t:    A_string_Str(s), \
    string_t: A_string_String(s) \
    size_t:   A_string_SizeT(s) \
))

const char* A_cstr_Str(str_t s);
const char* A_cstr_String(string_t s);

#define A_cstr(s) (_Generic((s), \
    str_t:    A_cstr_Str(s), \
    string_t: A_cstr_String(s), \
))

size_t A_strlen_Str(str_t s):
size_t A_strlen_String(string_t s);

#define A_strlen(s) (_Generic((s), \
    str_t:    A_strlen_Str(s), \
    string_t: A_strlen_String(s), \
))

bool A_strcpyz_Str(string_t dest, size_t dest_off, str_t src, size_t src_off);
bool A_strcpyz_String(
    string_t dest, size_t dest_off, string_t src, size_t src_off
);

#define A_strcpyz(d, do, s, so) (_Generic((s), \
    str_t:    A_strcpyz_Str(d, do, s, so), \
    string_t: A_strcpyz_String(d, do, s, so), \
))

string_t A_strdup_Str(str_t s);
string_t A_strdup_String(string_t s);

#define A_strdup(s) (_Generic((s), \
    str_t:    A_strdup_Str(s), \
    string_t: A_strdup_String(s), \
))

bool A_strext(string_t s, size_t n);

bool A_strcat_Str(string_t dest, str_t src);
bool A_strcat_String(string_t dest, string_t src);

#define A_strcat(d, s) (_Generic((s), \
    str_t:    A_strdup_Str(d, s), \
    string_t: A_strdup_String(d, s), \
))

size_t A_strchr_Str(str_t s, char c);
size_t A_strchr_String(string_t s, char c);

#define A_strchr(s, c) (_Generic((s), \
    str_t:    A_strchr_Str(s, c), \
    string_t: A_strchr_String(s, c), \
))

size_t A_strrchr_Str(str_t s, char c);
size_t A_strrchr_String(string_t s, char c);

#define A_strrchr(s, c) (_Generic((s), \
    str_t:    A_strrchr_Str(s, c), \
    string_t: A_strrchr_String(s, c), \
))

bool A_streq_Str_Str(str_t a, str_t b);
bool A_streq_Str_String(str_t a, string_t b);
bool A_streq_String_Str(string_t a, str_t b);
bool A_streq_String_String(string_t a, string_t b);

#define A_streq_Str(a, b) (_Generic((b), \
    str_t:    A_streq_Str_Str(a, b), \
    string_t: A_streq_Str_String(a, b), \
))

#define A_streq_String(a, b) (_Generic((b), \
    str_t:    A_streq_String_Str(a, b), \
    string_t: A_streq_String_String(a, b), \
))

#define A_streq(a, b) (_Generic((a), \
    str_t:    A_streq_Str(a, b), \
    string_t: A_streq_String(a, b), \
))

bool A_strpbrk_Str_Str(str_t a, str_t b);
bool A_strpbrk_Str_String(str_t a, string_t b);
bool A_strpbrk_String_Str(string_t a, str_t b);
bool A_strpbrk_String_String(string_t a, string_t b);

#define A_strpbrk_Str(a, b) (_Generic((b), \
    str_t:    A_strpbrk_Str_Str(a, b), \
    string_t: A_strpbrk_Str_String(a, b), \
))

#define A_strpbrk_String(a, b) (_Generic((b), \
    str_t:    A_strpbrk_String_Str(a, b), \
    string_t: A_strpbrk_String_String(a, b), \
))

#define A_strpbrk(a, b) (_Generic((a), \
    str_t:    A_strpbrk_Str(a, b), \
    string_t: A_strpbrk_String(a, b), \
))

bool A_strstr_Str_Str(str_t a, str_t b);
bool A_strstr_Str_String(str_t a, string_t b);
bool A_strstr_String_Str(string_t a, str_t b);
bool A_strstr_String_String(string_t a, string_t b);

#define A_strstr_Str(a, b) (_Generic((b), \
    str_t:    A_strstr_Str_Str(a, b), \
    string_t: A_strstr_Str_String(a, b), \
))

#define A_strstr_String(a, b) (_Generic((b), \
    str_t:    A_strstr_String_Str(a, b), \
    string_t: A_strstr_String_String(a, b), \
))

#define A_strstr(a, b) (_Generic((a), \
    str_t:    A_strstr_Str(a, b), \
    string_t: A_strstr_String(a, b), \
))
