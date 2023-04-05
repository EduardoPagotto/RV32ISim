#pragma once
#include <cstdint>
#include <map>
#include <string>

class Register32 {
  public:
    Register32() = default;
    Register32(const Register32& v) : val(v.val), nextVal(v.nextVal) {}
    Register32(const int32_t& value) : val(value), nextVal(value) {}
    virtual ~Register32() = default;
    inline int32_t getValue() const { return this->val; }
    inline int32_t getValueLow() const { return this->val & 0xffff; }
    inline int32_t getValueHi() const { return this->val >> 16; }
    inline int32_t getNextValue() const { return this->nextVal; }
    inline int32_t getNextValueLow() const { return this->nextVal & 0xffff; }
    inline int32_t getNextValueHi() const { return this->nextVal >> 16; }
    inline void setValue(const int32_t& value) { this->nextVal = value; }
    inline void latchNext() { this->val = this->nextVal; }
    inline void reset() {
        this->val = 0;
        this->nextVal = 0;
    }

  private:
    int32_t val = 0;
    int32_t nextVal = 0;
};

class Register64 {
  public:
    Register64() = default;
    Register64(const Register64& v) : val(v.val), nextVal(v.nextVal) {}
    Register64(const int64_t& value) : val(value), nextVal(value) {}
    virtual ~Register64() = default;
    inline int64_t getValue() const { return this->val; }
    inline int64_t getValueLow() const { return this->val & 0xffff; }
    inline int64_t getValueHi() const { return this->val >> 16; }
    inline int64_t getNextValue() const { return this->nextVal; }
    inline int64_t getNextValueLow() const { return this->nextVal & 0xffff; }
    inline int64_t getNextValueHi() const { return this->nextVal >> 16; }
    inline void setValue(const int64_t& value) { this->nextVal = value; }
    inline void latchNext() { this->val = this->nextVal; }

    inline void reset() {
        this->val = 0;
        this->nextVal = 0;
    }

  private:
    int64_t val = 0;
    int64_t nextVal = 0;
};
