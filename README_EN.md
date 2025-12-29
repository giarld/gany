# GAny
C++ Dynamic Type and Dynamic Reflection Library

English | [简体中文](README.md)

## Features

### 1. Dynamic Types

GAny supports multiple dynamic types that can change at runtime:

```cpp
GAny obj = 1;              // int type
obj = true;                // boolean type
obj = 3.14f;               // float type
obj = 1e10;                // double type
obj = "hello";             // string type
obj = {1, 2.2, "4"};       // array type
obj = {{"a", 1}, {"b", 2}}; // object type
obj = [](){};              // function type
obj = UserType();          // custom type
obj = GAny::undefined();   // undefined type
obj = GAny::null();        // null type
```

### 2. Dynamic Reflection

Register C++ types using the `Class` template to enable dynamic reflection:

```cpp
// Register class and constructors
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
    });

// Create objects dynamically
GAny ClassTestA = GAny::Import("TestA");
GAny ta = ClassTestA(1, 123.123);

// Access properties and methods dynamically
ta.setItem("a", 3);
int a = ta.getItem("a").castAs<int>();
ta.call("print");
```

Class inheritance support:

```cpp
Class<TestB>("", "TestB", "class TestB")
    .construct<>()
    .construct<int, double, std::string>()
    .inherit<TestA>()  // Inherit from TestA
    .property("c", &TestB::getC, &TestB::setC);
```

### 3. Runtime Type Checking

Rich type checking functions:

```cpp
GAny intVal = 42;
GAny strVal = "hello";
GAny arrVal = {1, 2, 3};

if (intVal.isInt()) { /* ... */ }
if (intVal.isNumber()) { /* ... */ }  // Returns true for int, float, double
if (strVal.isString()) { /* ... */ }
if (arrVal.isArray()) { /* ... */ }

// Other type checks
obj.isFloat();
obj.isDouble();
obj.isBool();
obj.isObject();
obj.isFunction();
obj.isNull();
obj.isUndefined();
obj.isException();
```

### 4. Runtime Type Conversion

Support for conversions between multiple types:

```cpp
GAny intVal = 42;
int32_t i = intVal.toInt32();
int64_t l = intVal.toInt64();
float f = intVal.toFloat();
double d = intVal.toDouble();
bool b = intVal.toBool();
std::string s = intVal.toString();

// Use castAs for strong type conversion
int value = numVal.castAs<int>();
TestA* ptr = ta.castAs<TestA*>();

// String to number conversion
GAny strNum = "123";
int num = strNum.toInt32();  // 123
```

### 5. Runtime Type Information

Get runtime type information:

```cpp
GAny obj = 42;

// Get type enum
AnyType type = obj.type();

// Get type name
std::string typeName = obj.typeName();

// Check if it's a specific C++ type
if (obj.is<TestA>()) {
    TestA* ptr = obj.as<TestA>();
}

// Get length (for array, object, string, etc.)
size_t len = obj.length();
```

### 6. Functions and Function Calls

GAny supports functions as first-class citizens:

```cpp
// Lambda functions
GAny func = [](int a) { return a + 1; };
GAny result = func(1);  // Call function
int value = result.toInt32();  // 2

// Function overloading
GAny func2 = [](int a) { return a; };
func2.overload([](int a, int b) { return a + b; });
func2.overload([](int a, int b, int c) { return a + b + c; });

int r1 = func2(1).castAs<int>();           // 1
int r2 = func2(1, 2).castAs<int>();        // 3
int r3 = func2(1, 2, 3).castAs<int>();     // 6

// Variadic functions
GAny varFunc = GAnyFunction::createVariadicFunction("testFunc", 
    [](const GAny **args, int32_t argc) -> GAny {
        // Process arguments
        return GAny("Success");
    });
```

### 7. JSON

JSON serialization and deserialization:

```cpp
// Parse JSON string
std::string json = R"({"a":1,"b":2.0,"c":"str"})";
GAny obj = GAny::parseJson(json);

// Convert object to JSON string
std::string jsonStr = obj.toJsonString();
std::string prettyJson = obj.toJsonString(2);  // With indentation
```

### 8. Serialization/Deserialization

Custom type serialization and deserialization:

```cpp
// Register type properties
Class<InfoC>("", "InfoC", "")
    .construct<>()
    .readWrite("a", &InfoC::a)
    .readWrite("b", &InfoC::b)
    .readWrite("c", &InfoC::c)
    .readWrite("d", &InfoC::d)
    .readWrite("e", &InfoC::e);

InfoC info = {
    .a = 123,
    .b = 3.14,
    .c = "hello",
    .d = {1, 2, 3, 4, 5}
};

// Serialize to JSON
std::string json = GAny(info).toJsonString(2);

// Deserialize from JSON
GAny obj = GAny::parseJson(json);
InfoC info2 = obj.castAs<InfoC>();
```

