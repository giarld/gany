//
// Created by Gxin on 2022/5/25.
//

#ifndef GX_GANY_H
#define GX_GANY_H

#include "platform.h"
#include "gany_pfn.h"

#include "enum.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#if GX_COMPILER_MSVC
#include <malloc.h>
#endif

#if GX_PLATFORM_LINUX || GX_PLATFORM_OSX || GX_PLATFORM_EMSCRIPTEN
#include <alloca.h>
#endif


#if GX_COMPILER_GCC || (GX_COMPILER_CLANG && !GX_PLATFORM_WINDOWS)

#include <cxxabi.h>

#endif

#if defined(BUILD_SHARED_LIBS) || defined(USE_GANY_CORE)
#define REGISTER_GANY_MODULE(MODULE_NAME)                                   \
    PFN_ganyGetEnv pfnGanyGetEnv = nullptr;                                 \
    PFN_ganyClassInstance pfnGanyClassInstance = nullptr;                   \
    class GAnyModule##MODULE_NAME                                           \
    {                                                                       \
    private:                                                                \
        GAnyModule##MODULE_NAME()                                           \
        {                                                                   \
            GAny classDB;                                                   \
            if (pfnGanyGetEnv) {                                            \
                GAny envObj;                                                \
                pfnGanyGetEnv(&envObj);                                     \
                if (envObj.isObject()) {                                    \
                    classDB = (*envObj.as<GAnyObject>())["__CLASS_DB"];     \
                }                                                           \
            }                                                               \
            if (classDB) {                                                  \
                mModuleIdx = *classDB.call("assignModuleIdx").as<uint64_t>();\
                classDB.call("beginModule", mModuleIdx);                    \
            }                                                               \
            registerModule();                                               \
            if (classDB) {                                                  \
                classDB.call("endModule");                                  \
            }                                                               \
        }                                                                   \
    public:                                                                 \
        void unregisterModule()                                             \
        {                                                                   \
            if (mModuleIdx == 0)                                            \
                return;                                                     \
            GAny classDB;                                                   \
            if (pfnGanyGetEnv) {                                            \
                GAny envObj;                                                \
                pfnGanyGetEnv(&envObj);                                     \
                if (envObj.isObject()) {                                    \
                    classDB = (*envObj.as<GAnyObject>())["__CLASS_DB"];     \
                }                                                           \
            }                                                               \
            if (classDB) {                                                  \
                classDB.call("releaseModule", mModuleIdx);                  \
            }                                                               \
        }                                                                   \
        static GAnyModule##MODULE_NAME *instance()                          \
        {                                                                   \
            static GAnyModule##MODULE_NAME gAnyModule;                      \
            return &gAnyModule;                                             \
        }                                                                   \
    private:                                                                \
        void registerModule();                                              \
        uint64_t mModuleIdx = 0;                                            \
    };                                                                      \
    int32_t GX_API_CALL Register##MODULE_NAME(int64_t versionCode, PFN_ganyGetEnv pfnGetEnv, PFN_ganyClassInstance pfnClassInstance) \
    {                                                                       \
        if (versionCode != GANY_VERSION_CODE) {                             \
            fprintf(stderr, "Register" GX_STRINGIZE(MODULE_NAME) ", GAny version mismatch! code version: %d.%d.%d, module version: %d.%d.%d\n", \
                (int32_t)((GANY_VERSION_CODE >> 48)&0xffff), (int32_t)((GANY_VERSION_CODE >> 32)&0xffff), (int32_t)((GANY_VERSION_CODE >> 16)&0xffff), \
                (int32_t)((versionCode >> 48)&0xffff), (int32_t)((versionCode >> 32)&0xffff), (int32_t)((versionCode >> 16)&0xffff)); \
            return 1;                                                       \
        }                                                                   \
        pfnGanyGetEnv = pfnGetEnv;                                          \
        pfnGanyClassInstance = pfnClassInstance;                            \
        GAnyModule##MODULE_NAME *gAnyModule = GAnyModule##MODULE_NAME::instance();  \
        *gAnyModule;                                                        \
        return 0;                                                           \
    }                                                                       \
    void GX_API_CALL Unregister##MODULE_NAME()                              \
    {                                                                       \
        GAnyModule##MODULE_NAME *gAnyModule = GAnyModule##MODULE_NAME::instance();  \
        gAnyModule->unregisterModule();                                     \
    }                                                                       \
    extern "C" GX_API int32_t GX_API_CALL RegisterModule(int64_t versionCode, PFN_ganyGetEnv pfnGetEnv, PFN_ganyClassInstance pfnClassInstance) \
    {                                                                       \
        return Register##MODULE_NAME(versionCode, pfnGetEnv, pfnClassInstance); \
    }                                                                       \
    extern "C" GX_API void GX_API_CALL UnregisterModule()                   \
    {                                                                       \
        Unregister##MODULE_NAME();                                          \
    }                                                                       \
    void GAnyModule##MODULE_NAME::registerModule()
#else
#define REGISTER_GANY_MODULE(MODULE_NAME)                                   \
    class GAnyModule##MODULE_NAME                                           \
    {                                                                       \
    private:                                                                \
        GAnyModule##MODULE_NAME()                                           \
        {                                                                   \
            GAny classDB;                                                   \
            if (pfnGanyGetEnv) {                                            \
                GAny envObj;                                                \
                pfnGanyGetEnv(&envObj);                                     \
                if (envObj.isObject()) {                                    \
                    classDB = (*envObj.as<GAnyObject>())["__CLASS_DB"];     \
                }                                                           \
            }                                                               \
            if (classDB) {                                                  \
                mModuleIdx = *classDB.call("assignModuleIdx").as<uint64_t>();\
                classDB.call("beginModule", mModuleIdx);                    \
            }                                                               \
            registerModule();                                               \
            if (classDB) {                                                  \
                classDB.call("endModule");                                  \
            }                                                               \
        }                                                                   \
    public:                                                                 \
        void unregisterModule()                                             \
        {                                                                   \
            if (mModuleIdx == 0)                                            \
                return;                                                     \
            GAny classDB;                                                   \
            if (pfnGanyGetEnv) {                                            \
                GAny envObj;                                                \
                pfnGanyGetEnv(&envObj);                                     \
                if (envObj.isObject()) {                                    \
                    classDB = (*envObj.as<GAnyObject>())["__CLASS_DB"];     \
                }                                                           \
            }                                                               \
            if (classDB) {                                                  \
                classDB.call("releaseModule", mModuleIdx);                  \
            }                                                               \
        }                                                                   \
        static GAnyModule##MODULE_NAME *instance()                          \
        {                                                                   \
            static GAnyModule##MODULE_NAME gAnyModule;                      \
            return &gAnyModule;                                             \
        }                                                                   \
    private:                                                                \
        void registerModule();                                              \
        uint64_t mModuleIdx = 0;                                            \
    };                                                                      \
    int32_t GX_API_CALL Register##MODULE_NAME(int64_t versionCode, PFN_ganyGetEnv, PFN_ganyClassInstance) \
    {                                                                       \
        if (versionCode != GANY_VERSION_CODE) {                             \
            fprintf(stderr, "Register" GX_STRINGIZE(MODULE_NAME) ", GAny version mismatch! code version: %d.%d.%d, module version: %d.%d.%d\n", \
                (int32_t)((GANY_VERSION_CODE >> 48)&0xffff), (int32_t)((GANY_VERSION_CODE >> 32)&0xffff), (int32_t)((GANY_VERSION_CODE >> 16)&0xffff), \
                (int32_t)((versionCode >> 48)&0xffff), (int32_t)((versionCode >> 32)&0xffff), (int32_t)((versionCode >> 16)&0xffff)); \
            return 1;                                                       \
        }                                                                   \
        GAnyModule##MODULE_NAME *gAnyModule = GAnyModule##MODULE_NAME::instance();  \
        *gAnyModule;                                                        \
        return 0;                                                           \
    }                                                                       \
    void GX_API_CALL Unregister##MODULE_NAME()                              \
    {                                                                       \
        GAnyModule##MODULE_NAME *gAnyModule = GAnyModule##MODULE_NAME::instance();  \
        gAnyModule->unregisterModule();                                     \
    }                                                                       \
    void GAnyModule##MODULE_NAME::registerModule()
#endif

#define GANY_VERSION_MAJOR 1
#define GANY_VERSION_MINOR 0
#define GANY_VERSION_PATCH 5

#define GANY_VERSION_CODE (0 | (GANY_VERSION_PATCH << 16) | ((uint64_t) GANY_VERSION_MINOR << 32) | ((uint64_t) GANY_VERSION_MAJOR << 48))


namespace detail
{
template<typename T>
struct remove_class
{
};

template<typename C, typename R, typename... A>
struct remove_class<R (C::*)(A...)>
{
    typedef R type(A...);
};

template<typename C, typename R, typename... A>
struct remove_class<R (C::*)(A...) const>
{
    typedef R type(A...);
};

template<typename F>
struct strip_function_object
{
    using type = typename remove_class<decltype(&F::operator())>::type;
};

template<typename Function, typename F = std::remove_reference_t<Function> >
using function_signature_t = std::conditional_t<
    std::is_function_v<F>,
    F,
    typename std::conditional_t<
        std::is_pointer_v<F> || std::is_member_pointer_v<F>,
        std::remove_pointer<F>,
        strip_function_object<F>
    >::type
>;

template<size_t ...>
struct index_sequence
{
};

template<size_t N, size_t ... S>
struct make_index_sequence_impl : make_index_sequence_impl<N - 1, N - 1, S...>
{
};

template<size_t ... S>
struct make_index_sequence_impl<0, S...>
{
    typedef index_sequence<S...> type;
};

template<size_t N>
using make_index_sequence = typename make_index_sequence_impl<N>::type;

template<class T>
struct sfinae_true : std::true_type
{
    typedef T type;
};

template<class T>
struct sfinae_false : std::false_type
{
    typedef T type;
};

template<class T>
static auto test_call_op(int) -> sfinae_true<decltype(&T::operator())>;

template<class T>
static auto test_call_op(long) -> sfinae_false<T>;

template<class T, class T2 = decltype(test_call_op<T>(0))>
struct has_call_op_ : T2
{
};

template<class T>
struct has_call_op : std::is_member_function_pointer<typename has_call_op_<T>::type>
{
};

template<typename T>
struct is_callable : has_call_op<T>
{
};

template<typename TClass, typename TRet, typename... TArgs>
struct is_callable<TRet(TClass::*)(TArgs...)> : std::true_type
{
};

template<typename TClass, typename TRet, typename... TArgs>
struct is_callable<TRet(TClass::*)(TArgs...) const> : std::true_type
{
};

template<typename TRet, typename... TArgs>
struct is_callable<TRet(*)(TArgs...)> : std::true_type
{
};

template <typename>
struct is_std_function : std::false_type {};

template <typename R, typename... Args>
struct is_std_function<std::function<R(Args...)>> : std::true_type {};

template <typename>
struct is_array_t : std::false_type {};

template <typename T>
struct is_array_t<std::vector<T>> : std::true_type {};

template <typename T>
struct is_array_t<std::list<T>> : std::true_type {};

template <typename T, size_t N>
struct is_array_t<T[N]> : std::true_type {};

template <typename>
struct is_map_t : std::false_type {};

template <typename K, typename V>
struct is_map_t<std::map<K, V>> : std::true_type {};

template <typename K, typename V>
struct is_map_t<std::unordered_map<K, V>> : std::true_type {};

template <typename>
struct is_shared_ptr : std::false_type {};

template <typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template<typename T>
void hashCombine(size_t &s, const T &v)
{
    std::hash<T> h;
    s ^= h(v) + 0x9e3779b9u + (s << 6u) + (s >> 2u);
}
}

DEF_ENUM_14(AnyType, uint8_t, 0,
            undefined_t, ///< undefined (void)
            null_t,      ///< null (nullptr)
            boolean_t,   ///< boolean (bool)
            int_t,       ///< int  (int64_t)
            float_t,     ///< float
            double_t,    ///< double
            string_t,    ///< string
            array_t,     ///< array (ordered collection of values)
            object_t,    ///< object (unordered set of name/value pairs)
            function_t,  ///< function expressions
            class_t,     ///< class expressions
            property_t,  ///< class property
            exception_t, ///< exception (GAnyException)
            user_obj_t  ///< user object
)

constexpr const char *AnyTypeNames[] = {
    "undefined",
    "null",
    "boolean",
    "int",
    "float",
    "double",
    "string",
    "array",
    "object",
    "function",
    "class",
    "property",
    "exception",
    "user_obj"
};


DEF_ENUM_24(MetaFunction, uint8_t, 0,
            Init,
            Negate,
            Addition,
            Subtraction,
            Multiplication,
            Division,
            Modulo,
            BitXor,
            BitOr,
            BitAnd,
            BitNot,
            EqualTo,
            LessThan,
            GetItem,
            SetItem,
            DelItem,
            Length,
            ToString,
            ToInt,
            ToFloat,
            ToDouble,
            ToBoolean,
            ToObject,
            FromObject
)

constexpr const char *MetaFunctionNames[] = {
    "__init",
    "__neg",
    "__add",
    "__sub",
    "__mul",
    "__div",
    "__mod",
    "__xor",
    "__or",
    "__and",
    "__not",
    "__eq",
    "__lt",
    "__getitem",
    "__setitem",
    "__delitem",
    "__len",
    "__str",
    "__to_int",
    "__to_float",
    "__to_double",
    "__to_bool",
    "__to_object",
    "__from_object"
};

class GAny;

class GAnyFunction;

class GAnyClass;

class GAnyObject;

class GAnyArray;

class GAnyException;

class GAnyTypeInfo;

template<typename T>
class GAnyValueP;

using GAnyBytePtr = char *;

using GAnyConstBytePtr = const char *;

using GAnyIteratorItem = std::pair<GAny, GAny>;

namespace gany
{
template<typename>
class Class;
}

class GAnyValue
{
public:
    GAnyValue() = default;

    virtual ~GAnyValue() = default;

public:
    typedef std::type_index TypeID;

    virtual const void *as(const TypeID &tp) const
    {
        return nullptr;
    }

    virtual const void *ptr() const
    {
        return this;
    }

    virtual GAnyClass *classObject() const
    {
        return nullptr;
    }

    virtual size_t length() const
    {
        return 0;
    }

    virtual void clone(GAny &v) const
    {
    }

    virtual bool isSharedPtr() const
    {
        return false;
    }
};

/// ================ GAny ================

class GAny
{
public:
    GAny();

    template<typename T>
    GAny(const T &var);

    template<typename T>
    GAny(std::enable_if_t<!std::is_same_v<T, GAny>, T> &&var);

    template<typename T>
    GAny(std::unique_ptr<T> &&v);

    GAny(const std::initializer_list<GAny> &init);

    template<typename Return, typename... Args>
    GAny(Return (*f)(Args...));

    template<typename Return, typename Class, typename... arg>
    GAny(Return (Class::*f)(arg...));

    template<typename Return, typename Class, typename... arg>
    GAny(Return (Class::*f)(arg...) const);

public:
    static GAny object(const std::map<std::string, GAny> &m);

    static GAny object(const std::unordered_map<std::string, GAny> &m = {});

    static GAny array(const std::vector<GAny> &vec = {});

    static GAny array(const std::list<GAny> &lst);

    static GAny undefined();

    static GAny null();

    template<typename T, typename... Args>
    static GAny New(Args &&... args);

public:
    const std::shared_ptr<GAnyValue> &value() const;

    static GAny fromValue(std::shared_ptr<GAnyValue> val);

    GAny clone() const;

    const std::string &classTypeName() const;

    std::string typeName() const;

    GAnyTypeInfo typeInfo() const;

    AnyType type() const;

    GAnyClass &classObject() const;

    size_t length() const;

    size_t size() const
    {
        return length();
    }

public:
    template<typename T>
    bool is() const;

    bool is(const GAnyTypeInfo &typeInfo) const;

    bool is(const std::string &typeStr) const;

    bool isUndefined() const;

    bool isNull() const;

    bool isFunction() const;

    bool isClass() const;

    bool isException() const;

    bool isProperty() const;

    bool isObject() const;

    bool isArray() const;

    bool isInt() const;

    bool isFloat() const;

    bool isDouble() const;

    bool isNumber() const;

    bool isString() const;

    bool isBoolean() const;

    bool isUserObject() const;

public:
    template<typename T>
    const T *as() const;

    template<typename T>
    T *as();

    template<typename T>
    T *unsafeAs();

    template<typename T>
    const T *unsafeAs() const;

    template<typename T>
    std::enable_if_t<std::is_pointer_v<T> && !std::is_void_v<std::remove_pointer_t<T>>, T>
    castAs();

    template<typename T>
    std::enable_if_t<std::is_pointer_v<T> && std::is_void_v<std::remove_pointer_t<T>>, T>
    castAs();

    template<typename T>
    std::enable_if_t<std::is_reference_v<T>, T>
    castAs();

    template<typename T>
    std::enable_if_t<!std::is_reference_v<T> && !std::is_pointer_v<T>, T>
    castAs() const;

    template<typename F>
    std::enable_if_t<detail::is_std_function<F>::value, F>
    castAs();

    const void *getPointer() const;

    /**
     * @brief Determine the existence of elements (only applicable to array (check if the corresponding element exists),
     *          object (check if the value for a given key exists),
     *          and environment (check if the value for a given key exists)).
     * @param id    Element IDs, array(GAny), object keys(string)
     * @return
     */
    bool contains(const GAny &id) const;

    /**
     * @brief Delete element (arrays and objects only).
     * @param id    Element ID, array index (int32), object key (string)
     */
    void erase(const GAny &id);

    /**
     * @brief Add an element to the array.
     * @param rh
     */
    void pushBack(const GAny &rh);

    void clear();

    GAny iterator() const;

    bool hasNext() const;

    GAnyIteratorItem next() const;

public:
    GAny &overload(GAny func);

    template<typename... Args>
    GAny call(const std::string &methodName, Args &&... args) const;

    template<typename... Args>
    GAny call(const std::string &methodName, Args &&... args);

    template<typename... Args>
    GAny call(MetaFunction metaFunc, Args &&... args) const
    {
        return call(MetaFunctionNames[static_cast<size_t>(metaFunc)], std::forward<Args>(args)...);
    }

    template<typename... Args>
    GAny call(MetaFunction metaFunc, Args &&... args)
    {
        return call(MetaFunctionNames[static_cast<size_t>(metaFunc)], std::forward<Args>(args)...);
    }

    GAny _call(const std::string &methodName, const GAny **args, int32_t argc) const;

    GAny _call(const std::string &methodName, const GAny **args, int32_t argc);

    GAny _call(const std::string &methodName, const std::vector<GAny> &args) const;

    GAny _call(const std::string &methodName, const std::vector<GAny> &args);

    GAny _call(MetaFunction metaFunc, const GAny **args, int32_t argc) const;

    GAny _call(MetaFunction metaFunc, const GAny **args, int32_t argc);

    GAny _call(MetaFunction metaFunc, std::vector<GAny> &args) const;

    GAny _call(MetaFunction metaFunc, std::vector<GAny> &args);

    GAny _call(const GAny **args, int32_t argc) const;

    GAny _call(const GAny **args, int32_t argc);

    GAny _call(const std::vector<GAny> &args) const;

    GAny _call(const std::vector<GAny> &args);

    template<typename... Args>
    GAny operator()(Args... args) const;

    template<typename... Args>
    GAny operator()(Args... args);

public:
    GAny operator-() const; // Negate

    GAny operator+(const GAny &rh) const; // Addition

    GAny operator-(const GAny &rh) const; // Subtraction

    GAny operator*(const GAny &rh) const; // Multiplication

    GAny operator/(const GAny &rh) const; // Division

    GAny operator%(const GAny &rh) const; // Modulo

    GAny operator^(const GAny &rh) const; // BitXor

    GAny operator|(const GAny &rh) const; // BitOr

    GAny operator&(const GAny &rh) const; // BitAnd

    GAny operator~() const; // BitNot

    bool operator==(const GAny &rh) const; // EqualTo

    bool operator<(const GAny &rh) const; // LessThan

    bool operator!=(const GAny &rh) const
    {
        return !((*this) == rh);
    }

    bool operator>(const GAny &rh) const
    {
        return !((*this) == rh || (*this) < rh);
    }

    bool operator<=(const GAny &rh) const
    {
        return ((*this) < rh || (*this) == rh);
    }

    bool operator>=(const GAny &rh) const
    {
        return !((*this) < rh);
    }

    bool operator==(std::nullptr_t) const;

    bool operator!=(std::nullptr_t) const;

    explicit operator bool() const noexcept
    {
        return toBool();
    }

    GAny &operator+=(const GAny &rh)
    {
        *this = *this + rh;
        return *this;
    }

    GAny &operator-=(const GAny &rh)
    {
        *this = *this - rh;
        return *this;
    }

    GAny &operator*=(const GAny &rh)
    {
        *this = *this * rh;
        return *this;
    }

    GAny &operator/=(const GAny &rh)
    {
        *this = *this / rh;
        return *this;
    }

    GAny &operator%=(const GAny &rh)
    {
        *this = *this % rh;
        return *this;
    }

    GAny &operator^=(const GAny &rh)
    {
        *this = *this ^ rh;
        return *this;
    }

    GAny &operator|=(const GAny &rh)
    {
        *this = *this | rh;
        return *this;
    }

    GAny &operator&=(const GAny &rh)
    {
        *this = *this & rh;
        return *this;
    }


    GAny getItem(const GAny &i) const; // GetItem

    bool setItem(const GAny &i, const GAny &v); // SetItem

    bool delItem(const GAny &i); // DelItem

    const GAny operator[](const GAny &key) const;

    GAny &operator[](const GAny &key);

    template<typename T>
    std::enable_if_t<std::is_copy_assignable_v<T>, GAny &> operator=(const T &v)
    {
        if (is<T>()) {
            if (isUserObject()) {
                *as<T>() = v;
            } else {
                (*this) = GAny(v);
            }
        } else {
            (*this) = GAny(v);
        }
        return *this;
    }

    template<typename T>
    std::enable_if_t<!std::is_copy_assignable_v<T>, GAny &> operator=(const T &v)
    {
        (*this) = GAny(v);
        return *this;
    }

    std::string toString() const;

    int32_t toInt32() const;

    int64_t toInt64() const;

    float toFloat() const;

    double toDouble() const;

    bool toBool() const;

    GAny toObject() const;


    friend std::ostream &operator<<(std::ostream &ost, const GAny &self)
    {
        return self.dumpJson(ost, 2);
    }

    std::string toJsonString(int indent = -1) const;

    static GAny parseJson(const std::string &json);

    template<typename T>
    static GAny parseJsonToUserObject(const std::string &json);

    static const GAny Import(const std::string &path);

    static void Export(const std::shared_ptr<GAnyClass> &classObj);

    static bool Load(const std::string &path);

    static bool Unload(const std::string &path);

    static GAny Bind(const GAny &obj, const std::string &methodName);

private:
    template<class T>
    static GAny create(const T &t);

    template<class T>
    static GAny create(T &&t);

    std::ostream &dumpJson(std::ostream &o, int indent = -1, int current_indent = 0) const;

private:
    template<typename>
    friend
    class caster;

    std::shared_ptr<GAnyValue> mVal;
};

inline std::stack<std::vector<GAny>>& GetLocVarCache()
{
    thread_local std::stack<std::vector<GAny>> cache;
    return cache;
}

/// ================ GAnyException ================

class GAnyException
{
public:
    enum class Type : uint8_t
    {
        Error,
        CastAsFailed,
        FuncMismatch
    };

public:
    explicit GAnyException(std::string wt)
        : mType(Type::Error), mWhat(std::move(wt))
    {
    }

    explicit GAnyException(Type type, std::string wt)
        : mType(type), mWhat(std::move(wt))
    {
    }

    Type getType() const noexcept
    {
        return mType;
    }

    const char *what() const noexcept
    {
        return mWhat.c_str();
    }

    const std::string &toString() const noexcept
    {
        return mWhat;
    }

private:
    Type mType;
    std::string mWhat;
};

/// ================ GAnyFunction ================

struct GAnyFuncInfo
{
    std::string doc;
    std::vector<std::string> args;
    std::string returnType;
};

class GAnyFunction
{
public:
    explicit GAnyFunction() = default;

    template<typename Return, typename... Args>
    GAnyFunction(Return (*f)(Args...))
    {
        initialize(f, f);
    }

    template<typename Return, typename... Args>
    GAnyFunction(Return (*f)(Args...) noexcept)
    {
        initialize(f, f);
    }

    template<typename Func>
    GAnyFunction(Func &&f)
    {
        initialize(
            std::forward<Func>(f),
            static_cast<detail::function_signature_t<Func> *>(nullptr));
    }

    template<typename Return, typename Class, typename... Arg>
    GAnyFunction(Return (Class::*f)(Arg...))
    {
        initialize(
            [f](Class *c, Arg... args) -> Return {
                return (c->*f)(args...);
            },
            static_cast<Return (*)(Class *, Arg...)>(nullptr));
    }

    template<typename Return, typename Class, typename... Arg>
    GAnyFunction(Return (Class::*f)(Arg...) const)
    {
        initialize(
            [f](const Class *c, Arg... args) -> Return {
                return (c->*f)(args...);
            },
            static_cast<Return (*)(const Class *, Arg...)>(nullptr));
    }

