#pragma once
#include <stdint.h>

#define MAX_TLB 10  // Maximo de entradas no Table lookup
#define MAX_PROC 10 // Maximo de processos
#define MAX_PTE 16  // Total de paginas
#define PGSIZE 4096

struct TLB {
    TLB() = default;
    uint8_t status = 0;       // readyOnly, changed, valid -> 0000 0000
    uint32_t virtualPage = 0; // 20 bits superiores do Virtual Page Address
    uint32_t framePage = 0;   // indice do Page Table Entry (PTE)
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
        const uint32_t virtualAddressHi = address & 0xfffff000;
        const uint32_t offset = 0xfff & address;

        // Confere o Translation Lookaside Buffer (TLB)
        for (uint8_t i; i < MAX_TLB; i++) {
            if (tlb[i].virtualPage == virtualAddressHi) {
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
};