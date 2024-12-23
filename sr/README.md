
# Opencv静态链接以及项目构建
## 1. 下载并构建静态库
在源码文件夹中创建`build`文件夹和`install`文件夹。
在`build`文件夹中输入：
```
cmake -DBUILD_SHARED_LIBS=OFF -DBUILD_opencv_world=ON -DCMAKE_INSTALL_PREFIX="C:/Users/ThirtySevenTrees/lib/opencv-4.x/install" ..
# DBUILD_SHARED_LIBS=OFF用于设置生成静态库
```
构建好之后安装用于debug的库
```
cmake --build . --config Debug --target install
```
安装用于release的库
```
cmake --build . --config Release --target install
```
## 2. Cmake 文件编写
```
# 设置最低 CMake 版本
cmake_minimum_required(VERSION 3.10)

# 设置项目名称和 C++ 标准
project(MyProject)
set(CMAKE_CXX_STANDARD 17)

# 设置 OpenCV 的安装路径（替换为你的实际路径）
set(OpenCV_DIR "C:\Users\ThirtySevenTrees\lib\opencv-4.x\install")

# 包含 OpenCV 的头文件
include_directories(${OpenCV_DIR}/include)

# 链接 OpenCV 的静态库
link_directories(${OpenCV_DIR}/x64/vc17/lib)

# 添加本地 include 目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 添加源代码
# 第一个参数表示最后生成的可执行文件名字
add_executable(test src/test.cpp)

# 链接 OpenCV 的静态库
target_link_libraries(test opencv_world4100)

# 设置可执行文件的输出目录为构建目录
# ${CMAKE_BINARY_DIR}是CMake的内置变量，表示构建目录的根路径（通常是build/文件夹）
# bulid/文件夹视作最终程序运行的目录，写相对路径的时候相对于build/文件夹
set_target_properties(test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
```
## 3. 构建流程
在`build/`目录中运行：
```
cmake ..
```
完成之后编译项目：
```
cmake --build . --config Release
```

# Windows鼠标相关
在windows中可以使用`<windows.h>`来实现鼠标的相关操作  
在windows中鼠标位置用（x,y）来表示位置, 以主屏幕右上角为坐标原点  
在游戏中，鼠标被固定在屏幕中心并隐藏  
  
函数`BOOL __stdcall GetCursorPos(LPPOINT lpPoint)`用于获取鼠标位置  
```
typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *PPOINT, NEAR *NPPOINT, FAR *LPPOINT;
```
POINT是结构体tagPOINT的别名， PPOINT是POINT*， NEAR* 和FAR*分别表示近指针和远指针，在32位和64位系统中没有区别  
  
函数`BOOL __stdcall SetCursorPos(int X, int Y)`用于移动鼠标到（x,y）
  
函数`void __stdcall mouse_event(DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, ULONG_PTR dwExtraInfo)`用于控制鼠标事件  
```
void mouse_event(
    DWORD dwFlags, // 鼠标事件类型
    DWORD dx,      // x 坐标
    DWORD dy,      // y 坐标
    DWORD dwData,  // 附加信息，取决于事件类型
    ULONG_PTR dwExtraInfo // 自定义附加信息
);
```
1. dwFlags  
指定鼠标事件的类型，常见的标志包括：  
MOUSEEVENTF_LEFTDOWN：鼠标左键按下。  
MOUSEEVENTF_LEFTUP：鼠标左键释放。  
MOUSEEVENTF_RIGHTDOWN：鼠标右键按下。  
MOUSEEVENTF_RIGHTUP：鼠标右键释放。  
MOUSEEVENTF_MOVE：鼠标移动。  
MOUSEEVENTF_WHEEL：鼠标滚轮滚动。  
2. dx 和 dy  
用于指定鼠标事件的目标位置，单位是像素或相对于屏幕的比例。  
具体解释：  
如果 dwFlags 包含 MOUSEEVENTF_MOVE，则 dx 和 dy 表示鼠标的新位置。  
如果不包含 MOUSEEVENTF_MOVE，这些参数会被忽略，可以填 0。  
3. dwData  
此参数的作用取决于 dwFlags 的具体事件类型：  
MOUSEEVENTF_WHEEL：表示鼠标滚轮滚动的方向和步数。  
正值：向前滚动。  
负值：向后滚动。  
对于其他事件类型，这个参数通常被忽略，可以填 0。  
4. dwExtraInfo  
传递自定义的附加信息，通常可以用作事件标识或调试用途。  
如果不需要自定义信息，可以设置为 0。  

