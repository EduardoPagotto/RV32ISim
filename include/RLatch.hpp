#pragma once
#include <cstdint>
#include <map>
#include <string>

template <class T>
class RLatch {
  public:
    RLatch() = default;
    RLatch(const RLatch& v) : val(v.val), nextVal(v.nextVal) {}
    RLatch(const T& value) : val(value), nextVal(value) {}
    virtual ~RLatch() = default;
    inline void commit() { this->val = this->nextVal; }
    inline void set(const T& value) { this->nextVal = value; }
    inline const T& get() const { return this->val; }
    inline T getLow() const { return this->val & (1 << (sizeof(T) * 2) * 2); }
    inline T getHigh() const { return this->val >> (sizeof(T) * 2) * 2; }
    inline T next() const { return this->nextVal; }
    inline T nextLow() const { return this->nextVal & (1 << (sizeof(T) * 2) * 2); }
    inline T nextHigh() const { return this->nextVal >> (sizeof(T) * 2) * 2; }

    inline void reset() {
        val = 0;
        nextVal = 0;
    }

  private:
    T val = 0;
    T nextVal = 0;
};
