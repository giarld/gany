//
// Created by Gxin on 2022/6/8.
//

#include "gx/gany_c_api.h"

#include <gx/gany.h>

#include <mutex>
#include <shared_mutex>


static CAnyFunctionProxy sCAnyFunctionProxy = nullptr;

static CAnyFunctionDtorListener sCAnyFunctionDtorListener = nullptr;

static std::shared_mutex sLockFuncProxy;

static std::shared_mutex sLockFDtor;

const static std::string EmptyStr;

static std::vector<std::unique_ptr<std::string> > sStringCache;

static std::mutex sStrCacheLock;

static GAny sLogger;

static GAnyPtr ganyCreatePtr(const GAny &v)
{
    return reinterpret_cast<GAnyPtr>(GX_NEW(GAny, v));
}

static GAnyString cacheString(std::string &&str)
{
    std::lock_guard locker(sStrCacheLock);

    sStringCache.push_back(std::make_unique<std::string>(std::move(str)));
    return sStringCache.back()->c_str();
}

static void printLogE(const std::string &log)
{
    if (sLogger.isUndefined()) {
        sLogger = GAny::Import("GLog");
        if (!sLogger.isClass()) {
            sLogger = GAny::null();
        }
    }
    if (sLogger) {
        sLogger.call("LogE", log);
    } else {
        fprintf(stderr, "%s", log.c_str());
    }
}


class CAnyFunctionDtorHandler
{
public:
    explicit CAnyFunctionDtorHandler(CAnyFuncPtr funcPtr)
        : mFuncPtr(funcPtr)
    {
    }

    ~CAnyFunctionDtorHandler()
    {
        sLockFDtor.lock_shared();
        if (sCAnyFunctionDtorListener) {
            sCAnyFunctionDtorListener(mFuncPtr);
        }
        sLockFDtor.unlock_shared();
    }

private:
    CAnyFuncPtr mFuncPtr;
};

void ganyFreeString(GAnyString str)
{
    if (str == EmptyStr.c_str()) {
        return;
    }
    std::lock_guard locker(sStrCacheLock);
    const auto it = std::find_if(sStringCache.begin(), sStringCache.end(), [&](const auto &s) {
        return str == s->c_str();
    });
    if (it != sStringCache.end()) {
        if (sStringCache.size() > 1) {
            std::swap(*it, *(sStringCache.end() - 1));
        }
        sStringCache.pop_back();
    }
}

void ganySetFunctionProxy(CAnyFunctionProxy proxy)
{
    sLockFuncProxy.lock();
    sCAnyFunctionProxy = proxy;
    sLockFuncProxy.unlock();
}

void ganySetFunctionDtorListener(CAnyFunctionDtorListener listener)
{
    sLockFDtor.lock();
    sCAnyFunctionDtorListener = listener;
    sLockFDtor.unlock();
}


GAnyPtr ganyCreate(GAnyPtr v)
{
    return ganyCreatePtr(*reinterpret_cast<const GAny *>(v));
}

GAnyPtr ganyCreateBool(bool v)
{
    return ganyCreatePtr(v);
}

GAnyPtr ganyCreateInt32(int32_t v)
{
    return ganyCreatePtr(v);
}

GAnyPtr ganyCreateInt64(int64_t v)
{
    return ganyCreatePtr(v);
}

GAnyPtr ganyCreateInt8(int8_t v)
{
    return ganyCreatePtr(v);
}

GAnyPtr ganyCreateInt16(int16_t v)
{
    return ganyCreatePtr(v);
}

GAnyPtr ganyCreateFloat(float v)
{
    return ganyCreatePtr(v);
}

GAnyPtr ganyCreateDouble(double v)
{
    return ganyCreatePtr(v);
}

GAnyPtr ganyCreateString(const char *v)
{
    return ganyCreatePtr(std::string(v));
}

GAnyPtr ganyCreatePointer(void *v)
{
    return ganyCreatePtr(GAny(static_cast<GAnyBytePtr>(v)));
}

GAnyPtr ganyCreateArray()
{
    return ganyCreatePtr(GAny::array());
}

GAnyPtr ganyCreateObject()
{
    return ganyCreatePtr(GAny::object());
}

GAnyPtr ganyCreateFunction(CAnyFuncPtr funcPtr)
{
    auto dtorHandler = std::make_shared<CAnyFunctionDtorHandler>(funcPtr);

    // **不要删除 lambda 捕获表中的 dtorHandler
    const GAnyFunction func = GAnyFunction::createVariadicFunction(
        "CFunction",
        [funcPtr, dtorHandler](const GAny **args, int32_t argc) {
            // 无意义, 仅防止 dtorHandler 变量被编译器优化
            if (!dtorHandler) {
                return GAny::null();
            }

            auto *tArgs = static_cast<GAnyPtr *>(alloca(sizeof(GAnyPtr) * argc));
            for (int32_t i = 0; i < argc; i++) {
                // Stored by the target language and released at the appropriate time.
                tArgs[i] = ganyCreatePtr(*args[i]);
            }

            GAnyPtr ret = 0;

            sLockFuncProxy.lock_shared();
            if (sCAnyFunctionProxy) {
                ret = sCAnyFunctionProxy(funcPtr, tArgs, argc);
            }
            sLockFuncProxy.unlock_shared();

            if (ret == 0) {
                return GAny::null();
            }
            GAny retCopy = *reinterpret_cast<GAny *>(ret);
            ganyDestroy(ret);

            if (retCopy.isException()) {
                printLogE(retCopy.as<GAnyException>()->toString());
                return GAny::undefined();
            }
            return retCopy;
        });
    return ganyCreatePtr(GAny(func));
}

