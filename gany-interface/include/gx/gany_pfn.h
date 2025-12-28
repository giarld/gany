//
// Created by Gxin on 2023/10/11.
//

#ifndef GX_GANY_PFN_H
#define GX_GANY_PFN_H

#include "gglobal.h"


typedef void (GX_API_PTR *PFN_ganyGetEnv)(void *env);
typedef void (GX_API_PTR *PFN_ganyClassInstance)(void *typeInfo, void *ret);

extern PFN_ganyGetEnv pfnGanyGetEnv;
extern PFN_ganyClassInstance pfnGanyClassInstance;

#endif //GX_GANY_PFN_H