    template<typename Return, typename Class, typename... Arg>
    GAnyFunction(Return (Class::*f)(Arg...) noexcept)
    {
        initialize(
            [f](Class *c, Arg... args) -> Return {
                return (c->*f)(args...);
            },
            static_cast<Return (*)(Class *, Arg...)>(nullptr));
    }

    template<typename Return, typename Class, typename... Arg>
    GAnyFunction(Return (Class::*f)(Arg...) const noexcept)
    {
        initialize(
            [f](const Class *c, Arg... args) -> Return {
                return (c->*f)(args...);
            },
            static_cast<Return (*)(const Class *, Arg...)>(nullptr));
    }

    static GAnyFunction createVariadicFunction(const std::string &name,
                                               std::function<GAny(const GAny **args, int32_t argc)> func);

    GAny _call(const GAny **args, int32_t argc) const;

    template<typename... Args>
    GAny call(Args... args) const
    {
        const std::initializer_list<GAny> argv = {
            (GAny(std::move(args)))...
        };

        const auto tArgc = static_cast<int32_t>(argv.size());
        const GAny **tArgs = static_cast<const GAny **>(alloca(sizeof(GAny *) * tArgc));
        for (int32_t i = 0; i < tArgc; i++) {
            tArgs[i] = &argv.begin()[i];
        }

        return _call(tArgs, tArgc);
    }

    template<typename Func, typename Return, typename... Args>
    void initialize(Func &&f, Return (*)(Args...));

    template<typename Func, typename Return, typename... Args>
    void initialize(Func &&f, Return (*)(Args...) noexcept);

    template<typename Func, typename Return, typename... Args, size_t... Is>
    std::enable_if_t<std::is_void_v<Return>, GAny>
    call_impl(Func &&f, Return (*)(Args...), const GAny **args, detail::index_sequence<Is...>);

    template<typename Func, typename Return, typename... Args, size_t... Is>
    std::enable_if_t<!std::is_void_v<Return>, GAny>
    call_impl(Func &&f, Return (*)(Args...), const GAny **args, detail::index_sequence<Is...>);

    std::string signature() const;

    bool compareArgs(const GAnyFunction &rFunc) const;

    bool matchingArgv(const GAny **args, int32_t argc) const;

    void setName(const std::string &name)
    {
        mName = name;
    }

    const std::string &getName() const
    {
        return mName;
    }

    void setInfo(GAnyFuncInfo info)
    {
        mInfo = std::move(info);
        mSetInfo = true;
    }

    bool isSetInfo() const
    {
        return mSetInfo;
    }

    GAnyFuncInfo getInfo() const
    {
        return mInfo;
    }

    bool isStatic() const
    {
        return mIsStatic;
    }

    bool isMethod() const
    {
        return mIsMethod;
    }

    bool isVariadicFunction() const
    {
        return mIsVariadicFunc;
    }

    GAny getReturnType() const;

    std::vector<GAny> getArgTypes() const;

    GAny nextOverload() const;

    friend std::ostream &operator<<(std::ostream &sst, const GAnyFunction &self)
    {
        auto strReplace = [](const std::string &str, const std::string &src, const std::string &dst) -> std::string {
            std::string out = str;
            size_t index = 0;
            while (index < out.size() && (index = out.find(src, index)) != std::string::npos) {
                out = out.replace(index, src.size(), dst);
                index += dst.size();
            }
            return out;
        };

        sst << (self.mName.empty() ? "function" : self.mName) << "(...)";
        const GAnyFunction *overload = &self;
        while (overload) {
            sst << "\n";
            if (overload->mSetInfo && !overload->mInfo.doc.empty()) {
                std::string doc = overload->mInfo.doc;
                doc = strReplace(doc, "\n\r", "\n");
                doc = strReplace(doc, "\r\n", "\n");
                doc = strReplace(doc, "\n", "\n  // ");
                sst << "  // " << doc << "\n";
            }
            sst << "  ";
            sst << overload->signature();
            if (!overload->mNext.isFunction()) {
                break;
            }
            overload = overload->mNext.as<GAnyFunction>();
        }
        return sst;
    }

    GAny dump() const;

    void setBoundData(const GAny &data)
    {
        mBoundData = data;
    }

    GAny getBoundData() const
    {
        return mBoundData;
    }

private:
    friend class GAny;

    friend class GAnyClass;

    template<typename>
    friend class gany::Class;

    std::string mName;
    GAnyFuncInfo mInfo;
    bool mSetInfo = false;

    GAny mNext;
    std::vector<GAnyClass *> mArgTypes;

    std::function<GAny(const GAny **args, int32_t argc)> mFunc;
    bool mIsStatic = true;
    bool mIsMethod = false;
    bool mIsVariadicFunc = false;

    GAny mBoundData;
};

/// ================ GAnyTypeInfo ================

class GAnyTypeInfo
{
public:
    explicit GAnyTypeInfo(std::type_index typeIndex);

public:
    std::type_index typeIndex() const
    {
        return mType;
    }

    AnyType anyType() const
    {
        return mAnyType;
    }

    int32_t basicTypeIndex() const
    {
        return mBasicTypeIndex;
    }

    const char *getName() const
    {
        return mType.name();
    }

    std::string getDemangleName() const
    {
        const char *typeName = getName();

#if GX_COMPILER_GCC || (GX_COMPILER_CLANG && !GX_PLATFORM_WINDOWS)
        int status;
        char *realname = abi::__cxa_demangle(typeName, 0, 0, &status);
        std::string result(realname);
        free(realname);
#else
        std::string result(typeName);
#endif
        return result;
    }

    bool isClassGAny() const
    {
        return mBasicTypeIndex == 0;
    }

    bool operator==(const GAnyTypeInfo &rhs) const
    {
        return EqualType(this->typeIndex(), rhs.typeIndex());
    }

    static bool EqualType(const std::type_index &ta, const std::type_index &tb)
    {
#if GX_PLATFORM_OSX
        return strcmp(ta.name(), tb.name()) == 0;
#else
        return ta == tb;
#endif
    }

protected:
    std::type_index mType;
    AnyType mAnyType = AnyType::user_obj_t;
    int32_t mBasicTypeIndex = -1;
};

template<typename T>
class GAnyTypeInfoP : public GAnyTypeInfo
{
public:
    explicit GAnyTypeInfoP()
        : GAnyTypeInfo(typeid(T))
    {
    }
};

template<typename T>
class GAnyTypeInfoP<std::shared_ptr<T> > : public GAnyTypeInfo
{
public:
    explicit GAnyTypeInfoP()
        : GAnyTypeInfo(typeid(T))
    {
    }
};

template<typename T>
class GAnyTypeInfoP<std::unique_ptr<T> > : public GAnyTypeInfo
{
public:
    explicit GAnyTypeInfoP()
        : GAnyTypeInfo(typeid(T))
    {
    }
};

/// ================ GAnyClass ================

class GAnyClass
{
public:
    class GAnyProperty
    {
    public:
        explicit GAnyProperty(std::string name, std::string doc, GAny fGet, GAny fSet)
            : mName(std::move(name)), mDoc(std::move(doc)), mFGet(std::move(fGet)), mFSet(std::move(fSet))
        {
            if (mFGet.isFunction()) {
                const auto &func = mFGet.castAs<GAnyFunction&>();
                if (!func.mIsVariadicFunc && !func.mArgTypes.empty()) {
                    mType = func.mArgTypes[0];
                }
            }
            if (mType == nullptr && mFSet.isFunction()) {
                const auto &func = mFSet.castAs<GAnyFunction&>();
                if (!func.mIsVariadicFunc && func.mArgTypes.size() >= 3) {
                    mType = func.mArgTypes[2];
                }
            }
        }

        GAny dump() const
        {
            GAny dumpObj = GAny::object();
            dumpObj["name"] = mName;
            dumpObj["doc"] = mDoc;
            std::string type;

            if (mType != nullptr) {
                type = mType->getName();
            } else {
                if (mFGet.isFunction()) {
                    const auto *func = mFGet.as<GAnyFunction>();
                    if (func && func->mSetInfo && !func->mInfo.returnType.empty()) {
                        type = func->mInfo.returnType;
                    }
                }
            }
            dumpObj["type"] = type;

            if (mFGet.isFunction()) {
                dumpObj["getter"] = true;
            } else {
                dumpObj["getter"] = false;
            }
            if (mFSet.isFunction()) {
                dumpObj["setter"] = true;
            } else {
                dumpObj["setter"] = false;
            }

            return dumpObj;
        }

    public:
        const std::string &getName() const
        {
            return mName;
        }

        const std::string &getDoc() const
        {
            return mDoc;
        }

        const GAnyClass *getType() const
        {
            return mType;
        }

        bool hasGetter() const
        {
            return mFGet.isFunction();
        }

        bool hasSetter() const
        {
            return mFSet.isFunction();
        }

    private:
        friend class GAnyClass;

        std::string mName;
        std::string mDoc;
        GAny mFGet;
        GAny mFSet;
        GAnyClass *mType = nullptr;
    };

public:
    struct VirtualClass
    {
    };

    GAnyClass(std::string nameSpace, std::string name, std::string doc,
              const GAnyTypeInfo &typeInfo = GAnyTypeInfoP<VirtualClass>());

public:
    GAnyClass &setNameSpace(const std::string &ns);

    GAnyClass &setName(const std::string &name);

    GAnyClass &setDoc(const std::string &doc);

    std::string getNameSpace() const;

    const std::string &getName() const;

    const std::string &getDoc() const;

    const GAnyTypeInfo &getTypeInfo() const;

public:
    GAnyClass &func(const std::string &name, const GAny &function,
                    const GAnyFuncInfo &info = {"::"}, bool isStatic = false);

    GAnyClass &func(MetaFunction metaFunc, const GAny &function,
                    const GAnyFuncInfo &info = {"::"}, bool isStatic = false);

    GAnyClass &staticFunc(const std::string &name, const GAny &function, const GAnyFuncInfo &info = {"::"});

    GAnyClass &staticFunc(MetaFunction metaFunc, const GAny &function, const GAnyFuncInfo &info = {"::"});

    GAnyClass &property(const std::string &name,
                        const GAny &fGet, const GAny &fSet = GAny(),
                        const std::string &doc = "");

    template<typename C, typename D>
    GAnyClass &readWrite(const std::string &name, D C::*pm, const std::string &doc = "")
    {
        const GAny fGet = GAnyFunction([pm](const C &c) -> const D &{
            return c.*pm;
        });
        const GAny fSet = GAnyFunction([pm](C &c, const D &value) {
            c.*pm = value;
        });
        return property(name, fGet, fSet, doc);
    }

    template<typename C, typename D>
    GAnyClass &readOnly(const std::string &name, D C::*pm, const std::string &doc = "")
    {
        const GAny fGet = GAnyFunction([pm](const C &c) -> const D &{
            return c.*pm;
        });
        return property(name, fGet, GAny(), doc);
    }

    GAnyClass &constant(const std::string &name, const GAny &value);

    GAnyClass &defEnum(const std::vector<std::pair<std::string, GAny>> &enumItems);

    template<typename Base, typename ChildType>
    GAnyClass &inherit()
    {
        if (isVirtualClass()) {
            assert(false && "VirtualClass does not support templated inheritance");
            return *this;
        }
        const GAny base = {
            GAnyClass::instance<Base>(),
            GAnyFunction(
                [](ChildType *v) {
                    return static_cast<Base *>(v);
                }),
            GAnyFunction(
                [](std::shared_ptr<ChildType> v) {
                    return std::dynamic_pointer_cast<Base>(v);
                })
        };
        mParents.push_back(base);
        return *this;
    }

    GAnyClass &inherit(const GAny &parent);

public:
    template<typename... Args>
    GAny call(const GAny &inst, const std::string &methodName, Args... args) const
    {
        const std::initializer_list<GAny> argv = {
            (GAny(std::move(args)))...
        };

        const auto tArgc = static_cast<int32_t>(argv.size());
        const auto tArgs = static_cast<const GAny **>(alloca(sizeof(GAny *) * tArgc));
        for (int32_t i = 0; i < tArgc; i++) {
            tArgs[i] = &argv.begin()[i];
        }

        return _call(inst, methodName, tArgs, tArgc);
    }

    GAny _call(const GAny &inst, const std::string &methodName, const GAny **args, int32_t argc) const;

    /**
     * @brief Attempt to convert to the base class type.
     * @param targetClass
     * @param inst          Current instance
     * @return
     */
    GAny castToBase(const GAnyClass &targetClass, const GAny &inst);


    GAny _new(const GAny **args, int32_t argc) const;

    GAny _new(std::vector<GAny> &args) const;

    template<typename... Args>
    GAny _new(Args... args) const;

public:
    template<typename T>
    static std::shared_ptr<GAnyClass> instance()
    {
        if constexpr (std::is_reference_v<T>) {
            typedef std::remove_const_t<std::remove_reference_t<T> > RmConstRefType;
            return _instance(GAnyTypeInfoP<RmConstRefType>());
        }

        if constexpr (std::is_pointer_v<T>) {
            typedef std::remove_const_t<std::remove_pointer_t<T> > RmConstPtr;
            return _instance(GAnyTypeInfoP<RmConstPtr>());
        }

        return _instance(GAnyTypeInfoP<T>());
    }

    template<typename T>
    static std::shared_ptr<GAnyClass> Class()
    {
        return instance<T>();
    }

    static std::shared_ptr<GAnyClass> Class(std::string nameSpace, std::string name, std::string doc)
    {
        return std::shared_ptr<GAnyClass>(GX_NEW(GAnyClass, std::move(nameSpace), std::move(name), std::move(doc)));
    }

    static std::shared_ptr<GAnyClass> createFromGAnyObject(GAny object);

    void swap(GAnyClass &b) noexcept;

    GAny dump() const;

    friend std::ostream &operator<<(std::ostream &ost, const GAnyClass &rh)
    {
        ost << rh.dump();
        return ost;
    }

    friend bool operator==(const GAnyClass &lh, const GAnyClass &rh)
    {
        if (lh.mTypeInfo.anyType() == rh.mTypeInfo.anyType() && lh.mTypeInfo.anyType() != AnyType::user_obj_t) {
            return true;
        }

        if (lh.mHash != rh.mHash) {
            return false;
        }
        return lh.mNameSpace == rh.mNameSpace && lh.mName == rh.mName;
    }

    friend bool operator!=(const GAnyClass &lh, const GAnyClass &rh)
    {
        return !(lh == rh);
    }

    bool isVirtualClass() const;

    std::vector<GAny> getParents() const;

    bool containsMember(const std::string &name, bool checkBase) const;

    bool containsMember(MetaFunction func, bool checkBase) const
    {
        return containsMember(MetaFunctionNames[static_cast<size_t>(func)], checkBase);
    }

    GAny findMember(const std::string &name, bool checkBase) const;

    GAny findMember(MetaFunction func, bool checkBase) const
    {
        return findMember(MetaFunctionNames[static_cast<size_t>(func)], checkBase);
    }

    std::vector<std::pair<std::string, GAny> > getMembers(bool checkBase) const;

    std::vector<std::pair<std::string, GAny> > getConstants() const;

private:
    void makeConstructor(GAny pFunc);

    GAny *getAttr(const std::string &name) const;

    GAny *setAttr(const std::string &name, const GAny &attr);

    GAny getItem(const GAny &key, const GAny &inst) const;

    bool setItem(const GAny &key, const GAny &inst, const GAny &v, bool justProperty = false);

    void updateHash();

private:
    static std::shared_ptr<GAnyClass> _instance(GAnyTypeInfo typeInfo);

private:
    std::string mNameSpace;
    std::string mName;
    std::string mDoc;
    GAnyTypeInfo mTypeInfo;
    bool mIsVirtualClass = false;
    size_t mHash = 0;

    std::unordered_map<std::string, int32_t> mAttrMap;
    mutable std::vector<std::pair<std::string, GAny> > mAttrs;

    std::unordered_map<std::string, int32_t> mConstantMap;
    std::vector<std::pair<std::string, GAny> > mConstants;

    GAny mInitFn;
    GAny mGetItemFn;
    GAny mSetItemFn;

    std::vector<GAny> mParents;

    friend class GAny;

    friend class GAnyFunction;

    template<typename>
    friend class gany::Class;

    template<typename>
    friend class caster;
};

/// ================ Class ================

namespace gany
{
template<typename C>
class Class
{
public:
    /**
     * 构造函数
     * @param nameSpace 名字空间
     * @param name      类型名称
     * @param doc       注释文档
     */
    Class(const std::string &nameSpace, const std::string &name, const std::string &doc)
        : mClazz(GAnyClass::Class<C>())
    {
        mClazz->setNameSpace(nameSpace);
        mClazz->setName(name);
        mClazz->setDoc(doc);
        GAny::Export(mClazz);
    }

public:
    Class &func(const std::string &name, const GAny &function, const GAnyFuncInfo &info = {"::"})
    {
        mClazz->func(name, function, info);
        return *this;
    }

    Class &func(MetaFunction metaFunc, const GAny &function, const GAnyFuncInfo &info = {"::"})
    {
        mClazz->func(MetaFunctionNames[static_cast<size_t>(metaFunc)], function, info);
        return *this;
    }

    Class &staticFunc(const std::string &name, const GAny &function, const GAnyFuncInfo &info = {"::"})
    {
        mClazz->func(name, function, info, true);
        return *this;
    }

    Class &staticFunc(MetaFunction metaFunc, const GAny &function, const GAnyFuncInfo &info = {"::"})
    {
        mClazz->func(MetaFunctionNames[static_cast<size_t>(metaFunc)], function, info, true);
        return *this;
    }

    template<typename BaseType>
    Class &inherit()
    {
        mClazz->inherit<BaseType, C>();
        return *this;
    }

    template<typename... Args>
    Class &construct(const GAnyFuncInfo &info = {"::"})
    {
        return staticFunc(MetaFunction::Init, [](Args... args) {
            return GAny::New<C>(std::forward<Args>(args)...);
        }, info);
    }

    Class &property(const std::string &name,
                    const GAny &fGet, const GAny &fSet = GAny(),
                    const std::string &doc = "")
    {
        mClazz->property(name, fGet, fSet, doc);
        return *this;
    }

    template<typename E, typename D>
    Class &readWrite(const std::string &name, D E::*pm, const std::string &doc = "")
    {
        static_assert(std::is_base_of_v<E, C>, "readWrite requires a class member (or base class member)");

        const GAny fGet = GAnyFunction([pm](const C &c) -> const D &{
            return c.*pm;
        });
        const GAny fSet = GAnyFunction([pm](C &c, const D &value) {
            c.*pm = value;
        });
        return property(name, fGet, fSet, doc);
    }

    template<typename E, typename D>
    Class &readOnly(const std::string &name, D E::*pm, const std::string &doc = "")
    {
        static_assert(std::is_base_of_v<E, C>, "readOnly requires a class member (or base class member)");

        const GAny fGet = GAnyFunction([pm](const C &c) -> const D &{
            return c.*pm;
        });
        return property(name, fGet, GAny(), doc);
    }

    Class &constant(const std::string &name, const GAny &value)
    {
        mClazz->constant(name, value);
        return *this;
    }

    Class &defEnum(const std::vector<std::pair<std::string, GAny>> &enumItems)
    {
        mClazz->defEnum(enumItems);
        return *this;
    }

private:
    std::shared_ptr<GAnyClass> mClazz;
};
}

/// ================================================================================================

class GAnyValueVoid : public GAnyValue
{
public:
    GAnyValueVoid() = default;

    ~GAnyValueVoid() override = default;

public:
    const void *as(const TypeID &tp) const override
    {
        if (GAnyTypeInfo::EqualType(tp, typeid(void))) {
            return this;
        }
        return nullptr;
    }

    const void *ptr() const override
    {
        return this;
    }

    GAnyClass *classObject() const override
    {
        static GAnyClass *clazz = nullptr;
        return clazz ? clazz : clazz = GAnyClass::instance<void>().get();
    }

    size_t length() const override
    {
        return 0;
    }
};

template<typename T>
class GAnyValueP : public GAnyValue
{
public:
    explicit GAnyValueP(const T &v)
        : var(v)
    {
    }

    explicit GAnyValueP(T &&v)
        : var(std::move(v))
    {
    }

    const void *as(const TypeID &tp) const override
    {
        if (GAnyTypeInfo::EqualType(tp, typeid(T))) {
            return &var;
        }
        return nullptr;
    }

    const void *ptr() const override
    {
        return &var;
    }

    GAnyClass *classObject() const override
    {
        static GAnyClass *clazz = nullptr;
        return clazz ? clazz : clazz = GAnyClass::instance<T>().get();
    }

    void clone(GAny &v) const override
    {
        if constexpr (std::is_copy_constructible_v<T>) {
            v = var;
        }
    }

public:
    T var;
};


template<>
class GAnyValueP<GAny> : public GAnyValue
{
private:
    GAnyValueP(const GAny &)
    {
    }
};


template<>
class GAnyValueP<float> : public GAnyValue
{
public:
    explicit GAnyValueP(float v)
        : var(v)
    {
    }

    const void *as(const TypeID &tp) const override
    {
        if (GAnyTypeInfo::EqualType(tp, typeid(float))) {
            return &var;
        }
        return nullptr;
    }

    const void *ptr() const override
    {
        return &var;
    }

    GAnyClass *classObject() const override
    {
        static GAnyClass *clazz = nullptr;
        return clazz ? clazz : clazz = GAnyClass::instance<float>().get();
    }

    void clone(GAny &v) const override
    {
        v = var;
    }

public:
    float var;
};


template<>
class GAnyValueP<double> : public GAnyValue
{
public:
    explicit GAnyValueP(double v)
        : var(v)
    {
    }

    const void *as(const TypeID &tp) const override
    {
        if (GAnyTypeInfo::EqualType(tp, typeid(double))) {
            return &var;
        }
        return nullptr;
    }

    const void *ptr() const override
    {
        return &var;
    }

    GAnyClass *classObject() const override
    {
        static GAnyClass *clazz = nullptr;
        return clazz ? clazz : clazz = GAnyClass::instance<double>().get();
    }

    void clone(GAny &v) const override
    {
        v = var;
    }

public:
    double var;
};


template<typename T>
class GAnyValueP<std::shared_ptr<T> > : public GAnyValue
{
public:
    explicit GAnyValueP(const std::shared_ptr<T> &v)
        : var(v)
    {
    }

    explicit GAnyValueP(std::shared_ptr<T> &&v)
        : var(std::move(v))
    {
    }

    GAnyClass *classObject() const override
    {
        static GAnyClass *clazz = nullptr;
        return clazz ? clazz : clazz = GAnyClass::instance<T>().get();
    }

    void clone(GAny &v) const override
    {
        if constexpr (std::is_copy_constructible_v<T>) {
            v = *var;
        }
    }

    const void *as(const TypeID &tp) const override
    {
        if (GAnyTypeInfo::EqualType(tp, typeid(T))) {
            return var.get();
        }
        if (GAnyTypeInfo::EqualType(tp, typeid(std::shared_ptr<T>))) {
            return &var;
        }
        return nullptr;
    }

    const void *ptr() const override
    {
        return var.get();
    }

    bool isSharedPtr() const override
    {
        return true;
    }

protected:
    std::shared_ptr<T> var;
};


template<typename T>
class GAnyValueP<std::unique_ptr<T> > : public GAnyValue
{
public:
    explicit GAnyValueP(std::unique_ptr<T> &&v)
        : var(std::move(v))
    {
    }

    GAnyClass *classObject() const override
    {
        static GAnyClass *clazz = nullptr;
        return clazz ? clazz : clazz = GAnyClass::instance<T>().get();
    }

    const void *as(const TypeID &tp) const override
    {
        if (GAnyTypeInfo::EqualType(tp, typeid(T))) {
            return var.get();
        }
        if (GAnyTypeInfo::EqualType(tp, typeid(std::unique_ptr<T>))) {
            return &var;
        }
        return nullptr;
    }

    const void *ptr() const override
    {
        return var.get();
    }

    void clone(GAny &v) const override
    {
        if constexpr (std::is_copy_constructible_v<T>) {
            v = *var;
        }
    }

protected:
    std::unique_ptr<T> var;
};


template<typename T>
class GAnyValueP<T *> : public GAnyValue
{
public:
    explicit GAnyValueP(T *v)
        : var(v)
    {
    }

    GAnyClass *classObject() const override
    {
        static GAnyClass *clazz = nullptr;
        return clazz ? clazz : clazz = GAnyClass::instance<T>().get();
    }

    const void *as(const TypeID &tp) const override
    {
        if (GAnyTypeInfo::EqualType(tp, typeid(T))) {
            return var;
        }
        if (GAnyTypeInfo::EqualType(tp, typeid(T *))) {
            return &var;
        }
        return nullptr;
    }

    const void *ptr() const override
    {
        return var;
    }

    void clone(GAny &v) const override
    {
        if constexpr (std::is_copy_constructible_v<T>) {
            v = *var;
        }
    }

protected:
    T *var;
};


class GAnyObject : public GAnyValueP<std::unordered_map<std::string, GAny> >
{
public:
    GAnyObject(const std::map<std::string, GAny> &o)
        : GAnyValueP<std::unordered_map<std::string, GAny> >({})
    {
        var.insert(o.begin(), o.end());
    }

    GAnyObject(const std::unordered_map<std::string, GAny> &o)
        : GAnyValueP<std::unordered_map<std::string, GAny> >(o)
    {
    }

