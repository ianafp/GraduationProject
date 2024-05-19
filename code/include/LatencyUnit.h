#pragma once 
#include <iostream>
#include <vector>
#include <mutex>
#include <stdexcept>
class cClockCycle{
    public:
        cClockCycle();
        void ResetClock();
        int GetClock();
        void Tick();
    private:
        int clock;
};
template<typename T>
class LatencyUnitFifo{
public:
    // 构造函数，初始化缓冲区大小
    LatencyUnitFifo(size_t size) :size_(size),buffer_(size+1), head_(0), tail_(0), full_(false),ElementsCount(0),WaitCycles(0) {

    }
    // 向缓冲区中添加元素
    void push(const std::pair<int,T>& value) {
        if(full_){
            return;
        }
        ElementsCount++;
        buffer_[tail_] = value;
        tail_ = (tail_ + 1) % size_;
        if ((tail_+1)%size_ == head_) {
            full_ = true;
            // head_ = (head_ + 1) % buffer_.size();
        }
    }

    // 从缓冲区中取出元素
    T pop(int clock_cycle) {
        if (!full_ && head_ == tail_) {
            throw std::runtime_error("Buffer is empty");
        }
        std::pair<int,T> value = buffer_[head_];
        head_ = (head_ + 1) % size_;
        full_ = false;
        WaitCycles += clock_cycle - value.first; 
        return value.second;
    }

    // 检测缓冲区中的第一个元素是否到达了延迟时间
    bool CanPop(int currentCycle) const {
        if (!full_ && head_ == tail_) {
            return false;
        }
        return buffer_[head_].first <= currentCycle;
    }
    bool CanPush() const{
        return !full_;
    }
    bool Empty() const{
        return head_ == tail_;
    }
    int Size() const{
        if(tail_ == head_) return 0;
        if(tail_ > head_) return tail_ - head_;
        return tail_ + size_ - head_ - 1; 
    }
    int GetElementsCount() const{
        return ElementsCount;
    }
    int GetWaitCycles() const{
        return WaitCycles;
    }
private:
    int size_;
    std::vector<std::pair<int,T>> buffer_; // 缓冲区
    size_t head_; // 头指针
    size_t tail_; // 尾指针
    bool full_; // 缓冲区是否已满
    int ElementsCount;
    int WaitCycles;
};