GAnyPtr ganyCreateUndefined()
{
    return ganyCreatePtr(GAny::undefined());
}

GAnyPtr ganyCreateNull()
{
    return ganyCreatePtr(GAny::null());
}

void ganyDestroy(GAnyPtr any)
{
    if (!any) {
        return;
    }
    const auto *anyPtr = reinterpret_cast<GAny *>(any);
    GX_DELETE(anyPtr);
}

GAnyPtr ganyImport(const char *path)
{
    return ganyCreatePtr(GAny::Import(path));
}

void ganyExport(GAnyPtr clazz)
{
    const GAny clazzCopy = *reinterpret_cast<GAny *>(clazz);
    GAny::Export(clazzCopy.castAs<std::shared_ptr<GAnyClass>>());
}

GAnyPtr ganyLoad(const char *path)
{
    return ganyCreatePtr(GAny::Load(path));
}

GAnyPtr ganyBind(GAnyPtr obj, const char *methodName)
{
    return ganyCreatePtr(GAny::Bind(*reinterpret_cast<GAny *>(obj), methodName));
}


GAnyPtr ganyClone(GAnyPtr any)
{
    return ganyCreatePtr(reinterpret_cast<GAny *>(any)->clone());
}

const char *ganyClassTypeName(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->classTypeName().c_str();
}

const char *ganyTypeName(GAnyPtr any)
{
    return AnyTypeNames[static_cast<size_t>(reinterpret_cast<GAny *>(any)->type())];
}

int32_t ganyLength(GAnyPtr any)
{
    return static_cast<int32_t>(reinterpret_cast<GAny *>(any)->length());
}

int32_t ganySize(GAnyPtr any)
{
    return static_cast<int32_t>(reinterpret_cast<GAny *>(any)->size());
}

bool ganyIs(GAnyPtr any, const char *typeStr)
{
    return reinterpret_cast<GAny *>(any)->is(typeStr);
}

bool ganyIsUndefined(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->isUndefined();
}

bool ganyIsNull(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->isNull();
}

bool ganyIsFunction(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->isFunction();
}

bool ganyIsClass(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->isClass();
}

bool ganyIsException(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->isException();
}

bool ganyIsProperty(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->isProperty();
}

bool ganyIsObject(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->isObject();
}

bool ganyIsArray(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->isArray();
}

bool ganyIsInt(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->isInt();
}

bool ganyIsFloat(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->isFloat();
}

bool ganyIsNumber(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->isNumber();
}

bool ganyIsString(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->isString();
}

bool ganyIsBoolean(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->isBoolean();
}

bool ganyIsUserObject(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->isUserObject();
}

bool ganyIsPointer(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->is<GAnyBytePtr>();
}

int32_t ganyToInt32(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->toInt32();
}

int64_t ganyToInt64(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->toInt64();
}

float ganyToFloat(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->toFloat();
}

double ganyToDouble(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->toDouble();
}

bool ganyToBool(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->toBool();
}

GAnyString ganyToString(GAnyPtr any)
{
    std::string str = reinterpret_cast<GAny *>(any)->toString();
    return cacheString(std::move(str));
}

GAnyString ganyToJsonString(GAnyPtr any, int32_t indent)
{
    std::string str = reinterpret_cast<GAny *>(any)->toJsonString(indent);
    return cacheString(std::move(str));
}

GAnyPtr ganyToObject(GAnyPtr any)
{
    return ganyCreatePtr(reinterpret_cast<GAny *>(any)->toObject());
}

GAnyPtr ganyParseJson(const char *json)
{
    return ganyCreatePtr(GAny::parseJson(json));
}

void *ganyToPointer(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->castAs<GAnyBytePtr>();
}

GAnyString ganyDump(GAnyPtr any)
{
    GAny *anyPtr = reinterpret_cast<GAny *>(any);
    std::stringstream ss;
    if (anyPtr->is<GAnyClass>()) {
        ss << *anyPtr->as<GAnyClass>();
    } else if (anyPtr->is<GAnyFunction>()) {
        ss << *anyPtr->as<GAnyFunction>();
    } else {
        ss << *anyPtr;
    }
    std::string str = ss.str();
    return cacheString(std::move(str));
}

