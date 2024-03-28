#include "acommon.h"

#include <stdlib.h>
#include <string.h>

int A_memcmp(const void* a, const void* b, size_t n) {
    return memcmp(a, b, n);
}

void* A_memcpy(void* dest, const void* src, size_t n) {
    return memcpy(dest, src, n);
}

void* A_memchr(const void* p, int c, size_t n) {
    return memchr(p, c, n);
}

void* A_memrchr(const void* p, int c, size_t n) {
    for(int i = n; i > 0; i--) {
        if(((char*)p)[i] == (char)c)
            return (void*)((const char*)p + i);
    }

    return NULL;
}

str_t A_str_Literal(const char* s, size_t c) {
    str_t n;
    n.__data = s;
    n.__len  = c;
    return n;
}

str_t A_str_String(string_t s) {
    str_t n;
    n.__data = s.__data;
    n.__len  = s.__len;
    return n;
}

string_t A_string_Str(str_t s) {
    return A_strdup(s);
}

string_t A_string_String(string_t s) {
    return A_strdup(s);
}

string_t A_string_SizeT(size_t c) {
    string_t n;
    n.__data = malloc(c + 1);
    if(n.__data != NULL) {
        n.__len = c;
        n.__data[c] = '\0';
    } else {
        n.__len = 0;
    }
    return n;   
}

const char* A_cstr_Str(str_t s) {
    return s.__data;
}

const char* A_cstr_String(string_t s) {
    return (const char*)s.__data;
}

size_t A_strlen_Str(str_t s) {
    return s.__len;
}

size_t A_strlen_String(string_t s) {
    return s.__len;
}

char A_strat_Str(str_t s, size_t i) {
    return A_cstr(s)[i];
}

char A_strat_String(string_t s, size_t i) {
    return A_cstr(s)[i];
}

bool A_strcpyz_Str(string_t dest, size_t dest_off, str_t src, size_t src_off) {
    size_t c = A_strlen(src) - src_off;

    if(dest_off + c > A_strlen(dest))
        return false;

    memcpy(dest.__data + dest_off, src.__data + src_off, c);

    dest.__data[A_strlen(dest)] = '\0';

    return true;
}

bool A_strcpyz_String(
    string_t dest, size_t dest_off, string_t src, size_t src_off
) {
    size_t c = A_strlen(src) - src_off;

    if(dest_off + c > A_strlen(dest))
        return false;

    memcpy(dest.__data + dest_off, src.__data + src_off, c);

    dest.__data[A_strlen(dest)] = '\0';

    return true;
}

string_t A_strdup_Str(str_t s) {
    string_t n = A_string(A_strlen(s));
    A_strcpyz(n, 0, s, 0);
    return n;
}

string_t A_strdup_String(string_t s) {
    string_t n = A_string(A_strlen(s));
    A_strcpyz(n, 0, s, 0);
    return n;
}

bool A_strext(string_t s, size_t n) {
    s.__data = realloc(s.__data, A_strlen(s) + 1 + n);
    if(s.__data != NULL) {
        s.__len += n;
        return true;
    } else {
        s.__len = 0;
        return false;
    }
}

bool A_strcat_Str(string_t dest, str_t src) {
    size_t off = A_strlen(dest);

    if(!A_strext(dest, A_strlen(src)))
        return false;

    A_strcpyz(dest, off, src, 0);
    return true;
}

bool A_strcat_String(string_t dest, string_t src) {
    size_t off = A_strlen(dest);

    if(!A_strext(dest, A_strlen(src)))
        return false;

    A_strcpyz(dest, off, src, 0);
    return true;
}

size_t A_strchr_Str(str_t s, char c) {
    const char* p = (const char*)A_memchr(A_cstr(s), (int)c, A_strlen(s));
    if(!p)
        return A_NPOS;
    return (size_t)(p - A_cstr(s));
}

size_t A_strchr_String(string_t s, char c) {
    const char* p = (const char*)A_memchr(A_cstr(s), (int)c, A_strlen(s));
    if(!p)
        return A_NPOS;
    return (size_t)(p - A_cstr(s));
}

size_t A_strrchr_Str(str_t s, char c) {
    const char* p = (const char*)A_memrchr(A_cstr(s), (int)c, A_strlen(s));
    if(!p)
        return A_NPOS;
    return (size_t)(p - A_cstr(s));
}

size_t A_strrchr_String(string_t s, char c) {
    const char* p = (const char*)A_memrchr(A_cstr(s), (int)c, A_strlen(s));
    if(!p)
        return A_NPOS;
    return (size_t)(p - A_cstr(s));
}

bool A_strcont_Str(str_t s, char c) {
    return A_strchr(s, c) != A_NPOS;
}

bool A_strcont_String(string_t s, char c) {
    return A_strchr(s, c) != A_NPOS;
}

