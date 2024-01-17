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
#define MMU_ACC_USER 0x008    // Permissão usuario

#define MMU_VALID 0x010 // Memoria valida

#define MMU_CHANGED 0x020 // Memoria alterada, obs: validado somente no TLB
#define MMU_REF 0x040     // Memoria referenciada, obs: validado somente no TLB

// Erros
#define MMU_PAGE_FAULT 1
#define MMU_FORBIDEN_ACCESS -1
#define MMU_FORBIDEN_READ -2
#define MMU_FORBIDEN_WRITE -3
#define MMU_FORBIDEN_EXECUTE -4
#define MMU_TLB_MISS -5
#define MMU_SWAPOUT -6
// #define MMU_FORBIDEN_READ
// #define MMU_FORBIDEN_READ

struct TLB {
    TLB() = default;
    bool valid = false;     // entrada no TLB valida
    bool changed = false;   // memoria foi alterada
    uint8_t protection = 0; // URWX
    uint32_t page = 0;      // 20 bits superiores do Virtual Page Address + 12 inferiores de acesso
    uint32_t framePage = 0; // Frame Page (numero da moldura da pagina)
};

// physical table entry (PTE)
struct TablePage {
    TablePage() = default;
    bool valid = false;
    bool changed = false;
    bool refed = false;
    uint8_t protection = 0;
    uint32_t framePage = 0;
};

struct VirtualPageLv0 {
    TablePage entrys[MAX_PAGES];
};

struct ProcPage {
    ProcPage() = default;
    VirtualPageLv0* virtualPageLv1[MAX_PAGES] = {nullptr};
};

class MMU {
  private:
    ProcPage pages[MAX_PROC];
    TLB tlb[MAX_TLB];

    bool memPages[MAX_PTE] = {false};

  public:
    MMU() = default;
    virtual ~MMU() = default;

    void flushTLB() {
        for (uint8_t i; i < MAX_TLB; i++) {
            tlb[i].valid = false;
            tlb[i].changed = false;
            tlb[i].protection = 0;
            tlb[i].page = 0;
            tlb[i].framePage = 0;
        }
    }

    // TLB hit -> encontrado no TLB
    // TLB miss -> nao esta na TLB
    // Page fault -> frame memory sem mapeamento, verificar swap
    // Page hit -> frame memory encontrado
    // Access fail -> tentativa de acesso nao autorizado ou nao existente

    const std::tuple<int32_t, uint32_t> getTLB(const uint32_t& vAddress, const uint8_t& opp) { // opp URWX

        // Virtual Page Number (VPN)
        const uint32_t page = vAddress & 0xFFFFF000 >> 12;
        const uint32_t offset = 0xfff & vAddress;

        // Confere o Translation Lookaside Buffer (TLB)
        for (uint8_t i{0}; i < MAX_TLB; i++) {

            // entrada existe e é valida
            if ((tlb[i].valid) && (page == tlb[i].page)) {

                int32_t v = this->validaAcesso(tlb[i].protection, opp);
                if (v == 0) {
                    // Grant
                    // se escrita modificar flag do tlb
                    if ((opp & MMU_ACC_WRITE) == MMU_ACC_WRITE)
                        tlb[i].changed = true;

                    // TLB hit (encontrou no TLB)
                    return std::make_tuple(0, tlb[i].framePage);

                } else {
                    // Forbiden
                    return std::make_tuple(v, 0);
                }
            }
        }
        return std::make_tuple(MMU_TLB_MISS, 0);
    }

    const std::tuple<int32_t, uint32_t> getTables(const uint32_t& vAddress, const uint32_t& pid,
                                                  const uint8_t& opp) { // opp URWX

        // Virtual Page Number (VPN)
        const uint32_t vAddressHi = vAddress & 0xFFFFF000;
        const uint32_t offset = 0xfff & vAddress;
        // const uint32_t oppMMU = (opp | MMU_VALID) & 0x0000001F;

        const uint32_t lv1 = (vAddressHi >> 22) & 0x3FF;
        const uint32_t lv0 = (vAddressHi >> 12) & 0x3FF;

        ProcPage* page = &pages[pid];

        VirtualPageLv0* ptrVirtualPageLv0 = page->virtualPageLv1[lv1];
        if (ptrVirtualPageLv0 == nullptr) {

            ptrVirtualPageLv0 = new VirtualPageLv0;
            page->virtualPageLv1[lv1] = ptrVirtualPageLv0;

            return std::make_tuple(MMU_PAGE_FAULT, 0);
        }

        TablePage& tablePage = ptrVirtualPageLv0->entrys[lv0];
        if (!tablePage.valid)
            return std::make_tuple(MMU_PAGE_FAULT, 0);

        uint8_t v = this->validaAcesso(tablePage.protection, opp);
        if (v == 0) {

            // tablePage.valid = true;
            if ((opp & MMU_ACC_WRITE) == MMU_ACC_WRITE)
                tablePage.changed = true;

            tablePage.refed = true;

            // TODO: adicionar entrada a TLB!
            this->insertTLB(tablePage, vAddress >> 12);

            // PTE
            // physical page number mask 0xfffff000 (20 upper bits)
            return std::make_tuple(0, ((tablePage.framePage << 12) | offset));
        }

        return std::make_tuple(v, 0);
    }

