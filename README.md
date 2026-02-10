# GAny
C++动态类型和动态反射库

[English](README_EN.md) | 简体中文

## 功能介绍

### 1. 动态类型

GAny 支持多种动态类型，可以在运行时改变变量的类型：

```cpp
GAny obj = 1;              // int 类型
obj = true;                // boolean 类型
obj = 3.14f;               // float 类型
obj = 1e10;                // double 类型
obj = "hello";             // string 类型
obj = {1, 2.2, "4"};       // array 类型
obj = {{"a", 1}, {"b", 2}}; // object 类型
obj = [](){};              // function 类型
obj = UserType();          // 自定义类型
obj = GAny::undefined();   // undefined 类型
obj = GAny::null();        // null 类型
```

### 2. 动态反射

通过 `Class` 模板注册 C++ 类型，实现动态反射功能：

```cpp
// 注册类及其构造函数
Class<TestA>("", "TestA", "class TestA")
    .construct<>()
    .construct<int, double>()
    .constant("N", 10)
    .property("a", &TestA::getA, &TestA::setA)
    .property("b", &TestA::getB, &TestA::setB)
    .func("print", &TestA::print);

// 动态创建对象
GAny ClassTestA = GAny::Import("TestA");
GAny ta = ClassTestA(1, 123.123);

// 动态访问属性和方法
ta.setItem("a", 3);
int a = ta.getItem("a").castAs<int>();
ta.call("print");
```

支持类继承：

```cpp
Class<TestB>("", "TestB", "class TestB")
    .construct<>()
    .construct<int, double, std::string>()
    .inherit<TestA>()  // 继承 TestA
    .property("c", &TestB::getC, &TestB::setC);
```

### 3. 运行时类型检查

提供丰富的类型检查函数：

```cpp
GAny intVal = 42;
GAny strVal = "hello";
GAny arrVal = {1, 2, 3};

if (intVal.isInt()) { /* ... */ }
if (intVal.isNumber()) { /* ... */ }  // int, float, double 都返回 true
if (strVal.isString()) { /* ... */ }
if (arrVal.isArray()) { /* ... */ }

// 其他类型检查
obj.isFloat();
obj.isDouble();
obj.isBool();
obj.isObject();
obj.isFunction();
obj.isNull();
obj.isUndefined();
obj.isException();
```

### 4. 运行时类型转换

支持多种类型之间的转换：

```cpp
GAny intVal = 42;
int32_t i = intVal.toInt32();
int64_t l = intVal.toInt64();
float f = intVal.toFloat();
double d = intVal.toDouble();
bool b = intVal.toBool();
std::string s = intVal.toString();

// 使用 castAs 进行强类型转换
int value = numVal.castAs<int>();
TestA* ptr = ta.castAs<TestA*>();

// 字符串转数字
GAny strNum = "123";
int num = strNum.toInt32();  // 123
```

### 5. 运行时类型信息

获取运行时类型信息：

```cpp
GAny obj = 42;

// 获取类型枚举
AnyType type = obj.type();

// 获取类型名称
std::string typeName = obj.typeName();

// 判断是否为特定 C++ 类型
if (obj.is<TestA>()) {
    TestA* ptr = obj.as<TestA>();
}

// 获取长度（适用于 array、object、string 等）
size_t len = obj.length();
```

### 6. 函数、函数调用

GAny 支持将函数作为一等公民：

```cpp
// Lambda 函数
GAny func = [](int a) { return a + 1; };
GAny result = func(1);  // 调用函数
int value = result.toInt32();  // 2

// 函数重载
GAny func2 = [](int a) { return a; };
func2.overload([](int a, int b) { return a + b; });
func2.overload([](int a, int b, int c) { return a + b + c; });

int r1 = func2(1).castAs<int>();           // 1
int r2 = func2(1, 2).castAs<int>();        // 3
int r3 = func2(1, 2, 3).castAs<int>();     // 6

// 可变参数函数
GAny varFunc = GAnyFunction::createVariadicFunction("testFunc", 
    [](const GAny **args, int32_t argc) -> GAny {
        // 处理参数
        return GAny("Success");
    });
```

### 7. JSON

JSON 序列化和反序列化：

```cpp
// JSON 字符串解析
std::string json = R"({"a":1,"b":2.0,"c":"str"})";
GAny obj = GAny::parseJson(json);

// 对象转 JSON 字符串
std::string jsonStr = obj.toJsonString();
std::string prettyJson = obj.toJsonString(2);  // 带缩进
```

### 8. 序列化/反序列化

自定义类型的序列化和反序列化：

```cpp
// 注册类型的属性
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

// 序列化为 JSON
std::string json = GAny(info).toJsonString(2);

// 从 JSON 反序列化
GAny obj = GAny::parseJson(json);
InfoC info2 = obj.castAs<InfoC>();
```

### 9. 数组（Array）

动态数组操作：

```cpp
GAny array = {1, 2, 3, "4", 5.01};
array.pushBack("6");
array.pushBack(7);

// 访问元素
GAny item = array[3];

// 删除元素
array.erase(3);

// 检查包含
if (array.contains("4")) { /* ... */ }

// 遍历
for (int32_t i = 0; i < array.size(); i++) {
    auto item = array[i];
    std::cout << item.toString() << std::endl;
}

// 克隆
GAny array2 = array.clone();
```

### 10. 对象（Object）

动态对象操作：

```cpp
GAny obj = GAny::object();
obj["a"] = 1;
obj["b"] = 2.0;
obj["c"] = "string";
obj["d"] = {1, 2, 3};

// 访问元素
int a = obj["a"].toInt32();

// 设置元素
obj.setItem("f", "value");

// 删除元素
obj.erase("b");
// 或使用 undefined 标记删除
obj["i"] = GAny::undefined();

// 遍历对象
for (auto it = obj.iterator(); it.hasNext();) {
    auto item = it.next();
    std::string key = item.first.toString();
    GAny value = item.second;
}

// 使用 forEach
obj.call("forEach", [](const std::string &k, const GAny &v) {
    printf("%s: %s\n", k.c_str(), v.toString().c_str());
    return true;
});
```

## 使用示例

完整示例请参考 `examples/test_gany.cpp`。

## 性能警告

GAny 是一个动态类型系统，虽然提供了极大的灵活性，但在进行数值计算时，其性能由于动态类型检查和分发机制，会显著低于原生 C++ 类型。

> [!WARNING]
> **请勿在性能敏感的循环或繁重计算中使用 GAny 进行算术运算！**

如果需要进行大量计算，请先将 GAny 转换为原生类型（如 `int`, `double` 等），计算完成后再转换回 GAny。

**错误示例（低效）：**
```cpp
GAny sum = 0;
for (int i = 0; i < 1000000; i++) {
    sum = sum + i;  // 每次加法都涉及动态类型检查和内存分配
}
```

**正确示例（高效）：**
```cpp
int64_t nativeSum = 0;
for (int i = 0; i < 1000000; i++) {
    nativeSum += i; // 原生 C++ 运算，性能极高
}
GAny sum = nativeSum; // 仅在最后进行一次转换
```

## 主程序初始化

```cpp
#define USE_GANY_CORE
#include "gx/gany.h"

int main() {
    // 初始化 GAny Core
    initGAnyCore();
    
    // 你的代码...
    
    return 0;
}
```