    GAnyObject(std::unordered_map<std::string, GAny> &&o)
        : GAnyValueP<std::unordered_map<std::string, GAny> >(o)
    {
    }

public:
    const void *as(const TypeID &tp) const override
    {
        if (GAnyTypeInfo::EqualType(tp, typeid(GAnyObject))) {
            return this;
        }
        if (GAnyTypeInfo::EqualType(tp, typeid(std::unordered_map<std::string, GAny>))) {
            return &var;
        }
        return nullptr;
    }

    size_t length() const override
    {
        return var.size();
    }

    GAnyClass *classObject() const override
    {
        return clazz ? clazz : clazz = GAnyClass::instance<GAnyObject>().get();
    }

    GAny operator[](const std::string &key) const
    {
        std::lock_guard locker(lock);
        const auto it = var.find(key);
        if (it == var.end()) {
            return GAny::undefined();
        }
        return it->second;
    }

    void set(const std::string &key, const GAny &value)
    {
        std::lock_guard locker(lock);
        const auto it = var.find(key);
        if (it == var.end()) {
            var.insert(std::make_pair(key, value));
        } else {
            it->second = value;
        }
    }

    void erase(const std::string &key)
    {
        std::lock_guard locker(lock);
        const auto it = var.find(key);
        if (it != var.end()) {
            var.erase(it);
        }
    }

    bool contains(const std::string &key) const
    {
        std::lock_guard locker(lock);
        return var.find(key) != var.end();
    }

    void clear()
    {
        std::lock_guard locker(lock);
        var.clear();
    }

    void clone(GAny &v) const override
    {
        v = GAny::object();

        for (const auto &i: var) {
            v.setItem(i.first, i.second.clone());
        }
    }

public:
    mutable std::mutex lock;
    mutable GAnyClass *clazz = nullptr;
};


class GAnyArray : public GAnyValueP<std::vector<GAny> >
{
public:
    GAnyArray(const std::vector<GAny> &v)
        : GAnyValueP<std::vector<GAny> >(v)
    {
    }

    GAnyArray(std::vector<GAny> &&v)
        : GAnyValueP<std::vector<GAny> >(std::move(v))
    {
    }

public:
    GAnyClass *classObject() const override
    {
        static GAnyClass *clazz = nullptr;
        return clazz ? clazz : clazz = GAnyClass::instance<GAnyArray>().get();
    }

    size_t length() const override
    {
        return var.size();
    }

    const void *as(const TypeID &tp) const override
    {
        if (GAnyTypeInfo::EqualType(tp, typeid(GAnyArray))) {
            return this;
        }
        if (GAnyTypeInfo::EqualType(tp, typeid(std::vector<GAny>))) {
            return &var;
        }
        return nullptr;
    }

    GAny operator[](int32_t i) const
    {
        std::lock_guard locker(lock);
        if (i < var.size()) {
            return var[i];
        }
        return GAny::undefined();
    }

    void push_back(const GAny &v)
    {
        std::lock_guard locker(lock);
        var.push_back(v);
    }

    void set(int32_t i, const GAny &v)
    {
        std::lock_guard locker(lock);
        if (i < var.size()) {
            var[i] = v;
        }
    }

    void erase(int32_t i)
    {
        std::lock_guard locker(lock);
        if (i >= 0 && i < var.size()) {
            var.erase(var.begin() + i);
        }
    }

    bool contains(const GAny &v) const
    {
        std::lock_guard locker(lock);
        for (const auto &i: var) {
            if (i == v) {
                return true;
            }
        }
        return false;
    }

    void clear()
    {
        std::lock_guard locker(lock);
        var.clear();
    }

    void clone(GAny &v) const override
    {
        v = GAny::array();

        for (const auto &i: var) {
            v.pushBack(i.clone());
        }
    }

public:
    mutable std::mutex lock;
};

/// ================================================================================================

template<typename T>
class caster
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<T>()) {
            return var;
        }
        if (var.isObject()) {
            return objectCastAs(var);
        }

        return GAny::undefined();
    }

    template<typename T1>
    static std::enable_if_t<detail::is_callable<T1>::value, GAny> castFrom(const T1 &func)
    {
        return GAnyFunction(func);
    }

    template<typename T1>
    static std::enable_if_t<!detail::is_callable<T1>::value, GAny> castFrom(const T1 &var)
    {
        static_assert(!std::is_same_v<T1, GAnyValue>, "Constructing from GAnyValue should not enter this call path.");
        return GAny::create(var);
    }

    template<typename T1>
    static std::enable_if_t<!detail::is_callable<T1>::value, GAny> castFrom(T1 &&var)
    {
        static_assert(!std::is_same_v<T1, GAnyValue>, "Constructing from GAnyValue should not enter this call path.");
        return GAny::create(std::forward<T1>(var));
    }

private:
    static GAny objectCastAsClass(const GAnyClass *clazz, const GAny &object)
    {
        if (!object.isObject()) {
            return GAny::undefined();
        }
        GAny obj = clazz->_new();
        if (!obj.isUserObject()) {
            return GAny::undefined();
        }

        const GAny fromObjFunc = clazz->findMember(MetaFunction::FromObject, false);
        if (fromObjFunc.isFunction()) {
            obj.call(MetaFunction::FromObject, object);
            return obj;
        }

        object.call("forEach", [&](const std::string &name, const GAny &v) {
            const GAny member = clazz->findMember(name, true);
            if (member.isProperty() && member.as<GAnyClass::GAnyProperty>()->hasSetter()) {
                obj.setItem(name, v);
            }
        });
        return obj;
    }

    template<typename Ty = T>
    static GAny objectCastAs(const GAny &object)
    {
        auto clazz = GAnyClass::instance<Ty>();
        if (!clazz) {
            return GAny::undefined();
        }
        return objectCastAsClass(clazz.get(), object);
    }
};

template<>
class caster<bool>
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<bool>()) {
            return var;
        }

        return var.toBool();
    }

    static GAny castFrom(bool v)
    {
        return GAny::create(v);
    }
};

template<>
class caster<int32_t>
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<int32_t>()) {
            return var;
        }
        return var.toInt32();
    }

    static GAny castFrom(int32_t v)
    {
        return GAny::create(v);
    }
};

template<>
class caster<uint32_t>
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<uint32_t>()) {
            return var;
        }
        return static_cast<uint32_t>(var.toInt32());
    }

    static GAny castFrom(uint32_t v)
    {
        return GAny::create(v);
    }
};

template<>
class caster<int64_t>
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<int64_t>()) {
            return var;
        }
        return var.toInt64();
    }

    static GAny castFrom(int64_t v)
    {
        return GAny::create(v);
    }
};

template<>
class caster<uint64_t>
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<uint64_t>()) {
            return var;
        }
        return static_cast<uint64_t>(var.toInt64());
    }

    static GAny castFrom(uint64_t v)
    {
        return GAny::create(v);
    }
};

template<>
class caster<int16_t>
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<int16_t>()) {
            return var;
        }
        return static_cast<int16_t>(var.toInt32());
    }

    static GAny castFrom(int16_t v)
    {
        return GAny::create(v);
    }
};

template<>
class caster<uint16_t>
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<uint16_t>()) {
            return var;
        }
        return static_cast<uint16_t>(var.toInt32());
    }

    static GAny castFrom(uint16_t v)
    {
        return GAny::create(v);
    }
};

template<>
class caster<int8_t>
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<int8_t>()) {
            return var;
        }
        return static_cast<int8_t>(var.toInt32());
    }

    static GAny castFrom(int8_t v)
    {
        return GAny::create(v);
    }
};

template<>
class caster<uint8_t>
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<uint8_t>()) {
            return var;
        }
        return static_cast<uint8_t>(var.toInt32());
    }

    static GAny castFrom(uint8_t v)
    {
        return GAny::create(v);
    }
};

template<>
class caster<float>
{
public:
    static GAny castTo(const GAny &var)
    {
        return var.toFloat();
    }

    static GAny castFrom(float v)
    {
        return GAny::create(v);
    }
};

template<>
class caster<double>
{
public:
    static GAny castTo(const GAny &var)
    {
        return var.toDouble();
    }

    static GAny castFrom(double v)
    {
        return GAny::create(v);
    }
};

template<>
class caster<std::string>
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<std::string>()) {
            return var;
        }

        return var.toString();
    }

    static GAny castFrom(const std::string &v)
    {
        return GAny::create(v);
    }

    static GAny castFrom(std::string &&v)
    {
        return GAny::create(std::move(v));
    }
};

template<typename T>
class caster<std::vector<T> >
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<std::vector<T> >()) {
            return var;
        }

        GAny tVar = var;
        if (!tVar.isArray()) {
            tVar = tVar.toObject();
        }

        if (!tVar.isArray()) {
            return GAny::undefined();
        }

        std::vector<T> ret;
        ret.reserve(tVar.length());

        const auto *arrayVar = tVar.as<GAnyArray>();

        std::lock_guard locker(arrayVar->lock);
        for (const GAny &v: arrayVar->var) {
            try {
                ret.push_back(const_cast<GAny &>(v).castAs<T>());
            } catch (GAnyException &) {
            }
        }

        return GAny::create(ret);
    }

    static GAny castFrom(const std::vector<T> &var)
    {
        static_assert(!std::is_same_v<T, GAnyValue>, "Constructing from GAnyValue should not enter this call path.");
        return GAny::fromValue(std::make_shared<GAnyArray>(std::vector<GAny>(var.begin(), var.end())));
    }
};

template<>
class caster<std::vector<GAny> >
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<std::vector<GAny> >()) {
            return var;
        }
        GAny tVar = var.toObject();
        if (tVar.isArray()) {
            return tVar;
        }
        return {};
    }

    static GAny castFrom(const std::vector<GAny> &var)
    {
        return GAny::fromValue(std::make_shared<GAnyArray>(var));
    }

    static GAny castFrom(std::vector<GAny> &&var)
    {
        return GAny::fromValue(std::make_shared<GAnyArray>(std::move(var)));
    }
};

template<typename T>
class caster<std::list<T> >
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<std::list<T> >()) {
            return var;
        }

        GAny tVar = var;
        if (!tVar.isArray()) {
            tVar = tVar.toObject();
        }

        if (!tVar.isArray()) {
            return GAny::undefined();
        }

        std::list<T> ret;

        const auto *arrayVar = tVar.as<GAnyArray>();

        std::lock_guard locker(arrayVar->lock);
        for (const GAny &v: arrayVar->var) {
            try {
                ret.push_back(const_cast<GAny &>(v).castAs<T>());
            } catch (GAnyException &) {
            }
        }

        return GAny::create(ret);
    }

    static GAny castFrom(const std::list<T> &var)
    {
        static_assert(!std::is_same_v<T, GAnyValue>, "Constructing from GAnyValue should not enter this call path.");
        return GAny::fromValue(std::make_shared<GAnyArray>(std::vector<GAny>(var.begin(), var.end())));
    }
};

template<typename T, size_t N>
class caster<T[N]>
{
public:
    static GAny castFrom(const T *var)
    {
        if constexpr (std::is_same_v<T, char>) {
            return GAny(std::string(var));
        }
        std::vector<T> array(var, var + N);
        return GAny(array);
    }
};

template<typename T>
class caster<std::map<std::string, T> >
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<std::map<std::string, T> >()) {
            return var;
        }

        GAny tVar = var;
        if (!tVar.isObject()) {
            tVar = tVar.toObject();
        }

        if (!tVar.isObject()) {
            return GAny::undefined();
        }

        std::map<std::string, T> ret;

        const auto *objVar = tVar.as<GAnyObject>();

        std::lock_guard locker(objVar->lock);
        for (const auto &v: objVar->var) {
            try {
                ret.insert(std::make_pair(v.first, const_cast<GAny &>(v.second).castAs<T>()));
            } catch (GAnyException &) {
            }
        }

        return GAny::create(ret);
    }

    static GAny castFrom(const std::map<std::string, T> &var)
    {
        static_assert(!std::is_same_v<T, GAnyValue>, "Constructing from GAnyValue should not enter this call path.");
        return GAny::fromValue(std::make_shared<GAnyObject>(std::unordered_map<std::string, GAny>(var.begin(), var.end())));
    }
};

template<typename T>
class caster<std::unordered_map<std::string, T> >
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<std::unordered_map<std::string, T> >()) {
            return var;
        }

        GAny tVar = var;
        if (!tVar.isObject()) {
            tVar = tVar.toObject();
        }

        if (!tVar.isObject()) {
            return GAny::undefined();
        }

        std::unordered_map<std::string, T> ret;

        const auto *objVar = tVar.as<GAnyObject>();

        std::lock_guard locker(objVar->lock);
        for (const auto &v: objVar->var) {
            try {
                ret.insert(std::make_pair(v.first, const_cast<GAny &>(v.second).castAs<T>()));
            } catch (GAnyException &) {
            }
        }

        return GAny::create(ret);
    }

    static GAny castFrom(const std::unordered_map<std::string, T> &var)
    {
        static_assert(!std::is_same_v<T, GAnyValue>, "Constructing from GAnyValue should not enter this call path.");
        return GAny::fromValue(std::make_shared<GAnyObject>(std::unordered_map<std::string, GAny>(var.begin(), var.end())));
    }
};

template<typename T>
class caster<std::shared_ptr<T> >
{
    using SType = std::shared_ptr<T>;

public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<SType>()) {
            return var;
        }
        if (var.isNull()) {
            return GAny::create(SType());
        }

        return GAny::undefined();
    }

    static GAny castFrom(const std::shared_ptr<T> &var)
    {
        static_assert(!std::is_same_v<T, GAnyValue>, "Constructing from GAnyValue should not enter this call path.");
        if (!var) {
            return GAny::null();
        }
        return GAny::create(var);
    }
};

template<>
class caster<std::nullptr_t>
{
public:
    static GAny castTo(const GAny &var)
    {
        return var;
    }

    static GAny castFrom(std::nullptr_t)
    {
        return GAny::null();
    }
};

template<>
class caster<const char *>
{
public:
    static GAny castTo(const GAny &var)
    {
        if (var.is<std::string>()) {
            return GAny::create(var.as<std::string>()->c_str());
        }
        return {};
    }

    static GAny castFrom(const char *v)
    {
        return std::string(v);
    }
};

#if !defined(__LP64__) || (GX_PLATFORM_OSX && GX_CPU_ARM)

template<>
class caster<long>
{
public:
    static GAny castTo(const GAny &var)
    {
        return GAny::create(static_cast<long>(var.toInt64()));
    }

    static GAny castFrom(long v)
    {
        return GAny::create(static_cast<int64_t>(v));
    }
};

template<>
class caster<unsigned long>
{
public:
    static GAny castTo(const GAny &var)
    {
        return GAny::create(static_cast<unsigned long>(var.toInt64()));
    }

    static GAny castFrom(long v)
    {
        return GAny::create(static_cast<int64_t>(v));
    }
};

#endif

/// ================ JsonParser ================

namespace json
{
class JsonParser
{
private:
    struct Position
    {
        size_t line = 1;
        size_t column = 1;
    };

    explicit JsonParser() = default;

public:
    static GAny parseJson(const std::string &json)
    {
        size_t idx = 0;
        Position pos;
        skipWhitespace(json, idx, pos);

        if (idx >= json.size())
            throw GAnyException(errorMessage(pos, "empty json document"));

        GAny result = parseValue(json, idx, pos);

        skipWhitespace(json, idx, pos);
        if (idx != json.size())
            throw GAnyException(errorMessage(pos, "unexpected trailing characters after root element"));

        return result;
    }

private:
    static void advance(const std::string &json, size_t &idx, Position &pos)
    {
        if (json[idx] == '\n') {
            ++pos.line;
            pos.column = 1;
        } else {
            ++pos.column;
        }
        ++idx;
    }

    static bool isWhitespace(char c)
    {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
    }

    static bool isDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    static void skipWhitespace(const std::string &json, size_t &idx, Position &pos)
    {
        while (idx < json.size() && isWhitespace(json[idx])) {
            advance(json, idx, pos);
        }
    }

    static std::string parseString(const std::string &json, size_t &idx, Position &pos)
    {
        skipWhitespace(json, idx, pos);
        if (json[idx] != '"')
            throw GAnyException(errorMessage(pos, "expected '\"' at start of string"));
        advance(json, idx, pos);

        std::string result;
        while (idx < json.size() && json[idx] != '"') {
            if (json[idx] == '\\') {
                advance(json, idx, pos);
                if (idx >= json.size())
                    throw GAnyException(errorMessage(pos, "incomplete escape sequence"));

                const char escapeChar = json[idx];
                if (escapeChar == '"')
                    result += '"';
                else if (escapeChar == '\\')
                    result += '\\';
                else if (escapeChar == '/')
                    result += '/';
                else if (escapeChar == 'n')
                    result += '\n';
                else if (escapeChar == 't')
                    result += '\t';
                else if (escapeChar == 'r')
                    result += '\r';
                else if (escapeChar == 'b')
                    result += '\b';
                else if (escapeChar == 'f')
                    result += '\f';
                else if (escapeChar == 'u') {
                    advance(json, idx, pos);
                    if (idx + 3 >= json.size())
                        throw GAnyException(errorMessage(pos, "incomplete unicode escape"));

                    unsigned int high = 0;
                    for (int i = 0; i < 4; ++i) {
                        char ch = json[idx];
                        high <<= 4;
                        if (ch >= '0' && ch <= '9')
                            high |= (ch - '0');
                        else if (ch >= 'a' && ch <= 'f')
                            high |= (ch - 'a' + 10);
                        else if (ch >= 'A' && ch <= 'F')
                            high |= (ch - 'A' + 10);
                        else
                            throw GAnyException(errorMessage(pos, "invalid character in unicode escape"));
                        advance(json, idx, pos);
                    }

                    if (high >= 0xD800 && high <= 0xDBFF) {
                        // High surrogate
                        if (idx + 5 >= json.size() || json[idx] != '\\' || json[idx + 1] != 'u') {
                            throw GAnyException(errorMessage(pos, "expected low surrogate pair after high surrogate"));
                        }
                        advance(json, idx, pos); // skip '\'
                        advance(json, idx, pos); // skip 'u'

                        unsigned int low = 0;
                        for (int i = 0; i < 4; ++i) {
                            char ch = json[idx];
                            low <<= 4;
                            if (ch >= '0' && ch <= '9')
                                low |= (ch - '0');
                            else if (ch >= 'a' && ch <= 'f')
                                low |= (ch - 'a' + 10);
                            else if (ch >= 'A' && ch <= 'F')
                                low |= (ch - 'A' + 10);
                            else
                                throw GAnyException(errorMessage(pos, "invalid character in unicode escape"));
                            advance(json, idx, pos);
                        }
                        if (low < 0xDC00 || low > 0xDFFF) {
                            throw GAnyException(errorMessage(pos, "invalid low surrogate value"));
                        }

                        unsigned int codepoint = 0x10000 + (((high - 0xD800) << 10) | (low - 0xDC00));
                        result += static_cast<char>(0xF0 | (codepoint >> 18));
                        result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
                        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                        result += static_cast<char>(0x80 | (codepoint & 0x3F));
                    } else if (high >= 0xDC00 && high <= 0xDFFF) {
                        throw GAnyException(errorMessage(pos, "unexpected low surrogate without preceding high surrogate"));
                    } else {
                        if (high <= 0x7F) {
                            result += static_cast<char>(high);
                        } else if (high <= 0x7FF) {
                            result += static_cast<char>(0xC0 | (high >> 6));
                            result += static_cast<char>(0x80 | (high & 0x3F));
                        } else {
                            result += static_cast<char>(0xE0 | (high >> 12));
                            result += static_cast<char>(0x80 | ((high >> 6) & 0x3F));
                            result += static_cast<char>(0x80 | (high & 0x3F));
                        }
                    }
                    continue;
                } else {
                    throw GAnyException(errorMessage(pos, "invalid escape character"));
                }
            } else {
                if (json[idx] == '\n' || json[idx] == '\r')
                    throw GAnyException(errorMessage(pos, "unexpected line break in string"));
                result += json[idx];
            }
            advance(json, idx, pos);
        }
        if (idx >= json.size() || json[idx] != '"')
            throw GAnyException(errorMessage(pos, "expected '\"' at end of string"));
        advance(json, idx, pos);
        return result;
    }

    static GAny parseNumber(const std::string &json, size_t &idx, Position &pos)
    {
        skipWhitespace(json, idx, pos);
        const size_t start = idx;

        if (json[idx] == '-')
            advance(json, idx, pos);

        if (idx >= json.size() || !isDigit(json[idx]))
            throw GAnyException(errorMessage(pos, "invalid number format"));

        if (json[idx] == '0') {
            advance(json, idx, pos);
            if (idx < json.size() && isDigit(json[idx]))
                throw GAnyException(errorMessage(pos, "leading zeros are not allowed"));
        } else {
            while (idx < json.size() && isDigit(json[idx])) {
                advance(json, idx, pos);
            }
        }

        if (idx < json.size() && json[idx] == '.') {
            advance(json, idx, pos);
            if (idx >= json.size() || !isDigit(json[idx]))
                throw GAnyException(errorMessage(pos, "at least one digit required after decimal point"));
            while (idx < json.size() && isDigit(json[idx])) {
                advance(json, idx, pos);
            }
        }

        if (idx < json.size() && (json[idx] == 'e' || json[idx] == 'E')) {
            advance(json, idx, pos);
            if (idx < json.size() && (json[idx] == '+' || json[idx] == '-')) {
                advance(json, idx, pos);
            }
            if (idx >= json.size() || !isDigit(json[idx]))
                throw GAnyException(errorMessage(pos, "at least one digit required after exponent"));
            while (idx < json.size() && isDigit(json[idx])) {
                advance(json, idx, pos);
            }
        }

        try {
            const std::string numStr = json.substr(start, idx - start);
            if (numStr.find('.') != std::string::npos || numStr.find('e') != std::string::npos || numStr.find('E') != std::string::npos)
                return std::stod(numStr);
            return std::stoll(numStr);
        } catch (...) {
            throw GAnyException(errorMessage(pos, "invalid number format"));
        }
    }

    static bool parseBoolean(const std::string &json, size_t &idx, Position &pos)
    {
        skipWhitespace(json, idx, pos);
        if (json.substr(idx, 4) == "true") {
            for (int i = 0; i < 4; ++i)
                advance(json, idx, pos);
            return true;
        }
        if (json.substr(idx, 5) == "false") {
            for (int i = 0; i < 5; ++i)
                advance(json, idx, pos);
            return false;
        }
        throw GAnyException(errorMessage(pos, "invalid boolean value"));
    }

    static void parseNull(const std::string &json, size_t &idx, Position &pos)
    {
        skipWhitespace(json, idx, pos);
        if (json.substr(idx, 4) == "null") {
            for (int i = 0; i < 4; ++i)
                advance(json, idx, pos);
        } else {
            throw GAnyException(errorMessage(pos, "invalid null value"));
        }
    }

    static GAny parseValue(const std::string &json, size_t &idx, Position &pos)
    {
        skipWhitespace(json, idx, pos);
        if (idx >= json.size())
            throw GAnyException(errorMessage(pos, "unexpected end of input"));

        const char currentChar = json[idx];
        if (currentChar == '"')
            return GAny(parseString(json, idx, pos));
        if (currentChar == '-' || isDigit(currentChar))
            return GAny(parseNumber(json, idx, pos));
        if (currentChar == '{')
            return GAny(parseObject(json, idx, pos));
        if (currentChar == '[')
            return GAny(parseArray(json, idx, pos));
        if (currentChar == 't' || currentChar == 'f')
            return GAny(parseBoolean(json, idx, pos));
        if (currentChar == 'n') {
            parseNull(json, idx, pos);
            return GAny::null();
        }

        throw GAnyException(errorMessage(pos, "unexpected character while parsing value"));
    }

    static std::map<std::string, GAny> parseObject(const std::string &json, size_t &idx, Position &pos)
    {
        skipWhitespace(json, idx, pos);
        if (json[idx] != '{')
            throw GAnyException(errorMessage(pos, "expected '{' at start of object"));
        advance(json, idx, pos);

        std::map<std::string, GAny> object;
        skipWhitespace(json, idx, pos);

        bool expectKey = true;
        while (true) {
            if (idx >= json.size())
                throw GAnyException(errorMessage(pos, "unexpected end of input while parsing object"));

            if (json[idx] == '}') {
                if (expectKey && !object.empty())
                    throw GAnyException(errorMessage(pos, "trailing comma in object"));
                advance(json, idx, pos);
                break;
            }

            if (!expectKey)
                throw GAnyException(errorMessage(pos, "expected ',' between object members"));

            std::string key = parseString(json, idx, pos);
            skipWhitespace(json, idx, pos);

            if (idx >= json.size() || json[idx] != ':')
                throw GAnyException(errorMessage(pos, "expected ':' after key in object"));
            advance(json, idx, pos);

            const GAny value = parseValue(json, idx, pos);
            object[key] = value;

            skipWhitespace(json, idx, pos);

            if (idx >= json.size())
                throw GAnyException(errorMessage(pos, "unexpected end of input after object value"));

            if (json[idx] == ',') {
                advance(json, idx, pos);
                skipWhitespace(json, idx, pos);
                expectKey = true;
            } else {
                expectKey = false;
            }
        }

        return object;
    }

