#pragma once
#include "Memory2.hpp"
#include "TLB.hpp"
#include "defs.hpp"

struct VirtualPageLv0 {
    VirtualPageLv0() = default;
    TablePageEntry ptes[MAX_PAGES] = {0};
};

struct ProcessData {
    ProcessData() = default;
    VirtualPageLv0* virtualPageLv1[MAX_PAGES] = {nullptr};
};

class MMU {
  private:
    TLB tlb;
    ProcessData procs[MAX_PROC];
    bool memPages[MAX_PTE] = {false};

    uint32_t last_pid{0};
    uint32_t page_fault{0};
    uint32_t page_hit{0};
    uint32_t page_forbiden{0};

  public:
    MMU() = default;
    virtual ~MMU() = default;

    /**
     * @brief Create a TablePageEntry in Pages table
     *
     * @param vAddress Virtual Address
     * @param pid PID do processo
     * @param access bits protection URWX
     * @return std::tuple<int32_t, uint32_t> (MMU_OK, physic addr) | (MMU_FORBIDEN_*, 0) | (MMU_PAGE_FAULT, 0)
     */
    std::tuple<int32_t, uint32_t> createEntry(const uint32_t& vAddress, const uint32_t& pid, const uint32_t& access) {

        const uint32_t page{MMU_GET_PAGE(vAddress)};
        const uint32_t lv1{MMU_GET_LV1_INDEX(page)};
        const uint32_t lv0{MMU_GET_LV0_INDEX(page)};

        if (pid != last_pid) {
            tlb.flush();
        }

        ProcessData* proc = &procs[pid];
        VirtualPageLv0* ptrVirtualPageLv0 = proc->virtualPageLv1[lv1];
        if (ptrVirtualPageLv0 == nullptr) {
            ptrVirtualPageLv0 = new VirtualPageLv0;
            proc->virtualPageLv1[lv1] = ptrVirtualPageLv0;
        }

        TablePageEntry& tablePage = ptrVirtualPageLv0->ptes[lv0];

        if (!tablePage.valid) {

            tablePage.valid = true;
            tablePage.refed = true;
            tablePage.protection = access;

            for (uint32_t j{0}; j < MAX_PTE; j++) {
                if (memPages[j] == false) {

                    memPages[j] = true;
                    tablePage.framePage = j;

                    tlb.save(tablePage, page);

                    return std::make_tuple(MMU_OK, MMU_GET_MEM_ADDRESS(j, vAddress));
                }
            }

            // TODO: Aqui a memoria acaba e precisaremos fazer o swap!!!!
        }

        page_forbiden++;
        return std::make_tuple(MMU_FORBIDEN_ACCESS, 0);
    }

    const std::tuple<int32_t, uint32_t> getPhysicalAddress(const uint32_t& vAddress, const uint32_t& pid,
                                                           const uint8_t& access) { // access URWX

        // verifica o TLB
        auto result = tlb.find(vAddress, access);
        const int32_t error = std::get<0>(result);
        if (error == MMU_TLB_MISS) {

            // Procura nos pages disponiveis
            auto result2 = this->getFromTables(vAddress, pid, access);
            if (std::get<0>(result2) == MMU_PAGE_FAULT) {

                // cria nova entrada
                auto result3 = this->createEntry(vAddress, pid, access);
                return result3;
            }

            return result2;
        } else if (error < 0) {
            page_forbiden++;
        }

        // TLB_hit ou Forbiden
        return result;
    }

  private:
    // Page fault -> frame memory sem mapeamento, verificar swap
    // Page hit -> frame memory encontrado
    // Access fail -> tentativa de acesso nao autorizado ou nao existente

    /**
     * @brief Get physical address from Tables
     *
     * @param vAddress Virtual Address
     * @param pid PID do processo
     * @param access bits protection URWX
     * @return const std::tuple<int32_t, uint32_t>  (MMU_OK, physic addr) | MMU_PAGE_FAULT,0 |(MMU_FORBIDEN_*, 0)
     */
    const std::tuple<int32_t, uint32_t> getFromTables(const uint32_t& vAddress, const uint32_t& pid,
                                                      const uint8_t& access) { // access URWX
        const uint32_t page{MMU_GET_PAGE(vAddress)};
        const uint32_t lv1{MMU_GET_LV1_INDEX(page)};
        const uint32_t lv0{MMU_GET_LV0_INDEX(page)};

        ProcessData* proc = &procs[pid];
        VirtualPageLv0* ptrVirtualPageLv0 = proc->virtualPageLv1[lv1];
        if (ptrVirtualPageLv0 == nullptr) {
            ptrVirtualPageLv0 = new VirtualPageLv0;
            proc->virtualPageLv1[lv1] = ptrVirtualPageLv0;
        }

        TablePageEntry& tablePage = ptrVirtualPageLv0->ptes[lv0];
        if (!tablePage.valid) {
            page_fault++;
            return std::make_tuple(MMU_PAGE_FAULT, 0);
        }

        uint8_t v = checkPermission(tablePage.protection, access);
        if (v == 0) {
            tablePage.refed = true;
            tlb.save(tablePage, MMU_GET_PAGE(vAddress));

            // physical memory address
            page_hit++;
            return std::make_tuple(MMU_OK, MMU_GET_MEM_ADDRESS(tablePage.framePage, vAddress));
        } else {
            page_forbiden++;
        }

        return std::make_tuple(v, 0); // Forbiden Access
    }
};