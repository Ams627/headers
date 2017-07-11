#pragma once

namespace ams
{

template <typename T, class Comparator> class Heap
{
    std::vector<T> v_;
public:
    T GetFront()
    {
        return v_.front();
    }

    inline void Pop()
    {
        std::pop_heap(v_.begin(), v_.end(), Comparator());
        v_.pop_back();
    }

    inline void Push(const T& t)
    {
        v_.push_back(t);
        std::push_heap(v_.begin(), v_.end(), Comparator());
    }

    inline bool IsEmpty()
    {
        return v_.empty();
    }
};


}