    static std::vector<GAny> parseArray(const std::string &json, size_t &idx, Position &pos)
    {
        skipWhitespace(json, idx, pos);
        if (json[idx] != '[')
            throw GAnyException(errorMessage(pos, "expected '[' at start of array"));
        advance(json, idx, pos);

        std::vector<GAny> array;
        skipWhitespace(json, idx, pos);

        bool expectValue = true;
        while (true) {
            if (idx >= json.size())
                throw GAnyException(errorMessage(pos, "unexpected end of input while parsing array"));

            if (json[idx] == ']') {
                if (expectValue && !array.empty())
                    throw GAnyException(errorMessage(pos, "trailing comma in array"));
                advance(json, idx, pos);
                break;
            }

            if (!expectValue)
                throw GAnyException(errorMessage(pos, "expected ',' between array elements"));

            array.push_back(parseValue(json, idx, pos));
            skipWhitespace(json, idx, pos);

            if (idx >= json.size())
                throw GAnyException(errorMessage(pos, "unexpected end of input after array element"));

            if (json[idx] == ',') {
                advance(json, idx, pos);
                skipWhitespace(json, idx, pos);
                expectValue = true;
            } else {
                expectValue = false;
            }
        }

        return array;
    }

    static std::string errorMessage(const Position &pos, const std::string &msg)
    {
        return "JsonLexer error at (" + std::to_string(pos.line) + ", " + std::to_string(pos.column) + "): " + msg;
    }
};
}

/// ================ GAny ================

inline GAny::GAny()
    : mVal(std::make_shared<GAnyValueVoid>())
{
}

template<typename T>
GAny::GAny(const T &var)
    : GAny(caster<T>::castFrom(var))
{
}

template<typename T>
GAny::GAny(std::enable_if_t<!std::is_same_v<T, GAny>, T> &&var)
    : GAny(caster<T>::castFrom(std::move(var)))
{
}

template<typename T>
GAny::GAny(std::unique_ptr<T> &&v)
    : mVal(new GAnyValueP<std::unique_ptr<T> >(std::move(v)))
{
}

template<typename Return, typename... Args>
GAny::GAny(Return (*f)(Args...))
    : GAny(GAnyFunction(f))
{
}

template<typename Return, typename Class, typename... Args>
GAny::GAny(Return (Class::*f)(Args...))
    : GAny(GAnyFunction(f))
{
}

template<typename Return, typename Class, typename... Args>
GAny::GAny(Return (Class::*f)(Args...) const)
    : GAny(GAnyFunction(f))
{
}

template<class T>
GAny GAny::create(const T &t)
{
    static_assert(!std::is_same_v<T, GAny>, "This should not happen.");
    return fromValue(std::make_shared<GAnyValueP<std::remove_const_t<std::remove_reference_t<T> > > >(t));
}

template<class T>
GAny GAny::create(T &&t)
{
    static_assert(!std::is_same_v<T, GAny>, "This should not happen.");
    return fromValue(std::make_shared<GAnyValueP<std::remove_const_t<std::remove_reference_t<T> > > >(
        std::forward<T>(t)));
}

template<typename T>
bool GAny::is() const
{
    T *ptr = (T *) mVal->as(typeid(T));
    return ptr != nullptr;
}

template<>
inline bool GAny::is<GAny>() const
{
    return true;
}

template<typename T>
const T *GAny::as() const
{
    return (T *) mVal->as(typeid(T));
}

template<>
inline const GAny *GAny::as<GAny>() const
{
    return this;
}

template<typename T>
T *GAny::as()
{
    return (T *) mVal->as(typeid(T));
}

template<>
inline GAny *GAny::as<GAny>()
{
    return this;
}

template<typename T>
T *GAny::unsafeAs()
{
    return const_cast<T *>(static_cast<const T*>(mVal->ptr()));
}

template<typename T>
const T *GAny::unsafeAs() const
{
    return static_cast<const T*>(mVal->ptr());
}

template<typename T>
std::enable_if_t<!std::is_reference_v<T> && !std::is_pointer_v<T>, T>
GAny::castAs() const
{
    T *ptr = (T *) as<T>();
    if (ptr) {
        return *ptr;
    }

    GAny ret = caster<T>::castTo(*this);
    if (ret.isUndefined()) {
        ret = classObject().castToBase(*GAnyClass::instance<T>(), *this);
    }
    if (ret.is<T>()) {
        if (!GetLocVarCache().empty()) {
            auto &cache = GetLocVarCache().top();
            cache.push_back(ret);
        }
        return *ret.as<T>();
    }
    throw GAnyException(GAnyException::Type::CastAsFailed,
                        "Unable cast " + classTypeName() + " to " + GAnyClass::Class<T>()->getName());
}

template<typename T>
std::enable_if_t<std::is_reference_v<T>, T>
GAny::castAs()
{
    typedef std::remove_const_t<std::remove_reference_t<T> > RmConstRefType;

    if (!is<RmConstRefType>()) {
        return *castAs<RmConstRefType *>();
    }

    return *as<RmConstRefType>();
}

template<typename T>
std::enable_if_t<std::is_pointer_v<T> && !std::is_void_v<std::remove_pointer_t<T>>, T> GAny::castAs()
{
    typedef std::remove_const_t<std::remove_pointer_t<T> > RmConstPtrType;

    if constexpr (detail::is_std_function<RmConstPtrType>::value) {
        GAny ret = GAny::create(castAs<RmConstPtrType>());
        if (!GetLocVarCache().empty()) {
            auto &cache = GetLocVarCache().top();
            cache.push_back(ret);
        }
        T ptr = ret.as<RmConstPtrType>();
        return ptr;
    }

    T ptr = as<RmConstPtrType>();
    if (ptr) {
        return ptr;
    }

    RmConstPtrType **pptr = as<RmConstPtrType *>();
    if (pptr) {
        return (RmConstPtrType *) *pptr;
    }

    if (isNull()) {
        return (T) nullptr;
    }

    GAny ret = caster<RmConstPtrType>::castTo(*this);
    if (ret.isUndefined()) {
        ret = classObject().castToBase(*GAnyClass::instance<RmConstPtrType>(), *this);
    }
    if (ret.is<RmConstPtrType>()) {
        if (!GetLocVarCache().empty()) {
            auto &cache = GetLocVarCache().top();
            cache.push_back(ret);
        }
        ptr = ret.as<RmConstPtrType>();
        if (ptr) {
            return ptr;
        }
    }

    throw GAnyException(GAnyException::Type::CastAsFailed,
                        "Unable cast " + classTypeName() + " to " + GAnyClass::Class<T>()->getName());
}

template<typename T>
std::enable_if_t<std::is_pointer_v<T> && std::is_void_v<std::remove_pointer_t<T>>, T> GAny::castAs()
{
    return const_cast<void*>(mVal->ptr());
}

template<typename Ret, typename... Args>
std::enable_if_t<!std::is_void_v<Ret>, std::function<Ret(Args...)>>
genLambda(Ret (*)(Args...), GAny c)
{
    return [c](Args... args)-> Ret {
        return c(std::forward<Args>(args)...).template castAs<Ret>();
    };
}

template<typename Ret, typename... Args>
std::enable_if_t<std::is_void_v<Ret>, std::function<void(Args...)>>
genLambda(Ret (*)(Args...), GAny c)
{
    return [c](Args... args) {
        c(std::forward<Args>(args)...);
    };
}

template<typename F>
std::enable_if_t<detail::is_std_function<F>::value, F> GAny::castAs()
{
    return genLambda(static_cast<detail::function_signature_t<F> *>(nullptr), *this);
}

template<>
inline GAny GAny::castAs<GAny>() const
{
    return *this;
}

template<typename... Args>
GAny GAny::call(const std::string &methodName, Args &&... args) const
{
    const AnyType tType = type();
    if (tType == AnyType::class_t) {
        // 先尝试调用 GAnyClass 的元方法
        if (classObject().findMember(methodName, false).isFunction()) {
            return classObject().call(*this, methodName, std::forward<Args>(args)...);
        }
        // static method
        return as<GAnyClass>()->call(GAny(), methodName, std::forward<Args>(args)...);
    }
    if (tType == AnyType::object_t) {
        auto func = (*as<GAnyObject>())[methodName];
        if (func.isFunction()) {
            return func(std::forward<Args>(args)...);
        }
    }
    return classObject().call(*this, methodName, std::forward<Args>(args)...);
}

template<typename... Args>
GAny GAny::call(const std::string &methodName, Args &&... args)
{
    const AnyType tType = type();
    if (tType == AnyType::class_t) {
        // 先尝试调用 GAnyClass 的元方法
        if (classObject().findMember(methodName, false).isFunction()) {
            return classObject().call(*this, methodName, std::forward<Args>(args)...);
        }
        // static method
        return as<GAnyClass>()->call(GAny(), methodName, std::forward<Args>(args)...);
    }
    if (tType == AnyType::object_t) {
        auto func = (*as<GAnyObject>())[methodName];
        if (func.isFunction()) {
            return func(std::forward<Args>(args)...);
        }
    }
    return classObject().call(*this, methodName, std::forward<Args>(args)...);
}

template<typename... Args>
GAny GAny::operator()(Args... args) const
{
    const std::initializer_list<GAny> argv = {
        (GAny(std::move(args)))...
    };

    const auto tArgc = static_cast<int32_t>(argv.size());
    const auto tArgs = static_cast<const GAny **>(alloca(sizeof(GAny *) * tArgc));
    for (int32_t i = 0; i < tArgc; i++) {
        tArgs[i] = &argv.begin()[i];
    }

    return _call(tArgs, tArgc);
}

template<typename... Args>
GAny GAny::operator()(Args... args)
{
    const std::initializer_list<GAny> argv = {
        (GAny(std::move(args)))...
    };

    const auto tArgc = static_cast<int32_t>(argv.size());
    const auto tArgs = static_cast<const GAny **>(alloca(sizeof(GAny *) * tArgc));
    for (int32_t i = 0; i < tArgc; i++) {
        tArgs[i] = &argv.begin()[i];
    }

    return _call(tArgs, tArgc);
}


inline GAny::GAny(const std::initializer_list<GAny> &init)
    : GAny()
{
    const bool is_an_object = std::all_of(
        init.begin(), init.end(),
        [](const GAny &ele) {
            return ele.isArray() && ele.length() == 2 && ele.getItem(0).is<std::string>();
        });

    if (is_an_object) {
        std::map<std::string, GAny> obj;
        for (const GAny &p: init) {
            obj.insert(std::make_pair(*p.getItem(0).as<std::string>(), p.getItem(1)));
        }
        *this = GAny(obj);
        return;
    }

    *this = GAny(std::vector<GAny>(init));
}

inline GAny GAny::object(const std::map<std::string, GAny> &m)
{
    return GAny(m);
}

inline GAny GAny::object(const std::unordered_map<std::string, GAny> &m)
{
    return GAny(m);
}

inline GAny GAny::array(const std::vector<GAny> &vec)
{
    return GAny(vec);
}

inline GAny GAny::array(const std::list<GAny> &lst)
{
    return GAny(lst);
}

inline GAny GAny::undefined()
{
    return GAny();
}

inline GAny GAny::null()
{
    return create<std::nullptr_t>(nullptr);
}

