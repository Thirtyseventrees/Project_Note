- [1. STL概论](#1-stl概论)
  - [1. 容器（containers）](#1-容器containers)
  - [2. 算法（algorithms）](#2-算法algorithms)
  - [3. 迭代器（iterators）](#3-迭代器iterators)
  - [4. 仿函数（functors）](#4-仿函数functors)
  - [5. 配接器（adapters）](#5-配接器adapters)
  - [6. 配置器（allocators）](#6-配置器allocators)
- [2. 配置器 (allocator)](#2-配置器-allocator)
  - [2.1 配置器的标准接口](#21-配置器的标准接口)
  - [2.2 实现](#22-实现)
- [迭代器（iterator）](#迭代器iterator)
- [容器](#容器)
  - [vector](#vector)
 

# 1. STL概论

STL提供了六大组件，彼此可以组合套用

## 1. 容器（containers）
包含了各类数据结构。如vector, list, deque, set, map等  

## 2. 算法（algorithms）
常用算法  

## 3. 迭代器（iterators）
用来访问容器中元素的一种对象，就像一个指针，可以遍历容器中的每一个元素，而不需要知道底层容器的具体实现方式。例如：  
```
std::vector<int> vec = {1, 2, 3, 4, 5};
std::vector<int>::iterator it = vec.begin();
```
  
根据功能和容器类型，STL中迭代器分为以下几种：

1. 输入迭代器（Input Iterator）
- 只支持只读访问。
- 只能单向遍历（从前往后）。
- 常用于读取输入流等数据结构。
- 操作：*iter（只读）、++iter。

2. 输出迭代器（Output Iterator）
- 只支持写入操作。
- 只能单向遍历。
- 操作：*iter = value、++iter。

3. 前向迭代器（Forward Iterator）
- 支持单向遍历。
- 可以读写。
- 常用于链表（如 std::forward_list）。
- 操作：*iter（读写）、++iter。

4. 双向迭代器（Bidirectional Iterator）
- 支持双向遍历（前进和后退）。
- 常用于双向链表、std::map、std::set。
- 操作：*iter（读写）、++iter、--iter。

5. 随机访问迭代器（Random Access Iterator）
- 支持随机访问容器元素（类似于数组下标访问）。
- 常用于动态数组（如 std::vector, std::deque）。
- 操作：*iter（读写）、++iter、--iter、iter + n、iter - n、iter[n]。

## 4. 仿函数（functors）
仿函数（Functor）是一个可以像函数一样使用的对象。在C++中，它是通过重载operator()运算符实现的。STL中大量使用仿函数来增强算法的灵活性和可扩展性。  

简单来说，仿函数是一个行为类似于函数的类或结构体实例。它既可以像普通函数一样调用，又可以携带状态（成员变量）或实现复杂逻辑。  

例如：  
简单仿函数
通过重载 operator()，定义一个仿函数类：
```
#include <iostream>
#include <vector>
#include <algorithm>

// 定义一个仿函数类
struct Add {
    int operator()(int a, int b) const {
        return a + b;
    }
};

int main() {
    Add add;                  // 创建仿函数对象
    std::cout << add(3, 5);   // 调用仿函数，输出：8
    return 0;
}
```

- STL中的仿函数：
```
std::plus<T>        // 加法
std::minus<T>       // 减法
std::multiplies<T>  // 乘法
std::divides<T>     // 除法
std::modulus<T>     // 取模
std::negate<T>      // 取反

std::equal_to<T>    // 等于
std::not_equal_to<T>// 不等于
std::greater<T>     // 大于
std::less<T>        // 小于
std::greater_equal<T>// 大于等于
std::less_equal<T>  // 小于等于

std::logical_and<T>  // 逻辑与
std::logical_or<T>   // 逻辑或
std::logical_not<T>  // 逻辑非
```

## 5. 配接器（adapters）
配接器（Adapter） 是C++标准模板库（STL）中的一种设计模式，用于改变接口或功能的方式，使某些组件能够在不同场景中更灵活地使用。它们可以作用于容器、迭代器、仿函数，提供了一种包装机制，将原有的组件改造成新的形式。  
配接器的类型
STL中主要有以下三种配接器：

1. 容器配接器（Container Adapter）  
容器配接器是基于已有容器（如 deque、list）实现的包装器，它们提供特定的数据结构行为，屏蔽底层容器的复杂实现。  
例如：  
```
std::stack<int> s;  // 默认使用 deque 作为底层容器
```

2. 迭代器配接器（Iterator Adapter）  
迭代器配接器用于扩展或修改已有迭代器的行为，提供更灵活的访问模式。  

3. 函数配接器（Function Adapter）  
函数配接器用于对仿函数或普通函数进行包装，改变其行为或接口，使其适配不同的算法。  

## 6. 配置器（allocators）
用于分配和释放内存，负责容器的动态内存分配


# 2. 配置器 (allocator)

## 2.1 配置器的标准接口

```
template <class T>
class allocator {...}
```
allocator的必要接口：
1. 类型定义：
```
using value_type = T;                     // 分配器分配的元素类型
using pointer = T*;                       // 元素指针类型
using const_pointer = const T*;           // 常量元素指针类型
using reference = T&;                     // 引用类型
using const_reference = const T&;         // 常量引用
using size_type = std::size_t;            // 表示内存大小的类型
using difference_type = std::ptrdiff_t;   // 指针差值类型
```
2. 构造和析构函数：
```
Allocator() noexcept;                         // 默认构造函数

Allocator(const Allocator&) noexcept;         // 拷贝构造函数

template<typename U>
Allocator(const Allocator<U>&) noexcept;      // 从其他类型的分配器构造

~Allocator();                                 // 析构函数
```

3. 分配和释放内存：
```
pointer allocate(size_type n);

void deallocate(pointer p, size_type n);
```

4. 对象的构造和销毁
```
template<typename U, typename... Args>
void construct(U* p, Args&&... args);

template<typename U>
void destroy(U* p);
```
这里再次单独使用`typename U`而不直接使用T是因为construct可能处理的不仅仅是`T`类型的对象  
例如`std::map`容器内部可能需要为与模板类型`T`不同的类型构造对象  
在`std::map<key, value>`中，分配器需要为`std::pair<const key, value>`类型的节点构造对象。

## 2.2 实现
1. 内存分配和内存分配并初始化对象分开实现  
- STL容器的特性需要先分配内存，稍后再构造对象  
  例如`std::vector`可能会一次性分配更大的内存块但不会立刻初始化所有对象，仅当元素被`push_back()`或`emplace_back()`插入时才初始化。


# 迭代器（iterator）
C++ STL 中的迭代器（Iterator）是用于在容器（如 vector、list、map 等）之间遍历和操作数据的对象。
迭代器本质上是一种抽象化的指针，它提供了一种统一的方式来访问 STL 容器中的元素，而不关心底层数据结构的具体实现。  
迭代器类型：  
| **迭代器类型**             | **支持的操作**                                      | **适用容器**                 |
|---------------------------|-------------------------------------------------|----------------------------|
| **输入迭代器（Input Iterator）** | 只支持**单向读取**，不支持修改                     | `istream_iterator`         |
| **输出迭代器（Output Iterator）** | 只支持**单向写入**，不支持读取                     | `ostream_iterator`         |
| **前向迭代器（Forward Iterator）** | 支持**单向遍历**，可以读写                        | `forward_list`             |
| **双向迭代器（Bidirectional Iterator）** | 支持**双向遍历**，可以读写                        | `list`、`map`、`set`       |
| **随机访问迭代器（Random Access Iterator）** | 支持**随机访问**，可以读写                        | `vector`、`deque`、`array` |


# 容器
## vector
容器的尾迭代器是指向最后一个元素的下一个位置。