bool ganyContains(GAnyPtr any, GAnyPtr id)
{
    const GAny idAny = *reinterpret_cast<GAny *>(id);
    return reinterpret_cast<GAny *>(any)->contains(idAny);
}

void ganyErase(GAnyPtr any, GAnyPtr id)
{
    const GAny idAny = *reinterpret_cast<GAny *>(id);
    reinterpret_cast<GAny *>(any)->erase(idAny);
}

void ganyPushBack(GAnyPtr any, GAnyPtr rh)
{
    const GAny rhAny = *reinterpret_cast<GAny *>(rh);
    reinterpret_cast<GAny *>(any)->pushBack(rhAny);
}

void ganyClear(GAnyPtr any)
{
    reinterpret_cast<GAny *>(any)->clear();
}

GAnyPtr ganyIterator(GAnyPtr any)
{
    return ganyCreatePtr(reinterpret_cast<GAny *>(any)->iterator());
}

bool ganyHasNext(GAnyPtr any)
{
    return reinterpret_cast<GAny *>(any)->hasNext();
}

GAnyPtr ganyNext(GAnyPtr any)
{
    return ganyCreatePtr(reinterpret_cast<GAny *>(any)->next());
}


GAnyPtr ganyCallMethod(GAnyPtr any, const char *methodName, GAnyPtr *args, int32_t argc)
{
    std::vector<GAny> argv;
    for (int32_t i = 0; i < argc; i++) {
        argv.push_back(*reinterpret_cast<GAny *>(args[i]));
    }
    const GAny ret = reinterpret_cast<GAny *>(any)->_call(methodName, argv);
    return ganyCreatePtr(ret);
}

GAnyPtr ganyCallFunction(GAnyPtr any, GAnyPtr *args, int32_t argc)
{
    std::vector<GAny> argv;
    for (int32_t i = 0; i < argc; i++) {
        argv.push_back(*reinterpret_cast<GAny *>(args[i]));
    }
    const GAny ret = reinterpret_cast<GAny *>(any)->_call(argv);
    return ganyCreatePtr(ret);
}


GAnyPtr ganyGetItem(GAnyPtr any, GAnyPtr i)
{
    const GAny iAny = *reinterpret_cast<GAny *>(i);
    const GAny ret = reinterpret_cast<GAny *>(any)->getItem(iAny);
    return ganyCreatePtr(ret);
}

bool ganySetItem(GAnyPtr any, GAnyPtr i, GAnyPtr v)
{
    const GAny iAny = *reinterpret_cast<GAny *>(i);
    const GAny vAny = *reinterpret_cast<GAny *>(v);
    return reinterpret_cast<GAny *>(any)->setItem(iAny, vAny);
}

bool ganyDelItem(GAnyPtr any, GAnyPtr i)
{
    const GAny iAny = *reinterpret_cast<GAny *>(i);
    return reinterpret_cast<GAny *>(any)->delItem(iAny);
}


GAnyPtr ganyOperatorNeg(GAnyPtr any)
{
    return ganyCreatePtr(-(*reinterpret_cast<GAny *>(any)));
}

GAnyPtr ganyOperatorAdd(GAnyPtr a, GAnyPtr b)
{
    return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) + (*reinterpret_cast<GAny *>(b)));
}

GAnyPtr ganyOperatorSub(GAnyPtr a, GAnyPtr b)
{
    return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) - (*reinterpret_cast<GAny *>(b)));
}

GAnyPtr ganyOperatorMul(GAnyPtr a, GAnyPtr b)
{
    return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) * (*reinterpret_cast<GAny *>(b)));
}

GAnyPtr ganyOperatorDiv(GAnyPtr a, GAnyPtr b)
{
    return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) / (*reinterpret_cast<GAny *>(b)));
}

GAnyPtr ganyOperatorMod(GAnyPtr a, GAnyPtr b)
{
    return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) % (*reinterpret_cast<GAny *>(b)));
}

GAnyPtr ganyOperatorBitXor(GAnyPtr a, GAnyPtr b)
{
    return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) ^ (*reinterpret_cast<GAny *>(b)));
}

GAnyPtr ganyOperatorBitOr(GAnyPtr a, GAnyPtr b)
{
    return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) | (*reinterpret_cast<GAny *>(b)));
}

GAnyPtr ganyOperatorBitAnd(GAnyPtr a, GAnyPtr b)
{
    return ganyCreatePtr((*reinterpret_cast<GAny *>(a)) & (*reinterpret_cast<GAny *>(b)));
}

GAnyPtr ganyOperatorBitNot(GAnyPtr v)
{
    return ganyCreatePtr(~(*reinterpret_cast<GAny *>(v)));
}

bool ganyOperatorEqualTo(GAnyPtr a, GAnyPtr b)
{
    return (*reinterpret_cast<GAny *>(a)) == (*reinterpret_cast<GAny *>(b));
}

bool ganyOperatorLessThan(GAnyPtr a, GAnyPtr b)
{
    return (*reinterpret_cast<GAny *>(a)) < (*reinterpret_cast<GAny *>(b));
}
