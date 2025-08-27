

# 进程地址空间

地址空间通常为连续的，大小取决于计算机架构，等于能够寻址的范围，（32位的大小为2^32, 4GB, 64的为2^64）

在32位操作系统中，地址空间被分为1GB的内核空间和3GB的用户空间，在3GB的用户空间中，进程会有一块内存区域（memory area），进程可以通过内核来增加和移除这个内存区域，进程只可以访问有效的内存区域，如果进程访问了其他地址则会出现"Segmentation Fault"(段错误)

内存区域包括了下列内存对象：

- A memory map of the executable file's code, called the text section (代码段: 程序刚开始执行的时候可执行文件(包括代码段，数据段，bss段都还在磁盘中)，当机器开始执行指令时触发缺页中断后才会将可执行文件加载到内存中)
- A memory map of the executable file's initialized global variables, called the data section (数据段)
- A memory map of the zero page (a page consisting of all zeros) containing uninitialized global variables, called the bss section.
- A additional text，data, and bss section for each shared library, such as the C library and dynamic linker, loaded into the process's address space
- Any memory mapped files
- Any shared memory segments
- Any anonymous memory mappings, such as those associated with malloc()

## 页

在操作系统中，内存管理的最小单位不是字节，是页  
一页就是一段固定大小的连续虚拟内存块，操作系统通过它把虚拟内存和物理内存联系起来

页通常大小为4KB



## mmap(), mumap(), mprotect()

进程的初始状态：

只有ELF文件里声明的内存和一些操作系统分配的内存
- 任何其他指针的访问都是非法的
- 如果我们从输入读取一个size，然后malloc，内存应该从哪来呢

所以一定有一个系统调用可以改变进程的地址空间 

在当前状态机状态上增加/删除/修改一段可访问的内存：  
映射：
- mmap()
- munmap()
修改映射权限
- mprotect()

mmap():  
mmap是一种内存映射技术，能把一个文件或者一段匿名内存直接映射到进程的虚拟地址空间中
```cpp
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
```
- addr: 映射的起始地址，通常传NULL让内核自己选择
- length: 映射的长度（字节数），会按页大小对齐
- prot: 内存保护方式，决定映射区的权限
  - PROT_READ：可读
  - PROT_WRITE：可写
  - PROT_EXEC：可执行
  - PROT_NONE：不可访问
- flag: 映射选项，控制共享/私有等
- fd: 被映射文件的文件描述符(不映射文件时为-1)
- offset: 从文件的什么位置开始映射，必须是页大小的整数倍

函数成功时返回映射区域的起始地址，失败时返回'MAP_FAILER'