template<typename T, typename... Args>
GAny GAny::New(Args &&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

inline const std::shared_ptr<GAnyValue> &GAny::value() const
{
    return mVal;
}

inline GAny GAny::fromValue(std::shared_ptr<GAnyValue> val)
{
    GAny var;
    var.mVal = std::move(val);
    return var;
}

inline GAny GAny::clone() const
{
    GAny copy;
    mVal->clone(copy);
    return copy;
}

inline const std::string &GAny::classTypeName() const
{
    return classObject().getName();
}

inline std::string GAny::typeName() const
{
    return AnyTypeNames[static_cast<size_t>(type())];
}

inline GAnyTypeInfo GAny::typeInfo() const
{
    return classObject().mTypeInfo;
}

inline AnyType GAny::type() const
{
    return classObject().mTypeInfo.anyType();
}

inline GAnyClass &GAny::classObject() const
{
    return *mVal->classObject();
}

inline size_t GAny::length() const
{
    if (isUndefined() || isNull()) {
        return 0;
    }
    if (isArray()) {
        return as<GAnyArray>()->length();
    }
    if (type() == AnyType::object_t) {
        return as<GAnyObject>()->length();
    }
    const auto &cl = classObject();
    const GAny ret = cl.call(*this, MetaFunctionNames[static_cast<size_t>(MetaFunction::Length)]);
    return ret.toInt64();
}

inline bool GAny::is(const GAnyTypeInfo &typeInfo) const
{
    return mVal->as(typeInfo.typeIndex()) != nullptr;
}

inline bool GAny::is(const std::string &typeStr) const
{
    const auto &classObj = classObject();
    if (typeStr.find('.') != std::string::npos) {
        return classObj.getNameSpace() + "." + classObj.getName() == typeStr;
    }
    return classObj.getName() == typeStr;
}

inline bool GAny::isUndefined() const
{
    return type() == AnyType::undefined_t;
}

inline bool GAny::isNull() const
{
    return type() == AnyType::null_t || mVal == nullptr || mVal->ptr() == nullptr;
}

inline bool GAny::isFunction() const
{
    return type() == AnyType::function_t;
}

inline bool GAny::isClass() const
{
    return type() == AnyType::class_t;
}

inline bool GAny::isException() const
{
    return is<GAnyException>();
}

inline bool GAny::isProperty() const
{
    return type() == AnyType::property_t;
}

inline bool GAny::isObject() const
{
    return is<GAnyObject>();
}

inline bool GAny::isArray() const
{
    return type() == AnyType::array_t;
}

inline bool GAny::isInt() const
{
    return type() == AnyType::int_t;
}

inline bool GAny::isFloat() const
{
    return type() == AnyType::float_t;
}

inline bool GAny::isDouble() const
{
    return type() == AnyType::double_t;
}

inline bool GAny::isNumber() const
{
    switch (type()) {
        case AnyType::int_t:
        case AnyType::float_t:
        case AnyType::double_t:
            return true;
        default:
            break;
    }
    return false;
}

inline bool GAny::isString() const
{
    return type() == AnyType::string_t;
}

inline bool GAny::isBoolean() const
{
    return type() == AnyType::boolean_t;
}

inline bool GAny::isUserObject() const
{
    return type() == AnyType::user_obj_t;
}

inline const void *GAny::getPointer() const
{
    return mVal->ptr();
}

inline bool GAny::contains(const GAny &id) const
{
    const AnyType tType = type();
    if (tType == AnyType::undefined_t || tType == AnyType::null_t) {
        return false;
    }

    if (tType == AnyType::object_t) {
        if (!id.isString()) {
            return false;
        }
        auto *obj = as<GAnyObject>();
        return obj->contains(id.toString());
    }
    if (tType == AnyType::array_t) {
        auto *arr = as<GAnyArray>();
        return arr->contains(id);
    }
    if (classObject().containsMember("contains", true)) {
        return call("contains", id).toBool();
    }
    if (tType == AnyType::class_t && id.isString()) {
        return as<GAnyClass>()->containsMember(id.toString(), true);
    }
    return false;
}

inline void GAny::erase(const GAny &id)
{
    const AnyType tType = type();
    if (tType == AnyType::object_t) {
        if (!id.isString()) {
            return;
        }
        auto *obj = as<GAnyObject>();
        obj->erase(*id.as<std::string>());
    } else if (tType == AnyType::array_t) {
        if (!id.isInt()) {
            return;
        }
        auto *arr = as<GAnyArray>();
        arr->erase(*id.as<int32_t>());
    }
}

inline void GAny::pushBack(const GAny &rh)
{
    if (isArray()) {
        as<GAnyArray>()->push_back(rh);
    }
}

inline void GAny::clear()
{
    const AnyType tType = type();
    if (tType == AnyType::object_t) {
        auto *obj = as<GAnyObject>();
        obj->clear();
    } else if (tType == AnyType::array_t) {
        auto *arr = as<GAnyArray>();
        arr->clear();
    }
}

inline GAny GAny::iterator() const
{
    if (classObject().containsMember("iterator", true)) {
        return call("iterator");
    }
    return undefined();
}

inline bool GAny::hasNext() const
{
    if (isUndefined()) {
        return false;
    }
    return call("hasNext").toBool();
}

inline GAnyIteratorItem GAny::next() const
{
    return *call("next").as<GAnyIteratorItem>();
}

inline GAny &GAny::overload(GAny func)
{
    if (!isFunction() || !func.isFunction()) {
        return *this;
    }

    // function with unknown parameters, overload is not supported
    if (this->as<GAnyFunction>()->mIsVariadicFunc || func.as<GAnyFunction>()->mIsVariadicFunc) {
        return *this;
    }

    const auto &rFunc = *func.as<GAnyFunction>();

    GAny *dest = this;
    while (dest->isFunction()) {
        if (dest->as<GAnyFunction>()->compareArgs(rFunc)) {
            break;
        }
        dest = &dest->as<GAnyFunction>()->mNext;
    }
    if (!dest->isFunction()) {
        *dest = func;
        dest->as<GAnyFunction>()->mNext = GAny();
    }

    return *this;
}

inline GAny GAny::_call(const std::string &methodName, const GAny **args, int32_t argc) const
{
    const AnyType tType = type();
    if (tType == AnyType::class_t) {
        // 先尝试调用 GAnyClass 的元方法
        if (classObject().findMember(methodName, false).isFunction()) {
            return classObject()._call(*this, methodName, args, argc);
        }
        // static method
        return as<GAnyClass>()->_call(GAny(), methodName, args, argc);
    }
    if (tType == AnyType::object_t) {
        const auto func = (*as<GAnyObject>())[methodName];
        if (func.isFunction()) {
            return func._call(args, argc);
        }
    }
    return classObject()._call(*this, methodName, args, argc);
}


inline GAny GAny::_call(const std::string &methodName, const GAny **args, int32_t argc)
{
    const AnyType tType = type();
    if (tType == AnyType::class_t) {
        // 先尝试调用 GAnyClass 的元方法
        if (classObject().findMember(methodName, false).isFunction()) {
            return classObject()._call(*this, methodName, args, argc);
        }
        // static method
        return as<GAnyClass>()->_call(GAny(), methodName, args, argc);
    }
    if (tType == AnyType::object_t) {
        const auto func = (*as<GAnyObject>())[methodName];
        if (func.isFunction()) {
            return func._call(args, argc);
        }
    }
    return classObject()._call(*this, methodName, args, argc);
}

inline GAny GAny::_call(const std::string &methodName, const std::vector<GAny> &args) const
{
    const auto tArgc = static_cast<int32_t>(args.size());
    const auto tArgs = static_cast<const GAny **>(alloca(sizeof(GAny *) * tArgc));
    for (int32_t i = 0; i < tArgc; i++) {
        tArgs[i] = &args.begin()[i];
    }

    return _call(methodName, tArgs, tArgc);
}

inline GAny GAny::_call(const std::string &methodName, const std::vector<GAny> &args)
{
    const auto tArgc = static_cast<int32_t>(args.size());
    const auto tArgs = static_cast<const GAny **>(alloca(sizeof(GAny *) * tArgc));
    for (int32_t i = 0; i < tArgc; i++) {
        tArgs[i] = &args.begin()[i];
    }

    return _call(methodName, tArgs, tArgc);
}

inline GAny GAny::_call(MetaFunction metaFunc, const GAny **args, int32_t argc) const
{
    return _call(MetaFunctionNames[static_cast<size_t>(metaFunc)], args, argc);
}

inline GAny GAny::_call(MetaFunction metaFunc, const GAny **args, int32_t argc)
{
    return _call(MetaFunctionNames[static_cast<size_t>(metaFunc)], args, argc);
}

inline GAny GAny::_call(MetaFunction metaFunc, std::vector<GAny> &args) const
{
    const auto tArgc = static_cast<int32_t>(args.size());
    const auto tArgs = static_cast<const GAny **>(alloca(sizeof(GAny *) * tArgc));
    for (int32_t i = 0; i < tArgc; i++) {
        tArgs[i] = &args.begin()[i];
    }

    return _call(metaFunc, tArgs, tArgc);
}

inline GAny GAny::_call(MetaFunction metaFunc, std::vector<GAny> &args)
{
    const auto tArgc = static_cast<int32_t>(args.size());
    const auto tArgs = static_cast<const GAny **>(alloca(sizeof(GAny *) * tArgc));
    for (int32_t i = 0; i < tArgc; i++) {
        tArgs[i] = &args.begin()[i];
    }

    return _call(metaFunc, tArgs, tArgc);
}

inline GAny GAny::_call(const GAny **args, int32_t argc) const
{
    if (isFunction()) {
        return as<GAnyFunction>()->_call(args, argc);
    }
    if (isClass()) {
        // constructor
        const auto *cls = as<GAnyClass>();
        return cls->_new(args, argc);
    }
    return GAnyException(classTypeName() + " can't be called as a function or constructor.");
}

inline GAny GAny::_call(const GAny **args, int32_t argc)
{
    if (isFunction()) {
        return as<GAnyFunction>()->_call(args, argc);
    }
    if (isClass()) {
        // constructor
        const auto *cls = as<GAnyClass>();
        return cls->_new(args, argc);
    }
    return GAnyException(classTypeName() + " can't be called as a function or constructor.");
}

inline GAny GAny::_call(const std::vector<GAny> &args) const
{
    const auto tArgc = static_cast<int32_t>(args.size());
    const auto tArgs = static_cast<const GAny **>(alloca(sizeof(GAny *) * tArgc));
    for (int32_t i = 0; i < tArgc; i++) {
        tArgs[i] = &args.begin()[i];
    }

    return _call(tArgs, tArgc);
}

inline GAny GAny::_call(const std::vector<GAny> &args)
{
    const auto tArgc = static_cast<int32_t>(args.size());
    const auto tArgs = static_cast<const GAny **>(alloca(sizeof(GAny *) * tArgc));
    for (int32_t i = 0; i < tArgc; i++) {
        tArgs[i] = &args.begin()[i];
    }

    return _call(tArgs, tArgc);
}

inline GAny GAny::operator-() const
{
    if (this->isFloat()) {
        return -(*as<double>());
    }
    if (this->isInt()) {
        return -(this->toInt64());
    }

    const auto &cl = classObject();
    auto ret = cl.call(*this, MetaFunctionNames[static_cast<size_t>(MetaFunction::Negate)]);
    if (ret.isException()) {
        return undefined();
    }
    return ret;
}

#define DEF_OPERATOR_IMPL(METAF) \
    auto& cl = classObject(); \
    GAny ret = cl.call(*this, MetaFunctionNames[(size_t) METAF], rh); \
    if(ret.isException()) return GAny::undefined(); \
    return ret

#define DEF_FOUR_OPERATOR_IMPL(OP, METAF) \
    if (this->isFloat() && rh.isFloat()) { return this->toFloat() OP rh.toFloat(); } \
    if (this->isDouble() && rh.isDouble()) { return this->toDouble() OP rh.toDouble(); } \
    if (this->isFloat() || this->isDouble() || rh.isFloat() || rh.isDouble()) { return this->toDouble() OP rh.toDouble(); } \
    if (this->isInt() || rh.isInt()) { return this->toInt64() OP rh.toInt64(); } \
    DEF_OPERATOR_IMPL(METAF)


inline GAny GAny::operator+(const GAny &rh) const
{
    DEF_FOUR_OPERATOR_IMPL(+, MetaFunction::Addition);
}

inline GAny GAny::operator-(const GAny &rh) const
{
    DEF_FOUR_OPERATOR_IMPL(-, MetaFunction::Subtraction);
}

inline GAny GAny::operator*(const GAny &rh) const
{
    DEF_FOUR_OPERATOR_IMPL(*, MetaFunction::Multiplication);
}

inline GAny GAny::operator/(const GAny &rh) const
{
    DEF_FOUR_OPERATOR_IMPL(/, MetaFunction::Division);
}

inline GAny GAny::operator%(const GAny &rh) const
{
    if (this->isFloat() || rh.isFloat()) {
        return std::fmod(this->toDouble(), rh.toDouble());
    }
    if (this->isInt() || rh.isInt()) {
        return this->toInt64() % rh.toInt64();
    }
    DEF_OPERATOR_IMPL(MetaFunction::Modulo);
}

inline GAny GAny::operator^(const GAny &rh) const
{
    if (this->isInt() || rh.isInt()) {
        return this->toInt64() ^ rh.toInt64();
    }
    DEF_OPERATOR_IMPL(MetaFunction::BitXor);
}

inline GAny GAny::operator|(const GAny &rh) const
{
    if (this->isInt() || rh.isInt()) {
        return this->toInt64() | rh.toInt64();
    }
    DEF_OPERATOR_IMPL(MetaFunction::BitOr);
}

inline GAny GAny::operator&(const GAny &rh) const
{
    if (this->isInt() || rh.isInt()) {
        return this->toInt64() & rh.toInt64();
    }
    DEF_OPERATOR_IMPL(MetaFunction::BitAnd);
}

#undef DEF_OPERATOR_IMPL
#undef DEF_FOUR_OPERATOR_IMPL

inline GAny GAny::operator~() const
{
    if (this->isBoolean()) {
        return !(*this->as<bool>());
    }
    if (this->isInt()) {
        return ~(this->toInt64());
    }

    const auto &cl = classObject();
    GAny ret = cl.call(*this, MetaFunctionNames[static_cast<size_t>(MetaFunction::BitNot)]);
    if (ret.isException()) {
        return undefined();
    }
    return ret;
}

inline bool GAny::operator==(const GAny &rh) const
{
    const AnyType lType = this->type();
    const AnyType rType = rh.type();

    if (lType == AnyType::undefined_t) {
        return rType == AnyType::undefined_t;
    }
    if (lType == AnyType::null_t) {
        return rType == AnyType::null_t;
    }
    if (lType == AnyType::boolean_t && rType == AnyType::boolean_t) {
        return *this->as<bool>() == *rh.as<bool>();
    }
    if (lType == AnyType::float_t || lType == AnyType::double_t
        || rType == AnyType::float_t || rType == AnyType::double_t) {
        return std::abs(this->toDouble() - rh.toDouble()) < std::numeric_limits<double>::epsilon();
    }
    if (lType == AnyType::int_t || rType == AnyType::int_t) {
        return this->toInt64() == rh.toInt64();
    }
    if (lType == AnyType::string_t && rType == AnyType::string_t) {
        return *this->as<std::string>() == *rh.as<std::string>();
    }

    const auto &cl = classObject();
    GAny ret = cl.call(*this, MetaFunctionNames[static_cast<size_t>(MetaFunction::EqualTo)], rh);
    if (!ret.is<bool>()) {
        return false;
    }
    return *ret.as<bool>();
}

inline bool GAny::operator<(const GAny &rh) const
{
    if (this->isFloat() || rh.isFloat()) {
        return this->toDouble() < rh.toDouble();
    }
    if (this->isInt() || rh.isInt()) {
        return this->toInt64() < rh.toInt64();
    }

    const auto &cl = classObject();
    GAny ret = cl.call(*this, MetaFunctionNames[static_cast<size_t>(MetaFunction::LessThan)], rh);
    if (!ret.is<bool>()) {
        return false;
    }
    return *ret.as<bool>();
}

inline bool GAny::operator==(std::nullptr_t) const
{
    return isUndefined() || isNull();
}

inline bool GAny::operator!=(std::nullptr_t) const
{
    return !(*this == nullptr);
}

inline GAny GAny::getItem(const GAny &i) const
{
    const AnyType tType = type();

    switch (tType) {
        case AnyType::array_t: {
            if (!i.isInt()) {
                break;
            }
            auto &arr = *as<GAnyArray>();
            return arr[i.toInt32()];
        }
        case AnyType::object_t: {
            if (!i.isString()) {
                break;
            }

            const auto &key = *i.as<std::string>();

            if (key.find('.') != std::string::npos) {
                const auto paths = [](const std::string &str) {
                    size_t previous = 0;
                    size_t current = str.find('.');
                    std::vector<std::string> elems;
                    while (current != std::string::npos) {
                        if (current > previous) {
                            elems.push_back(str.substr(previous, current - previous));
                        }
                        previous = current + 1;
                        current = str.find('.', previous);
                    }
                    if (previous != str.size()) {
                        elems.push_back(str.substr(previous));
                    }
                    return elems;
                }(key);
                if (!paths.empty()) {
                    GAny ret = *this;
                    for (const auto &path: paths) {
                        if (path.empty()) {
                            continue;
                        }
                        ret = ret.getItem(path);
                    }
                    return ret;
                }
                return undefined();
            }

            auto &obj = *as<GAnyObject>();
            GAny ret = obj[key];
            if (!ret.isUndefined()) {
                return ret;
            }
        }
        break;
        case AnyType::class_t: {
            if (!i.isString()) {
                break;
            }
            const auto &key = *i.as<std::string>();

            // 先查找匹配的 GAnyClass 元方法
            if (classObject().findMember(key, false).isFunction()) {
                return Bind(*this, key);
            }
            return as<GAnyClass>()->findMember(key, true);
        }
        default:
            break;
    }

    return classObject().getItem(i, *this);
}

inline bool GAny::setItem(const GAny &i, const GAny &v)
{
    if (isUndefined()) {
        *this = object();
    }

    switch (type()) {
        case AnyType::array_t: {
            if (!i.isInt()) {
                return false;
            }
            auto &arr = *as<GAnyArray>();
            arr.set(i.toInt32(), v);
            return true;
        }
        case AnyType::object_t: {
            if (!i.isString()) {
                return false;
            }
            const auto &key = *i.as<std::string>();
            auto &obj = *as<GAnyObject>();
            obj.set(key, v);
            return true;
        }
        default: {
            return classObject().setItem(i, *this, v);
        }
    }
    return false;
}

inline bool GAny::delItem(const GAny &i)
{
    if (isUndefined()) {
        return false;
    }

    const AnyType tType = type();
    if (tType == AnyType::object_t) {
        if (!i.isString()) {
            return false;
        }
        auto &obj = *as<GAnyObject>();
        obj.erase(*i.as<std::string>());
        return true;
    }
    if (tType == AnyType::array_t) {
        if (!i.isInt()) {
            return false;
        }
        auto &arr = *as<GAnyArray>();
        arr.erase(i.toInt32());
        return true;
    }

    const auto &cl = classObject();
    return !cl.call(*this, MetaFunctionNames[static_cast<size_t>(MetaFunction::DelItem)], i).isException();
}

inline const GAny GAny::operator[](const GAny &key) const
{
    return getItem(key);
}

inline GAny &GAny::operator[](const GAny &key)
{
    if (isUndefined()) {
        *this = object();
    }
    if (isNull()) {
        return *this;
    }

    const AnyType tType = type();
    if (tType == AnyType::object_t) {
        if (!key.isString()) {
            assert("GAny::operator[] object expects a string as index.");
            return *this;
        }
        auto &obj = *as<GAnyObject>();
        const auto &keyS = *key.as<std::string>();
        std::lock_guard locker(obj.lock);
        const auto it = obj.var.find(keyS);
        if (it != obj.var.end()) {
            return it->second;
        }
        const auto ret = obj.var.insert(std::make_pair(keyS, GAny()));
        return ret.first->second;
    }
    if (tType == AnyType::array_t) {
        if (!key.isInt()) {
            assert("GAny::operator[] array expects a integer as index.");
            return *this;
        }
        auto &arr = *as<GAnyArray>();
        std::lock_guard locker(arr.lock);
        return arr.var[key.toInt32()];
    }
    assert(false && "The current type cannot use the \"[]\" operator to take values");
    return *this;
}

inline std::string GAny::toString() const
{
    switch (type()) {
        case AnyType::undefined_t: {
            return "undefined";
        }
        case AnyType::null_t: {
            return "null";
        }
        case AnyType::boolean_t: {
            return *as<bool>() ? "true" : "false";
        }
        case AnyType::int_t: {
            if (is<int64_t>()) {
                return std::to_string(*as<int64_t>());
            }
            if (is<uint64_t>()) {
                return std::to_string(*as<uint64_t>());
            }
            if (is<int32_t>()) {
                return std::to_string(*as<int32_t>());
            }
            if (is<uint32_t>()) {
                return std::to_string(*as<uint32_t>());
            }
            if (is<int16_t>()) {
                return std::to_string(*as<int16_t>());
            }
            if (is<uint16_t>()) {
                return std::to_string(*as<uint16_t>());
            }
            if (is<int8_t>()) {
                return std::to_string(*as<int8_t>());
            }
            if (is<uint8_t>()) {
                return std::to_string(*as<uint8_t>());
            }
        }
        case AnyType::float_t: {
            return std::to_string(*as<float>());
        }
        case AnyType::double_t: {
            return std::to_string(*as<double>());
        }
        case AnyType::string_t: {
            return *as<std::string>();
        }
        case AnyType::array_t:
        case AnyType::object_t:
        case AnyType::class_t: {
            return toJsonString();
        }
        default:
            break;
    }

    const auto &cl = classObject();
    if (!cl.containsMember(MetaFunction::ToString, true)) {
        return "";
    }
    GAny ret = cl.call(*this, MetaFunctionNames[static_cast<size_t>(MetaFunction::ToString)]);
    if (!ret.is<std::string>()) {
        return "";
    }
    return *ret.as<std::string>();
}

inline int32_t GAny::toInt32() const
{
    if (isNumber()) {
        if (is<int32_t>()) {
            return *as<int32_t>();
        }
        if (is<uint32_t>()) {
            return static_cast<int32_t>(*as<uint32_t>());
        }
        if (is<int64_t>()) {
            return static_cast<int32_t>(*as<int64_t>());
        }
        if (is<uint64_t>()) {
            return static_cast<int32_t>(*as<uint64_t>());
        }
        if (is<int16_t>()) {
            return *as<int16_t>();
        }
        if (is<uint16_t>()) {
            return *as<uint16_t>();
        }
        if (is<int8_t>()) {
            return *as<int8_t>();
        }
        if (is<uint8_t>()) {
            return *as<uint8_t>();
        }
        if (is<float>()) {
            return static_cast<int32_t>(*as<float>());
        }
        if (is<double>()) {
            return static_cast<int32_t>(*as<double>());
        }
    }
    return static_cast<int32_t>(toInt64());
}

inline int64_t GAny::toInt64() const
{
    switch (type()) {
        case AnyType::undefined_t:
        case AnyType::null_t: {
            return 0LL;
        }
        case AnyType::boolean_t: {
            return *as<bool>() ? 1LL : 0LL;
        }
        case AnyType::int_t: {
            if (is<int64_t>()) {
                return *as<int64_t>();
            }
            if (is<uint64_t>()) {
                return static_cast<int64_t>(*as<uint64_t>());
            }
            if (is<int32_t>()) {
                return *as<int32_t>();
            }
            if (is<uint32_t>()) {
                return *as<uint32_t>();
            }
            if (is<int16_t>()) {
                return *as<int16_t>();
            }
            if (is<uint16_t>()) {
                return *as<uint16_t>();
            }
            if (is<int8_t>()) {
                return *as<int8_t>();
            }
            if (is<uint8_t>()) {
                return *as<uint8_t>();
            }
            return 0;
        }
        case AnyType::float_t: {
            return *as<float>();
        }
        case AnyType::double_t: {
            return static_cast<int64_t>(*as<double>());
        }
        case AnyType::string_t: {
            std::stringstream ss;
            ss << *as<std::string>();
            int64_t v;
            ss >> v;
            return v;
        }
        default:
            break;
    }

    const auto &cl = classObject();
    if (!cl.containsMember(MetaFunction::ToInt, true)) {
        return 0;
    }
    const GAny ret = cl.call(*this, MetaFunctionNames[static_cast<size_t>(MetaFunction::ToInt)]);
    if (!ret.isNumber()) {
        return 0;
    }
    return ret.toInt64();
}

inline float GAny::toFloat() const
{
    if (type() == AnyType::float_t) {
        return *as<float>();
    }
    return static_cast<float>(toDouble());
}

inline double GAny::toDouble() const
{
    switch (type()) {
        case AnyType::undefined_t:
        case AnyType::null_t: {
            return 0;
        }
        case AnyType::boolean_t: {
            return *as<bool>() ? 1 : 0;
        }
        case AnyType::int_t: {
            if (is<int64_t>()) {
                return static_cast<double>(*as<int64_t>());
            }
            if (is<uint64_t>()) {
                return static_cast<double>(*as<uint64_t>());
            }
            if (is<int32_t>()) {
                return *as<int32_t>();
            }
            if (is<uint32_t>()) {
                return *as<uint32_t>();
            }
            if (is<int16_t>()) {
                return *as<int16_t>();
            }
            if (is<uint16_t>()) {
                return *as<uint16_t>();
            }
            if (is<int8_t>()) {
                return *as<int8_t>();
            }
            if (is<uint8_t>()) {
                return *as<uint8_t>();
            }
            return 0;
        }
        case AnyType::float_t: {
            return *as<float>();
        }
        case AnyType::double_t: {
            return *as<double>();
        }
        case AnyType::string_t: {
            std::stringstream ss;
            ss << *as<std::string>();
            double v;
            ss >> v;
            return v;
        }
        default:
            break;
    }

    const auto &cl = classObject();
    if (!cl.containsMember(MetaFunction::ToDouble, true)) {
        return 0;
    }
    GAny ret = cl.call(*this, MetaFunctionNames[static_cast<size_t>(MetaFunction::ToDouble)]);
    if (ret.is<double>()) {
        return *ret.as<double>();
    }
    if (!cl.containsMember(MetaFunction::ToFloat, true)) {
        return 0;
    }
    ret = cl.call(*this, MetaFunctionNames[static_cast<size_t>(MetaFunction::ToFloat)]);
    if (ret.is<float>()) {
        return *ret.as<float>();
    }
    return 0;
}

inline bool GAny::toBool() const
{
    switch (type()) {
        case AnyType::undefined_t:
        case AnyType::null_t: {
            return false;
        }
        case AnyType::boolean_t: {
            return *as<bool>();
        }
        case AnyType::int_t: {
            if (is<int64_t>()) {
                return *as<int64_t>() != 0;
            }
            if (is<uint64_t>()) {
                return *as<uint64_t>() != 0;
            }
            if (is<int32_t>()) {
                return *as<int32_t>() != 0;
            }
            if (is<uint32_t>()) {
                return *as<uint32_t>() != 0;
            }
            if (is<int16_t>()) {
                return *as<int16_t>() != 0;
            }
            if (is<uint16_t>()) {
                return *as<uint16_t>() != 0;
            }
            if (is<int8_t>()) {
                return *as<int8_t>() != 0;
            }
            if (is<uint8_t>()) {
                return *as<uint8_t>() != 0;
            }
            return false;
        }
        case AnyType::float_t: {
            return *as<float>() != 0;
        }
        case AnyType::double_t: {
            return *as<double>() != 0;
        }
        case AnyType::string_t: {
            return !as<std::string>()->empty();
        }
        case AnyType::array_t:
        case AnyType::object_t:
        case AnyType::function_t:
        case AnyType::class_t:
        case AnyType::property_t:
            return true;
        case AnyType::exception_t:
            return false;
        case AnyType::user_obj_t: {
            const auto &cl = classObject();
            if (!cl.containsMember(MetaFunction::ToBoolean, true)) {
                return true;
            }
            GAny ret = cl.call(*this, MetaFunctionNames[static_cast<size_t>(MetaFunction::ToBoolean)]);
            if (!ret.is<bool>()) {
                return false;
            }
            return *ret.as<bool>();
        }
        break;
    }

    return false;
}

inline GAny GAny::toObject() const
{
    const AnyType tType = type();
    if (tType == AnyType::undefined_t || tType == AnyType::null_t) {
        return object();
    }
    if (tType == AnyType::object_t || tType == AnyType::array_t) {
        return *this;
    }

    const auto &cl = classObject();
    if (!cl.containsMember(MetaFunction::ToObject, true)) {
        return object();
    }
    return cl.call(*this, MetaFunctionNames[static_cast<size_t>(MetaFunction::ToObject)]);
}

inline std::ostream &GAny::dumpJson(std::ostream &o, const int indent, const int current_indent) const
{
    static auto dump_str = [](const std::string &value) -> std::string {
        std::string out;
        out.reserve(value.size() * 2);
        out += '"';
        for (size_t i = 0; i < value.length(); i++) {
            const char ch = value[i];
            const uint8_t uch = ch;
            switch (uch) {
                case '\\':
                    out += "\\\\";
                    break;
                case '"':
                    out += "\\\"";
                    break;
                case '\b':
                    out += "\\b";
                    break;
                case '\f':
                    out += "\\f";
                    break;
                case '\n':
                    out += "\\n";
                    break;
                case '\r':
                    out += "\\r";
                    break;
                case '\t':
                    out += "\\t";
                    break;
                case 0xe2:
                    if (static_cast<uint8_t>(value[i + 1]) == 0x80
                        && static_cast<uint8_t>(value[i + 2]) == 0xa8) {
                        out += "\\u2028";
                        i += 2;
                    } else if (static_cast<uint8_t>(value[i + 1]) == 0x80
                               && static_cast<uint8_t>(value[i + 2]) == 0xa9) {
                        out += "\\u2029";
                        i += 2;
                    } else {
                        out.push_back(ch);
                    }
                    break;
                default:
                    if (uch <= 0x1f) {
                        char buf[8];
                        snprintf(buf, sizeof buf, "\\u%04x", ch);
                        out.append(buf, buf + 8);
                    } else {
                        out.push_back(ch);
                    }
                    break;
            }
        }
        out += '"';
        return out;
    };
    switch (type()) {
        case AnyType::undefined_t: {
            return o;
        }
        case AnyType::null_t: {
            return o << "null";
        }
        case AnyType::boolean_t: {
            return o << (*as<bool>() ? "true" : "false");
        }
        case AnyType::int_t: {
            return o << toInt64();
        }
        case AnyType::float_t:
        case AnyType::double_t: {
            std::stringstream floats;
            floats << std::setprecision(15) << toDouble();
            return o << floats.str();
        }
        case AnyType::string_t: {
            return o << dump_str(*as<std::string>());
        }
        case AnyType::array_t: {
            const auto &vec = *unsafeAs<std::vector<GAny> >();
            const auto N = vec.size();
            if (N == 0) {
                return o << "[]";
            }
            o << '[';
            std::string indent_front, ident_back("]");
            int new_indent = current_indent + indent;
            if (indent >= 0) {
                indent_front = std::string(new_indent + 1, ' ');
                indent_front[0] = '\n';
                ident_back = std::string(current_indent + 2, ' ');
                ident_back[0] = '\n';
                ident_back.back() = ']';
            }
            int index = 0;
            for (auto it = vec.begin(); it != vec.end(); ++it) {
                if (it->isUndefined()) {
                    continue;
                }
                if (index > 0) {
                    o << ",";
                }
                o << indent_front;
                it->dumpJson(o, indent, new_indent);
                index++;
            }
            o << ident_back;
            return o;
        }
        case AnyType::object_t: {
            const auto &obj = *unsafeAs<std::unordered_map<std::string, GAny> >();
            const auto N = obj.size();
            if (N == 0) {
                return o << "{}";
            }
            o << '{';
            int new_indent = current_indent + indent;
            std::string indent_front, ident_back("}");
            if (indent >= 0) {
                indent_front = std::string(new_indent + 1, ' ');
                indent_front[0] = '\n';
                ident_back = std::string(current_indent + 2, ' ');
                ident_back[0] = '\n';
                ident_back.back() = '}';
            }
            int index = 0;
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                if (it->second.isUndefined()) {
                    continue;
                }
                if (index > 0) {
                    o << ",";
                }
                o << indent_front;
                o << dump_str(it->first);
                o << ":";
                if (indent > 0) {
                    o << " ";
                }
                it->second.dumpJson(o, indent, new_indent);
                index++;
            }
            o << ident_back;
            return o;
        }
        case AnyType::class_t: {
            auto &cl = *as<GAnyClass>();
            return o << "\"<Class: " << cl.mName << ">\"";
        }
        case AnyType::exception_t: {
            return o << dump_str(toString());
        }
        default: {
            if (type() == AnyType::user_obj_t) {
                if (classObject().containsMember(MetaFunction::ToObject, true)) {
                    return toObject().dumpJson(o, indent, current_indent);
                }
                auto members = classObject().getMembers(true);
                GAny jsonObj = object();
                for (const auto &mb: members) {
                    const std::string &name = mb.first;
                    const GAny &member = mb.second;
                    if (member.isProperty() && member.as<GAnyClass::GAnyProperty>()->hasGetter()) {
                        jsonObj[mb.first] = getItem(name);
                    }
                }
                return jsonObj.dumpJson(o, indent, current_indent);
            }

            return o << "\"<" << classTypeName() << " at " << value().get() << ">\"";
        }
    }
}

inline std::string GAny::toJsonString(int indent) const
{
    std::stringstream sst;
    dumpJson(sst, indent);
    return sst.str();
}

inline GAny GAny::parseJson(const std::string &json)
{
    try {
        return json::JsonParser::parseJson(json);
    } catch (GAnyException &e) {
        return GAnyException(e);
    }
}

template<typename T>
GAny GAny::parseJsonToUserObject(const std::string &json)
{
    const GAny obj = GAny::parseJson(json);
    if (obj.isObject()) {
        return caster<T>::castTo(obj);
    }
    return obj.isException() ? obj : GAnyException("Failed to parse JSON");
}

inline const GAny GAny::Import(const std::string &path)
{
    if (!pfnGanyGetEnv) {
        return GAny();
    }
    if (path.empty()) {
        return GAny();
    }

    GAny envObj;
    pfnGanyGetEnv(&envObj);
    if (!envObj.isObject()) {
        return GAny();
    }

    const GAny classDB = (*envObj.as<GAnyObject>())["__CLASS_DB"];
    if (!classDB.isUserObject()) {
        return GAny();
    }

    return classDB.call("getClass", path);
}

inline void GAny::Export(const std::shared_ptr<GAnyClass> &classObj)
{
    if (!pfnGanyGetEnv) {
        return;
    }

    GAny envObj;
    pfnGanyGetEnv(&envObj);
    if (!envObj.isObject()) {
        return;
    }

    GAny classDB = (*envObj.as<GAnyObject>())["__CLASS_DB"];
    if (!classDB.isUserObject()) {
        return;
    }

    classDB.call("addClass", classObj);
}

inline bool GAny::Load(const std::string &path)
{
    if (!pfnGanyGetEnv) {
        return false;
    }

    GAny envObj;
    pfnGanyGetEnv(&envObj);
    if (!envObj.isObject()) {
        return false;
    }
    const GAny tFunc = envObj.getItem("__F_LOAD_PLUGIN");

    assert(tFunc.isFunction());
    if (tFunc.isFunction()) {
        return tFunc(path).toBool();
    }
    return false;
}

inline bool GAny::Unload(const std::string &path)
{
    if (!pfnGanyGetEnv) {
        return false;
    }

    GAny envObj;
    pfnGanyGetEnv(&envObj);
    if (!envObj.isObject()) {
        return false;
    }
    const GAny tFunc = envObj.getItem("__F_UNLOAD_PLUGIN");

    assert(tFunc.isFunction());
    if (tFunc.isFunction()) {
        return tFunc(path).toBool();
    }
    return false;
}

inline GAny GAny::Bind(const GAny &obj, const std::string &methodName)
{
    if (!obj) {
        return GAnyException("Invalid parameter: obj");
    }
    return GAnyFunction::createVariadicFunction("@" + obj.classTypeName() + "::" + methodName, [=](const GAny **args, int32_t argc) -> GAny {
        return obj._call(methodName, args, argc);
    });
}

/// ================ GAnyFunction ================

inline GAnyFunction GAnyFunction::createVariadicFunction(const std::string &name,
                                                         std::function<GAny(const GAny **args, int32_t argc)> func)
{
    GAnyFunction anyFunc;
    anyFunc.mName = name;
    anyFunc.mFunc = std::move(func);
    anyFunc.mIsVariadicFunc = true;

    return anyFunc;
}

inline GAny GAnyFunction::_call(const GAny **args, int32_t argc) const
{
    std::vector<const GAnyFunction *> unmatched;

    // [1] perfect match
    for (const GAnyFunction *overload = this; overload != nullptr; overload = overload->mNext.as<GAnyFunction>()) {
        if (overload->matchingArgv(args, argc)) {
            GAny ret = overload->mFunc(args, argc);
            if (ret.isException()) {
                return GAnyException(ret.as<GAnyException>()->toString() + "\n at " + signature());
            }
            return ret;
        }
        if (overload->mIsVariadicFunc || (overload->mArgTypes.size() == argc + 1)) {
            unmatched.push_back(overload);
        }
        if (!overload->mNext.isFunction()) {
            overload = nullptr;
            break;
        }
    }

    // [2] fuzzy matching

    GAny exception;

    for (const GAnyFunction *overload: unmatched) {
        GAny ret = overload->mFunc(args, argc);
        if (!ret.isException()) {
            return ret;
        }
        if (ret.as<GAnyException>()->getType() == GAnyException::Type::CastAsFailed) {
            exception = ret;
        } else {
            return ret;
        }
    }

    if (exception.isException()) {
        return GAnyException(exception.as<GAnyException>()->toString() + "\n at " + signature());
    }
    return GAnyException(GAnyException::Type::FuncMismatch, " at " + signature() + ": Function mismatch");
}

template<typename Func, typename Return, typename... Args, size_t... Is>
std::enable_if_t<std::is_void_v<Return>, GAny>
GAnyFunction::call_impl(Func &&f, Return (*)(Args...), const GAny **args, detail::index_sequence<Is...>)
{
    GetLocVarCache().emplace();
    f(const_cast<GAny *>(args[Is])->castAs<Args>()...);
    GetLocVarCache().pop();
    return GAny::undefined();
}

template<typename Func, typename Return, typename... Args, size_t... Is>
std::enable_if_t<!std::is_void_v<Return>, GAny>
GAnyFunction::call_impl(Func &&f, Return (*)(Args...), const GAny **args, detail::index_sequence<Is...>)
{
    if constexpr (std::is_reference_v<Return>) {
        typedef std::remove_const_t<std::remove_reference_t<Return> > RmConstRefRetType;

        GetLocVarCache().emplace();
        auto *r = &f(const_cast<GAny *>(args[Is])->castAs<Args>()...);
        GAny rv;
        if constexpr (detail::is_array_t<RmConstRefRetType>::value
                      || detail::is_map_t<RmConstRefRetType>::value
                      || detail::is_shared_ptr<RmConstRefRetType>::value) {
            rv = *r;
        } else {
            rv = r;
        }
        GetLocVarCache().pop();
        return rv;
    } else {
        GetLocVarCache().emplace();
        GAny r = f(const_cast<GAny *>(args[Is])->castAs<Args>()...);
        GetLocVarCache().pop();
        return r;
    }
}

