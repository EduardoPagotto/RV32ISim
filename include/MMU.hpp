#pragma once
#include "Memory2.hpp"
#include <stdint.h>

#define MAX_TLB 10     // Maximo de entradas no Table lookup
#define MAX_PROC 10    // Maximo de processos
#define MAX_PTE 16     // Total de paginas
#define MAX_PAGES 1024 // total de pages em tablePages
#define PGSIZE 4096

// Page Table Entry (PTE) 12 bits inferiores
#define MMU_ACC_EXECUTE 0x001 // Permissão executar
#define MMU_ACC_WRITE 0x002   // Permissão escrita
#define MMU_ACC_READ 0x004    // permissão leitura
#define MMU_ACC_SUPER 0x008   // Permissão usuario

// Erros
#define MMU_OK 0
#define MMU_PAGE_FAULT -1
#define MMU_FORBIDEN_ACCESS -2
#define MMU_FORBIDEN_READ -3
#define MMU_FORBIDEN_WRITE -4
#define MMU_FORBIDEN_EXECUTE -5
#define MMU_TLB_MISS -6
#define MMU_SWAPOUT -7

#define MMU_GET_PAGE(vAddr) (vAddr >> 12)
#define MMU_GET_LV1_INDEX(page) ((page >> 10) & 0x000003FF)
#define MMU_GET_LV0_INDEX(page) (page & 0x000003FF)
#define MMU_GET_MEM_ADDRESS(framePage, vAddress) ((framePage << 12) | (vAddress & 0x00000FFF))

struct TLB {
    TLB() = default;
    bool valid{false}, changed{false}; // entrada no TLB valida
    uint8_t protection{0};             // URWX
    uint32_t page{0}, framePage{0};
};

