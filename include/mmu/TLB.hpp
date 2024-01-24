#pragma once
#include "defs.hpp"

class TLB {
  private:
    struct __TLB_data {
        __TLB_data() = default;
        bool valid{false}, changed{false}; // entrada no TLB valida
        uint8_t protection{0};             // URWX
        uint32_t page{0}, framePage{0}, references{0};
    };

    __TLB_data bufer[MAX_TLB];

    uint32_t tlb_hit{0}, tlb_miss{0}, pid{0};

  public:
    TLB() = default;

    bool set(const uint32_t& pid) {

        if (this->pid == pid)
            return false;

        this->flush();
        this->pid = pid;
        return true;
    }

    void flush() {
        for (uint32_t i{0}; i < MAX_TLB; i++) {
            bufer[i].valid = false;
            bufer[i].changed = false;
            bufer[i].protection = 0;
            bufer[i].page = 0;
            bufer[i].framePage = 0;
            bufer[i].references = 0;
        }
    }

    void save(const TablePageEntry& pte, const uint32_t& page) {

        uint32_t minimalRef{0xFFFFFFFF};
        uint32_t entryMinimal{0};

        for (uint32_t i{0}; i < MAX_TLB; i++) {
            if (!bufer[i].valid) {
                bufer[i].valid = true;
                bufer[i].changed = pte.changed;
                bufer[i].references = 0;
                bufer[i].protection = pte.protection;
                bufer[i].page = page;
                bufer[i].framePage = pte.framePage;
                return;
            } else {
                if (bufer[i].references < minimalRef) {
                    minimalRef = bufer[i].references;
                    entryMinimal = i;
                }
            }
        }

        bufer[entryMinimal].changed = pte.changed;
        bufer[entryMinimal].references = minimalRef + 1;
        bufer[entryMinimal].protection = pte.protection;
        bufer[entryMinimal].page = page;
        bufer[entryMinimal].framePage = pte.framePage;
    }

    /**
     * @brief Get physical address from TLB array
     * TLB hit -> encontrado no TLB
     * TLB miss -> nao esta na TLB
     *
     * @param vAddress Virtual Address
     * @param protection bits protection URWX
     * @return const std::tuple<int32_t, uint32_t> (MMU_OK, physic addr) | (MMU_TLB_MISS, 0) | (MMU_FORBIDEN_*, 0)
     */
    const std::tuple<int32_t, uint32_t> find(const uint32_t& vAddress, const uint8_t& protection) {

        // Confere o Translation Lookaside Buffer (TLB)
        for (uint8_t i{0}; i < MAX_TLB; i++) {

            if ((bufer[i].valid) && (MMU_GET_PAGE(vAddress) == bufer[i].page)) { // entrada existe e é valida
                int32_t v = checkPermission(bufer[i].protection, protection);    // verifica se acesso e permitido
                if (v == 0) {
                    // TLB hit (encontrou no TLB)
                    tlb_hit++;
                    bufer[i].references++;
                    return std::make_tuple(MMU_OK, MMU_GET_MEM_ADDRESS(bufer[i].framePage, vAddress));
                }

                return std::make_tuple(v, 0); // Forbiden
            }
        }
        tlb_miss++;
        return std::make_tuple(MMU_TLB_MISS, 0); // TLB miss
    }
};