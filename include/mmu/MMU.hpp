#pragma once
#include "TLB.hpp"
#include "defs.hpp"

struct VirtualPageLv0 {
    VirtualPageLv0() = default;
    TablePageEntry ptes[MAX_PAGES];
};

struct ProcessData {
    ProcessData() = default;
    VirtualPageLv0* virtualPageLv1[MAX_PAGES] = {nullptr};
    TablePageEntry* mapped[MAX_PTE] = {nullptr};
};

class MMU {
  private:
    TLB m_tlb;
    ProcessData m_procs[MAX_PROC];
    uint32_t m_page_fault{0}, m_page_hit{0}, m_page_forbiden{0};

  public:
    MMU() = default;
    virtual ~MMU() = default;

    /**
     * @brief Create a TablePageEntry in Pages table
     *
     * @param vAddress Virtual Address
     * @param pid PID do processo
     * @param protection bits protection URWX
     * @return std::tuple<int32_t, uint32_t> (MMU_OK, physic addr) | (MMU_FORBIDEN_*, 0) | (MMU_PAGE_FAULT, 0)
     */
    std::tuple<int32_t, uint32_t> createEntry(const uint32_t& vAddress, const uint32_t& pid,
                                              const uint8_t& protection) {

        const uint32_t page{MMU_GET_PAGE(vAddress)};
        const uint32_t lv1{MMU_GET_LV1_INDEX(page)};
        const uint32_t lv0{MMU_GET_LV0_INDEX(page)};

        m_tlb.set(pid);

        ProcessData* proc = &m_procs[pid];
        VirtualPageLv0* ptrVirtualPageLv0 = proc->virtualPageLv1[lv1];
        if (ptrVirtualPageLv0 == nullptr) {
            ptrVirtualPageLv0 = new VirtualPageLv0;
            proc->virtualPageLv1[lv1] = ptrVirtualPageLv0;
        }

        TablePageEntry& tablePage = ptrVirtualPageLv0->ptes[lv0];

        uint32_t minimalRef{0xffffffff};
        uint32_t indiceMininmal{0};

        if (!tablePage.valid) {
            for (uint32_t j{0}; j < MAX_PTE; j++) {
                if (proc->mapped[j] == nullptr) {
                    proc->mapped[j] = &tablePage;

                    tablePage.valid = true;
                    tablePage.referenced = 0;
                    tablePage.protection = protection;
                    tablePage.framePage = j;

                    m_tlb.save(tablePage, page);

                    return std::make_tuple(MMU_OK, MMU_GET_MEM_ADDRESS(j, vAddress));
                } else {
                    if (tablePage.referenced < minimalRef) {
                        minimalRef = tablePage.referenced;
                        indiceMininmal = j;
                        // FIXME: por causa do TLB este valor é baixo, muito baixo, alternativas ???
                    }
                }
            }

            // TODO: Aqui a memoria acaba e precisaremos fazer o swap!!!!
            // ir no tables
        }

        m_page_forbiden++;
        return std::make_tuple(MMU_FORBIDEN_ACCESS, 0);
    }

    /**
     * @brief Get the Physical Address object
     *
     * @param vAddress virtual address
     * @param pid pid of app
     * @param protection bits protection URWX
     * @return const std::tuple<int32_t, uint32_t> (MMU_OK, physic addr) | (MMU_FORBIDEN_*, 0) | (MMU_PAGE_FAULT, 0)
     */
    const std::tuple<int32_t, uint32_t> getPhysicalAddress(const uint32_t& vAddress, const uint32_t& pid,
                                                           const uint8_t& protection) { // protection URWX

        // verifica o TLB
        m_tlb.set(pid);
        auto result = m_tlb.find(vAddress, protection);
        const int32_t error = std::get<0>(result);
        if (error == MMU_TLB_MISS) {

            // Procura nos pages disponiveis
            auto result2 = this->getFromTables(vAddress, pid, protection);
            if (std::get<0>(result2) == MMU_PAGE_FAULT) {

                // FIXME: cria nova entrada no SO nao aqui!!!!
                auto result3 = this->createEntry(vAddress, pid, protection);
                return result3;
            }

            return result2;
        } else if (error < 0) {
            m_page_forbiden++;
        }

        // TLB_hit ou Forbiden
        return result;
    }

  private:
    /**
     * @brief Get physical address from Tables
     *
     * @param vAddress Virtual Address
     * @param pid PID do processo
     * @param protection bits protection URWX
     * @return const std::tuple<int32_t, uint32_t>  (MMU_OK, physic addr) | MMU_PAGE_FAULT,0 |(MMU_FORBIDEN_*, 0)
     */
    const std::tuple<int32_t, uint32_t> getFromTables(const uint32_t& vAddress, const uint32_t& pid,
                                                      const uint8_t& protection) { // protection URWX
        const uint32_t page{MMU_GET_PAGE(vAddress)};
        const uint32_t lv1{MMU_GET_LV1_INDEX(page)};
        const uint32_t lv0{MMU_GET_LV0_INDEX(page)};

        ProcessData* proc = &m_procs[pid];
        VirtualPageLv0* ptrVirtualPageLv0 = proc->virtualPageLv1[lv1];
        if (ptrVirtualPageLv0 == nullptr) {
            ptrVirtualPageLv0 = new VirtualPageLv0;
            proc->virtualPageLv1[lv1] = ptrVirtualPageLv0;
        }

        TablePageEntry& tablePage = ptrVirtualPageLv0->ptes[lv0];
        if (!tablePage.valid) {
            m_page_fault++;
            return std::make_tuple(MMU_PAGE_FAULT, 0);
        }

        uint8_t v = checkPermission(tablePage.protection, protection);
        if (v == 0) {
            tablePage.referenced++;
            m_tlb.save(tablePage, MMU_GET_PAGE(vAddress));

            // physical memory address
            m_page_hit++;
            return std::make_tuple(MMU_OK, MMU_GET_MEM_ADDRESS(tablePage.framePage, vAddress));
        } else {
            m_page_forbiden++;
        }

        return std::make_tuple(v, 0); // Forbiden Access
    }
};