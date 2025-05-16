- [细节](#细节)
  - [如果函数返回一个对象的指针记得要在里面返回new对象的指针](#如果函数返回一个对象的指针记得要在里面返回new对象的指针)
- [分治归并](#分治归并)
  - [分治](#分治)
    - [递归写法](#递归写法)
    - [循环写法](#循环写法)
  - [归并](#归并)
- [回溯](#回溯)
- [快排](#快排)

# 细节

## 如果函数返回一个对象的指针记得要在里面返回new对象的指针

```
ListNode* mergeTwoLists(ListNode* node1, ListNode* node2){
    ListNode* result = new ListNode();
    ...
}
```

# 分治归并

## 分治

### 递归写法
```cpp
#include <vector>
using namespace std;

void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;
    
    while (i <= mid && j <= right) {
        temp[k++] = arr[i] <= arr[j] ? arr[i++] : arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    
    for (int t = 0; t < temp.size(); ++t) {
        arr[left + t] = temp[t];
    }
}

void mergeSortRecursive(vector<int>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSortRecursive(arr, left, mid);
    mergeSortRecursive(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

```

### 循环写法
```cpp
#include <vector>
using namespace std;

void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;
    
    while (i <= mid && j <= right) {
        temp[k++] = arr[i] <= arr[j] ? arr[i++] : arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    
    for (int t = 0; t < temp.size(); ++t) {
        arr[left + t] = temp[t];
    }
}

void mergeSortIterative(vector<int>& arr) {
    int n = arr.size();
    for (int width = 1; width < n; width *= 2) {
        for (int i = 0; i < n; i += 2 * width) {
            int left = i;
            int mid = min(i + width - 1, n - 1);
            int right = min(i + 2 * width - 1, n - 1);
            if (mid < right) {
                merge(arr, left, mid, right);
            }
        }
    }
}

```


## 归并

```cpp
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
ListNode* mergeTwoLists(ListNode* node1, ListNode* node2){
        ListNode* result = new ListNode();
        ListNode* return_node = result;
        while(node1 != nullptr && node2 != nullptr){
             if(node1->val < node2->val){
                result->next = node1;
                node1 = node1->next;
            }
            else{
                result->next = node2;
                node2 = node2->next;
            }
            result = result->next;
        }
        while(node1 != nullptr){
            result->next = node1;
            node1 = node1->next;
            result = result->next;
        }
        while(node2 != nullptr){
            result->next = node2;
            node2 = node2->next;
            result = result->next;
        }
        return return_node->next;
    }
```

# 回溯
回溯的核心思想就是在递归之后将状态返回到递归前的状态并记录已经完成了这种递归情况  
标准模板：  
```cpp
// 回溯模板

void backtracking(参数) {
    if (终止条件) {
        存放结果;
        return;
    }
    for (选择 : 本层集合中的元素) {
        处理节点;
        backtracking(路径, 选择列表); // 递归
        撤销处理; // 回溯
    }
}
```
例如
```cpp
class Solution {
public:
    vector<vector<int>> permute(vector<int>& nums) {
        seq = nums;
        size = nums.size();
        vector<int> used(size, 0);
        vector<int> path;
        backtrack(path, used);
        return result;
    }

    void backtrack(vector<int>& path, vector<int>& used){
        if(path.size() == size){
            result.push_back(path);
            return;
        }
        for(int i = 0; i < size; ++i){
            if(used[i] == 1){
                continue;
            }
            path.push_back(seq[i]);
            used[i] = 1;
            backtrack(path, used);
            path.pop_back();
            used[i] = 0;
        }
    }

private:
    int size;
    vector<vector<int>> result;
    vector<int> seq;
};
```
这个解法中每次调用递归函数之前用push_back来改变状态，结束之后用pop_back来还原状态，并且使用used[]来记录递归的情况

# 快排
先切分，再合并

选取一个pivot，比pivot小的放左边，大的放右边，再对左右两边重复操作。

链表快排：
```cpp
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
class Solution {
public:
    ListNode* sortList(ListNode* head) {
        return qsort(head);
    }

    //将链表切分为三个链表，一个是全部比pivot小的，一个是等于pivot的，一个是全部大于pivot的
    //其实less_end和large_end可以不需要
    void partition(ListNode* head, ListNode*& less_begin, ListNode*& less_end, ListNode*& equal_begin, ListNode*& equal_end, ListNode*& large_begin, ListNode*& large_end, int pivot){
        while(head){
            ListNode* next = head->next;
            if(head->val < pivot){
                if(less_end == nullptr){
                    less_begin = head;
                    less_end = head;
                    less_end->next = nullptr;
                }
                else{
                    less_end->next = head;
                    less_end = less_end->next;
                    less_end->next = nullptr;
                }
            }
            else if(head->val == pivot){
                if(equal_end == nullptr){
                    equal_begin = head;
                    equal_end = head;
                    equal_end->next = nullptr;
                }
                else{
                    equal_end->next = head;
                    equal_end = equal_end->next;
                    equal_end->next = nullptr;
                }
            }
            else{
                if(large_end == nullptr){
                    large_begin = head;
                    large_end = head;
                    large_end->next = nullptr;
                }
                else{
                    large_end->next = head;
                    large_end = large_end->next;
                    large_end->next = nullptr;
                }
            }
            head = next;
        }
    }

    ListNode* qsort(ListNode* head){
        if(!head || !head->next)
            return head;
        
        ListNode* pivot_node = head;
        ListNode* fast_node = head;
        int count = 1;
        while(fast_node){
            fast_node = fast_node->next;
            if(count % 2 == 0)
                pivot_node = pivot_node->next;
            ++count;
        }
        int pivot = pivot_node->val;
        ListNode *less_begin = nullptr, *less_end = nullptr, *equal_begin = nullptr, *equal_end = nullptr, *large_begin = nullptr, *large_end = nullptr;

        //将链表切分为三块
        partition(head, less_begin, less_end, equal_begin, equal_end, large_begin, large_end, pivot);

        //对左右两块再次排序
        ListNode* less = qsort(less_begin);
        ListNode* large = qsort(large_begin);
        
        //把三段列表拼接起来
        ListNode* less_tail = less;
        while(less_tail && less_tail->next){
            less_tail = less_tail->next;
        }

        if(less_tail)
            less_tail->next = equal_begin;
        equal_end->next = large;

        if(less)
            return less;
        return equal_begin;
    }
};
```