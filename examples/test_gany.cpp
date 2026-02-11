//
// Created by Gxin on 2022/5/25.
//

#define USE_GANY_CORE
#include "gx/gany.h"

#include <cstdlib>
#include <cassert>
#include <inttypes.h>


using namespace gany;

class TestA
{
public:
    TestA()
    {
        printf("TestA()\n");
    }

    TestA(int a, double b)
            : a(a), b(b)
    {
        printf("TestA(int a, float b)\n");
    }

    virtual ~TestA()
    {
        printf("~TestA()\n");
    }

    int getA() const
    { return a; }

    void setA(int a)
    {
        this->a = a;
    }

    double getB() const
    {
        return b;
    }

    void setB(double b)
    {
        this->b = b;
    }

    virtual void print()
    {
        printf("TestA print a: %d, b: %f\n", a, b);
    }

protected:
    int a = 0;
    double b = 0;
};

class TestB : public TestA
{
public:
    TestB()
    {
        printf("TestB()\n");
    }

    TestB(int a, double b, std::string c)
            : TestA(a, b), c(std::move(c))
    {
        printf("TestB(int a, float b, string c)\n");
    }

    ~TestB()
    {
        printf("~TestB()\n");
    }

    void setC(const std::string &v)
    {
        c = v;
    }

    std::string getC() const
    {
        return c;
    }

    void print() override
    {
        printf("TestB print a: %d, b: %f, c: %s\n", a, b, c.c_str());
    }

    void print(int v)
    {
        printf("TestB print v: %d\n", v);
    }

    void print(double v)
    {
        printf("TestB print v: %.10lf\n", v);
    }

protected:
    std::string c;
};

struct TestC
{
    int a;
    double b;
};


struct InfoA
{
    int32_t v1;
    int64_t v2;

    bool operator==(const InfoA &b) const
    {
        return v1 == b.v1 && v2 == b.v2;
    }
};

struct InfoB
{
    std::map<std::string, InfoA> map;

    bool operator==(const InfoB &b) const
    {
        return map == b.map;
    }
};

struct InfoC
{
    int32_t a;
    float b;
    std::string c;
    std::vector<int32_t> d;
    std::unordered_map<std::string, std::string> e;
    std::vector<InfoB> f;
    InfoA g;

    bool operator==(const InfoC &rhs) const
    {
        return a == rhs.a
               && this->b == rhs.b
               && c == rhs.c
               && d == rhs.d
               && e == rhs.e
               && f == rhs.f
               && g == rhs.g;
    }
};

DEF_ENUM_FLAGS_8(TypeA, int8_t,
                 None, 0,
                 A, 0x01,
                 B, 0x02,
                 C, 0x04,
                 D, 0x08,
                 E, 0x10,
                 F, 0x20,
                 G, 0x40
);

enum class TypeB : int8_t
{
    A = 1,
    B,
    C,
    D,
    E
};
DEF_ENUM_OPERATORS(TypeB, int8_t);


std::string testFunc(int32_t a, float b)
{
    std::stringstream ss;
    ss << "{a=" << a << ", b=" << b << "}";
    return ss.str();
}


