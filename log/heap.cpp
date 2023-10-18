#include <iostream>
#include <vector>

using namespace std;

// 堆类，使用std::vector作为底层数据结构
class Heap {
private:
    vector<int> _arr; // 存储堆元素的动态数组
    int parent(int i) { return (i - 1) / 2; } // 返回父节点的索引
    int left(int i) { return 2 * i + 1; } // 返回左子节点的索引
    int right(int i) { return 2 * i + 2; } // 返回右子节点的索引

    // 上浮操作，用于维护堆的性质
    void heapify_up(int i) {
        // 循环直到到达根节点
        while (i > 0) {
            if (_arr[parent(i)] > _arr[i]) {
                // 如果父节点的值大于当前节点的值，就交换它们
                swap(_arr[parent(i)], _arr[i]);
                // 更新当前节点的索引为父节点的索引
                i = parent(i);
            } else {
                // 否则，说明堆的性质已经满足，退出循环
                break;
            }
        }
    }

    // 下沉操作，用于维护堆的性质
    void heapify_down(int i) {
        // 循环直到到达叶子节点
        while (i < _arr.size()) {
            int min_index = i; // 记录最小值的索引，初始为当前节点
            if (left(i) < _arr.size() && _arr[left(i)] < _arr[min_index]) {
                // 如果左子节点存在且小于当前节点，更新最小值的索引为左子节点的索引
                min_index = left(i);
            }
            if (right(i) < _arr.size() && _arr[right(i)] < _arr[min_index]) {
                // 如果右子节点存在且小于当前节点，更新最小值的索引为右子节点的索引
                min_index = right(i);
            }
            if (min_index != i) {
                // 如果最小值的索引不等于当前节点的索引，就交换它们
                swap(_arr[min_index], _arr[i]);
                // 更新当前节点的索引为最小值的索引
                i = min_index;
            } else {
                // 否则，说明堆的性质已经满足，退出循环
                break;
            }
        }
    }

public:
    // 构造函数，可以接受一个数组作为初始元素
    Heap(vector<int> arr = {}) {
        for (int x : arr) {
            push(x); // 调用push方法插入元素
        }
    }

    // 插入元素到堆中
    void push(int x) {
        _arr.push_back(x); // 在数组末尾添加元素
        heapify_up(_arr.size() - 1); // 对最后一个元素进行上浮操作
    }

    // 删除并返回堆中的最小元素（根节点）
    int pop() {
        if (_arr.empty()) {
            throw runtime_error("Heap is empty!"); // 如果堆为空，抛出异常
        }
        int min = _arr[0]; // 记录最小元素（根节点）
        swap(_arr[0], _arr.back()); // 交换根节点和最后一个元素
        _arr.pop_back(); // 删除最后一个元素（原来的根节点）
        heapify_down(0); // 对根节点进行下沉操作
        return min; // 返回最小元素
    }

    // 返回堆中的最小元素（根节点），但不删除它
    int top() {
        if (_arr.empty()) {
            throw runtime_error("Heap is empty!"); // 如果堆为空，抛出异常
        }
        return _arr[0]; // 返回根节点（最小元素）
    }

    // 返回堆中元素的个数
    int size() {
        return _arr.size();
    }

    // 判断堆是否为空
    bool empty() {
        return _arr.empty();
    }
};

// 测试代码
int main() {
    // 创建一个空的堆
    Heap heap;
    // 插入一些元素
    heap.push(3);
    heap.push(5);
    heap.push(1);
    heap.push(4);
    heap.push(2);
    // 输出堆中元素的个数和最小元素
    cout << "Heap size: " << heap.size() << endl;
    cout << "Heap top: " << heap.top() << endl;
    // 删除并输出堆中的所有元素，应该是升序排列
    while (!heap.empty()) {
        cout << heap.pop() << " ";
    }
    cout << endl;
    return 0;
}
