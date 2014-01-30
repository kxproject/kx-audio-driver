
// stub header

#ifndef KALI_DBG_UTILS_INCLUDED
#define KALI_DBG_UTILS_INCLUDED

#if _MSC_VER >= 1400
    #define trace(...) __noop
#else
    inline void trace(...) {}
#endif

#define FUNCTION_  ""
#define FUNCSIG_   __FUNCSIG__
#define inline_    __forceinline

#endif // ~ KALI_DBG_UTILS_INCLUDED