### 9. Arrays

Dynamic array operations:

```cpp
GAny array = {1, 2, 3, "4", 5.01};
array.pushBack("6");
array.pushBack(7);

// Access elements
GAny item = array[3];

// Remove elements
array.erase(3);

// Check containment
if (array.contains("4")) { /* ... */ }

// Iteration
for (int32_t i = 0; i < array.size(); i++) {
    auto item = array[i];
    std::cout << item.toString() << std::endl;
}

// Clone
GAny array2 = array.clone();
```

### 10. Objects

Dynamic object operations:

```cpp
GAny obj = GAny::object();
obj["a"] = 1;
obj["b"] = 2.0;
obj["c"] = "string";
obj["d"] = {1, 2, 3};

// Access properties
int a = obj["a"].toInt32();

// Set properties
obj.setItem("f", "value");

// Delete properties
obj.erase("b");
// Or use undefined to mark deletion
obj["i"] = GAny::undefined();

// Iterate over object
for (auto it = obj.iterator(); it.hasNext();) {
    auto item = it.next();
    std::string key = item.first.toString();
    GAny value = item.second;
}

// Use forEach
obj.call("forEach", [](const std::string &k, const GAny &v) {
    printf("%s: %s\n", k.c_str(), v.toString().c_str());
    return true;
});
```

### 11. Enums

Support for enum flags and regular enums:

```cpp
// Define enum flags
DEF_ENUM_FLAGS_8(TypeA, int8_t,
    None, 0,
    A, 0x01,
    B, 0x02,
    C, 0x04
);

// Register enum
REF_ENUM(TypeA, "", "Enum TypeA");

// Use enum
auto a = GAny::Import("TypeA").getItem("A");
auto e = GAny::Import("TypeA").getItem("E");
auto ae = a | e;  // Bitwise operation

if ((ae & e) == e) {
    // Check flags
}

// Regular enum
enum class TypeB : int8_t { A = 1, B, C, D, E };
DEF_ENUM_OPERATORS(TypeB, int8_t);

Class<TypeB>("Gx.Test", "TypeB", "Enum TypeB")
    .defEnum({{"A", TypeB::A}, {"B", TypeB::B}});
```

### 12. Operator Support

Support for common operators:

```cpp
// Arithmetic operations
GAny a = 10;
GAny b = 3;
GAny sum = a + b;      // 13
GAny diff = a - b;     // 7
GAny prod = a * b;     // 30
GAny quot = a / b;     // 3
GAny mod = a % b;      // 1

// Comparison operations
bool eq = (a == b);    // false
bool ne = (a != b);    // true
bool lt = (a < b);     // false
bool gt = (a > b);     // true
bool le = (a <= b);    // false
bool ge = (a >= b);    // true

// String concatenation
GAny str1 = "Hello ";
GAny str2 = "World";
GAny greeting = str1 + str2;  // "Hello World"
```

### 13. Exception Handling

```cpp
// Create exception
GAny ex = GAnyException("Error message");

// Check exception
if (result.isException()) {
    std::string msg = result.toString();
}

// Use try-catch
try {
    TestA obj = strVal.castAs<TestA>();
} catch (const GAnyException &e) {
    std::cout << "Error: " << e.what() << std::endl;
}

// Function returning exception
GAny func = GAnyFunction::createVariadicFunction("test",
    [](const GAny **args, int32_t argc) -> GAny {
        if (argc < 1) {
            return GAnyException("Missing argument");
        }
        return GAny("Success");
    });
```

### 14. Dynamic Classes

Create classes at runtime:

```cpp
// Create dynamic class
auto dTypeA = GAnyClass::Class("", "DTypeA", "Dynamic type A");
dTypeA->inherit(GAny::Import("TestA"))
      .staticFunc(MetaFunction::Init, [](GAny &self, int32_t a, double b) {
          self.setItem("__parent", GAny::New<TestA>(a, b));
      })
      .property("c",
          [](GAny &self) { return self.getItem("__p_c"); },
          [](GAny &self, const std::string &v) { self.setItem("__p_c", v); });

// Create instance
auto obj = dTypeA->_new(123, 3.1415);
obj.setItem("a", 456);
obj.call("print");
```

## Usage Examples

For complete examples, please refer to `examples/test_gany.cpp`.

## Initialization

```cpp
#define USE_GANY_CORE
#include "gx/gany.h"

int main() {
    // Initialize GAny Core
    initGAnyCore();
    
    // Your code...
    
    return 0;
}
```