inline std::string GAnyFunction::signature() const
{
    std::stringstream sst;

    if (mIsVariadicFunc) {
        sst << (mName.empty() ? "function" : mName);
        if (mIsMethod && !mIsStatic) {
            sst << "(self";
        } else {
            sst << "(";
        }
        if (mSetInfo) {
            for (size_t i = 0, n = mInfo.args.size(); i < n; i++) {
                if (i != 0 || (i == 0 && mIsMethod && !mIsStatic)) {
                    sst << ", ";
                }
                sst << mInfo.args[i];
            }
        } else {
            if (mIsMethod && !mIsStatic) {
                sst << ", ";
            }
            sst << "...";
        }

        sst << ")";
    } else {
        sst << (mName.empty() ? "function" : mName) << "(";
        for (size_t i = 1; i < mArgTypes.size(); i++) {
            if (!mIsStatic && i == 1 &&
                mName.substr(mName.size() - 6) != MetaFunctionNames[static_cast<size_t>(MetaFunction::Init)]) {
                sst << "self" << (i + 1 == mArgTypes.size() ? "" : ", ");
            } else {
                const size_t argIndex = mIsStatic ? i - 1 : i - 2;
                bool incType = false;
                if (argIndex < mInfo.args.size()) {
                    std::string argName = mInfo.args[argIndex];
                    if (argName.find(':') != std::string::npos) {
                        incType = true;
                        sst << argName;
                    } else {
                        sst << argName << ":";
                    }
                } else {
                    sst << "arg" << i - 1 << ":";
                }
                if (!incType) {
                    sst << mArgTypes[i]->getName();
                }
                sst << (i + 1 == mArgTypes.size() ? "" : ", ");
            }
        }
        sst << ")";
    }

    std::string returnType;
    if (!mInfo.returnType.empty()) {
        returnType = mInfo.returnType;
    } else if (!mArgTypes.empty()) {
        returnType = mArgTypes[0]->getName();
    }
    if (!returnType.empty()) {
        sst << ":" << returnType;
    }

    return sst.str();
}

inline bool GAnyFunction::compareArgs(const GAnyFunction &rFunc) const
{
    if (mIsVariadicFunc || rFunc.mIsVariadicFunc) {
        return false;
    }
    if (mArgTypes.size() != rFunc.mArgTypes.size()) {
        return false;
    }
    for (size_t i = 1; i < mArgTypes.size(); i++) {
        if (*(mArgTypes[i]) != *(rFunc.mArgTypes[i])) {
            return false;
        }
    }
    return true;
}

inline bool GAnyFunction::matchingArgv(const GAny **args, int32_t argc) const
{
    if (mIsVariadicFunc) {
        return true;
    }
    if (mArgTypes.size() != argc + 1) {
        return false;
    }
    for (size_t i = 1; i < mArgTypes.size(); i++) {
        const auto &lClazz = *mArgTypes[i];
        if (lClazz.mTypeInfo.isClassGAny()) {
            continue;
        }
        if (lClazz != args[i - 1]->classObject()) {
            return false;
        }
    }
    return true;
}

inline GAny GAnyFunction::getReturnType() const
{
    if (mIsVariadicFunc) {
        return GAnyClass::Class<GAny>();
    }

    return mArgTypes[0];
}

inline std::vector<GAny> GAnyFunction::getArgTypes() const
{
    if (mIsVariadicFunc) {
        return {};
    }

    if (mArgTypes.size() == 1) {
        return {};
    }

    std::vector<GAny> types;
    const size_t n = mArgTypes.size();
    types.reserve(n);
    for (size_t i = 1; i < n; i++) {
        types.emplace_back(mArgTypes[i]);
    }
    return types;
}

inline GAny GAnyFunction::nextOverload() const
{
    return mNext;
}

inline GAny GAnyFunction::dump() const
{
    GAny dumpObj = GAny::object();

    dumpObj["name"] = mName;
    dumpObj["isStatic"] = isStatic();
    dumpObj["isMethod"] = isMethod();

    GAny overloads = GAny::array();
    dumpObj["overloads"] = overloads;

    const GAnyFunction *overload = this;
    while (overload) {
        GAny ovi = GAny::object();
        overloads.pushBack(ovi);
        ovi["doc"] = overload->mInfo.doc;
        if (!overload->mIsVariadicFunc) {
            GAny args = GAny::array();
            ovi["args"] = args;
            for (size_t i = 1; i < overload->mArgTypes.size(); i++) {
                GAny arg = GAny::object();
                if (overload->isMethod() && !overload->mIsStatic && i == 1
                    && overload->mName.substr(mName.size() - 6) != MetaFunctionNames[static_cast<size_t>(MetaFunction::Init)]) {
                    arg["key"] = "self";
                    arg["type"] = overload->mArgTypes[i]->getName();
                } else {
                    std::stringstream argNameSS;

                    size_t argIndex = overload->mIsStatic ? i - 1 : i - 2;
                    std::string argType = overload->mArgTypes[i]->getName();
                    if (argIndex < overload->mInfo.args.size()) {
                        std::string argName = overload->mInfo.args[argIndex];
                        size_t colonPos = argName.find(':');
                        if (colonPos == std::string::npos) {
                            argNameSS << argName;
                        } else {
                            argNameSS << argName.substr(0, colonPos);
                            argType = argName.substr(colonPos + 1);
                        }
                    } else {
                        argNameSS << "arg" << i - 1;
                    }

                    arg["key"] = argNameSS.str();
                    arg["type"] = argType;
                }
                args.pushBack(arg);
            }
        } else {
            GAny args = GAny::array();
            ovi["args"] = args;
            if (overload->isMethod() && !overload->isStatic()) {
                size_t cnIndex = mName.find('.');
                std::string type;
                if (cnIndex != std::string::npos) {
                    type = mName.substr(0, cnIndex);
                }

                args.pushBack({
                    {"key", "self"},
                    {"type", type}
                });
            }
            if (overload->mSetInfo) {
                if (!overload->mInfo.args.empty()) {
                    for (std::string argName: overload->mInfo.args) {
                        std::string argType;
                        size_t colonPos = argName.find(':');
                        if (colonPos != std::string::npos) {
                            argType = argName.substr(colonPos + 1);
                            argName = argName.substr(0, colonPos);
                        }

                        args.pushBack({
                            {"key", argName},
                            {"type", argType}
                        });
                    }
                }
            } else {
                args.pushBack({
                    {"key", "..."},
                    {"type", ""}
                });
            }
        }
        std::string returnType = overload->mInfo.returnType;
        if (returnType.empty() && !overload->mArgTypes.empty()) {
            returnType = overload->mArgTypes[0]->getName();
        }
        ovi["return"] = returnType;

        if (!overload->mNext.isFunction()) {
            break;
        }
        overload = overload->mNext.as<GAnyFunction>();
    }

    std::stringstream docSS;
    docSS << *this;

    dumpObj["doc"] = docSS.str();

    return dumpObj;
}

template<typename Func, typename Return, typename... Args>
void GAnyFunction::initialize(Func &&f, Return (*)(Args...))
{
    this->mArgTypes = {GAnyClass::instance<Return>().get(), GAnyClass::instance<Args>().get()...};

    this->mFunc = [this, f](const GAny **args, int32_t argc) -> GAny {
        assert(argc == sizeof...(Args));
        using indices = detail::make_index_sequence<sizeof...(Args)>;
        try {
            return call_impl(f, static_cast<Return(*)(Args...)>(nullptr), args, indices{});
        } catch (GAnyException &e) {
            return e;
        }
    };
}

template<typename Func, typename Return, typename... Args>
void GAnyFunction::initialize(Func &&f, Return (*)(Args...) noexcept)
{
    this->mArgTypes = {GAnyClass::instance<Return>().get(), GAnyClass::instance<Args>().get()...};

    this->mFunc = [this, f](const GAny **args, int32_t argc) -> GAny {
        assert(argc == sizeof...(Args));
        using indices = detail::make_index_sequence<sizeof...(Args)>;
        try {
            return call_impl(f, static_cast<Return(*)(Args...)>(nullptr), args, indices{});
        } catch (GAnyException &e) {
            return e;
        }
    };
}

/// ================ GAnyTypeInfo ================

inline GAnyTypeInfo::GAnyTypeInfo(std::type_index typeIndex)
    : mType(typeIndex)
{
    if (EqualType(typeIndex, typeid(GAny))) {
        mBasicTypeIndex = 0;
        mAnyType = AnyType::user_obj_t;
    } else if (EqualType(typeIndex, typeid(void))) {
        mBasicTypeIndex = 1;
        mAnyType = AnyType::undefined_t;
    } else if (EqualType(typeIndex, typeid(nullptr))) {
        mBasicTypeIndex = 2;
        mAnyType = AnyType::null_t;
    } else if (EqualType(typeIndex, typeid(bool))) {
        mBasicTypeIndex = 3;
        mAnyType = AnyType::boolean_t;
    } else if (EqualType(typeIndex, typeid(char))) {
        mBasicTypeIndex = 4;
        mAnyType = AnyType::int_t;
    } else if (EqualType(typeIndex, typeid(int8_t))) {
        mBasicTypeIndex = 5;
        mAnyType = AnyType::int_t;
    } else if (EqualType(typeIndex, typeid(uint8_t))) {
        mBasicTypeIndex = 6;
        mAnyType = AnyType::int_t;
    } else if (EqualType(typeIndex, typeid(int16_t))) {
        mBasicTypeIndex = 7;
        mAnyType = AnyType::int_t;
    } else if (EqualType(typeIndex, typeid(uint16_t))) {
        mBasicTypeIndex = 8;
        mAnyType = AnyType::int_t;
    } else if (EqualType(typeIndex, typeid(int32_t))) {
        mBasicTypeIndex = 9;
        mAnyType = AnyType::int_t;
    } else if (EqualType(typeIndex, typeid(uint32_t))) {
        mBasicTypeIndex = 10;
        mAnyType = AnyType::int_t;
    } else if (EqualType(typeIndex, typeid(int64_t))) {
        mBasicTypeIndex = 11;
        mAnyType = AnyType::int_t;
    } else if (EqualType(typeIndex, typeid(uint64_t))) {
        mBasicTypeIndex = 12;
        mAnyType = AnyType::int_t;
    } else if (EqualType(typeIndex, typeid(long))) {
        mBasicTypeIndex = 13;
        mAnyType = AnyType::int_t;
    } else if (EqualType(typeIndex, typeid(float))) {
        mBasicTypeIndex = 14;
        mAnyType = AnyType::float_t;
    } else if (EqualType(typeIndex, typeid(double))) {
        mBasicTypeIndex = 15;
        mAnyType = AnyType::double_t;
    } else if (EqualType(typeIndex, typeid(std::string))) {
        mBasicTypeIndex = 16;
        mAnyType = AnyType::string_t;
    } else if (EqualType(typeIndex, typeid(GAnyArray))) {
        mBasicTypeIndex = 17;
        mAnyType = AnyType::array_t;
    } else if (EqualType(typeIndex, typeid(GAnyObject))) {
        mBasicTypeIndex = 18;
        mAnyType = AnyType::object_t;
    } else if (EqualType(typeIndex, typeid(GAnyFunction))) {
        mBasicTypeIndex = 19;
        mAnyType = AnyType::function_t;
    } else if (EqualType(typeIndex, typeid(GAnyClass))) {
        mBasicTypeIndex = 20;
        mAnyType = AnyType::class_t;
    } else if (EqualType(typeIndex, typeid(GAnyClass::GAnyProperty))) {
        mBasicTypeIndex = 21;
        mAnyType = AnyType::property_t;
    } else if (EqualType(typeIndex, typeid(GAnyException))) {
        mBasicTypeIndex = 22;
        mAnyType = AnyType::exception_t;
    } else {
        mBasicTypeIndex = -1;
        mAnyType = AnyType::user_obj_t;
    }
}

/// ================ GAnyClass ================

inline GAnyClass::GAnyClass(std::string nameSpace, std::string name, std::string doc, const GAnyTypeInfo &typeInfo)
    : mNameSpace(std::move(nameSpace)), mName(std::move(name)), mDoc(std::move(doc)), mTypeInfo(typeInfo)
{
    std::replace(mNameSpace.begin(), mNameSpace.end(), '.', '_');

    mIsVirtualClass = mTypeInfo == GAnyTypeInfoP<VirtualClass>();
    updateHash();
}

inline GAnyClass &GAnyClass::setNameSpace(const std::string &ns)
{
    mNameSpace = ns;
    std::replace(mNameSpace.begin(), mNameSpace.end(), '.', '_');

    updateHash();
    return *this;
}

inline GAnyClass &GAnyClass::setName(const std::string &name)
{
    mName = name;
    updateHash();
    return *this;
}

inline GAnyClass &GAnyClass::setDoc(const std::string &doc)
{
    mDoc = doc;
    return *this;
}

inline std::string GAnyClass::getNameSpace() const
{
    std::string ns = mNameSpace;
    std::replace(ns.begin(), ns.end(), '_', '.');
    return ns;
}

inline const std::string &GAnyClass::getName() const
{
    return mName;
}

inline const std::string &GAnyClass::getDoc() const
{
    return mDoc;
}

inline const GAnyTypeInfo &GAnyClass::getTypeInfo() const
{
    return mTypeInfo;
}

inline GAnyClass &GAnyClass::func(const std::string &name, const GAny &function, const GAnyFuncInfo &info,
                                  bool isStatic)
{
    if (name.empty()) {
        assert(false && "Function name cannot be empty.");
        return *this;
    }
    if (!function.isFunction()) {
        assert(false && "Function must be a function.");
        return *this;
    }

    // Copy function
    GAny cloneFunc = function.clone();
    auto &func = *cloneFunc.as<GAnyFunction>();

    func.mIsStatic = isStatic;
    func.mIsMethod = true;
    func.mName = mName + "." + name;
    if (info.doc != "::") {
        func.mInfo = info;
        func.mSetInfo = true;
    }

    if (!func.mIsStatic && !func.mIsVariadicFunc) {
        if (func.mArgTypes.size() <= 1) {
            std::cerr << "Error: GAnyClass.func, The first parameter of a non-static member function(" << func.mName
                    << ") must be an object of the current class(" << mName << ")"
                    << std::endl;
            assert(false);
            return *this;
        }
    }

    GAny *dest = getAttr(name);
    if (dest) {
        if (dest && dest->isFunction()) {
            // if (dest->as<GAnyFunction>()->mIsStatic != func.mIsStatic) {
            //     std::cerr
            //             << "Error: GAnyClass.func, Static and non-static member function cannot be mixed and overloaded: \n"
            //             << "Class: " << mName << "\n"
            //             << "Function: " << func.mName
            //             << std::endl;
            //     assert(false);
            //     return *this;
            // }

            dest = &(dest->overload(cloneFunc));
            return *this;
        }
        dest = nullptr;
    } else {
        dest = setAttr(name, cloneFunc);
    }
    if (!dest) {
        // 这种情况发生在已经存在一个同名非函数成员的情况下
        //        assert(false && "The member name is already in use.");
        return *this;
    }

    if (mInitFn.isUndefined() && name == MetaFunctionNames[static_cast<size_t>(MetaFunction::Init)]) {
        mInitFn = cloneFunc;
        if (isVirtualClass()) {
            makeConstructor(*dest);
        }
    }
    if (mGetItemFn.isUndefined() && name == MetaFunctionNames[static_cast<size_t>(MetaFunction::GetItem)]) {
        mGetItemFn = cloneFunc;
    }
    if (mSetItemFn.isUndefined() && name == MetaFunctionNames[static_cast<size_t>(MetaFunction::SetItem)]) {
        mSetItemFn = cloneFunc;
    }

    return *this;
}

inline GAnyClass &GAnyClass::func(MetaFunction metaFunc, const GAny &function, const GAnyFuncInfo &info,
                                  bool isStatic)
{
    return func(MetaFunctionNames[static_cast<size_t>(metaFunc)], function, info, isStatic);
}

inline GAnyClass &GAnyClass::staticFunc(const std::string &name, const GAny &function, const GAnyFuncInfo &info)
{
    return func(name, function, info, true);
}

inline GAnyClass &GAnyClass::staticFunc(MetaFunction metaFunc, const GAny &function, const GAnyFuncInfo &info)
{
    return staticFunc(MetaFunctionNames[static_cast<size_t>(metaFunc)], function, info);
}

inline GAnyClass &GAnyClass::property(const std::string &name, const GAny &fGet, const GAny &fSet,
                                      const std::string &doc)
{
    setAttr(name, GAnyProperty(name, doc, fGet, fSet));
    return *this;
}

inline GAnyClass &GAnyClass::constant(const std::string &name, const GAny &value)
{
    if (value.isUndefined()) {
        return *this;
    }
    if (mConstantMap.find(name) == mConstantMap.end()) {
        mConstants.emplace_back(name, value);
        mConstantMap[name] = static_cast<int32_t>(mConstants.size()) - 1;
    }
    return *this;
}

inline GAnyClass &GAnyClass::defEnum(const std::vector<std::pair<std::string, GAny>> &enumItems)
{
    for (const auto &item: enumItems) {
        constant(item.first, item.second);
    }
    return *this;
}

inline GAnyClass &GAnyClass::inherit(const GAny &parent)
{
    if (isVirtualClass() && !mParents.empty()) {
        assert(false && "VirtualClass only allows single inheritance");
        return *this;
    }
    if (parent.isClass()) {
        this->mParents.push_back(parent);
    }
    return *this;
}

inline GAny GAnyClass::_call(const GAny &inst, const std::string &methodName, const GAny **args, int32_t argc) const
{
    GAny exception;

    GAny *attr = getAttr(methodName);
    if (attr && attr->isFunction()) {
        const auto &func = *attr->as<GAnyFunction>();
        GAny ret;
        if (!func.mIsStatic) {
            if (inst.isUndefined()) {
                return GAnyException(" at Class " + mName + ": method should be called with self");
            }

            const auto tArgc = argc + 1;
            const auto tArgs = static_cast<const GAny **>(alloca(sizeof(GAny *) * tArgc));
            tArgs[0] = &inst;
            for (int32_t i = 0; i < argc; i++) {
                tArgs[i + 1] = args[i];
            }

            ret = func._call(tArgs, tArgc);
        } else {
            ret = func._call(args, argc);
        }
        if (!ret.isException()) {
            return ret;
        }
        exception = ret;
    }

    GAny baseInstance = inst;

    if (exception.isException() && exception.as<GAnyException>()->getType() != GAnyException::Type::FuncMismatch) {
        goto failure;
    }

    if (isVirtualClass() && inst.as<GAnyObject>()->contains("__parent")) {
        baseInstance = (*inst.as<GAnyObject>())["__parent"];
    }

    for (const GAny &p: this->mParents) {
        GAny ret;
        if (p.isClass()) {
            ret = p.as<GAnyClass>()->_call(baseInstance, methodName, args, argc);
        } else {
            ret = p.getItem(0).as<GAnyClass>()->_call(p.getItem(1)(baseInstance), methodName, args, argc);
        }
        if (!ret.isException()) {
            return ret;
        }
        if (ret.as<GAnyException>()->getType() != GAnyException::Type::FuncMismatch) {
            exception = ret;
            goto failure;
        }
    }

failure: {
        if (!exception.isException()) {
            return GAnyException(GAnyException::Type::FuncMismatch, "No matching method found\n at Class " + mName);
        }
        return GAnyException(exception.as<GAnyException>()->toString()
                             + "\n at Class " + mName);
    }
}

inline void GAnyClass::makeConstructor(GAny pFunc)
{
    auto &f = *pFunc.as<GAnyFunction>();
    auto func = f.mFunc;
    f.mFunc = [this, func](const GAny **args, int32_t argc) -> GAny {
        GAny self = GAny::object();
        self.as<GAnyObject>()->clazz = this;
        std::vector<GAny> args1 = {self};

        if (!mParents.empty()) {
            const GAny parent = mParents[0];
            const GAny parentClass = parent.isClass() ? parent : parent.getItem(0);
            self.setItem("__parent_class", parentClass);
        }

        const int32_t tArgc = argc + 1;
        const auto tArgs = static_cast<const GAny **>(alloca(sizeof(GAny *) * tArgc));

        tArgs[0] = &self;
        for (int32_t i = 0; i < argc; i++) {
            tArgs[i + 1] = args[i];
        }

        func(tArgs, tArgc);

        if (!mParents.empty() && !self.getItem("__parent").isUserObject()) {
            GAny parent = mParents[0];
            GAny p;
            if (parent.isClass()) {
                p = parent.as<GAnyClass>()->_new();
            } else {
                p = parent.getItem(0).as<GAnyClass>()->_new();
            }
            if (!p.isUserObject()) {
                std::cerr << "Failed to create parent class object" << std::endl;
                assert(false && "Failed to create parent class object");
            }
            self.setItem("__parent", p);
        }

        return self;
    };
    if (!f.mIsVariadicFunc && (f.mArgTypes.begin() + 1 != f.mArgTypes.end())) {
        f.mArgTypes.erase(f.mArgTypes.begin() + 1);
    }
}

inline GAny GAnyClass::castToBase(const GAnyClass &targetClass, const GAny &inst)
{
    std::queue<GAny> baseClassQ;
    for (GAny &base: this->mParents) {
        if (base.isArray()) {
            baseClassQ.push(base);
        }
    }

    while (!baseClassQ.empty()) {
        GAny base = baseClassQ.front();
        baseClassQ.pop();

        auto &parent = *base.getItem(0).as<GAnyClass>();
        if (parent == targetClass) {
            if (inst.value()->isSharedPtr()) {
                return base.getItem(2)(inst);
            }
            return base.getItem(1)(inst);
        }
        for (GAny &cBase: parent.mParents) {
            if (cBase.isArray()) {
                baseClassQ.push(cBase);
            }
        }
    }

    return GAny::undefined();
}

inline GAny GAnyClass::_new(const GAny **args, int32_t argc) const
{
    if (!mInitFn.isFunction()) {
        return GAnyException("Class " + mName + " does not have MetaFunc::Init function.");
    }
    return mInitFn.as<GAnyFunction>()->_call(args, argc);
}

inline GAny GAnyClass::_new(std::vector<GAny> &args) const
{
    const auto tArgc = static_cast<int32_t>(args.size());
    const auto tArgs = static_cast<const GAny **>(alloca(sizeof(GAny *) * tArgc));
    for (int32_t i = 0; i < tArgc; i++) {
        tArgs[i] = &args.begin()[i];
    }

    return _new(tArgs, tArgc);
}

template<typename... Args>
GAny GAnyClass::_new(Args... args) const
{
    const std::initializer_list<GAny> argv = {
        (GAny(std::move(args)))...
    };

    const auto tArgc = static_cast<int32_t>(argv.size());
    const auto tArgs = static_cast<const GAny **>(alloca(sizeof(GAny *) * tArgc));
    for (int32_t i = 0; i < tArgc; i++) {
        tArgs[i] = &argv.begin()[i];
    }

    return _new(tArgs, tArgc);
}

inline void GAnyClass::updateHash()
{
    detail::hashCombine(mHash, mNameSpace);
    detail::hashCombine(mHash, mName);
}

inline std::shared_ptr<GAnyClass> GAnyClass::_instance(GAnyTypeInfo typeInfo)
{
    if (pfnGanyClassInstance) {
        std::shared_ptr<GAnyClass> cls;
        pfnGanyClassInstance(&typeInfo, &cls);
        return cls;
    }
    return nullptr;
}

inline bool GAnyClass::containsMember(const std::string &name, bool checkBase) const
{
    if (mAttrMap.find(name) != mAttrMap.end()) {
        return true;
    }
    if (mConstantMap.find(name) != mConstantMap.end()) {
        return true;
    }
    if (!checkBase) {
        return false;
    }

    for (const GAny &p: mParents) {
        if (p.isClass()) {
            return p.as<GAnyClass>()->containsMember(name, checkBase);
        }
        return p.getItem(0).as<GAnyClass>()->containsMember(name, checkBase);
    }

    return false;
}

inline GAny GAnyClass::findMember(const std::string &name, bool checkBase) const
{
    const GAny *attrPtr = getAttr(name);
    GAny attr = attrPtr ? *attrPtr : GAny::undefined();
    if (!attr.isUndefined()) {
        return attr;
    }

    // Constant
    {
        const auto findConstant = mConstantMap.find(name);
        if (findConstant != mConstantMap.end()) {
            return mConstants[findConstant->second].second;
        }
    }
    if (!checkBase) {
        return attr;
    }

    for (const GAny &p: mParents) {
        if (p.isClass()) {
            attr = p.as<GAnyClass>()->findMember(name, checkBase);
        } else {
            attr = p.getItem(0).as<GAnyClass>()->findMember(name, checkBase);
        }

        if (!attr.isUndefined()) {
            return attr;
        }
    }
    return attr;
}

inline std::vector<std::pair<std::string, GAny> > GAnyClass::getMembers(bool checkBase) const
{
    std::vector<std::pair<std::string, GAny> > attrs;
    std::set<std::string> attrNotes;

    for (const auto &item: mAttrs) {
        attrNotes.insert(item.first);
        attrs.push_back(item);
    }

    if (!checkBase) {
        return attrs;
    }

    for (const GAny &p: mParents) {
        std::vector<std::pair<std::string, GAny> > childAttrs;
        if (p.isClass()) {
            childAttrs = p.as<GAnyClass>()->getMembers(checkBase);
        } else {
            childAttrs = p.getItem(0).as<GAnyClass>()->getMembers(checkBase);
        }
        for (const auto &item: childAttrs) {
            if (attrNotes.find(item.first) == attrNotes.end()) {
                attrNotes.insert(item.first);
                attrs.push_back(item);
            }
        }
    }
    return attrs;
}

inline std::vector<std::pair<std::string, GAny> > GAnyClass::getConstants() const
{
    return mConstants;
}

inline bool GAnyClass::isVirtualClass() const
{
    return mIsVirtualClass;
}