    std::tuple<int32_t, uint32_t> createEntry(const uint32_t& vAddress, const uint32_t& pid, const uint32_t& opp) {

        const uint32_t vAddressHi = vAddress & 0xFFFFF000;
        const uint32_t offset = 0xfff & vAddress;

        // TLB Miss (procura nas tables)
        const uint32_t lv1 = (vAddressHi >> 22) & 0x3FF;
        const uint32_t lv0 = (vAddressHi >> 12) & 0x3FF;

        ProcPage* page = &pages[pid];

        VirtualPageLv0* ptrVirtualPageLv0 = page->virtualPageLv1[lv1];

        for (uint32_t i = 0; i < MAX_PAGES; i++) {
            TablePage& pte = ptrVirtualPageLv0->entrys[i];

            if (!pte.valid) {

                pte.valid = true;

                if ((opp & MMU_ACC_WRITE) == MMU_ACC_WRITE)
                    pte.changed = true;

                pte.protection = opp;

                for (uint32_t j = 0; j < MAX_PAGES; j++) {
                    if (memPages[j] == false) {
                        memPages[j] = true;

                        pte.framePage = j;

                        this->insertTLB(pte, vAddress >> 12);

                        return std::make_tuple(0, (j | offset));
                    }
                }
            }
        }

        // FIXME:
        return std::make_tuple(MMU_PAGE_FAULT, 0);
    }

  private:
    void insertTLB(const TablePage& pte, const uint32_t& page) {

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

    int32_t validaAcesso(const uint32_t& current, const uint8_t& opp) {

        const uint32_t res = current ^ opp;
        if (((res & MMU_ACC_USER) != 0) && ((current & MMU_ACC_USER) == 0))
            return MMU_FORBIDEN_ACCESS;

        if (((res & MMU_ACC_READ) != 0) && ((current & MMU_ACC_READ) == 0))
            return MMU_FORBIDEN_READ;

        if (((res & MMU_ACC_WRITE) != 0) && ((current & MMU_ACC_WRITE) == 0))
            return MMU_FORBIDEN_WRITE;

        if (((res & MMU_ACC_EXECUTE) != 0) && ((current & MMU_ACC_EXECUTE) == 0))
            return MMU_FORBIDEN_EXECUTE;

        return 0;
    }

    // const uint32_t getVirtualAddress(const uint32_t& vAddress, const uint32_t& pid, const uint8_t& opp) { // opp RWX

    //     // Virtual Page Number (VPN)
    //     const uint32_t vAddressHi = vAddress & 0xFFFFF000;
    //     const uint32_t offset = 0xfff & vAddress;

    //     // Confere o Translation Lookaside Buffer (TLB)
    //     for (uint8_t i; i < MAX_TLB; i++) {
    //         const uint32_t virtualPage = tlb[i].pte & 0xFFFFF000;
    //         if (virtualPage == vAddressHi) {
    //             // TODO: testar se bit's de status RO, changed, valid estao validos

    //             // TLB hit (encontrou no TLB)
    //             return tlb[i].framePage;
    //         }
    //     }

    //     // TLB Miss (procura nas tables)
    //     const uint32_t lv1 = (vAddressHi >> 22) & 0x3FF;
    //     const uint32_t lv0 = (vAddressHi >> 12) & 0x3FF;

    //     ProcPage* page = &pages[pid];

    //     TablePage* ptrVirtualPageLv0 = page->virtualPageLv1[lv1];

    //     if (ptrVirtualPageLv0 == nullptr) {
    //         // TODO: implementar page fault
    //     } else {
    //         const uint32_t indexPTE = ptrVirtualPageLv0->pages[lv0];
    //         const uint32_t pte = page->pte[indexPTE];

    //         // TODO: verifica se esta disponoivel e se é possivel fazer a operação
    //         if (pte & 01) {
    //             // TODO: adicionar entrada a TLB!

    //             // 0xffc; 0x3ff; 0xfff

    //             // PTE
    //             // Controller mask 0x01f (12 bits -> 0000 000U XWRV
    //             // V -> valid mask 0x001
    //             // R -> reader     0x002
    //             // W -> writer     0x004
    //             // X -> executable 0x008
    //             // U -> usermode   0x010
    //             //
    //             // obs: faltam os bits (necessarios para swap):
    //             // - modificada (pagina sofreu modificação)
    //             // - referenciada (pagina foi lida recentemente)
    //             //
    //             // physical page number mask 0xfffff000 (20 upper bits)

    //             return ((pte & 0xfffff000) | offset); // page hit (entrada encontrada na tables)
    //         } else {
    //             // TODO: implementar page fault e swap-in
    //         }
    //     }

    //     return 0;
    // }

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