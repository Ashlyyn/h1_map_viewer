#include <format>

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
void inline Com_Print(
    print_msg_dest_t dest, std::string_view fmt, Args&&... args
) {
    Com_PrintMessage(dest, std::vformat(fmt, std::make_format_args(args...)));
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
    Com_Print(dest, "{}\n", std::vformat(fmt, std::make_format_args(args...)));
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
        dest, "{}\n", std::vformat(fmt, std::make_format_args(args...))
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
        std::vformat(fmt, std::make_format_args(args...))
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
        CON_DEST_FATAL_ERR, "FATAL ERROR: {}",
        std::vformat(fmt, std::make_format_args(args...))
    );
    Sys_NormalExit(ec);
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