#pragma once
#include <stdint.h>

#define MAX_TLB 10  // Maximo de entradas no Table lookup
#define MAX_PROC 10 // Maximo de processos
#define MAX_PTE 16  // Total de paginas
#define PGSIZE 4096

// Page Table Entry (PTE) 12 bits inferiores
#define MEM_EXECUTE 0x001 // Permissão executar
#define MEM_WRITE 0x002   // Permissão escrita
#define MEM_READ 0x004    // permissão leitura
#define MEM_VALID 0x008   // Memoria valida
#define MEM_USER 0x010    // Permissão usuario
#define MEM_CHANGED 0x020 // Memoria alterada
#define MEM_REF 0x040     // Memoria referenciada

//
#define TLB_RO 0x01
#define TLB_CHANGED 0x02
#define TLB_VALID 0x04

#define MEM_USERMODE

struct TLB {
    TLB() = default;
    uint32_t pte = 0;       // 20 bits superiores do Virtual Page Address
    uint32_t framePage = 0; // indice do Page Table Entry (PTE)
};

struct TablePage {
    uint32_t pages[1024];
};

struct ProcPage {
    ProcPage() = default;
    TablePage* virtualPageLv1[1024] = {nullptr};
    uint32_t pte[MAX_PTE] = {0}; // physical table entry // memoria fisica real 64k
};

class MMU {
  private:
    ProcPage pages[MAX_PROC];
    TLB tlb[MAX_TLB];

  public:
    MMU() = default;
    virtual ~MMU() = default;

    void flushTLB() {}

    const uint32_t getVirtualAddress(const uint32_t& address, const uint32_t& pid, const uint8_t& opp) { // opp RWX

        // Virtual Page Number (VPN)
        const uint32_t virtualAddressHi = address & 0xFFFFF000;
        const uint32_t offset = 0xfff & address;

        // Confere o Translation Lookaside Buffer (TLB)
        for (uint8_t i; i < MAX_TLB; i++) {
            const uint32_t virtualPage = tlb[i].pte & 0xFFFFF000;
            if (virtualPage == virtualAddressHi) {
                // TODO: testar se bit's de status RO, changed, valid estao validos

                // TLB hit (encontrou no TLB)
                return tlb[i].framePage;
            }
        }

        // TLB Miss (procura nas tables)
        const uint32_t lv1 = (virtualAddressHi >> 22) & 0x3FF;
        const uint32_t lv0 = (virtualAddressHi >> 12) & 0x3FF;

        ProcPage* page = &pages[pid];

        TablePage* ptrVirtualPageLv0 = page->virtualPageLv1[lv1];

        if (ptrVirtualPageLv0 == nullptr) {
            // TODO: implementar page fault
        } else {
            const uint32_t indexPTE = ptrVirtualPageLv0->pages[lv0];
            const uint32_t pte = page->pte[indexPTE];

            // TODO: verifica se esta disponoivel e se é possivel fazer a operação
            if (pte & 01) {
                // TODO: adicionar entrada a TLB!

                // 0xffc; 0x3ff; 0xfff

                // PTE
                // Controller mask 0x01f (12 bits -> 0000 000U XWRV
                // V -> valid mask 0x001
                // R -> reader     0x002
                // W -> writer     0x004
                // X -> executable 0x008
                // U -> usermode   0x010
                //
                // obs: faltam os bits (necessarios para swap):
                // - modificada (pagina sofreu modificação)
                // - referenciada (pagina foi lida recentemente)
                //
                // physical page number mask 0xfffff000 (20 upper bits)

                return ((pte & 0xfffff000) | offset); // page hit (entrada encontrada na tables)
            } else {
                // TODO: implementar page fault e swap-in
            }
        }

        return 0;
    }

    // const bool write(const uint32_t& address, const MemoryAccessWidth& width, const uint32_t& reg) {

    //     // const uint32_t size = static_cast<uint32_t>(width);
    //     // for (auto& v : banks) {
    //     //     if (v->validRange(address, size))
    //     //         return v->write(address, size, reg);
    //     // }

    //     return false;
    // }

    // const std::optional<uint32_t> read(const uint32_t& address, const MemoryAccessWidth& width,
    //                                    const bool& signedVal = false) {

    //     // const uint32_t size = static_cast<uint32_t>(width);
    //     // for (auto& v : banks) {
    //     //     if (v->validRange(address, size)) {
    //     //         auto oValue = v->read(address, size, signedVal);
    //     //         if (oValue.has_value()) {
    //     //             return oValue;
    //     //         }
    //     //     }
    //     // }

    //     return {};
    // }
};