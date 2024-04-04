#pragma once

#include <array>
#include <charconv>
#include <deque>
#include <format>
#include <vector>

#include <glm/glm.hpp>

#include "acommon/acommon.h"

#include "com_defs.hpp"

enum print_msg_dest_t {
    CON_DEST_OUT,
    CON_DEST_ERR,
    CON_DEST_FATAL_ERR,
    CON_DEST_INFO,
    CON_DEST_DEBUG_INFO,
};

void inline Com_PrintMessage(
    print_msg_dest_t dest, std::string_view msg
) {
    // Since we can't guarantee a std::string_view's valid range is 
    // null-terminated, we need a format string that takes the a string of 
    // exactly msg.size().
    char fmt[8];
    snprintf(fmt, sizeof(fmt), "%%%zus", msg.size());

    if (dest == CON_DEST_ERR || dest == CON_DEST_FATAL_ERR)
        fprintf(stderr, fmt, msg.data());
    else if (dest == CON_DEST_DEBUG_INFO && _DEBUG)
        printf(fmt, msg.data());
    else
        printf(fmt, msg.data());
}

template<typename ...Args>
std::string inline Com_Format(std::string_view fmt, Args&&... args) {
    return std::vformat(fmt, std::make_format_args(args...));
}

bool inline Com_Split(std::string_view s, OUT std::deque<std::string>& v) {
    v = std::deque<std::string>(); 

    if (s.empty())
        return false;

    size_t pos_start = 0, pos_end;
    std::string token;

    while ((pos_end = s.find(' ', pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + 1;
        if ((token.find_first_not_of(" \n") != std::string::npos) 
            && !token.empty()
        ) {
            v.push_back(token);
        }
    }

    token = s.substr(pos_start);
    if ((token.find_first_not_of(" \n") != std::string::npos) && !token.empty())
        v.push_back(token);

    return true;
}

bool inline Com_Parse(std::string_view s, OUT bool& value) {
    value = false;

    int i = 0;
    auto r = std::from_chars(s.data(), s.data() + s.size(), i);
    if (r.ec == std::errc()) {
        value = i != 0;
        return true;
    } else if (r.ec == std::errc::invalid_argument) {
        str_t n = A_str_Literal(s.data(), s.length());
        if (A_streq(A_tolower(n), A_literal("true"))) {
            value = true;
            return true;
        } else if (A_streq(A_tolower(n), A_literal("false"))) {
            value = false;
            return true;
        }
    }
    return false;
}

bool inline Com_Parse(std::string_view s, OUT int& value) {
    value = 0;

    auto r = std::from_chars(s.data(), s.data() + s.size(), value);
    if (r.ec == std::errc()) {
        return true;
    }

    return false;
}

bool inline Com_Parse(std::string_view s, OUT float& value) {
    value = (float)atof(std::string(s).data());
    return true;
}

bool inline Com_Parse(
    const std::array<std::string_view, 2>& v, OUT glm::vec2& value
) {
    value = glm::vec2(0.0f, 0.0f);

    std::string_view xs = std::string_view(v[0]);
    float x = (float)atof(std::string(xs).data());

    std::string_view ys = std::string_view(v[1]);
    float y = (float)atof(std::string(ys).data());

    value = glm::vec2(x, y);

    return true;
}

bool inline Com_Parse(std::string_view s, OUT glm::vec2& value) {
    std::deque<std::string> v;
    Com_Split(s, v);

    if (v.size() < 2)
        return false;

    return Com_Parse(std::array<std::string_view, 2> { v[0], v[1] }, value);
}

bool inline Com_Parse(
    const std::array<std::string_view, 3>& v, OUT glm::vec3& value
) {
    value = glm::vec3(0.0f, 0.0f, 0.0f);

    std::string_view xs = std::string_view(v[0]);
    float x = (float)atof(std::string(xs).data());
    
    std::string_view ys = std::string_view(v[1]);
    float y = (float)atof(std::string(ys).data());
    
    std::string_view zs = std::string_view(v[2]);
    float z = (float)atof(std::string(zs).data());

    value = glm::vec3(x, y, z);

    return true;
}

bool inline Com_Parse(std::string_view s, OUT glm::vec3& value) {
    std::deque<std::string> v;
    Com_Split(s, v);

    if (v.size() < 3)
        return false;

    return Com_Parse(
        std::array<std::string_view, 3> { v[0], v[1], v[2] }, value
    );
}

bool inline Com_Parse(
    const std::array<std::string_view, 4>& v, OUT glm::vec4& value
) {
    value = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

    std::string_view xs = std::string_view(v[0]);
    float x = (float)atof(std::string(xs).data());
    
    std::string_view ys = std::string_view(v[1]);
    float y = (float)atof(std::string(ys).data());
    
    std::string_view zs = std::string_view(v[2]);
    float z = (float)atof(std::string(zs).data());

    std::string_view ws = std::string_view(v[3]);
    float w = (float)atof(std::string(ws).data());

    value = glm::vec4(x, y, z, w);

    return true;
}

bool inline Com_Parse(std::string_view s, OUT glm::vec4& value) {
    std::deque<std::string> v;
    Com_Split(s, v);

    if (v.size() < 4)
        return false;

    return Com_Parse(
        std::array<std::string_view, 4> { v[0], v[1], v[2], v[3] }, value
    );
}

template<>
struct std::formatter<glm::vec2> {
    template<class ParseCtx>
    constexpr auto parse(ParseCtx& ctx) const {
        return ctx.begin();
    }

    template<class FmtCtx>
    auto format(const glm::vec2& v, FmtCtx& ctx) const {
        return std::format_to(ctx.out(), "{} {}", v.x, v.y);
    }
};

template<>
struct std::formatter<glm::vec3> {
    template<class ParseCtx>
    constexpr auto parse(ParseCtx& ctx) const {
        return ctx.begin();
    }

    template<class FmtCtx>
    auto format(const glm::vec3& v, FmtCtx& ctx) const {
        return std::format_to(ctx.out(), "{} {} {}", v.x, v.y, v.z);
    }
};

template<>
struct std::formatter<glm::vec4> {
    template<class ParseCtx>
    constexpr auto parse(ParseCtx& ctx) const {
        return ctx.begin();
    }

    template<class FmtCtx>
    auto format(const glm::vec4& v, FmtCtx& ctx) const {
        return std::format_to(ctx.out(), "{} {} {} {}", v.x, v.y, v.z, v.w);
    }
};

template<typename ...Args>
void inline Com_Print(
    print_msg_dest_t dest, std::string_view fmt, Args&&... args
) {
    Com_PrintMessage(dest, Com_Format(fmt, args...));
}

// Shorthand for Com_Print(dest, "{}", t)
template<typename T>
void inline Com_Print(print_msg_dest_t dest, T t) {
    Com_Print(dest, "{}", t);
}

template<typename ...Args>
void inline Com_Println(
    print_msg_dest_t dest, std::string_view fmt, Args&&... args
) {
    Com_Print(dest, "{}\n", Com_Format(fmt, args...));
}

// Shorthand for Com_Println(dest, "")
void inline Com_Println(print_msg_dest_t dest) {
    Com_Println(dest, "");
}

// Shorthand for Com_Println(dest, "{}", t)
template<typename T>
void inline Com_Println(print_msg_dest_t dest, T t) {
    Com_Println(dest, "{}", t);
}

// Alias for Com_Print in Debug mode, nop in Release mode
#ifdef _DEBUG
template<typename ...Args>
void inline Com_DPrint(
    print_msg_dest_t dest, std::string_view fmt, Args&&... args
) {
    Com_Print(dest, fmt, args...);
}
#else // _DEBUG
template<typename ...Args>
void inline Com_DPrint(
    print_msg_dest_t dest, std::string_view fmt, Args&&... args
) {
    NOP();
}
#endif // _DEBUG

// Shorthand for Com_DPrint(CON_DEST_DEBUG_INFO, fmt, args...)
template<typename ...Args>
void inline Com_DPrint(
    std::string_view fmt, Args&&... args
) {
    Com_DPrint(CON_DEST_DEBUG_INFO, fmt, args...);
}

// Shorthand for Com_DPrint(dest, "{}", t)
template<typename T>
void inline Com_DPrint(print_msg_dest_t dest, T t) {
    Com_DPrint(dest, "{}", t);
}

// Shorthand for Com_DPrint(CON_DEST_DEBUG_INFO, t)
template<typename T>
void inline Com_DPrint(T t) {
    Com_DPrint(CON_DEST_DEBUG_INFO, t);
}

template<typename ...Args>
void inline Com_DPrintln(
    print_msg_dest_t dest, std::string_view fmt, Args&&... args
) {
    Com_DPrint(
        dest, "{}\n", Com_Format(fmt, args...)
    );
}

// Shorthand for Com_DPrintln(CON_DEST_DEBUG_INFO, fmt, args...)
template<typename ...Args>
void inline Com_DPrintln(std::string_view fmt, Args&&... args) {
    Com_DPrintln(CON_DEST_DEBUG_INFO, fmt, args...);
}

// Shorthand for Com_DPrintln(dest, "")
void inline Com_DPrintln(print_msg_dest_t dest) {
    Com_DPrintln(dest, "");
}

// Shorthand for Com_DPrintln(CON_DEST_DEBUG_INFO)
void inline Com_DPrintln() {
    Com_DPrintln(CON_DEST_DEBUG_INFO);
}

// Shorthand for Com_DPrintln(dest, "{}", t)
template<typename T>
void inline Com_DPrintln(print_msg_dest_t dest, T t) {
    Com_DPrintln(dest, "{}", t);
}

// Shorthand for Com_DPrintln(CON_DEST_DEBUG_INFO, t)
template<typename T>
void inline Com_DPrintln(T t) {
    Com_DPrintln(CON_DEST_DEBUG_INFO, t);
}

template<typename ...Args>
NO_RETURN inline Com_Error(int ec, std::string_view fmt, Args&&... args) {
    Com_Print(
        CON_DEST_FATAL_ERR, "FATAL ERROR: {}",
        Com_Format(fmt, args...)
    );
    Sys_NormalExit(ec);
}

// Shorthand for Com_Error(-1, fmt, args...)
template<typename ...Args>
NO_RETURN inline Com_Error(std::string_view fmt, Args&&... args) {
    Com_Error(-1, fmt, args...);
}

// Shorthand for Com_Error(ec, "{}", t)
template<typename T>
NO_RETURN inline Com_Error(int ec, T t) {
    Com_Error(ec, "{}", t);
}

// Shorthand for Com_Error(-1, t)
template<typename T>
NO_RETURN inline Com_Error(T t) {
    Com_Error(-1, t);
}

template<typename ...Args>
NO_RETURN inline Com_Errorln(int ec, std::string_view fmt, Args&&... args) {
    Com_Println(
        CON_DEST_FATAL_ERR, "FATAL ERROR: {} ()",
        Com_Format(fmt, args...),
        ec
    );
#if _DEBUG
    assert(false);
#else
    Sys_NormalExit(ec);
#endif // _DEBUG
}

// Shorthand for Com_Errorln(-1, fmt, args...)
template<typename ...Args>
NO_RETURN inline Com_Errorln(std::string_view fmt, Args&&... args) {
    Com_Errorln(-1, fmt, args...);
}

// Shorthand for Com_Errorln(ec, "{}", t)
template<typename T>
NO_RETURN inline Com_Errorln(int ec, T t) {
    Com_Errorln(ec, "{}", t);
}

// Shorthand for Com_Errorln(-1, t)
template<typename T>
NO_RETURN inline Com_Errorln(T t) {
    Com_Errorln(-1, t);
}