inline std::vector<GAny> GAnyClass::getParents() const
{
    std::vector<GAny> parents;
    parents.reserve(mParents.size());
    for (const GAny &p: mParents) {
        if (p.isClass()) {
            parents.push_back(p);
        } else {
            parents.push_back(p.getItem(0));
        }
    }
    return parents;
}

inline GAny GAnyClass::dump() const
{
    GAny dumpObj = GAny::object();
    dumpObj["class"] = this->mName;
    std::string ns = mNameSpace;
    std::replace(ns.begin(), ns.end(), '_', '.');
    dumpObj["nameSpace"] = ns;
    dumpObj["doc"] = this->mDoc;

    GAny parents = GAny::array();
    dumpObj["parents"] = parents;
    if (!this->mParents.empty()) {
        for (const GAny &p: this->mParents) {
            GAny c;
            if (p.isClass()) {
                c = p;
            } else {
                c = p.getItem(0);
            }
            if (c.isClass()) {
                parents.pushBack(c.as<GAnyClass>()->getName());
            }
        }
    }

    if (!this->mAttrs.empty()) {
        std::vector<std::pair<std::string, GAny> > functions;
        std::vector<std::pair<std::string, GAny> > properties;

        for (const auto &it: mAttrs) {
            const auto &attr = it.second;
            if (attr.isFunction()) {
                functions.emplace_back(it.first, attr);
            } else if (attr.isProperty()) {
                properties.emplace_back(it.first, attr);
            }
        }

        GAny methods = GAny::array();
        dumpObj["methods"] = methods;
        for (const auto &it: functions) {
            methods.pushBack(it.second.as<GAnyFunction>()->dump());
        }

        GAny propertiesObj = GAny::array();
        dumpObj["properties"] = propertiesObj;
        for (const auto &it: properties) {
            propertiesObj.pushBack(it.second.as<GAnyProperty>()->dump());
        }
    }
    if (!this->mConstants.empty()) {
        GAny constant = GAny::array();
        dumpObj["constants"] = constant;
        for (const auto &it: mConstants) {
            GAny c = GAny::object();
            c["name"] = it.first;
            c["value"] = it.second;
            constant.pushBack(c);
        }
    }
    return dumpObj;
}

inline std::shared_ptr<GAnyClass> GAnyClass::createFromGAnyObject(GAny object)
{
    if (!object.isObject()) {
        return nullptr;
    }
    std::string name;
    std::string nameSpace;
    std::string doc;
    if (object.contains("__name")) {
        name = object["__name"].toString();
        object.erase("__name");
    }
    if (name.empty()) {
        return nullptr;
    }

    if (object.contains("__namespace")) {
        nameSpace = object["__namespace"].toString();
        object.erase("__namespace");
    }
    if (object.contains("__doc")) {
        doc = object["__doc"].toString();
        object.erase("__doc");
    }

    auto clazz = Class(nameSpace, name, doc);

    if (object.contains("__inherit")) {
        auto inheritInfo = object.getItem("__inherit");
        object.erase("__inherit");
        if (inheritInfo.isClass()) {
            clazz->inherit(inheritInfo);
        }
    }

    GAny variables = GAny::object();
    GAny constructor;

    if (object.contains("__init")) {
        if (object["__init"].isFunction()) {
            constructor = object["__init"];
        }
        object.erase("__init");
    }

    if (object.contains("iterator")) {
        GAny iterator = object["iterator"];
        if (iterator.isFunction()) {
            clazz->func("iterator", iterator);
        }
        object.erase("iterator");
    }

    for (auto oIt = object.iterator(); oIt.hasNext();) {
        auto item = oIt.next();
        std::string k = item.first.toString();
        GAny v = item.second;

        if (v.isFunction()) {
            GAnyFuncInfo mi{"::"};
            std::string miKey = "__i_" + k;
            if (object.contains(miKey)) {
                auto miObj = object.getItem(miKey);
                if (miObj.isObject()) {
                    auto iDoc = miObj.getItem("doc");
                    auto args = miObj.getItem("args");
                    auto returnType = miObj.getItem("returnType");
                    if (iDoc.isString()) {
                        mi.doc = iDoc.toString();
                    }
                    if (args.isArray()) {
                        mi.args.reserve(args.size());
                        for (auto it = args.iterator(); it.hasNext();) {
                            auto aItem = it.next().second;
                            if (aItem.isString()) {
                                mi.args.push_back(aItem.toString());
                            }
                        }
                    }
                    if (returnType.isString()) {
                        mi.returnType = returnType.toString();
                    }
                }
            }
            clazz->func(k, v, mi);
        } else {
            if (k.size() > 4 && k.substr(0, 4) == "__i_") {
                continue;
            }
            if (v.isObject() && v.contains("set") && v["set"].isFunction()
                && v.contains("get") && v["get"].isFunction()) {
                clazz->property(k, v["get"], v["set"]);
            } else {
                variables[k] = v;
            }
        }
    }

    clazz->staticFunc(MetaFunction::Init, GAnyFunction::createVariadicFunction(
                          "",
                          [variables, constructor](const GAny **args, int32_t argc) -> GAny {
                              if (argc == 0) {
                                  return GAny::undefined();
                              }

                              GAny self = *args[0];

                              for (const auto varIt = variables.iterator(); varIt.hasNext();) {
                                  auto item = varIt.next();
                                  std::string k = item.first.toString();
                                  if (!k.empty()) {
                                      self.setItem("__p_" + k, item.second);
                                  }
                              }

                              if (constructor.isFunction()) {
                                  return constructor._call(args, argc);
                              }
                              return self;
                          }));

    for (auto varIt = variables.iterator(); varIt.hasNext();) {
        std::string k = varIt.next().first.toString();
        if (k.empty()) {
            continue;
        }
        clazz->property(
            k,
            [k](GAny &self) {
                return self.getItem("__p_" + k);
            },
            [k](GAny &self, const GAny &v) {
                self.setItem("__p_" + k, v);
            });
    }

    return clazz;
}

inline void GAnyClass::swap(GAnyClass &b) noexcept
{
    if (&b == this) {
        return;
    }
    std::swap(mNameSpace, b.mNameSpace);
    std::swap(mName, b.mName);
    std::swap(mDoc, b.mDoc);
    std::swap(mTypeInfo, b.mTypeInfo);
    std::swap(mHash, b.mHash);
    std::swap(mAttrMap, b.mAttrMap);
    std::swap(mAttrs, b.mAttrs);
    std::swap(mInitFn, b.mInitFn);
    std::swap(mGetItemFn, b.mGetItemFn);
    std::swap(mSetItemFn, b.mSetItemFn);
    std::swap(mParents, b.mParents);
}

inline GAny *GAnyClass::getAttr(const std::string &name) const
{
    const auto it = mAttrMap.find(name);
    if (it != mAttrMap.end()) {
        return &(mAttrs.at(it->second).second);
    }
    return nullptr;
}

inline GAny *GAnyClass::setAttr(const std::string &name, const GAny &attr)
{
    if (mAttrMap.find(name) != mAttrMap.end()) {
        return nullptr;
    }
    mAttrs.emplace_back(name, attr);
    mAttrMap[name] = static_cast<int32_t>(mAttrs.size()) - 1;
    return &(mAttrs.back().second);
}

inline GAny GAnyClass::getItem(const GAny &key, const GAny &inst) const
{
    const bool keyIsStr = key.isString();
    if (keyIsStr) {
        const auto &name = *key.as<std::string>();

        GAny *attr = getAttr(name);
        if (attr) {
            if (attr->isProperty()) {
                const auto &fGet = attr->as<GAnyProperty>()->mFGet;
                if (fGet.isFunction()) {
                    GAny ret = fGet(inst);
                    if (!ret.isException()) {
                        return ret;
                    }
                }
            } else if (attr->isFunction()) {
                if (!attr->as<GAnyFunction>()->mIsStatic) {
                    return GAny::Bind(inst, name);
                }
                return *attr;
            } else {
                return *attr;
            }
        } else {
            const auto findConstant = mConstantMap.find(name);
            if (findConstant != mConstantMap.end()) {
                return mConstants[findConstant->second].second;
            }
        }

        if (isVirtualClass() && inst.is<GAnyObject>()) {
            const auto &instObj = *inst.as<GAnyObject>();
            if (instObj.contains(name)) {
                return instObj[name];
            }
        }
    }

    if (mGetItemFn.isFunction()) {
        GAny ret = mGetItemFn(inst, key);
        if (!ret.isException()) {
            return ret;
        }
    }

    GAny baseInstance = inst;
    if (isVirtualClass() && inst.as<GAnyObject>()->contains("__parent")) {
        baseInstance = (*inst.as<GAnyObject>())["__parent"];
    }

    GAny r = GAny::undefined();
    for (const GAny &p: mParents) {
        if (p.isClass()) {
            r = p.as<GAnyClass>()->getItem(key, baseInstance);
        } else {
            r = p.getItem(0).as<GAnyClass>()->getItem(key, p.getItem(1)(baseInstance));
        }

        if (!r.isUndefined() && !r.isException()) {
            return r;
        }
    }

    return GAny::undefined();
}

inline bool GAnyClass::setItem(const GAny &key, const GAny &inst, const GAny &v, bool justProperty)
{
    const bool keyIsStr = key.isString();

    if (keyIsStr) {
        const auto &name = *key.as<std::string>();

        GAny *attr = getAttr(name);
        if (attr) {
            if (attr->isProperty()) {
                const auto &fSet = attr->as<GAnyProperty>()->mFSet;
                if (fSet.isFunction()) {
                    const GAny ret = fSet(inst, v);
                    if (!ret.isException()) {
                        return true;
                    }
                }
            }
        }

        if (isVirtualClass() && inst.is<GAnyObject>()) {
            auto &instObj = *const_cast<GAny &>(inst).as<GAnyObject>();
            if (instObj.contains(name)) {
                instObj.set(name, v);
                return true;
            }
        }
    }

    if (!isVirtualClass() && mSetItemFn.isFunction()) {
        const GAny ret = mSetItemFn(inst, key, v);
        if (!ret.isException()) {
            return true;
        }
    }

    GAny baseInstance = inst;
    if (isVirtualClass() && inst.as<GAnyObject>()->contains("__parent")) {
        baseInstance = (*inst.as<GAnyObject>())["__parent"];
    }

    bool ok = false;
    for (GAny &p: mParents) {
        if (p.isClass()) {
            ok = p.as<GAnyClass>()->setItem(key, baseInstance, v, true);
        } else {
            ok = p.getItem(0).as<GAnyClass>()->setItem(key, p.getItem(1)(baseInstance), v, true);
        }
        if (ok) {
            return ok;
        }
    }

    if (justProperty) {
        return false;
    }

    if (isVirtualClass() && keyIsStr && inst.is<GAnyObject>()) {
        auto &instObj = *const_cast<GAny &>(inst).as<GAnyObject>();
        instObj.set(*key.as<std::string>(), v);
        return true;
    }

    return false;
}

/// ================ Enum && Macros ================

#define REF_ENUM_OPERATORS(ENUM_TYPE) \
    .func(MetaFunction::EqualTo, [](ENUM_TYPE &self, ENUM_TYPE &b) { return self == b; }) \
    .func(MetaFunction::LessThan, [](ENUM_TYPE &self, ENUM_TYPE &b) { return self < b; }) \
    .func(MetaFunction::ToInt, [](ENUM_TYPE &self) { return (int64_t) self; })

#define REF_ENUM(ENUM_TYPE, NAME_SPACE, DOC) \
    do { \
        std::vector<std::pair<std::string, GAny>> ENUM_TYPE##EnumMap; \
        for (int32_t i = 0; i < Enum##ENUM_TYPE##Count; i++) { \
            ENUM_TYPE##EnumMap.emplace_back(Enum##ENUM_TYPE##Strs()[i], Enum##ENUM_TYPE##Keys()[i]); \
        } \
        gany::Class<ENUM_TYPE>(NAME_SPACE, GX_STRINGIZE(ENUM_TYPE), DOC) \
            .defEnum(ENUM_TYPE##EnumMap)   \
            .func(MetaFunction::ToString, [](ENUM_TYPE &self) { \
                return std::string(EnumToString(self)); \
            }) \
            .func(MetaFunction::EqualTo, [](ENUM_TYPE &self, ENUM_TYPE &b) { return self == b; }) \
            .func(MetaFunction::LessThan, [](ENUM_TYPE &self, ENUM_TYPE &b) { return self < b; }) \
            .func(MetaFunction::BitNot, [](ENUM_TYPE &self) { return ~self; }) \
            .func(MetaFunction::BitOr, [](ENUM_TYPE &self, ENUM_TYPE &b) { return self | b; }) \
            .func(MetaFunction::BitAnd, [](ENUM_TYPE &self, ENUM_TYPE &b) { return self & b; }) \
            .func(MetaFunction::BitXor, [](ENUM_TYPE &self, ENUM_TYPE &b) { return self ^ b; }) \
            .func(MetaFunction::ToInt, [](ENUM_TYPE &self) { return (int64_t) self; }); \
    } while (0)

#define REF_PROPERTY_RW(CLASS, TYPE, PROPERTY, DOC) \
    .property(#PROPERTY,   \
        [](const CLASS &self) -> TYPE {  \
            return self.PROPERTY; \
        },  \
        [](CLASS &self, const TYPE &v) {  \
            self.PROPERTY = v;    \
        }, DOC)

#define REF_PROPERTY_R(CLASS, TYPE, PROPERTY, doc) \
    .property(#PROPERTY,   \
        [](CLASS &self) {  \
            return self.PROPERTY; \
        }, GAny(), doc)



/// ================ GAny Plugin ================

#define GANY_LOAD_MODULE(MODULE_NAME) Register##MODULE_NAME(GANY_VERSION_CODE, pfnGanyGetEnv, pfnGanyClassInstance)
#define GANY_UNLOAD_MODULE(MODULE_NAME) Unregister##MODULE_NAME()

#endif //GX_GANY_H


/// ================ GAny Core ================

#ifdef USE_GANY_CORE

#ifndef GX_GANY_CORE_H
#define GX_GANY_CORE_H

#include <sys/stat.h>
#include <fstream>

#if GX_PLATFORM_WINDOWS

#include <windows.h>

#else

#include <dirent.h>
#include <dlfcn.h>

#endif


typedef int32_t (*RegisterModuleFunc)(int64_t versionCode, PFN_ganyGetEnv pfnGetEnv, PFN_ganyClassInstance pfnClassInstance);
typedef void (*UnregisterModuleFunc)();

namespace core
{
class ClassDB
{
public:
    using ClassPtr = std::shared_ptr<GAnyClass>;

public:
    explicit ClassDB() = default;

    ~ClassDB() = default;

public:
    uint64_t assignModuleIdx()
    {
        return mModuleIdxPool++;
    }

    void beginModule(uint64_t moduleIdx)
    {
        mCurrentModuleIdx = moduleIdx;
    }

    void endModule()
    {
        mCurrentModuleIdx = 0;
    }

    void addClass(const ClassPtr &clazz)
    {
        if (!clazz) {
            return;
        }
        if (clazz->getName().empty()) {
            return;
        }
        std::string key;
        if (!clazz->getNameSpace().empty()) {
            key.append(clazz->getNameSpace());
            key.append(".");
        }
        key.append(clazz->getName());

        std::lock_guard locker(mLock);
        if (mClassMap.find(key) != mClassMap.end()) {
            return;
        }

        mClassMap[key] = clazz;
        if (mCurrentModuleIdx != 0) {
            mModuleMap[mCurrentModuleIdx].push_back(key);
        }
    }

    GAny getClass(const std::string &key)
    {
        if (key.empty()) {
            return GAny::null();
        }

        if (key.size() > 2 && key.substr(key.size() - 2) == ".*") {
            const std::string beginNs = key.substr(0, key.size() - 1);
            std::unordered_map<std::string, ClassPtr> classes;
            std::lock_guard locker(mLock);
            for (const auto &i: mClassMap) {
                if (i.first.size() > beginNs.size()
                    && i.first.substr(0, beginNs.size()) == beginNs
                    && i.second->getNameSpace() + "." == beginNs) {
                    classes.emplace(std::make_pair(i.first.substr(beginNs.size()), i.second));
                }
            }
            return classes;
        }

        std::lock_guard locker(mLock);
        const auto findIt = mClassMap.find(key);
        if (findIt != mClassMap.end()) {
            return findIt->second;
        }
        return GAny::null();
    }

    void releaseModule(uint64_t moduleIdx)
    {
        if (moduleIdx == 0) {
            return;
        }
        std::lock_guard locker(mLock);
        const auto findIt = mModuleMap.find(moduleIdx);
        if (findIt == mModuleMap.end()) {
            return;
        }
        const auto &moduleCNs = findIt->second;
        for (const auto &k : moduleCNs) {
            auto findC = mClassMap.find(k);
            if (findC != mClassMap.end()) {
                mClassMap.erase(findC);
            }
        }

        mModuleMap.erase(findIt);
    }

    std::vector<ClassPtr> getAllClasses() const
    {
        std::vector<ClassPtr> classes;
        std::lock_guard locker(mLock);
        classes.reserve(mClassMap.size());
        for (const auto &i : mClassMap) {
            classes.emplace_back(i.second);
        }

        return classes;
    }

private:
    std::unordered_map<std::string, ClassPtr> mClassMap;
    std::unordered_map<uint64_t, std::vector<std::string>> mModuleMap;
    mutable std::mutex mLock;

    std::atomic<uint64_t> mModuleIdxPool = 1;
    uint64_t mCurrentModuleIdx = 0;
};

class GAnyArrayIterator
{
public:
    using Type = std::vector<GAny>;
public:
    explicit GAnyArrayIterator(Type &array)
        : mArray(array)
    {
        mIter = mArray.begin();
        mOpIter = mArray.end();
    }

    bool hasNext() const
    {
        return mIter != mArray.end();
    }

    GAnyIteratorItem next()
    {
        if (mIter == mArray.end()) {
            return std::make_pair(nullptr, nullptr);
        }
        GAny v = *mIter;
        mOpIter = mIter;
        ++mIter;
        return std::make_pair(static_cast<int32_t>(std::distance(mArray.begin(), mOpIter)), v);
    }

    void remove()
    {
        if (mOpIter != mArray.end()) {
            mIter = mArray.erase(mOpIter);
            mOpIter = mArray.end();
        }
    }

    bool hasPrevious() const
    {
        return mIter != mArray.begin();
    }

    GAnyIteratorItem previous()
    {
        if (mIter == mArray.begin()) {
            return std::make_pair(nullptr, nullptr);
        }
        --mIter;
        mOpIter = mIter;
        GAny v = *mIter;
        return std::make_pair(static_cast<int32_t>(std::distance(mArray.begin(), mOpIter)), v);
    }

    void toFront()
    {
        mIter = mArray.begin();
        mOpIter = mArray.end();
    }

    void toBack()
    {
        mIter = mArray.end();
        mOpIter = mArray.end();
    }

private:
    Type &mArray;
    Type::iterator mIter;
    Type::iterator mOpIter;
};


class GAnyObjectIterator
{
public:
    using Type = std::unordered_map<std::string, GAny>;

public:
    explicit GAnyObjectIterator(Type &map)
        : mMap(map)
    {
        mIter = mMap.begin();
        mOpIter = mMap.end();
    }

    bool hasNext() const
    {
        return mIter != mMap.end();
    }

    GAnyIteratorItem next()
    {
        if (mIter == mMap.end()) {
            return std::make_pair(nullptr, nullptr);
        }
        std::pair<const std::string, GAny> v = *mIter;
        mOpIter = mIter;
        ++mIter;
        return std::make_pair(v.first, v.second);
    }

    void remove()
    {
        if (mOpIter != mMap.end()) {
            mIter = mMap.erase(mOpIter);
            mOpIter = mMap.end();
        }
    }

    void toFront()
    {
        mIter = mMap.begin();
        mOpIter = mMap.end();
    }

private:
    Type &mMap;
    Type::iterator mIter;
    Type::iterator mOpIter;
};

class GAnyObjectConstIterator
{
public:
    using Type = const std::unordered_map<std::string, GAny>;

public:
    explicit GAnyObjectConstIterator(Type &map)
        : mMap(map)
    {
        mIter = mMap.begin();
        mOpIter = mMap.end();
    }

    bool hasNext() const
    {
        return mIter != mMap.end();
    }

    GAnyIteratorItem next()
    {
        if (mIter == mMap.end()) {
            return std::make_pair(nullptr, nullptr);
        }
        std::pair<const std::string, GAny> v = *mIter;
        mOpIter = mIter;
        ++mIter;
        return std::make_pair(v.first, v.second);
    }

    void toFront()
    {
        mIter = mMap.begin();
        mOpIter = mMap.end();
    }

private:
    Type &mMap;
    Type::const_iterator mIter;
    Type::const_iterator mOpIter;
};



static GAny &getEnv()
{
    static GAny _env = GAny::object();
    return _env;
}

static void GX_API_CALL ganyGetEnvImpl(void *env)
{
    const GAny &_env = getEnv();
    GAny &retEnv = *reinterpret_cast<GAny *>(env);
    retEnv = _env;
}

static void GX_API_CALL ganyClassInstanceImpl(void *typeInfo, void *ret)
{
    static std::mutex lock;
    static std::unordered_map<std::string_view, std::shared_ptr<GAnyClass>> clsMap;
    static std::array<std::shared_ptr<GAnyClass>, 25> basicTypeArray;

    const auto *typeInfoPtr = reinterpret_cast<GAnyTypeInfo *>(typeInfo);
    std::shared_ptr<GAnyClass> &cls = *reinterpret_cast<std::shared_ptr<GAnyClass> *>(ret);

    if (typeInfoPtr->basicTypeIndex() >= 0) {
        auto &basicTypeRef = basicTypeArray[typeInfoPtr->basicTypeIndex()];
        if (!basicTypeRef) {
            basicTypeRef = std::shared_ptr<GAnyClass>(
                GX_NEW(GAnyClass, "", typeInfoPtr->getDemangleName(), "", *typeInfoPtr));
        }
        cls = basicTypeRef;
        return;
    }

    std::string_view className = typeInfoPtr->getName();

    std::lock_guard locker(lock);
    const auto it = clsMap.find(className);
    if (it != clsMap.end()) {
        cls = it->second;
        return;
    }
    cls = std::shared_ptr<GAnyClass>(GX_NEW(GAnyClass, "", typeInfoPtr->getDemangleName(), "", *typeInfoPtr));
    clsMap.emplace(className, cls);
}


#define MAX_ABS_PATH 2048


#if GX_PLATFORM_WINDOWS

class GX_API WString
{
public:
    WString(const std::string &str)
    {
        int len = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int) str.size(), nullptr, 0);
        mData.resize(len + 1);
        MultiByteToWideChar(CP_UTF8, 0, str.data(), (int) str.size(), mData.data(), len);
        mData[len] = L'\0';
    }

    WString(const std::wstring &wStr)
    {
        mData.resize(wStr.size() + 1);
        memcpy(mData.data(), wStr.data(), sizeof(wchar_t) * wStr.size());
        mData[wStr.size()] = L'\0';
    }

    WString(const WString &b) = default;

    WString(WString &&b) noexcept
        : mData(std::move(b.mData))
    {
    }

    WString &operator=(const WString &b)
    {
        if (this != &b) {
            mData = b.mData;
        }
        return *this;
    }

    WString &operator=(WString &&b) noexcept
    {
        if (this != &b) {
            std::swap(mData, b.mData);
        }
        return *this;
    }

public:
    std::string toString() const
    {
        const int len = WideCharToMultiByte(CP_UTF8, 0, data(), length(), nullptr, 0, nullptr, nullptr);
        std::vector<char> utf8(len + 1);
        WideCharToMultiByte(CP_UTF8, 0, data(), length(), utf8.data(), len, nullptr, nullptr);
        utf8[len] = '\0';
        return std::string(utf8.data(), utf8.size() - 1);
    }

    const wchar_t *data() const
    {
        return mData.data();
    }

    int32_t length() const
    {
        return (int32_t) mData.size() - 1;
    }

private:
    std::vector<wchar_t> mData;
};

#endif

static void *dlOpen(const std::string &path)
{
#if GX_PLATFORM_WINDOWS
    const WString wStr = WString(path);
    return (void *) ::LoadLibraryW(wStr.data());
#else
    return ::dlopen(path.c_str(), RTLD_LOCAL | RTLD_LAZY);
#endif
}

static void dlClose(void *handle)
{
#if GX_PLATFORM_WINDOWS
    ::FreeLibrary((HMODULE) handle);
#else
    ::dlclose(handle);
#endif
}

static void *dlSym(void *handle, const std::string &symbol)
{
#if GX_PLATFORM_WINDOWS
    return (void *) ::GetProcAddress((HMODULE) handle, symbol.c_str());
#else
    return ::dlsym(handle, symbol.c_str());
#endif
}

static bool fileExists(const std::string &path)
{
    if (path.empty()) {
        return false;
    }
#if GX_PLATFORM_WINDOWS
    return _waccess(WString(path).data(), 0) == 0;
#else
    struct stat fstat{};
    return stat(path.c_str(), &fstat) == 0;
#endif
}

static std::string strReplace(const std::string &str, const std::string &src, const std::string &dst)
{
    std::string out = str;
    size_t index = 0;
    while (index < out.size() && (index = out.find(src, index)) != std::string::npos) {
        out = out.replace(index, src.size(), dst);
        index += dst.size();
    }
    return out;
}

