#pragma once
#include "defs.hpp"

class TLB {
  private:
    struct __TLB_data {
        __TLB_data() = default;

        bool valid{false}, changed{false}; // entrada no TLB valida
        uint8_t protection{0};             // URWX
        uint32_t page{0}, framePage{0};
    };

    __TLB_data bufer[MAX_TLB];

    uint32_t tlb_hit{0}, tlb_miss{0};

  public:
    TLB() = default;

    void flush() {
        for (uint32_t i{0}; i < MAX_TLB; i++) {
            bufer[i].valid = false;
            bufer[i].changed = false;
            bufer[i].protection = 0;
            bufer[i].page = 0;
            bufer[i].framePage = 0;
        }
    }

    void save(const TablePageEntry& pte, const uint32_t& page) {
        for (uint32_t i{0}; i < MAX_TLB; i++) {
            if (!bufer[i].valid) {
                bufer[i].valid = true;
                bufer[i].changed = pte.changed;
                // bufer[i].refed = pte.refed;
                bufer[i].protection = pte.protection;
                bufer[i].page = page;
                bufer[i].framePage = pte.framePage;
            }
        }

        // sem espaco, procurar mais antiga // FIXME: ver algoritimo de ref sem accesso
        // for (uint8_t i; i < MAX_TLB; i++) {
        //     if (!tlb[i].refed) {
        //         tlb[i].valid = true;
        //         tlb[i].changed = pte.changed;
        //         tlb[i].protection = pte.protection;
        //         tlb[i].page = page;
        //         tlb[i].framePage = pte.framePage;
        //         return;
        //     }
        // }
    }

    /**
     * @brief Get physical address from TLB array
     * TLB hit -> encontrado no TLB
     * TLB miss -> nao esta na TLB
     *
     * @param vAddress Virtual Address
     * @param access bits protection URWX
     * @return const std::tuple<int32_t, uint32_t> (MMU_OK, physic addr) | (MMU_TLB_MISS, 0) | (MMU_FORBIDEN_*, 0)
     */
    const std::tuple<int32_t, uint32_t> find(const uint32_t& vAddress, const uint8_t& access) {

        // Confere o Translation Lookaside Buffer (TLB)
        for (uint8_t i{0}; i < MAX_TLB; i++) {

            if ((bufer[i].valid) && (MMU_GET_PAGE(vAddress) == bufer[i].page)) { // entrada existe e é valida
                int32_t v = checkPermission(bufer[i].protection, access);        // verifica se acesso e permitido
                if (v == 0) {
                    // TLB hit (encontrou no TLB)
                    tlb_hit++;
                    return std::make_tuple(MMU_OK, MMU_GET_MEM_ADDRESS(bufer[i].framePage, vAddress));
                }

                return std::make_tuple(v, 0); // Forbiden
            }
        }
        tlb_miss++;
        return std::make_tuple(MMU_TLB_MISS, 0); // TLB miss
    }
};