struct TablePageEntry { // (PTE)
    TablePageEntry() = default;
    bool valid{false}, changed{false}, refed{false};
    uint8_t protection{0};
    uint32_t framePage{0};
};

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
    ProcessData procs[MAX_PROC];
    TLB tlb[MAX_TLB];
    bool memPages[MAX_PTE] = {false};

    uint32_t tlb_hit{0}, tlb_miss{0};

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

            for (uint32_t j{0}; j < MAX_PAGES; j++) {
                if (memPages[j] == false) {

                    memPages[j] = true;
                    tablePage.framePage = j;

                    this->insertTLB(tablePage, page);

                    return std::make_tuple(MMU_OK, MMU_GET_MEM_ADDRESS(j, vAddress));
                }
            }
        }

        // FIXME:
        return std::make_tuple(MMU_PAGE_FAULT, 0);
    }

    const std::tuple<int32_t, uint32_t> getPhysicalAddress(const uint32_t& vAddress, const uint32_t& pid,
                                                           const uint8_t& access) { // access URWX

        // verifica o TLB
        auto result = this->getFromTLB(vAddress, access);
        if (std::get<0>(result) == MMU_TLB_MISS) {

            // Procura nos pages disponiveis
            auto result2 = this->getFromTables(vAddress, pid, access);
            if (std::get<0>(result2) == MMU_PAGE_FAULT) {

                // cria nova entrada
                auto result3 = this->createEntry(vAddress, pid, access);
                return result3;
            }

            return result2;
        }

        // TLB_hit ou Forbiden
        return result;
    }

  private:
    // Page fault -> frame memory sem mapeamento, verificar swap
    // Page hit -> frame memory encontrado
    // Access fail -> tentativa de acesso nao autorizado ou nao existente

    /**
     * @brief Get physical address from TLB array
     * TLB hit -> encontrado no TLB
     * TLB miss -> nao esta na TLB
     *
     * @param vAddress Virtual Address
     * @param access bits protection URWX
     * @return const std::tuple<int32_t, uint32_t> (MMU_OK, physic addr) | (MMU_TLB_MISS, 0) | (MMU_FORBIDEN_*, 0)
     */
    const std::tuple<int32_t, uint32_t> getFromTLB(const uint32_t& vAddress, const uint8_t& access) {

        // Confere o Translation Lookaside Buffer (TLB)
        for (uint8_t i{0}; i < MAX_TLB; i++) {

            if ((tlb[i].valid) && (MMU_GET_PAGE(vAddress) == tlb[i].page)) { // entrada existe e é valida
                int32_t v = this->validaAcesso(tlb[i].protection, access);   // verifica se acesso e permitido
                if (v == 0) {
                    // TLB hit (encontrou no TLB)
                    tlb_hit++;
                    return std::make_tuple(MMU_OK, MMU_GET_MEM_ADDRESS(tlb[i].framePage, vAddress));
                }

                return std::make_tuple(v, 0); // Forbiden
            }
        }
        tlb_miss++;
        return std::make_tuple(MMU_TLB_MISS, 0); // TLB miss
    }

    /**
     * @brief Get physical address from Tables
     *
     * @param vAddress Virtual Address
     * @param pid PID do processo
     * @param access bits protection URWX
     * @return const std::tuple<int32_t, uint32_t>  (MMU_OK, physic addr) | (MMU_FORBIDEN_*, 0)
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
        if (!tablePage.valid)
            return std::make_tuple(MMU_PAGE_FAULT, 0);

        uint8_t v = this->validaAcesso(tablePage.protection, access);
        if (v == 0) {
            tablePage.refed = true;
            this->insertTLB(tablePage, MMU_GET_PAGE(vAddress));

            return std::make_tuple(0, MMU_GET_MEM_ADDRESS(tablePage.framePage, vAddress)); // physical memory address
        }

        return std::make_tuple(v, 0); // Forbiden Access
    }

    /**
     * @brief Limpa o TLB do processo
     *
     */
    void flushTLB() {
        for (uint8_t i; i < MAX_TLB; i++) {
            tlb[i].valid = false;
            tlb[i].changed = false;
            tlb[i].protection = 0;
            tlb[i].page = 0;
            tlb[i].framePage = 0;
        }
    }

    /**
     * @brief Insere nova entrada no TLB
     *
     * @param pte
     * @param page
     */
    void insertTLB(const TablePageEntry& pte, const uint32_t& page) {

        // Carrega no Translation Lookaside Buffer (TLB)
        for (uint8_t i; i < MAX_TLB; i++) {
            if (!tlb[i].valid) {
                tlb[i].valid = true;
                tlb[i].changed = pte.changed;
                // tlb[i].refed = pte.refed;
                tlb[i].protection = pte.protection;
                tlb[i].page = page;
                tlb[i].framePage = pte.framePage;
                return;
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
     * @brief Testa se acesso e permitido
     *
     * @param current valor do page
     * @param access  valor da nova operacao
     * @return int32_t  MMU_OK | MMU_FORBIDEN_*
     */
    int32_t validaAcesso(const uint8_t& current, const uint8_t& access) {

        const uint8_t res = current ^ access;
        if (res == 0)
            return MMU_OK;

        if (((res & MMU_ACC_SUPER) == MMU_ACC_SUPER) && ((access & MMU_ACC_SUPER) == 0))
            return MMU_FORBIDEN_ACCESS;

        if (((res & MMU_ACC_READ) == MMU_ACC_READ) && ((current & MMU_ACC_READ) == 0))
            return MMU_FORBIDEN_READ;

        if (((res & MMU_ACC_WRITE) == MMU_ACC_WRITE) && ((current & MMU_ACC_WRITE) == 0))
            return MMU_FORBIDEN_WRITE;

        if (((res & MMU_ACC_EXECUTE) == MMU_ACC_EXECUTE) && ((current & MMU_ACC_EXECUTE) == 0))
            return MMU_FORBIDEN_EXECUTE;

        return MMU_OK;
    }

    // const bool write(const uint32_t& vAddress, const MemoryAccessWidth& width, const uint32_t& reg) {

    //     // const uint32_t size = static_cast<uint32_t>(width);
    //     // for (auto& v : banks) {
    //     //     if (v->validRange(vAddress, size))
    //     //         return v->write(vAddress, size, reg);
    //     // }

    //     return false;
    // }

    // const std::optional<uint32_t> read(const uint32_t& vAddress, const MemoryAccessWidth& width,
    //                                    const bool& signedVal = false) {

    //     // const uint32_t size = static_cast<uint32_t>(width);
    //     // for (auto& v : banks) {
    //     //     if (v->validRange(vAddress, size)) {
    //     //         auto oValue = v->read(vAddress, size, signedVal);
    //     //         if (oValue.has_value()) {
    //     //             return oValue;
    //     //         }
    //     //     }
    //     // }

    //     return {};
    // }
};