static std::string formatPath(std::string path)
{
    if (path.empty()) {
        return "";
    }
    path = strReplace(path, "\\", "/");
    if (path.at(path.size() - 1) == '/') {
        path = path.substr(0, path.length() - 1);
#if GX_PLATFORM_WINDOWS || GX_PLATFORM_WINRT
        if (path.at(path.size() - 1) == ':') {
            path += "/";
        }
#else
        if (path.empty()) {
            path += "/";
        }
#endif
    }
#if GX_PLATFORM_WINDOWS || GX_PLATFORM_WINRT
    if (path.length() == 2 && path.at(path.size() - 1) == ':') {
        path += "/";
    }
#endif
    return path;
}

static std::string absoluteFilePath(const std::string &path)
{
    if (path.empty()) {
        return "";
    }
    std::string absString;
#if GX_PLATFORM_WINDOWS || GX_PLATFORM_WINRT
    wchar_t wAbsPath[MAX_ABS_PATH];
    _wfullpath(wAbsPath, WString(path).data(), MAX_ABS_PATH);
    absString = WString(wAbsPath).toString();
#elif GX_PLATFORM_POSIX
    char absPath[PATH_MAX];
    if (realpath(path.c_str(), absPath)) {
        absString = absPath;
    } else {
        absString = path;
    }
#else
    #error "Unsupported platform!!"
#endif
    return formatPath(absString);
}

static std::vector<std::pair<std::string, void*>> &getLibTable()
{
    static std::vector<std::pair<std::string, void*>> sLibTable;
    return sLibTable;
}

static bool loadCPlugin(std::string path)
{
    std::string inPath = path;

    path = formatPath(path);

    std::string libName;
    std::string dir;
    if (path.find_last_of('/') != std::string::npos) {
        libName = path.substr(path.find_last_of('/') + 1);
        dir = path.substr(0, path.find_last_of('/'));
    } else {
        libName = path;
        dir = ".";
    }

    std::string libFile = dir;
    libFile.append("/");
    libFile.append(libName);
    while (!fileExists(libFile)) {
#if GX_PLATFORM_WINDOWS
        libFile = dir;
        libFile.append("/");
        libFile.append(libName);
        libFile.append(".dll");
        if (fileExists(libFile)) {
            break;
        }
        libFile = dir;
        libFile.append("/lib");
        libFile.append(libName);
        libFile.append(".dll");
#else
#if GX_PLATFORM_LINUX || GX_PLATFORM_ANDROID || GX_PLATFORM_BSD
        libFile = dir;
        libFile.append("/lib");
        libFile.append(libName);
        libFile.append(".so");
        if (fileExists(libFile)) {
            break;
        }
        libFile = dir;
        libFile.append("/");
        libFile.append(libName);
        libFile.append(".so");
#elif GX_PLATFORM_OSX || GX_PLATFORM_IOS
        libFile = dir;
        libFile.append("/lib");
        libFile.append(libName);
        libFile.append(".dylib");
        if (fileExists(libFile)) {
            break;
        }
        libFile = dir;
        libFile.append("/lib");
        libFile.append(libName);
        libFile.append(".bundle");
        if (fileExists(libFile)) {
            break;
        }
        libFile = dir;
        libFile.append("/lib");
        libFile.append(libName);
        libFile.append(".so");
        if (fileExists(libFile)) {
            break;
        }
        libFile = dir;
        libFile.append("/");
        libFile.append(libName);
        libFile.append(".dylib");
        if (fileExists(libFile)) {
            break;
        }
        libFile = dir;
        libFile.append("/");
        libFile.append(libName);
        libFile.append(".bundle");
        if (fileExists(libFile)) {
            break;
        }
        libFile = dir;
        libFile.append("/");
        libFile.append(libName);
        libFile.append(".so");
#endif
#endif
        break;
    }

    if (fileExists(libFile)) {
        auto &libTable = getLibTable();
        if (std::find_if(libTable.begin(), libTable.end(), [&inPath](const auto &i) {
            return i.first == inPath;
        }) != libTable.end()) {
            return false;
        }

        void *lib = dlOpen(libFile);
        if (lib == nullptr) {
            return false;
        }
        void *regFuncPtr = dlSym(lib, "RegisterModule");
        if (!regFuncPtr) {
            return false;
        }
        const auto regFunc = reinterpret_cast<RegisterModuleFunc>(regFuncPtr);
        const int32_t retCode = regFunc(GANY_VERSION_CODE, pfnGanyGetEnv, pfnGanyClassInstance);

        if (retCode == 0) {
            libTable.push_back(std::make_pair(inPath, lib));
        }

        return retCode == 0;
    }
    return false;
}

static bool loadPluginImpl(const std::string &libPath)
{
    return loadCPlugin(libPath);
}

static bool unloadPluginImpl(const std::string &libPath)
{
    auto &libTable = getLibTable();
    const auto findIt = std::find_if(libTable.begin(), libTable.end(), [&libPath](const auto &i) {
        return i.first == libPath;
    });
    if (findIt == libTable.end()) {
        return false;
    }

    void *lib = findIt->second;
    void *regFuncPtr = dlSym(lib, "UnregisterModule");
    if (!regFuncPtr) {
        return false;
    }
    const auto regFunc = reinterpret_cast<UnregisterModuleFunc>(regFuncPtr);
    regFunc();

    dlClose(lib);
    libTable.erase(findIt);

    return true;
}

static void initPluginLoader()
{
    const GAny loadPluginFunc = [](const std::string &libPath) {
        return loadPluginImpl(libPath);
    };
    const GAny unloadPluginFunc = [](const std::string &libPath) {
        return unloadPluginImpl(libPath);
    };

    getEnv().setItem("__F_LOAD_PLUGIN", loadPluginFunc);
    getEnv().setItem("__F_UNLOAD_PLUGIN", unloadPluginFunc);
}
}


REGISTER_GANY_MODULE(Builtin)
{
    using namespace core;

    GAny &envObj = getEnv();

    // ClassDB
    const auto tClassDB = GAnyClass::Class<ClassDB>();
    tClassDB->setName("ClassDB")
            .setNameSpace("Core")
            .staticFunc(MetaFunction::Init, []() {
                return GAny::New<ClassDB>();
            })
            .func("assignModuleIdx", &ClassDB::assignModuleIdx)
            .func("beginModule", &ClassDB::beginModule)
            .func("endModule", &ClassDB::endModule)
            .func("addClass", &ClassDB::addClass)
            .func("getClass", &ClassDB::getClass)
            .func("releaseModule", &ClassDB::releaseModule)
            .func("getAllClasses", &ClassDB::getAllClasses);
    envObj["__CLASS_DB"] = tClassDB->_new();

    // Base type
    GAnyClass::Class < int32_t > ()->setName("int32");
    GAnyClass::Class < uint32_t > ()->setName("uint32");
    GAnyClass::Class < int64_t > ()->setName("int64");
    GAnyClass::Class < uint64_t > ()->setName("uint64");
    GAnyClass::Class < int8_t > ()->setName("int8");
    GAnyClass::Class < uint8_t > ()->setName("uint8");
    GAnyClass::Class < int16_t > ()->setName("int16");
    GAnyClass::Class < uint16_t > ()->setName("uint16");
    GAnyClass::Class < float > ()->setName("float");
    GAnyClass::Class < double > ()->setName("double");
    GAnyClass::Class < bool > ()->setName("bool");

    GAnyClass::Class < GAnyBytePtr > ()->setName("GAnyBytePtr");
    GAnyClass::Class < GAnyConstBytePtr > ()->setName("GAnyConstBytePtr");

    /// string
    GAnyClass::Class < std::string > ()
            ->setName("string")
            .setDoc("string")
            .func(MetaFunction::ToObject, [](std::string &s) { return s; })
            .func(MetaFunction::Length, [](const std::string &self) { return self.size(); })
            .func(MetaFunction::Addition,
                  [](const std::string &self, const std::string &rh) {
                      return self + rh;
                  })
            .func(MetaFunction::EqualTo, [](const std::string &self, std::string &rh) { return self == rh; })
            .func(MetaFunction::LessThan, [](const std::string &self, std::string &rh) { return self < rh; })
            .func(MetaFunction::SetItem, [](std::string &self, int32_t index, char c) {
                if (index < 0 || index >= self.size()) {
                    return;
                }
                self[index] = c;
            })
            .func(MetaFunction::GetItem, [](std::string &self, int32_t index) {
                if (index < 0 || index >= self.size()) {
                    return '\0';
                }
                return self[index];
            })
            .func("c_str", [](std::string &self) { return self.c_str(); });

    GAny::null()
            .classObject()
            .func(MetaFunction::ToString, [](const GAny &) { return "null"; })
            .func(MetaFunction::EqualTo, [](const GAny &a, const GAny &b) {
                return a.isNull() && b.isNull();
            })
            .func(MetaFunction::LessThan, [](const GAny &a, const GAny &b) {
                return false;
            });

    GAnyClass::Class < GAnyArray > ()
            ->setName("GAnyArray")
            .setDoc("GAnyArray")
            .func(MetaFunction::Addition,
                  [](const GAnyArray &self, const GAnyArray &other) {
                      std::lock_guard locker1(self.lock);
                      std::lock_guard locker2(other.lock);
                      std::vector<GAny> ret(self.var);
                      ret.insert(ret.end(), other.var.begin(), other.var.end());
                      return ret;
                  })
            .func(MetaFunction::Multiplication,
                  [](const GAnyArray &self, const int &num) {
                      std::lock_guard locker1(self.lock);
                      std::vector<GAny> ret;
                      for (int i = 0; i < num; ++i) {
                          ret.insert(ret.end(), self.var.begin(), self.var.end());
                      }
                      return ret;
                  })
            .func(MetaFunction::EqualTo, [](const GAnyArray &self, const GAnyArray &rhs) {
                if (self.length() != rhs.length()) {
                    return false;
                }

                auto size = (int32_t) self.length();
                for (int32_t i = 0; i < size; i++) {
                    if (self[i] != rhs[i]) {
                        return false;
                    }
                }

                return true;
            })
            .func("forEach", [](GAnyArray &self, const GAny &func) {
                // func: function(const GAny &value)->bool
                std::lock_guard locker(self.lock);
                for (auto &item: self.var) {
                    auto ret = func(item);
                    if (ret.isBoolean() && !ret.toBool()) {
                        break;
                    }
                }
            })
            .func("insert", [](GAnyArray &self, int32_t index, const GAny &val) {
                std::lock_guard locker(self.lock);
                if (index < 0 || index > self.var.size()) {
                    return;
                }
                auto it = self.var.begin() + index;
                self.var.insert(it, val);
            })
            .func("sort", [](GAnyArray &self, const GAny &comp) {
                std::lock_guard locker(self.lock);
                if (comp.isFunction()) {
                    std::sort(self.var.begin(), self.var.end(), [&comp](const GAny &lhs, const GAny &rhs) {
                        return comp(lhs, rhs).toBool();
                    });
                }
            })
            .func("iterator", [](GAnyArray &self) {
                return GAny::New<GAnyArrayIterator>(self.var);
            });

    GAnyClass::Class < GAnyObject > ()
            ->setName("GAnyObject")
            .setDoc("GAnyObject")
            .func(MetaFunction::Addition,
                  [](const GAnyObject &self, const GAnyObject &rhs) {
                      if (&self == &rhs) {
                          std::lock_guard locker1(self.lock);
                          return self.var;
                      } else {
                          auto ret = self.var;
                          std::lock_guard locker1(self.lock);
                          std::lock_guard locker2(rhs.lock);
                          for (const auto &it: rhs.var) {
                              if (ret.find(it.first) == ret.end()) {
                                  ret.insert(it);
                              }
                          }
                          return ret;
                      }
                  })
            .func(MetaFunction::EqualTo, [](const GAnyObject &self, const GAnyObject &rhs) {
                if (self.length() != rhs.length()) {
                    return false;
                }

                std::lock_guard locker1(self.lock);
                for (const auto &it: self.var) {
                    std::string k = it.first;
                    const auto &v = it.second;
                    if (!rhs.contains(k)) {
                        return false;
                    }
                    if (v != rhs[k]) {
                        return false;
                    }
                }

                return true;
            })
            .func("forEach", [](GAnyObject &self, const GAny &func) {
                // func: function(const std::string &key, const GAny &value)->bool
                std::lock_guard locker(self.lock);
                for (auto &item: self.var) {
                    auto ret = func(item.first, item.second);
                    if (ret.isBoolean() && !ret.toBool()) {
                        break;
                    }
                }
            })
            .func("iterator", [](GAnyObject &self) {
                return GAny::New<GAnyObjectIterator>(self.var);
            });

    GAnyClass::Class < GAnyArrayIterator > ()
            ->setName("GAnyArrayIterator")
            .setDoc("GAnyArray iterator.")
            .func("hasNext", &GAnyArrayIterator::hasNext)
            .func("next", &GAnyArrayIterator::next)
            .func("remove", &GAnyArrayIterator::remove)
            .func("hasPrevious", &GAnyArrayIterator::hasPrevious)
            .func("previous", &GAnyArrayIterator::previous)
            .func("toFront", &GAnyArrayIterator::toFront)
            .func("toBack", &GAnyArrayIterator::toBack);

    GAnyClass::Class < GAnyObjectIterator > ()
            ->setName("GAnyObjectIterator")
            .setDoc("GAnyObject iterator.")
            .func("hasNext", &GAnyObjectIterator::hasNext)
            .func("next", &GAnyObjectIterator::next)
            .func("remove", &GAnyObjectIterator::remove)
            .func("toFront", &GAnyObjectIterator::toFront);

    GAnyClass::Class < GAnyObjectConstIterator > ()
            ->setName("GAnyObjectConstIterator")
            .setDoc("GAnyObject iterator.")
            .func("hasNext", &GAnyObjectConstIterator::hasNext)
            .func("next", &GAnyObjectConstIterator::next)
            .func("toFront", &GAnyObjectConstIterator::toFront);

    GAnyClass::Class < GAnyFuncInfo > ()
            ->setName("GAnyFuncInfo")
            .setDoc("GAnyFuncInfo")
            .readWrite("doc", &GAnyFuncInfo::doc)
            .staticFunc(MetaFunction::Init, [](const GAny &arg) -> GAny {
                if (arg.is<GAnyFuncInfo>()) {
                    return arg;
                }

                GAnyFuncInfo info{};
                GAny obj = arg.toObject();
                if (obj.isObject()) {
                    if (obj.contains("doc")) {
                        info.doc = obj["doc"].toString();
                    }
                    if (obj.contains("args") && obj["args"].isArray()) {
                        const auto &array = *obj["args"].as<GAnyArray>();
                        info.args.resize(array.var.size());
                        for (size_t i = 0, n = array.var.size(); i < n; i++) {
                            info.args[i] = array.var[i].toString();
                        }
                    }
                }
                return info;
            })
            .property("args", [](GAnyFuncInfo &self) {
                return self.args;
            }, [](GAnyFuncInfo &self, const GAny &args) {
                GAny array = args.toObject();
                if (!array.isArray()) {
                    return;
                }
                const auto &arrVar = *array.as<GAnyArray>();

                self.args.clear();
                self.args.resize(arrVar.var.size());
                for (size_t i = 0, n = arrVar.var.size(); i < n; i++) {
                    self.args[i] = arrVar.var[i].toString();
                }
            });

    GAnyClass::Class < GAnyFunction > ()
            ->setName("GAnyFunction")
            .setDoc("GAnyFunction")
            .func(MetaFunction::ToString, [](GAnyFunction &self) {
                std::stringstream ss;
                ss << "<" << (self.isMethod() ? (self.isStatic() ? "StaticMethod" : "Method") : "Function");
                if (!self.getName().empty()) {
                    ss << ": " << self.getName();
                }
                ss << " at " << &self << ">";
                return ss.str();
            })
            .func(MetaFunction::EqualTo, [](GAnyFunction &self, GAnyFunction &rh) {
                return &self == &rh;
            })
            .func("signature", &GAnyFunction::signature)
            .func("getName", &GAnyFunction::getName)
            .func("setName", &GAnyFunction::setName)
            .func("isStatic", &GAnyFunction::isStatic)
            .func("isMethod", &GAnyFunction::isMethod)
            .func("isVariadicFunction", &GAnyFunction::isVariadicFunction)
            .func("getReturnType", &GAnyFunction::getReturnType)
            .func("getArgTypes", &GAnyFunction::getArgTypes)
            .func("nextOverload", &GAnyFunction::nextOverload)
            .func("compareArgs", &GAnyFunction::compareArgs)
            .func("setBoundData", &GAnyFunction::setBoundData)
            .func("getBoundData", &GAnyFunction::getBoundData);

    GAnyClass::Class < GAnyClass > ()
            ->setName("GAnyClass")
            .setDoc("GAnyClass")
            .func("__getParents", &GAnyClass::getParents)
            .func("__containsMember", [](GAnyClass &self, const std::string &name, bool checkBase) {
                return self.containsMember(name, checkBase);
            })
            .func("__findMember", [](GAnyClass &self, const std::string &name, bool checkBase) {
                return self.findMember(name, checkBase);
            })
            .func("__getMembers", [](GAnyClass &self, bool checkBase) {
                GAny array = GAny::array();
                auto members = self.getMembers(checkBase);
                for (const auto &item: members) {
                    GAny itemArray = {item.first, item.second};
                    array.pushBack(itemArray);
                }
                return array;
            });

    GAnyClass::Class < GAny > ()
            ->setName("GAny")
            .setDoc("GAny");

    GAnyClass::Class < GAnyException > ()
            ->setName("GAnyException")
            .setDoc("GAnyException")
            .func(MetaFunction::ToString, [](GAnyException &self) { return self.what(); })
            .func("what", [](GAnyException &self) { return (std::string) self.what(); });

    GAnyClass::Class < GAnyClass::GAnyProperty > ()
            ->setName("GAnyProperty")
            .setDoc("GAnyClass::GAnyProperty")
            .func(MetaFunction::ToObject, &GAnyClass::GAnyProperty::dump)
            .func(MetaFunction::ToString, [](GAnyClass::GAnyProperty &self) {
                std::string ss = "<Property: " + self.getName() + ">";
                return ss;
            })
            .func("getName", &GAnyClass::GAnyProperty::getName)
            .func("getDoc", &GAnyClass::GAnyProperty::getDoc)
            .func("hasGetter", &GAnyClass::GAnyProperty::hasGetter)
            .func("hasSetter", &GAnyClass::GAnyProperty::hasSetter);

    GAnyClass::Class<std::pair<const std::string, GAny>>
    ()
            ->setName("GAnyObjectPair")
            .property("first", [](std::pair<const std::string, GAny> &self) -> std::string {
                return self.first;
            })
            .property("second", [](std::pair<const std::string, GAny> &self) -> GAny {
                return self.second;
            })
            .property("key", [](std::pair<const std::string, GAny> &self) -> std::string {
                return self.first;
            })
            .property("value", [](std::pair<const std::string, GAny> &self) -> GAny {
                return self.second;
            })
            .func(MetaFunction::ToObject, [](std::pair<const std::string, GAny> &self) {
                GAny obj = GAny::object();
                obj[self.first] = self.second;
                return obj;
            })
            .func(MetaFunction::ToString, [](std::pair<const std::string, GAny> &self) {
                GAny obj = GAny::object();
                obj[self.first] = self.second;
                return obj.toString();
            });

    gany::Class<GAnyIteratorItem>("", "GAnyIteratorItem", "GAny iterator item.")
            .func(MetaFunction::Init, [](const GAny &k, const GAny &v) {
                return std::make_pair(k, v);
            })
            .property("first", [](GAnyIteratorItem &self) -> GAny {
                return self.first;
            })
            .property("second", [](GAnyIteratorItem &self) -> GAny {
                return self.second;
            })
            .property("key", [](GAnyIteratorItem &self) -> GAny {
                return self.first;
            })
            .property("value", [](GAnyIteratorItem &self) -> GAny {
                return self.second;
            })
            .func(MetaFunction::ToObject, [](GAnyIteratorItem &self) {
                GAny obj = GAny::array();
                obj.pushBack(self.first);
                obj.pushBack(self.second);
                return obj;
            })
            .func(MetaFunction::ToString, [](GAnyIteratorItem &self) {
                GAny obj = GAny::array();
                obj.pushBack(self.first);
                obj.pushBack(self.second);
                return obj.toString();
            });

    REF_ENUM(AnyType, "", "GAny Enum AnyType.");
    REF_ENUM(MetaFunction, "", "GAny Enum MetaFunction.");

    auto AnyClass = GAnyClass::Class("", "AnyClass", "Class Registration Tool.");
    GAny::Export(AnyClass);
    AnyClass->staticFunc(
                    MetaFunction::Init,
                    [](GAny &self,
                       const std::string &nameSpace,
                       const std::string &name,
                       const std::string &doc) {
                        self.setItem("clazz", GAnyClass::Class(nameSpace, name, doc));
                    })
            .func("getClass", [](GAny &self) {
                return self.getItem("clazz");
            })
            .func("inherit", [](GAny &self, const GAny &parent) {
                if (parent.is<GAnyClass>()) {
                    self.getItem("clazz").as<GAnyClass>()->inherit(parent);
                } else if (parent.is("AnyClass")) {
                    self.getItem("clazz").as<GAnyClass>()->inherit(parent.getItem("clazz"));
                }
                return self;
            }, {"Inherited from", {"parent:AnyClass"}, "AnyClass"})
            .func("func", [](GAny &self, const std::string &name, const GAny &function) {
                self.getItem("clazz").as<GAnyClass>()->func(name, function);
                return self;
            }, {"Defining function.", {"name", "function:function"}, "AnyClass"})
            .func("func", [](GAny &self, const std::string &name, const GAny &function, const GAnyFuncInfo &info) {
                self.getItem("clazz").as<GAnyClass>()->func(name, function, info);
                return self;
            }, {"Defining function.", {"name", "function:function", "info"}, "AnyClass"})
            .func("func", [](GAny &self, MetaFunction metaFunc, const GAny &function) {
                self.getItem("clazz").as<GAnyClass>()->func(metaFunc, function);
                return self;
            }, {"Defining function.", {"metaFunc", "function:function"}, "AnyClass"})
            .func("func", [](GAny &self, MetaFunction metaFunc, const GAny &function, const GAnyFuncInfo &info) {
                self.getItem("clazz").as<GAnyClass>()->func(metaFunc, function, info);
                return self;
            }, {"Defining function.", {"metaFunc", "function:function", "info"}, "AnyClass"})
            .func("staticFunc", [](GAny &self, const std::string &name, const GAny &function) {
                self.getItem("clazz").as<GAnyClass>()->staticFunc(name, function);
                return self;
            }, {"Defining static function.", {"name", "function:function"}, "AnyClass"})
            .func("staticFunc",
                  [](GAny &self, const std::string &name, const GAny &function, const GAnyFuncInfo &info) {
                      self.getItem("clazz").as<GAnyClass>()->staticFunc(name, function, info);
                      return self;
                  }, {"Defining static function.", {"name", "function:function", "info"}, "AnyClass"})
            .func("staticFunc", [](GAny &self, MetaFunction metaFunc, const GAny &function) {
                self.getItem("clazz").as<GAnyClass>()->staticFunc(metaFunc, function);
                return self;
            }, {"Defining static function.", {"metaFunc", "function:function"}, "AnyClass"})
            .func("staticFunc",
                  [](GAny &self, MetaFunction metaFunc, const GAny &function, const GAnyFuncInfo &info) {
                      self.getItem("clazz").as<GAnyClass>()->staticFunc(metaFunc, function, info);
                      return self;
                  }, {"Defining static function.", {"metaFunc", "function:function", "info"}, "AnyClass"})
            .func("property", [](GAny &self, const std::string &name,
                                 const GAny &fGet, const GAny &fSet) {
                self.getItem("clazz").as<GAnyClass>()->property(name, fGet, fSet);
                return self;
            }, {"Defining property.", {"name", "fGet:function", "fSet:function"}, "AnyClass"})
            .func("property", [](GAny &self, const std::string &name,
                                 const GAny &fGet, const GAny &fSet,
                                 const std::string &doc) {
                self.getItem("clazz").as<GAnyClass>()->property(name, fGet, fSet, doc);
                return self;
            }, {"Defining property.", {"name", "fGet:function", "fSet:function", "doc"}, "AnyClass"})
            .func("constant", [](GAny &self, const std::string &name, const GAny &value) {
                self.getItem("clazz").as<GAnyClass>()->constant(name, value);
                return self;
            })
            .func("defEnum", [](GAny &self, const GAny &enumObj) {
                if (enumObj.isObject()) {
                    std::vector<std::pair<std::string, GAny>> enumItems;
                    const auto enumMap = enumObj.castAs<std::unordered_map<std::string, GAny> >();
                    for (const auto i : enumMap) {
                        enumItems.emplace_back(i.first, i.second);
                    }
                    std::sort(enumItems.begin(), enumItems.end(), [](const auto &a, const auto &b) {
                        return a.second < b.second;
                    });
                    self.getItem("clazz").as<GAnyClass>()->defEnum(enumItems);
                }
                return self;
            }, {"Defining enum", {"enumObj:object"}});
}

static void GX_API_CALL initGAnyCore()
{
    static bool sInited = false;
    if (sInited) {
        return;
    }
    sInited = true;

    RegisterBuiltin(GANY_VERSION_CODE, core::ganyGetEnvImpl, core::ganyClassInstanceImpl);
    core::initPluginLoader();
}

#undef USE_GANY_CORE
#endif //GX_GANY_CORE_H
#endif //USE_GANY_CORE
