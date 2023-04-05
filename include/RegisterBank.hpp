#pragma once
#include "Register.hpp"

class RegisterBank {
  public:
    RegisterBank() {}

    virtual ~RegisterBank() {
        for (auto r : registers) {
            delete r.second;
            r.second = nullptr;
        }

        registers.clear();
    };

    Register32* add(const std::string& name, const int32_t initVal = 0) {
        Register32* r = new Register32(initVal);
        registers[name] = r;
        return r;
    }

    Register32* getValue(const std::string& name) { return this->registers[name]; }

    void latchNext() {
        for (auto r : registers) {
            r.second->latchNext();
        }
    }

    void reset() {
        for (auto r : registers) {
            r.second->reset();
        }
    }

  private:
    std::map<std::string, Register32*> registers;
};