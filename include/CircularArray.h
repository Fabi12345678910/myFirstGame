#pragma once

#include <cstddef>
#include <stdexcept>

template<typename T, std::size_t N>
class CircularArray {
private:
    T buffer[N];
    //the smallest invalid value
    //all values higher or equal than max are invalid
    std::size_t max;
    //the smallest but still valid value
    //all values smaller than min are invalid
    std::size_t min;
public:
    CircularArray() : max(0), min(0) {}
    size_t getMaxElements(){return N;}
    // Add element to the end
    void push(const T& value) {
        buffer[max % N] = value;
        if(max >= (ssize_t)N){min++;};
        max++;
    }

    T& back(){
        if(getSize() == 0){
            throw std::runtime_error("empty Array");
        }
        return buffer[getSize()-1];
    }

    const T& back(){
        if(getSize() == 0){
            throw std::runtime_error("empty Array");
        }
        return buffer[getSize()-1];
    }

    // Indexing with wrap-around
    T& operator[](std::size_t idx) {
        if (idx >= max || idx < min)
            throw std::out_of_range("index out of range");
        return buffer[idx % N];
    }

    const T& operator[](std::size_t idx) const {
        if (idx >= max || idx < min)
            throw std::out_of_range("index out of range");
        return buffer[idx % N];
    }

    size_t getSize(){
        return max;
    }

    size_t getMinIndex(){
        return min;
    }
};
