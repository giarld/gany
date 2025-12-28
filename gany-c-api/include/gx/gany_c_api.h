//
// Created by Gxin on 2022/6/8.
//

#ifndef GX_GANY_C_API_H
#define GX_GANY_C_API_H

#include "gx/gglobal.h"


#ifdef __cplusplus
extern "C"
{
#endif

typedef int64_t GAnyPtr;

typedef int64_t CAnyFuncPtr;

typedef const char *GAnyString;

typedef GAnyPtr(GX_API_PTR *CAnyFunctionProxy)(CAnyFuncPtr funcPtr, GAnyPtr *args, int32_t argc);

typedef void(GX_API_PTR *CAnyFunctionDtorListener)(CAnyFuncPtr funcPtr);

GX_API void GX_API_CALL ganyFreeString(GAnyString str);

GX_API void GX_API_CALL ganySetFunctionProxy(CAnyFunctionProxy proxy);

GX_API void GX_API_CALL ganySetFunctionDtorListener(CAnyFunctionDtorListener listener);

/// ================= GAny =================

GX_API GAnyPtr GX_API_CALL ganyCreate(GAnyPtr v);

GX_API GAnyPtr GX_API_CALL ganyCreateBool(bool v);

GX_API GAnyPtr GX_API_CALL ganyCreateInt32(int32_t v);

GX_API GAnyPtr GX_API_CALL ganyCreateInt64(int64_t v);

GX_API GAnyPtr GX_API_CALL ganyCreateInt8(int8_t v);

GX_API GAnyPtr GX_API_CALL ganyCreateInt16(int16_t v);

GX_API GAnyPtr GX_API_CALL ganyCreateFloat(float v);

GX_API GAnyPtr GX_API_CALL ganyCreateDouble(double v);

GX_API GAnyPtr GX_API_CALL ganyCreateString(const char *v);

GX_API GAnyPtr GX_API_CALL ganyCreatePointer(void *v);

GX_API GAnyPtr GX_API_CALL ganyCreateArray();

GX_API GAnyPtr GX_API_CALL ganyCreateObject();

GX_API GAnyPtr GX_API_CALL ganyCreateFunction(CAnyFuncPtr funcPtr);

GX_API GAnyPtr GX_API_CALL ganyCreateUndefined();

GX_API GAnyPtr GX_API_CALL ganyCreateNull();

GX_API void GX_API_CALL ganyDestroy(GAnyPtr any);

GX_API GAnyPtr GX_API_CALL ganyImport(const char *path);

GX_API void GX_API_CALL ganyExport(GAnyPtr clazz);

GX_API GAnyPtr GX_API_CALL ganyLoad(const char *path);

GX_API GAnyPtr GX_API_CALL ganyBind(GAnyPtr obj, const char *methodName);


GX_API GAnyPtr GX_API_CALL ganyClone(GAnyPtr any);

GX_API const char *GX_API_CALL ganyClassTypeName(GAnyPtr any);

GX_API const char *GX_API_CALL ganyTypeName(GAnyPtr any);

GX_API int32_t GX_API_CALL ganyLength(GAnyPtr any);

GX_API int32_t GX_API_CALL ganySize(GAnyPtr any);

GX_API bool GX_API_CALL ganyIs(GAnyPtr any, const char *typeStr);

GX_API bool GX_API_CALL ganyIsUndefined(GAnyPtr any);

GX_API bool GX_API_CALL ganyIsNull(GAnyPtr any);

GX_API bool GX_API_CALL ganyIsFunction(GAnyPtr any);

GX_API bool GX_API_CALL ganyIsClass(GAnyPtr any);

GX_API bool GX_API_CALL ganyIsProperty(GAnyPtr any);

GX_API bool GX_API_CALL ganyIsObject(GAnyPtr any);

GX_API bool GX_API_CALL ganyIsArray(GAnyPtr any);

GX_API bool GX_API_CALL ganyIsInt(GAnyPtr any);

GX_API bool GX_API_CALL ganyIsFloat(GAnyPtr any);

GX_API bool GX_API_CALL ganyIsNumber(GAnyPtr any);

GX_API bool GX_API_CALL ganyIsString(GAnyPtr any);

GX_API bool GX_API_CALL ganyIsBoolean(GAnyPtr any);

GX_API bool GX_API_CALL ganyIsUserObject(GAnyPtr any);

GX_API bool GX_API_CALL ganyIsPointer(GAnyPtr any);

GX_API int32_t GX_API_CALL ganyToInt32(GAnyPtr any);

GX_API int64_t GX_API_CALL ganyToInt64(GAnyPtr any);

GX_API float GX_API_CALL ganyToFloat(GAnyPtr any);

GX_API double GX_API_CALL ganyToDouble(GAnyPtr any);

GX_API bool GX_API_CALL ganyToBool(GAnyPtr any);

GX_API GAnyString GX_API_CALL ganyToString(GAnyPtr any);

GX_API GAnyString GX_API_CALL ganyToJsonString(GAnyPtr any, int32_t indent);

GX_API GAnyPtr GX_API_CALL ganyToObject(GAnyPtr any);

GX_API GAnyPtr GX_API_CALL ganyParseJson(const char *json);

GX_API void *GX_API_CALL ganyToPointer(GAnyPtr any);

GX_API GAnyString GX_API_CALL ganyDump(GAnyPtr any);

GX_API bool GX_API_CALL ganyContains(GAnyPtr any, GAnyPtr id);

GX_API void GX_API_CALL ganyErase(GAnyPtr any, GAnyPtr id);

GX_API void GX_API_CALL ganyPushBack(GAnyPtr any, GAnyPtr rh);

GX_API void GX_API_CALL ganyClear(GAnyPtr any);

GX_API GAnyPtr GX_API_CALL ganyIterator(GAnyPtr any);

GX_API bool GX_API_CALL ganyHasNext(GAnyPtr any);

GX_API GAnyPtr GX_API_CALL ganyNext(GAnyPtr any);


GX_API GAnyPtr GX_API_CALL ganyCallMethod(GAnyPtr any, const char *methodName, GAnyPtr args[], int32_t argc);

GX_API GAnyPtr GX_API_CALL ganyCallFunction(GAnyPtr any, GAnyPtr args[], int32_t argc);


GX_API GAnyPtr GX_API_CALL ganyGetItem(GAnyPtr any, GAnyPtr i);

GX_API bool GX_API_CALL ganySetItem(GAnyPtr any, GAnyPtr i, GAnyPtr v);

GX_API bool GX_API_CALL ganyDelItem(GAnyPtr any, GAnyPtr i);


GX_API GAnyPtr GX_API_CALL ganyOperatorNeg(GAnyPtr any);     // -any

GX_API GAnyPtr GX_API_CALL ganyOperatorAdd(GAnyPtr a, GAnyPtr b);  // a + b

GX_API GAnyPtr GX_API_CALL ganyOperatorSub(GAnyPtr a, GAnyPtr b);  // a - b

GX_API GAnyPtr GX_API_CALL ganyOperatorMul(GAnyPtr a, GAnyPtr b);  // a * b

GX_API GAnyPtr GX_API_CALL ganyOperatorDiv(GAnyPtr a, GAnyPtr b);  // a / b

GX_API GAnyPtr GX_API_CALL ganyOperatorMod(GAnyPtr a, GAnyPtr b);  // a % b

GX_API GAnyPtr GX_API_CALL ganyOperatorBitXor(GAnyPtr a, GAnyPtr b);  // a ^ b

GX_API GAnyPtr GX_API_CALL ganyOperatorBitOr(GAnyPtr a, GAnyPtr b);  // a | b

GX_API GAnyPtr GX_API_CALL ganyOperatorBitAnd(GAnyPtr a, GAnyPtr b);  // a & b

GX_API GAnyPtr GX_API_CALL ganyOperatorBitNot(GAnyPtr v);  // ~v

GX_API bool GX_API_CALL ganyOperatorEqualTo(GAnyPtr a, GAnyPtr b);  // a == b

GX_API bool GX_API_CALL ganyOperatorLessThan(GAnyPtr a, GAnyPtr b);  // a < b

#ifdef __cplusplus
}
#endif

#endif //GX_GANY_C_API_H
