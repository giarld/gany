//
// Created by gxin on 18-6-9.
//

#ifndef GX_GGLOBAL_H
#define GX_GGLOBAL_H

#include <cstddef>
#include <cstdint>
#include <new>

#include "platform.h"


#if defined(BUILD_SHARED_LIBS)
    #define GX_BUILD_MODE_SHARED 1
    #define GX_BUILD_MODE_STATIC 0
#else
    #define GX_BUILD_MODE_SHARED 0
    #define GX_BUILD_MODE_STATIC 1
#endif


#if GX_BUILD_MODE_SHARED
#   if GX_PLATFORM_WINDOWS
#       define GX_DECL_EXPORT __declspec(dllexport)
#       define GX_DECL_IMPORT __declspec(dllimport)
#   else
#       define GX_DECL_EXPORT __attribute__((visibility("default")))
#       define GX_DECL_IMPORT
#   endif // GX_PLATFORM_WINDOWS
#else
#   define GX_DECL_EXPORT
#   define GX_DECL_IMPORT
#endif

#ifndef GX_API
#   define GX_API GX_DECL_EXPORT
#endif

#if GX_PLATFORM_WINDOWS
    #define GX_API_ATTR
    #define GX_API_CALL __stdcall
    #define GX_API_PTR  GX_API_CALL
#else
    #define GX_API_ATTR
    #define GX_API_CALL
    #define GX_API_PTR
#endif


// functions
#define ARRAY_LEN(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))

#define GX_UNUSED(x) (void)x

#if GX_COMPILER_MSVC
#   define GX_NOINLINE
#elif __has_attribute(noinline)
#   define GX_NOINLINE __attribute__((noinline))
#endif

#if GX_COMPILER_MSVC
#    define GX_RESTRICT __restrict
#elif (defined(__clang__) || defined(__GNUC__))
#    define GX_RESTRICT __restrict__
#else
#    define GX_RESTRICT
#endif


#define GX_STRINGIZE(x) GX_STRINGIZE_(x)
#define GX_STRINGIZE_(x) #x

#define GX_NEW(Type, ...) new(std::nothrow) Type(__VA_ARGS__)

#define GX_DELETE(Obj)  \
while (Obj) {           \
    delete(Obj);        \
    Obj = nullptr;      \
} GX_UNUSED(0)

#endif //GX_GGLOBAL_H