bool A_streq_Str_Str(str_t a, str_t b) {
    return A_cstr(a) == A_cstr(b);
}

bool A_streq_Str_String(str_t a, string_t b) {
    return A_cstr(a) == A_cstr(b);
}

bool A_streq_String_Str(string_t a, str_t b) {
    return A_cstr(a) == A_cstr(b);
}

bool A_streq_String_String(string_t a, string_t b) {
    return A_cstr(a) == A_cstr(b);
}

size_t A_strpbrk_Str_Str(str_t a, str_t b) {
    for(int i = 0; i < A_strlen(a); i++) {
        for(int j = 0; j < A_strlen(b); j++) {
            if(A_strat(a, i) == A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strpbrk_Str_String(str_t a, string_t b) {
    for(int i = 0; i < A_strlen(a); i++) {
        for(int j = 0; j < A_strlen(b); j++) {
            if(A_strat(a, i) == A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strpbrk_String_Str(string_t a, str_t b) {
    for(int i = 0; i < A_strlen(a); i++) {
        for(int j = 0; j < A_strlen(b); j++) {
            if(A_strat(a, i) == A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strpbrk_String_String(string_t a, string_t b) {
    for(int i = 0; i < A_strlen(a); i++) {
        for(int j = 0; j < A_strlen(b); j++) {
            if(A_strat(a, i) == A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strrpbrk_Str_Str(str_t a, str_t b) {
    for(int i = A_strlen(a); i > 0; i--) {
        for(int j = A_strlen(b); j > 0; j--) {
            if(A_strat(a, i) == A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strrpbrk_Str_String(str_t a, string_t b) {
    for(int i = A_strlen(a); i > 0; i--) {
        for(int j = A_strlen(b); j > 0; j--) {
            if(A_strat(a, i) == A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strrpbrk_String_Str(string_t a, str_t b) {
    for(int i = A_strlen(a); i > 0; i--) {
        for(int j = A_strlen(b); j > 0; j--) {
            if(A_strat(a, i) == A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strrpbrk_String_String(string_t a, string_t b) {
    for(int i = A_strlen(a); i > 0; i--) {
        for(int j = A_strlen(b); j > 0; j--) {
            if(A_strat(a, i) == A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strpcnt_Str_Str(str_t a, str_t b) {
    for(int i = 0; i < A_strlen(a); i++) {
        for(int j = 0; j < A_strlen(b); j++) {
            if(A_strat(a, i) != A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strpcnt_Str_String(str_t a, string_t b) {
    for(int i = 0; i < A_strlen(a); i++) {
        for(int j = 0; j < A_strlen(b); j++) {
            if(A_strat(a, i) != A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strpcnt_String_Str(string_t a, str_t b) {
    for(int i = 0; i < A_strlen(a); i++) {
        for(int j = 0; j < A_strlen(b); j++) {
            if(A_strat(a, i) != A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strpcnt_String_String(string_t a, string_t b) {
    for(int i = 0; i < A_strlen(a); i++) {
        for(int j = 0; j < A_strlen(b); j++) {
            if(A_strat(a, i) != A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strrpcnt_Str_Str(str_t a, str_t b) {
    for(int i = A_strlen(a); i > 0; i--) {
        for(int j = A_strlen(b); j > 0; j--) {
            if(A_strat(a, i) != A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strrpcnt_Str_String(str_t a, string_t b) {
    for(int i = A_strlen(a); i > 0; i--) {
        for(int j = A_strlen(b); j > 0; j--) {
            if(A_strat(a, i) != A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strrpcnt_String_Str(string_t a, str_t b) {
    for(int i = A_strlen(a); i > 0; i--) {
        for(int j = A_strlen(b); j > 0; j--) {
            if(A_strat(a, i) != A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strrpcnt_String_String(string_t a, string_t b) {
    for(int i = A_strlen(a); i > 0; i--) {
        for(int j = A_strlen(b); j > 0; j--) {
            if(A_strat(a, i) != A_strat(b, j))
                return (size_t)i;
        }
    }

    return A_NPOS;
}

size_t A_strstr_Str_Str(str_t a, str_t b) {
    if(A_strlen(b) > A_strlen(a))
        return A_NPOS;

    if(A_strlen(b) == A_strlen(b))
        return A_streq(a, b);

    char c = A_strat(b, 0);
    for(int i = 0; i < A_strlen(a) - A_strlen(b); i++) {
        if(A_strat(a, i) == c) {
            if(A_memcmp(A_cstr(a), A_cstr(b), A_strlen(b)) == 0)
                return i;
        }
    }

    return A_NPOS;
}

size_t A_strstr_Str_String(str_t a, string_t b) {
    if(A_strlen(b) > A_strlen(a))
        return A_NPOS;

    if(A_strlen(b) == A_strlen(b))
        return A_streq(a, b);

    char c = A_strat(b, 0);
    for(int i = 0; i < A_strlen(a) - A_strlen(b); i++) {
        if(A_strat(a, i) == c) {
            if(A_memcmp(A_cstr(a), A_cstr(b), A_strlen(b)) == 0)
                return i;
        }
    }

    return A_NPOS;
}

size_t A_strstr_String_Str(string_t a, str_t b) {
    if(A_strlen(b) > A_strlen(a))
        return A_NPOS;

    if(A_strlen(b) == A_strlen(b))
        return A_streq(a, b);

    char c = A_strat(b, 0);
    for(int i = 0; i < A_strlen(a) - A_strlen(b); i++) {
        if(A_strat(a, i) == c) {
            if(A_memcmp(A_cstr(a), A_cstr(b), A_strlen(b)) == 0)
                return i;
        }
    }

    return A_NPOS;
}

size_t A_strstr_String_String(string_t a, string_t b) {
    if(A_strlen(b) > A_strlen(a))
        return A_NPOS;

    if(A_strlen(b) == A_strlen(b))
        return A_streq(a, b);

    char c = A_strat(b, 0);
    for(int i = 0; i < A_strlen(a) - A_strlen(b); i++) {
        if(A_strat(a, i) == c) {
            if(A_memcmp(A_cstr(a), A_cstr(b), A_strlen(b)) == 0)
                return i;
        }
    }

    return A_NPOS;
}

size_t A_strrstr_Str_Str(str_t a, str_t b) {
    if(A_strlen(b) > A_strlen(a))
        return A_NPOS;

    if(A_strlen(b) == A_strlen(b))
        return A_streq(a, b);

    char c = A_strat(b, 0);
    for(int i = A_strlen(a); i > A_strlen(a) - A_strlen(b); i--) {
        if(A_strat(a, i) == c) {
            if(A_memcmp(A_cstr(a), A_cstr(b), A_strlen(b)) == 0)
                return i;
        }
    }

    return A_NPOS;
}

size_t A_strrstr_Str_String(str_t a, string_t b) {
    if(A_strlen(b) > A_strlen(a))
        return A_NPOS;

    if(A_strlen(b) == A_strlen(b))
        return A_streq(a, b);

    char c = A_strat(b, 0);
    for(int i = A_strlen(a); i > A_strlen(a) - A_strlen(b); i--) {
        if(A_strat(a, i) == c) {
            if(A_memcmp(A_cstr(a), A_cstr(b), A_strlen(b)) == 0)
                return i;
        }
    }

    return A_NPOS;
}

size_t A_strrstr_String_Str(string_t a, str_t b) {
    if(A_strlen(b) > A_strlen(a))
        return A_NPOS;

    if(A_strlen(b) == A_strlen(b))
        return A_streq(a, b);

    char c = A_strat(b, 0);
    for(int i = A_strlen(a); i > A_strlen(a) - A_strlen(b); i--) {
        if(A_strat(a, i) == c) {
            if(A_memcmp(A_cstr(a), A_cstr(b), A_strlen(b)) == 0)
                return i;
        }
    }

    return A_NPOS;
}

size_t A_strrstr_String_String(string_t a, string_t b) {
    if(A_strlen(b) > A_strlen(a))
        return A_NPOS;

    if(A_strlen(b) == A_strlen(b))
        return A_streq(a, b);

    char c = A_strat(b, 0);
    for(int i = A_strlen(a); i > A_strlen(a) - A_strlen(b); i--) {
        if(A_strat(a, i) == c) {
            if(A_memcmp(A_cstr(a), A_cstr(b), A_strlen(b)) == 0)
                return i;
        }
    }

    return A_NPOS;
}

str_t A_substr_Str(str_t s, size_t i) {
    str_t n;

    if(i >= A_strlen(s)) {
        n.__data = NULL;
        n.__len  = 0;
        return n;
    }

    n.__data = s.__data + i;
    n.__len  = s.__len  - i;    
    return n;
}

str_t A_substr_String(string_t s, size_t i) {
    str_t n;

    if(i >= A_strlen(s)) {
        n.__data = NULL;
        n.__len  = 0;
        return n;
    }

    n.__data = s.__data + i;
    n.__len  = s.__len  - i;    
    return n;
}

string_t A_substring_Str(str_t s, size_t i) {
    string_t n;

    if(i >= A_strlen(s)) {
        n.__data = NULL;
        n.__len  = 0;
        return n;
    }

    size_t len = A_strlen(s) - i;
    A_strext(n, len);
    A_strcpyz(n, 0, s, i);
    return n;
}

string_t A_substring_String(string_t s, size_t i) {
    string_t n;

    if(i >= A_strlen(s)) {
        n.__data = NULL;
        n.__len  = 0;
        return n;
    }

    size_t len = A_strlen(s) - i;
    A_strext(n, len);
    A_strcpyz(n, 0, s, i);
    return n;
}
