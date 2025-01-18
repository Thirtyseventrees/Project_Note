- [未完成跳过部分](#未完成跳过部分)
- [class 相关：](#class-相关)
  - [static关键字](#static关键字)
  - [析构函数](#析构函数)
- [左值和右值](#左值和右值)
- [C++11的可变模板参数（`typename... Args`）](#c11的可变模板参数typename-args)
- [::operator new和::new和malloc()](#operator-new和new和malloc)
- [new(`new T()`)和placement new(`new(ptr) T()`)](#newnew-t和placement-newnewptr-t)

# 未完成跳过部分
1. std::forward()完美转发实现，在`construct.h`和`allocator.h`中调用`std::forward()`
2. std::move(), 在`allocator.h`中调用`std::move()`

# class 相关：
## static关键字
static 关键字用于声明静态成员函数。静态成员函数的特点是不依赖于具体的类实例，可以直接通过类名调用。  

静态函数的限制：  
1. 没有 this 指针  
静态成员函数不能访问类的非静态成员变量或函数，因为它们没有 this 指针。

2. 不能被虚函数覆盖
静态成员函数无法声明为虚函数，因为它们不依赖于具体对象。

## 析构函数
析构函数(`~ClassName()`)是C++类中一个特殊成员函数，它在对象的生命周期结束时自动调用，用于释放资源，例如文件，锁，网络连接等。  
注意： 析构函数只负责清理对象的资源，不会负责释放对象本身的内存。  
`delete()`函数先调用类的析构函数，然后再调用`operator delete()`释放对象的内存。

# 左值和右值
左值（lvalue）和右值（rvalue）是变量和表达式的一种分类方式，主要与`内存位置` (是否可寻址)和`生命周期`（是否可持久化）有关  

1. 左值
- 定义： 左值是指有名字，可以被引用的对象，它通常表示一个可以取地址，存储在内存中的对象  
- 特点：左值表示可以持续存在的内存位置，可以在赋值表达式的左边，也可以在右边，可以通过`&`操作符获取它的地址  
eg.  
```
int x = 10;     // x 是左值
int* p = &x;    // 左值可以取地址
x = 20;         // 左值可以出现在赋值号左侧
int y = x + 1;  // 左值也可以出现在赋值号右侧
```
- 左值引用：用`&`声明，可以绑定到左值
  
2. 右值
- 定义：右值是指没有名字，只能被读的临时对象，它通常是表达式的结果，无法通过`&`获取地址
- 特点：右值是临时的，在语句结束后会被销毁，只能出现在赋值表达式的右侧，而不能出现在左侧，通常是字面值或表达式的计算结果  
eg.
```
int x = 10;       // 10 是右值
int y = x + 1;    // (x + 1) 是右值
int* p = &(x + 1); // 错误，右值不能取地址
```
- 右值引用：用`&&声明，可以绑定到右值，用于捕获临时对象，延长其生命周期

3. 左值右值的底层区别
- 左值始终表示可以储存在`内存`中的对象  
eg.
```
int x = 10;
int y = x + 1;
```
对应的汇编指令：
```
mov dword ptr [x], 10       ; 将 10 存储到变量 x 的地址
mov eax, dword ptr [x]      ; 将 x 的值加载到寄存器 eax 中
add eax, 1                  ; 将 eax 加 1
mov dword ptr [y], eax      ; 将结果存储到 y 的地址
```
- 右值没有固定的内存地址，通常只在`寄存器`或`栈`上存在  
eg.
```
int y = 10 + 20;
```
对应的汇编指令：
```
mov eax, 10                 ; 将 10 加载到寄存器 eax
add eax, 20                 ; eax = 10 + 20
mov dword ptr [y], eax      ; 将结果存储到变量 y 的地址
```
- 编译器对右值的优化：  
**寄存器优化**  
右值是表达式的结果，通常是短暂存在的临时值。编译器会尽量将右值存储在寄存器中，而不是内存中，从而减少内存访问，提高性能。  
**返回值优化（ROV）**  
返回值优化（Return Value Optimization, RVO）是一种编译器优化技术，用于在函数返回对象时，避免创建临时对象和多余的拷贝/移动操作  
当函数返回值是右值（临时对象）时，编译器会直接将返回值构造到目标对象的位置，而不是创建临时对象并复制到目标对象

# C++11的可变模板参数（`typename... Args`）

`typename... Args`定义了一个模板参数包，表示可以接受任意数量和类型的模板参数  
`Args&&... args`表示函数参数包，表示可以接受不定数量和类型的参数，并通过完美转发将这些参数传递给另一个参数  
完美转发是指将模板函数接收到的参数原封不动地转发给另一个函数，保留参数的值类别
1. 转发给其它函数
```
#include <iostream>
#include <string>
#include <utility>

// 接收任意数量和类型的参数
template <typename... Args>
void call_function(Args&&... args) {
    // 转发参数包到目标函数
    target_function(std::forward<Args>(args)...);
}

// 示例目标函数
void target_function(int a, double b, const std::string& c) {
    std::cout << "a: " << a << ", b: " << b << ", c: " << c << std::endl;
}

int main() {
    int x = 42;
    double y = 3.14;
    std::string z = "Hello";

    call_function(x, y, z);  // 转发参数
    return 0;
}
```
其中`std::forward<T>`是一个用于条件转发的工具，根据模板参数`T`的类型，判断


# ::operator new和::new和malloc()

首先`::`是作用域解析符，作用是确保调用的是全局作用域下的函数或运算符。在c++中new可以被全局定义，也可以被类重载，如果一个类重载了new，直接调用new就会调用这个类的版本，而如果加上`::`就表明我们要调用全局作用域下的new。  

- `::operator new`: 只分配内存而不调用构造函数。
- `::new`: 分配内存 + 调用构造函数。在底层实现中调用`::operator new`分配内存后再构造对象

```
#include <iostream>

class MyClass {
public:
    MyClass() { std::cout << "Constructor Called\n"; }
    ~MyClass() { std::cout << "Destructor Called\n"; }
};

int main() {
    // 使用 new 关键字（会调用构造函数）
    MyClass* obj1 = new MyClass();  // 输出 "Constructor Called"
    delete obj1;                    // 输出 "Destructor Called"

    // 使用 operator new（仅分配内存，不调用构造函数）
    void* mem = ::operator new(sizeof(MyClass));
    MyClass* obj2 = static_cast<MyClass*>(mem);
    
    // 手动调用构造函数
    new (obj2) MyClass();  // 输出 "Constructor Called"

    // 手动调用析构函数
    obj2->~MyClass();      // 输出 "Destructor Called"

    // 释放内存
    ::operator delete(mem);
    
    return 0;
}

```

注：通过::operator new生成的空间绑定对象后调用析构函数时不会释放内存，要额外调用`::operator delete()`来释放内存。  

| **特性**             | **`::operator new`**         | **`::new`（new 运算符）** | **`malloc()`** |
|----------------------|-----------------------------|---------------------------|----------------|
| **作用**            | **仅分配内存**，不调用构造函数 | **分配内存 + 调用构造函数** | **仅分配内存**，不调用构造函数 |
| **返回类型**        | `void*`，需要转换为对象指针 | `T*`，返回具体类型指针 | `void*`，需要转换为对象指针 |
| **是否调用构造函数** | ❌ 否 | ✅ 是 | ❌ 否 |
| **是否调用析构函数** | ❌ 否 | ✅ 是（`delete` 调用析构） | ❌ 否 |
| **失败时的行为**    | 抛出 `std::bad_alloc` | 抛出 `std::bad_alloc` | 返回 `nullptr` |
| **是否可以重载**    | ✅ 可以 | ❌ 不能重载 | ❌ 不能重载 |
| **适用语言**        | **C++**（运算符函数） | **C++** | **C/C++** |


1. `::new`
- `new T();`会调用：  
  1. `::operator new(sizeof(T));`分配内存。
  2. `T::T();`T的构造函数。
- `delete p;`会调用
  1. `p->~T();`T的析构函数
  2. `::operator delete(p);`释放内存。
2. `::operator new`只负责内存分配，类似于`malloc()`。  
   与`malloc()`的区别在于：`::operator new`会抛出异常，而`malloc()`返回`nullptr`。
3. `malloc()`只分配内存，不知道`T`的构造函数，不会初始化对象。

# new(`new T()`)和placement new(`new(ptr) T()`)
1. 普通的new(`new T()`)会分配内存并构造对象：
```
#include <iostream>

class MyClass {
public:
    MyClass(int x) { std::cout << "Constructor: " << x << "\n"; }
    ~MyClass() { std::cout << "Destructor\n"; }
};

int main() {
    MyClass* obj = new MyClass(42);  // ✅ 申请新内存并构造对象
    delete obj;                      // ✅ 调用析构并释放内存
    return 0;
}
```
2. 而placement new(`new(ptr) T()`)不会分配新内存，只会在`ptr`指向的内存上调用构造函数。
```
#include <iostream>

class MyClass {
public:
    MyClass(int x) { std::cout << "Constructor: " << x << "\n"; }
    ~MyClass() { std::cout << "Destructor\n"; }
};

int main() {
    void* mem = ::operator new(sizeof(MyClass));  // ✅ 仅分配内存（不会构造对象）
    MyClass* obj = new (mem) MyClass(42);  // ✅ 在 `mem` 指向的内存上构造对象

    obj->~MyClass();  // ✅ 显式调用析构函数
    ::operator delete(mem);  // ✅ 释放 `operator new` 申请的内存

    return 0;
}
```