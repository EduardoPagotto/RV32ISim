#include "../include/MemoryAccess.hpp"

MemoryAccess::MemoryAccess(Controller* c, Bus* b, Execute* e, CSR* csr) : crt(c), bus(b), execute(e), csr(csr) {}

void MemoryAccess::step() {}