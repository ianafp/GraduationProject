#pragma once 
#include <iostream>
#include <vector>
#include <mutex>
#include <stdexcept>

// 定义点的结构体
struct Point {
    double x, y;
    Point();
    Point(double _x,double _y);
    Point(const Point& _p);
};
// 定义向量
typedef Point Vector; 
// 定义三角形的结构体
struct Triangle {
    Point p1, p2, p3;
    Point operator [](const int index);
    Triangle() = default;
    Triangle(Point _p1,Point _p2,Point _p3);
};
// 点在三角形内测试函数的指针
typedef bool (*PointInTriangle)(Point p,Triangle t);
// 计算两个向量的叉积
double crossProduct(Point p1, Point p2, Point p3) ;
double crossProduct(Vector v1,Vector v2);

// 环形缓冲区
template<typename T>
class CircularBuffer {
public:
    explicit CircularBuffer(size_t size) : buffer_(size), size_(size), head_(0), tail_(0), full_(false) {}

    // 写入数据到缓冲区
    void write(const T& item) {
        std::lock_guard<std::mutex> lock(mutex_);

        buffer_[head_] = item;
        head_ = (head_ + 1) % size_;

        if (head_ == tail_) {
            full_ = true;
            tail_ = (tail_ + 1) % size_; // 覆盖最老的数据
        }
    }

    // 从缓冲区读取数据
    T read() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (empty()) {
            throw std::out_of_range("Buffer is empty");
        }

        T item = buffer_[tail_];
        tail_ = (tail_ + 1) % size_;
        full_ = false;

        return item;
    }

    // 检查缓冲区是否为空
    bool empty() const {
        return (!full_ && (head_ == tail_));
    }

    // 检查缓冲区是否已满
    bool full() const {
        return full_;
    }

    // 返回缓冲区中元素的数量
    size_t size() const {
        size_t size = size_;
        if (!full_) {
            if (head_ >= tail_) {
                size = head_ - tail_;
            } else {
                size = size_ + head_ - tail_;
            }
        }
        return size;
    }

private:
    std::vector<T> buffer_;
    size_t size_;
    size_t head_;
    size_t tail_;
    bool full_;
    mutable std::mutex mutex_;
};

