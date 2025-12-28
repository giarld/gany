//
// Created by Gxin on 2022/6/4.
//

#ifndef GX_GANY_MODULE_DEF_H
#define GX_GANY_MODULE_DEF_H

#include "gany_pfn.h"


#define GANY_MODULE_DEFINE(MODULE_NAME) \
    extern "C" GX_API int32_t GX_API_CALL Register##MODULE_NAME(int64_t versionCode, PFN_ganyGetEnv pfnGetEnv, PFN_ganyClassInstance pfnClassInstance); \
    extern "C" GX_API void GX_API_CALL Unregister##MODULE_NAME()

#endif //GX_GANY_MODULE_DEF_H