int main(int, char **)
{
#if GX_PLATFORM_WINDOWS
    system("chcp 65001");
#endif

    // Initialize GAny Core
    initGAnyCore();

    // Dynamic type
    {
        printf("\n============= dynamic type =============\n\n");

        GAny obj = 1;
        assert(obj.type() == AnyType::int_t);
        printf("obj.type: %s\n", obj.typeName().c_str());

        obj = true;
        assert(obj.type() == AnyType::boolean_t);
        printf("obj.type: %s\n", obj.typeName().c_str());

        obj = 3.14f;
        assert(obj.type() == AnyType::float_t);
        printf("obj.type: %s\n", obj.typeName().c_str());

        obj = 1e10;
        assert(obj.type() == AnyType::double_t);
        printf("obj.type: %s\n", obj.typeName().c_str());

        obj = "hello";
        assert(obj.type() == AnyType::string_t);
        printf("obj.type: %s\n", obj.typeName().c_str());

        obj = {1, 2.2, "4"};
        assert(obj.type() == AnyType::array_t);
        printf("obj.type: %s\n", obj.typeName().c_str());

        obj = {{"a", 1}, {"b", 2}, {"c", {"x", "y", "z"}}};
        assert(obj.type() == AnyType::object_t);
        printf("obj.type: %s\n", obj.typeName().c_str());

        obj = []{};
        assert(obj.type() == AnyType::function_t);
        printf("obj.type: %s\n", obj.typeName().c_str());

        obj = GAny::undefined();
        assert(obj.type() == AnyType::undefined_t);
        printf("obj.type: %s\n", obj.typeName().c_str());

        obj = GAny::null();
        assert(obj.type() == AnyType::null_t);
        printf("obj.type: %s\n", obj.typeName().c_str());
    }

    // Type conversion
    {
        printf("\n============= type conversion =============\n\n");

        // Integer type conversion
        GAny intVal = 42;
        assert(intVal.toInt32() == 42);
        assert(intVal.toInt64() == 42LL);
        assert(intVal.toFloat() == 42.0f);
        assert(intVal.toDouble() == 42.0);
        assert(intVal.toBool() == true);
        assert(intVal.toString() == "42");
        printf("int conversion: %d -> float: %f, double: %lf, bool: %d, string: %s\n",
               intVal.toInt32(), intVal.toFloat(), intVal.toDouble(), intVal.toBool(), intVal.toString().c_str());

        // Float type conversion
        GAny floatVal = 3.14f;
        assert(floatVal.toInt32() == 3);
        assert(floatVal.toFloat() == 3.14f);
        assert(floatVal.toDouble() == (double)3.14f);
        assert(floatVal.toBool() == true);
        printf("float conversion: %f -> int: %d, double: %lf, bool: %d, string: %s\n",
               floatVal.toFloat(), floatVal.toInt32(), floatVal.toDouble(), floatVal.toBool(), floatVal.toString().c_str());

        // Double type conversion
        GAny doubleVal = 2.718281828;
        assert(doubleVal.toInt32() == 2);
        assert(doubleVal.toInt64() == 2LL);
        assert(doubleVal.toDouble() == 2.718281828);
        printf("double conversion: %lf -> int: %d, int64: %lld, string: %s\n",
               doubleVal.toDouble(), doubleVal.toInt32(), doubleVal.toInt64(), doubleVal.toString().c_str());

        // Boolean type conversion
        GAny boolTrue = true;
        GAny boolFalse = false;
        assert(boolTrue.toInt32() == 1);
        assert(boolFalse.toInt32() == 0);
        assert(boolTrue.toString() == "true");
        assert(boolFalse.toString() == "false");
        printf("bool conversion: true -> int: %d, false -> int: %d\n", boolTrue.toInt32(), boolFalse.toInt32());

        // String type conversion
        GAny strNum = "123";
        GAny strFloat = "45.67";
        GAny strBool = "true";
        printf("string conversion: \"%s\" -> int: %d, \"%s\" -> double: %lf\n",
               strNum.toString().c_str(), strNum.toInt32(), strFloat.toString().c_str(), strFloat.toDouble());

        // castAs type conversion
        GAny val = 100;
        assert(val.castAs<int>() == 100);
        assert(val.castAs<int64_t>() == 100LL);
        assert(val.castAs<float>() == 100.0f);
        assert(val.castAs<double>() == 100.0);
        printf("castAs conversion: %d -> int: %d, int64: %lld, float: %f, double: %lf\n",
               val.toInt32(), val.castAs<int>(), val.castAs<int64_t>(), val.castAs<float>(), val.castAs<double>());

        // Zero to boolean
        GAny zero = 0;
        GAny zeroFloat = 0.0;
        assert(zero.toBool() == false);
        assert(zeroFloat.toBool() == false);
        printf("zero to bool: int 0 -> %d, double 0.0 -> %d\n", zero.toBool(), zeroFloat.toBool());

        // Negative number conversion
        GAny negInt = -42;
        GAny negFloat = -3.14;
        assert(negInt.toInt32() == -42);
        assert(negInt.toBool() == true);
        assert(negFloat.toDouble() == -3.14);
        printf("negative conversion: %d -> bool: %d, %lf -> int: %d\n",
               negInt.toInt32(), negInt.toBool(), negFloat.toDouble(), negFloat.toInt32());

        printf("\nPass\n");
    }

    // Exception handling
    {
        printf("\n============= exception handling =============\n\n");

        // Test exception creation
        GAny ex = GAnyException("Test error message");
        assert(ex.isException());
        printf("Exception check: %s\n", ex.isException() ? "true" : "false");
        printf("Exception message: %s\n", ex.toString().c_str());

        // Test JSON parse exception
        GAny invalidJson = GAny::parseJson("{invalid json}");
        if (invalidJson.isException()) {
            printf("JSON parse error detected: %s\n", invalidJson.toString().c_str());
        }

        // Test function call exception
        GAny func = GAnyFunction::createVariadicFunction("testFunc", [](const GAny **/*args*/, int32_t argc) -> GAny {
            if (argc < 1) {
                return GAnyException("Missing argument");
            }
            return GAny("Success");
        });
        GAny result = func();
        if (result.isException()) {
            printf("Function call error: %s\n", result.toString().c_str());
        }

        // Test castAs exception with try-catch
        GAny strVal = "not a number";
        try {
            TestA testA = strVal.castAs<TestA>();
            printf("castAs should have thrown exception\n");
        } catch (const GAnyException &e) {
            printf("castAs exception caught (try-catch): %s\n", e.what());
        }

        // Test castAs with incompatible types using try-catch
        GAny intVal = 42;
        try {
            TestB testB = intVal.castAs<TestB>();
            printf("castAs should have thrown exception\n");
        } catch (const GAnyException &e) {
            printf("castAs incompatible type exception caught (try-catch): %s\n", e.what());
        }

        // Test successful castAs
        GAny numVal = 100;
        try {
            int value = numVal.castAs<int>();
            printf("castAs success: %d\n", value);
        } catch (const GAnyException &e) {
            printf("Unexpected exception: %s\n", e.what());
        }

        printf("\nPass\n");
    }

    // Type checking functions
    {
        printf("\n============= type checking =============\n\n");

        GAny intVal = 42;
        GAny floatVal = 3.14;
        GAny strVal = "hello";
        GAny arrVal = {1, 2, 3};
        GAny objVal = GAny::object();
        GAny funcVal = []{};
        GAny nullVal = GAny::null();
        GAny undefVal = GAny::undefined();

        printf("isInt: %s\n", intVal.isInt() ? "true" : "false");
        printf("isFloat: %s\n", floatVal.isFloat() ? "true" : "false");
        printf("isDouble: %s\n", floatVal.isDouble() ? "true" : "false");
        printf("isNumber (int): %s\n", intVal.isNumber() ? "true" : "false");
        printf("isNumber (float): %s\n", floatVal.isNumber() ? "true" : "false");
        printf("isString: %s\n", strVal.isString() ? "true" : "false");
        printf("isArray: %s\n", arrVal.isArray() ? "true" : "false");
        printf("isObject: %s\n", objVal.isObject() ? "true" : "false");
        printf("isFunction: %s\n", funcVal.isFunction() ? "true" : "false");
        printf("isNull: %s\n", nullVal.isNull() ? "true" : "false");
        printf("isUndefined: %s\n", undefVal.isUndefined() ? "true" : "false");

        printf("\nPass\n");
    }

    // Operators
    {
        printf("\n============= operators =============\n\n");

        // Arithmetic operators
        GAny a = 10;
        GAny b = 3;
        printf("Arithmetic operations: a = %d, b = %d\n", a.toInt32(), b.toInt32());
        printf("a + b = %d\n", (a + b).toInt32());
        printf("a - b = %d\n", (a - b).toInt32());
        printf("a * b = %d\n", (a * b).toInt32());
        printf("a / b = %d\n", (a / b).toInt32());
        printf("a %% b = %d\n", (a % b).toInt32());

        // Float operations
        GAny x = 10.5;
        GAny y = 2.5;
        printf("\nFloat operations: x = %f, y = %f\n", x.toDouble(), y.toDouble());
        printf("x + y = %f\n", (x + y).toDouble());
        printf("x - y = %f\n", (x - y).toDouble());
        printf("x * y = %f\n", (x * y).toDouble());
        printf("x / y = %f\n", (x / y).toDouble());

        // Mixed type operations
        GAny intVal = 20;
        GAny floatVal = 3.14;
        printf("\nMixed operations: int = %d, float = %f\n", intVal.toInt32(), floatVal.toDouble());
        printf("int + float = %f\n", (intVal + floatVal).toDouble());
        printf("int * float = %f\n", (intVal * floatVal).toDouble());

        // Comparison operators
        GAny num1 = 10;
        GAny num2 = 20;
        GAny num3 = 10;
        printf("\nComparison operations: num1 = %d, num2 = %d, num3 = %d\n", num1.toInt32(), num2.toInt32(), num3.toInt32());
        printf("num1 == num3: %s\n", (num1 == num3) ? "true" : "false");
        printf("num1 == num2: %s\n", (num1 == num2) ? "true" : "false");
        printf("num1 != num2: %s\n", (num1 != num2) ? "true" : "false");
        printf("num1 < num2: %s\n", (num1 < num2) ? "true" : "false");
        printf("num1 > num2: %s\n", (num1 > num2) ? "true" : "false");
        printf("num1 <= num3: %s\n", (num1 <= num3) ? "true" : "false");
        printf("num1 >= num3: %s\n", (num1 >= num3) ? "true" : "false");

        // String concatenation
        GAny str1 = "Hello ";
        GAny str2 = "World";
        printf("\nString operations: str1 = \"%s\", str2 = \"%s\"\n", str1.toString().c_str(), str2.toString().c_str());
        printf("str1 + str2 = \"%s\"\n", (str1 + str2).toString().c_str());

        printf("\nPass\n");
    }

    // Array
    {
        printf("\n============= array =============\n\n");
        GAny array = {1, 2, 3, "4", 5.01};
        printf("array.type: %s\n", array.typeName().c_str());
        array.pushBack("6");
        array.pushBack(7);

        GAny array2 = array.clone();
        array2.erase(3); // Remove array[3]

        if (array.contains("4")) {
            printf("array contains \"4\"\n");
        }
        if (array2.contains("4")) {
            printf("array2 contains \"4\"\n");
        }

        printf("array iteration:\n");
        for (int32_t i = 0; i < array.size(); i++) {
            auto item = array[i];
            printf("item.type: %s, ", item.typeName().c_str());
            printf("item: %s\n", item.toString().c_str());
        }
        std::cout << "array dump: " << array << std::endl;
        std::cout << "array2 dump: " << array2 << std::endl;

        GAny array3 = GAny::parseJson(array.toJsonString());
        std::cout << "array3 dump: " << array3 << std::endl;
    }

    // Object
    {
        printf("\n============= object =============\n\n");
        std::string json = R"({"a":1,"b":2.0,"c":"str","d":[1,2,3],"e":{"a":1,"b":2.0,"c":"str"}})";

        GAny obj = GAny::parseJson(json);
        printf("obj.type: %s\n", obj.typeName().c_str());
        obj.setItem("f", "string");
        obj.setItem("g", [](int v) {
            printf("call obj.g(%d)\n", v);
        });
//        obj.erase("b"); // Remove obj["b"]
        obj["i"] = GAny::undefined();   // undefined value can be used to indicate that the current item is removed, and will not appear in the dump (toJsonString) result.
        obj["j"] = {1, 2, "hjk", GAny::undefined(), 3.14f}; // Same as above, undefined items in the array will not appear in the dump result.

        printf("obj iteration: \n");
        for (auto objIt = obj.iterator(); objIt.hasNext();) {
            auto item = objIt.next();
            printf("    key: %s, ", item.first.toString().c_str());
            printf("value(%s): %s\n", item.second.typeName().c_str(), item.second.toString().c_str());
        }

        obj["g"](1);
        obj.call("g", 2);
        std::cout << "obj dump: " << obj << std::endl;

        printf("\nforEach obj:\n");
        printf("==========================\n");
        obj.call("forEach", [](const std::string &k, const GAny &v) {
            printf("%s: %s\n", k.c_str(), v.toString().c_str());
            return true;    // No return value is also acceptable
        });
        printf("==========================\n");
    }

    // Function
    {
        printf("\n============= function =============\n\n");
        GAny func = [](int a) { return a + 1; };
        printf("func.type: %s\n", func.typeName().c_str());
        printf("call func(1): %d\n", func(1).toInt32());

        GAny func2 = [](int a) {
            return a;
        };
        func2.overload([](int a, int b) {
            return a + b;
        });
        func2.overload([](int a, int b, int c) {
            return a + b + c;
        });
        // Functions with the same parameters will not be overridden
        func2.overload([](int a) {
            return a + 1;
        });

        printf("call func2(1): %d\n", func2(1).castAs<int>());
        printf("call func2(1, 2): %d\n", func2(1, 2).castAs<int>());
        printf("call func2(1, 2, 3): %d\n", func2(1, 2, 3).castAs<int>());
    }

    {
        printf("\n============= Enum =============\n\n");

        // Enum flags
        {
            REF_ENUM(TypeA, "", "Enum TypeA");
            // std::cout << GAny::Import("TypeA").castAs<GAnyClass>() << std::endl;

            auto a = GAny::Import("TypeA").getItem("A");
            auto e = GAny::Import("TypeA").getItem("E");

            printf("TypeA::C \"%s\"\n", GAny::Import("TypeA").getItem("C").toString().c_str());

            auto ae = a | e;

            printf("Check TypeA eq: a == TypeA::A, %s\n", (a == GAny::Import("TypeA").getItem("A") ? "true" : "false"));
            printf("Check TypeA eq: a == e, %s\n", (a == e ? "true" : "false"));

            if ((ae & e) == e) {
                printf("Check TypeA flag, e: 0x%x, ae: 0x%x\n", e.toInt32(), ae.toInt32());
            }
        }

        // General Enum
        {
            Class<TypeB>("Gx.Test", "TypeB", "Enum TypeB")
                    .defEnum({
                            {"A", TypeB::A},
                            {"B", TypeB::B},
                            {"C", TypeB::C},
                            {"D", TypeB::D},
                            {"E", TypeB::E}
                    })
                    .func(MetaFunction::ToString, [](TypeB &self) {
                        std::stringstream ss;
                        ss << "<Enum TypeB: ";
                        switch (self) {
                            case TypeB::A:
                                ss << "A";
                                break;
                            case TypeB::B:
                                ss << "B";
                                break;
                            case TypeB::C:
                                ss << "C";
                                break;
                            case TypeB::D:
                                ss << "D";
                                break;
                            case TypeB::E:
                                ss << "E";
                                break;
                            default:
                                break;
                        }
                        ss << " = " << (int32_t) self << ">";
                        return ss.str();
                    })
                            REF_ENUM_OPERATORS(TypeB);  // Other operators: ~, |, &, ^, ==, <, ToInt32

            // std::cout << std::endl;
            // std::cout << GAny::Import("Gx.Test.TypeB").castAs<GAnyClass>() << std::endl;

            printf("TypeB Enums: \n");
            GAny::Import("Gx.Test.TypeB").getItem("Enum").call("forEach", [](const std::string &k, const GAny &v) {
                std::cout << k << ": " << v.toString() << std::endl;
            });
        }
    }

    // Serialize
    {
        printf("\n============= Serialize =============\n\n");

        Class<InfoA>("", "InfoA", "")
        .construct<>()
        .readWrite("v1", &InfoA::v1)
        .readWrite("v2", &InfoA::v2)
        .func(MetaFunction::FromObject, [](InfoA &self, const GAny &obj) {
            // This is only used to demonstrate the usage of FromObject. The current struct has a complete property description, so FromObject is not actually needed to handle deserialization
            self.v1 = obj["v1"].toInt32();
            self.v2 = obj["v2"].toInt64();
        });

        Class<InfoB>("", "InfoB", "")
            .construct<>()
            .readWrite("map", &InfoB::map);

        Class<InfoC>("", "InfoC", "")
            .construct<>()
            .readWrite("a", &InfoC::a)
            .readWrite("b", &InfoC::b)
            .readWrite("c", &InfoC::c)
            .readWrite("d", &InfoC::d)
            .readWrite("e", &InfoC::e)
            .readWrite("f", &InfoC::f)
            .readWrite("g", &InfoC::g);

        InfoB b1{};
        InfoB b2{};
        b1.map["A"] = {.v1 = 1, .v2 = 2};
        b1.map["B"] = {.v1 = 3, .v2 = 4};
        b2.map["C"] = {.v1 = 5, .v2 = 6};
        b2.map["D"] = {.v1 = 7, .v2 = 8};

        InfoC info = {
            .a = 123,
            .b = 3.14,
            .c = "hello",
            .d = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
            .e = {{"XiaoMing", "Man"}, {"XiaoHong", "Woman"}},
            .f = {b1, b2},
            .g = {114, 514}
        };

        // Serialize
        std::string json = GAny(info).toJsonString(2);

        printf("info json: \n%s\n", json.c_str());

        // Deserialize
        GAny obj = GAny::parseJson(json);
        InfoC info2 = obj.castAs<InfoC>();

        assert(info == info2);
        printf("\nPass\n");
    }

    // User type
    {
        printf("\n============= user type =============\n\n");

        Class<TestA>("", "TestA", "class TestA")
                .construct<>()
                .construct<int, double>()
                .constant("N", 10)
                .property("a", &TestA::getA, &TestA::setA)
                .property("b", &TestA::getB, &TestA::setB)
                .func("print", [](GAny &self) {
                    if (self.is<TestA>()) {
                        self.as<TestA>()->print();
                    }
                })
                .func(MetaFunction::ToString, [](const TestA &a) {
                    std::stringstream ss;
                    ss << "TestA(" << a.getA() << ", " << a.getB() << ")";
                    return ss.str();
                })
                .func(MetaFunction::Length, [](const TestA &) {
                    return (size_t) 2;
                });

        Class<TestB>("", "TestB", "class TestB")
                .construct<>()
                .construct<int, double, std::string>()
                .inherit<TestA>()
                .property("c", &TestB::getC, &TestB::setC)
                .func("print", [](TestB &self) {
                    self.print();
                })
                .func("print", [](TestB &self, int v) {
                    self.print(v);
                })
                .func("print", [](TestB &self, double v) {
                    self.print(v);
                });

        Class<TestC>("", "TestC", "struct TestC")
                .func(MetaFunction::Init, [](int a, double b) {
                    return TestC{a, b};
                })
                .readWrite("a", &TestC::a)
                .readOnly("b", &TestC::b);

        GAny ClassTestA = GAny::Import("TestA");
        GAny ta = ClassTestA(1, 123.123);

        printf("TestA.N = %d\n", ClassTestA.getItem("N").toInt32());
        printf("ta.type = %s\n", ta.typeName().c_str());
        printf("ta: %s\n", ta.toString().c_str());
        printf("ta.length: %lld\n", ta.length());

        ta.setItem("a", GAny(3));
        printf("ta.a = %d\n", ta.getItem("a").castAs<int>());
        printf("ta.b = %lf\n", ta.getItem("b").castAs<double>());
        ta.setItem("b", 4.02);
        printf("ta.b = %lf\n", ta.getItem("b").castAs<double>());
        printf("call ta.print()\n");
        ta.call("print");

        auto *p = ta.castAs<TestA *>();
        printf("p->a = %d\n", p->getA());

        GAny refT = ta;
        printf("refT.a = %d\n", refT.getItem("a").castAs<int>());

        ta = nullptr;        // Has ref
        refT = nullptr;     // No ref

        auto ClassTestB = GAny::Import("TestB");
        // std::cout << ClassTestB.castAs<GAnyClass>() << std::endl;
        GAny tb = ClassTestB(1, 2.01, "Hello");

        tb.setItem("c", "Hello tb");
        tb.call("print");
        tb.call("print", 123);
//        ClassTestB["print"](tb, 3.1415926); // This operation is not allowed, because the call to a member function may be inherited from the parent class, and GAny's default call cannot complete this operation.
        // But you can do it this way
        auto caller = tb.getItem("print");
        if (caller.isFunction()) {
            caller(3.1415926);
        }

        printf("tb.toString() = %s\n", tb.toString().c_str());   // Call toString() inherited from TestA
        printf("tb.toObject() = %s\n", tb.toObject().toString().c_str());   // Automatically create GAny object through readable member variables

        auto tc = GAny::Import("TestC")(1, 2.01);
        printf("tc.a = %d\n", tc.getItem("a").castAs<int>());
        printf("tc.b = %f\n", tc.getItem("b").castAs<double>());
        tc.setItem("a", 3);
        printf("tc.a = %d\n", tc.getItem("a").castAs<int>());

        GAny jObj;
        jObj["a"] = 1;
        jObj["b"] = {{"tb", tb}};

        std::cout << "User objects automatically become part of json:\n" << jObj.toJsonString(2) << std::endl;
    }

    // createVariadicFunction
    {
        printf("\n============= createVariadicFunction =============\n\n");

        // 零参数调用
        {
            GAny fn = GAnyFunction::createVariadicFunction("noArgs", [](const GAny **/*args*/, int32_t argc) -> GAny {
                assert(argc == 0);
                return GAny("no args called");
            });
            GAny r = fn();
            assert(r.toString() == "no args called");
            printf("Test 1 (zero args): %s\n", r.toString().c_str());
        }

        // 单参数调用，返回参数本身
        {
            GAny fn = GAnyFunction::createVariadicFunction("echo", [](const GAny **args, int32_t argc) -> GAny {
                if (argc != 1) return GAnyException("Expected 1 arg");
                return *args[0];
            });
            GAny r = fn(42);
            assert(r.toInt32() == 42);
            printf("Test 2 (single arg echo): %d\n", r.toInt32());
        }

        // 多参数求和（整数）
        {
            GAny fn = GAnyFunction::createVariadicFunction("sum", [](const GAny **args, int32_t argc) -> GAny {
                int64_t sum = 0;
                for (int32_t i = 0; i < argc; i++) {
                    sum += args[i]->toInt64();
                }
                return GAny(sum);
            });
            assert(fn(1, 2, 3).toInt64() == 6);
            assert(fn(10, 20, 30, 40).toInt64() == 100);
            assert(fn().toInt64() == 0); // 零参数求和为0
            printf("Test 3 (variadic sum): 1+2+3=%lld, 10+20+30+40=%lld\n",
                   fn(1, 2, 3).toInt64(), fn(10, 20, 30, 40).toInt64());
        }

        // 混合类型参数（int, string, double, bool）
        {
            GAny fn = GAnyFunction::createVariadicFunction("mixedTypes", [](const GAny **args, int32_t argc) -> GAny {
                // 拼接所有参数的字符串表示
                std::string result;
                for (int32_t i = 0; i < argc; i++) {
                    if (i > 0) result += ", ";
                    result += args[i]->toString();
                }
                return GAny(result);
            });
            GAny r = fn(42, "hello", 3.14, true);
            printf("Test 4 (mixed types): %s\n", r.toString().c_str());
            // 验证包含所有参数
            assert(r.toString().find("42") != std::string::npos);
            assert(r.toString().find("hello") != std::string::npos);
            assert(r.toString().find("true") != std::string::npos);
        }

        // 返回不同类型（数组）
        {
            GAny fn = GAnyFunction::createVariadicFunction("toArray", [](const GAny **args, int32_t argc) -> GAny {
                // 将所有参数打包成数组返回
                GAny arr = GAny::array();
                for (int32_t i = 0; i < argc; i++) {
                    arr.pushBack(*args[i]);
                }
                return arr;
            });
            GAny r = fn("a", "b", "c");
            assert(r.isArray());
            assert(r.size() == 3);
            assert(r[0].toString() == "a");
            assert(r[1].toString() == "b");
            assert(r[2].toString() == "c");
            printf("Test 5 (return array): size=%lld, [0]=%s, [1]=%s, [2]=%s\n",
                   r.size(), r[0].toString().c_str(), r[1].toString().c_str(), r[2].toString().c_str());
        }

        // 返回对象
        {
            GAny fn = GAnyFunction::createVariadicFunction("makeObj", [](const GAny **args, int32_t argc) -> GAny {
                // 参数成对出现：key, value, key, value...
                GAny obj = GAny::object();
                for (int32_t i = 0; i + 1 < argc; i += 2) {
                    obj[args[i]->toString()] = *args[i + 1];
                }
                return obj;
            });
            GAny r = fn("name", "Alice", "age", 30);
            assert(r.isObject());
            assert(r["name"].toString() == "Alice");
            assert(r["age"].toInt32() == 30);
            printf("Test 6 (return object): name=%s, age=%d\n",
                   r["name"].toString().c_str(), r["age"].toInt32());
        }

        // 返回异常（参数不足）
        {
            GAny fn = GAnyFunction::createVariadicFunction("needsArgs", [](const GAny **args, int32_t argc) -> GAny {
                if (argc < 2) {
                    return GAnyException("Need at least 2 arguments, got " + std::to_string(argc));
                }
                return *args[0] + *args[1];
            });
            // 参数不足时返回异常
            GAny err = fn(1);
            assert(err.isException());
            printf("Test 7 (exception on too few args): %s\n", err.toString().c_str());
            // 参数充足时正常返回
            GAny ok = fn(10, 20);
            assert(!ok.isException());
            assert(ok.toInt32() == 30);
            printf("Test 7 (success with enough args): %d\n", ok.toInt32());
        }

        // isVariadicFunction 标记检查
        {
            GAny fn = GAnyFunction::createVariadicFunction("checkFlag", [](const GAny **/*args*/, int32_t /*argc*/) -> GAny {
                return GAny();
            });
            assert(fn.is<GAnyFunction>());
            assert(fn.as<GAnyFunction>()->isVariadicFunction());
            assert(fn.as<GAnyFunction>()->getName() == "checkFlag");
            printf("Test 8 (isVariadicFunction): %s, name=%s\n",
                   fn.as<GAnyFunction>()->isVariadicFunction() ? "true" : "false",
                   fn.as<GAnyFunction>()->getName().c_str());
        }

        // 赋值给 GAny 变量并作为对象方法使用
        {
            GAny obj = GAny::object();
            obj["add"] = GAnyFunction::createVariadicFunction("add", [](const GAny **args, int32_t argc) -> GAny {
                double sum = 0;
                for (int32_t i = 0; i < argc; i++) {
                    sum += args[i]->toDouble();
                }
                return GAny(sum);
            });
            // 通过对象调用
            GAny r = obj["add"](1.5, 2.5, 3.0);
            assert(r.toDouble() == 7.0);
            printf("Test 9 (as object method): 1.5+2.5+3.0=%f\n", r.toDouble());
        }

        // GAny 参数传递（传入数组和对象）
        {
            GAny fn = GAnyFunction::createVariadicFunction("complexArgs", [](const GAny **args, int32_t argc) -> GAny {
                // 要求第一个参数是数组，第二个参数是对象
                if (argc < 2) return GAnyException("Need 2 args");
                if (!args[0]->isArray()) return GAnyException("arg[0] must be array");
                if (!args[1]->isObject()) return GAnyException("arg[1] must be object");
                // 返回数组长度 + 对象的键数量
                return GAny((int64_t)(args[0]->size() + args[1]->size()));
            });
            GAny arr = {1, 2, 3};
            GAny obj = GAny::object();
            obj["x"] = 1;
            obj["y"] = 2;
            GAny r = fn(arr, obj);
            assert(r.toInt64() == 5); // 3 + 2
            printf("Test 10 (complex GAny args): array(3) + object(2) = %lld\n", r.toInt64());
        }

        // 大量参数调用
        {
            GAny fn = GAnyFunction::createVariadicFunction("countArgs", [](const GAny **/*args*/, int32_t argc) -> GAny {
                return GAny((int32_t)argc);
            });
            // 测试不同数量的参数
            assert(fn().toInt32() == 0);
            assert(fn(1).toInt32() == 1);
            assert(fn(1, 2).toInt32() == 2);
            assert(fn(1, 2, 3, 4, 5).toInt32() == 5);
            assert(fn(1, 2, 3, 4, 5, 6, 7, 8).toInt32() == 8);
            printf("Test 11 (arg count): 0=%d, 1=%d, 5=%d, 8=%d\n",
                   fn().toInt32(), fn(1).toInt32(),
                   fn(1, 2, 3, 4, 5).toInt32(),
                   fn(1, 2, 3, 4, 5, 6, 7, 8).toInt32());
        }

        printf("\nPass\n");
    }

    // Dynamic class
    {
        printf("\n============= dynamic class =============\n\n");

        auto tTestA = GAny::Import("TestA");

        auto dTypeA = GAnyClass::Class("", "DTypeA", "Dynamic type A");
        dTypeA->inherit(tTestA)
              .staticFunc(MetaFunction::Init, [](GAny &self, int32_t a, double b) {
                  // If Dynamic Class does not actively construct the parent class object, it will be constructed using the parent class default function after the constructor ends
                  self.setItem("__parent", GAny::New<TestA>(a, b));
              });

        auto dTypeB = GAnyClass::Class("", "DTypeB", "Dynamic type B");
        dTypeB->inherit(dTypeA)
              .staticFunc(MetaFunction::Init, [](GAny &self, int32_t a, double b, const std::string &c) {
                  self.setItem("__parent", self.getItem("__parent_class").as<GAnyClass>()->_new(a, b));
                  self.setItem("__p_c", c);
              })
              .property("c",
                        [](GAny &self) {
                            return self.getItem("__p_c");
                        },
                        [](GAny &self, const std::string &v) {
                            self.setItem("__p_c", v);
                        })
              .func(MetaFunction::ToString, [](GAny &self) {
                  GAny ps = self.getItem("__parent").toString();    // Call parent
                  if (ps.isException()) {
                      return ps;
                  }
                  return ps + ", c = " + self.getItem("c").toString();
              });

        auto objA = dTypeA->_new(123, 3.1415);
        printf("objA.a = %" PRId64 "\n", objA.getItem("a").toInt64());
        printf("objA.b = %lf\n", objA.getItem("b").toDouble());
        printf("objA.__str = %s\n", objA.toString().c_str());

        auto objB = dTypeB->_new(456, 7.535213231, "EEEEEE");
        printf("objB.a = %" PRId64 "\n", objB.getItem("a").toInt64());
        printf("objB.b = %lf\n", objB.getItem("b").toDouble());
        printf("objB.c = %s\n", objB.getItem("c").toString().c_str());
        printf("objB.__str = %s\n", objB.toString().c_str());
    }

    return EXIT_SUCCESS;
}