# Windows按键相关
函数`SHORT __stdcall GetAsyncKeyState(int vKey)`用于检测某个键的状态  
函数返回一个16位的short，通过最高位和最低位来表示状态  
1. 最高位0x8000(1000 0000 0000 0000)  
- 如果最高位是`1`，则表示按键当前正处于按下状态
- 如果最高位是`0`，则表示按键当前未按下  
2. 最低位0x0001(0000 0000 0000 0001)
- 如果此位为`1`，表示按键自上次调用以来曾经被按下过。
- 如果此位为`0`，表示按键自上次调用以来未被按下过。
例子：
```
while (true)
    {
        if (GetAsyncKeyState(VK_F1) & 0x8000) {
        mouse.move_mouse(400, 400);
        mouse.right_click();
        break;
        }
    }
```

# 图像匹配
 函数：`cv::matchTemplate`

`cv::matchTemplate` 是 OpenCV 中用于模板匹配的函数，用于在图像中搜索并定位模板图像的最佳匹配位置。

---

### **函数原型**
```cpp
void cv::matchTemplate(
    cv::InputArray image,
    cv::InputArray templ,
    cv::OutputArray result,
    int method,
    cv::InputArray mask = cv::noArray()
);
```
### **参数声明**

#### **1. `image`**
- 类型: `cv::InputArray`
- 说明: 输入的源图像。

#### **2. `templ`**
- 类型: `cv::InputArray`
- 说明: 模板图像。

#### **3. `result`**
- 类型: `cv::OutputArray`
- 说明: 输出的匹配结果矩阵。结果矩阵的每一个值表示模板与原图像对应区域的匹配分数

#### **4. `method`**
- 类型: `int`  
- 说明: 模板匹配方法。决定匹配分数计算方式，可选值：  
1. `cv::TM_SQDIFF`：平方差匹配法（越小越好）。  
2. `cv::TM_SQDIFF_NORMED`：归一化平方差匹配法（越小越好）。  
3. `cv::TM_CCORR`：相关性匹配法（越大越好）。  
4. `cv::TM_CCORR_NORMED`：归一化相关性匹配法（越大越好）。  
5. `cv::TM_CCOEFF`：相关系数匹配法（越大越好）。  
6. `cv::TM_CCOEFF_NORMED`：归一化相关系数匹配法（越大越好）。  


#### **5. `mask`**
- 类型: `cv::InputArray`（可选）
- 说明: 匹配时的掩码。  
  
如果模板图像不是对应原图像分辨率的图像，就可以使用多尺度模板匹配的方式  
使用函数`void cv::resize(cv::InputArray src, cv::OutputArray dst, cv::Size dsize, double fx = (0.0), double fy = (0.0), int interpolation = 1)`来改变模板大小进行匹配

# C++相关

## 函数返回值
函数返回值通常是以值的形式返回的临时对象，仅在当前语句结束前有效  
故函数的返回值主要用于复制或者初始化新的对象

## 引用
### 悬空引用（dangling reference）
如果一个引用所绑定的对象被销毁，引用会变成悬空引用，对悬空引用的访问会导致未定义行为  
故不能将引用值绑定于函数返回的对象

## std::vector
### RAII
std::vector是一个RAII类型的容器，它会在作用域结束时自动释放其所占用的内存

## push_back()
具体实现：
```
      _GLIBCXX20_CONSTEXPR
      void
      push_back(const value_type& __x)
      {
	if (this->_M_impl._M_finish != this->_M_impl._M_end_of_storage)
	  {
	    _GLIBCXX_ASAN_ANNOTATE_GROW(1);
	    _Alloc_traits::construct(this->_M_impl, this->_M_impl._M_finish,
				     __x);
	    ++this->_M_impl._M_finish;
	    _GLIBCXX_ASAN_ANNOTATE_GREW(1);
	  }
	else
	  _M_realloc_append(__x);
      }
```

```
// overload construct for non-standard pointer types
    template<typename _Ptr, typename... _Args>
      [[__gnu__::__always_inline__]]
      static _GLIBCXX14_CONSTEXPR
      std::__enable_if_t<__is_custom_pointer<_Ptr>::value>
      construct(_Alloc& __a, _Ptr __p, _Args&&... __args)
      noexcept(noexcept(_Base_type::construct(__a, std::__to_address(__p),
					      std::forward<_Args>(__args)...)))
      {
	_Base_type::construct(__a, std::__to_address(__p),
			      std::forward<_Args>(__args)...);
      }
```

# Windows编程相关

## 窗口相关

### HWND
HWND表示窗口的句柄。简单来说就是系统用来标识某个窗口或控件的一个引用（类似于一个ID或指针），通过他可以对该窗口进行操作获取信息等。
