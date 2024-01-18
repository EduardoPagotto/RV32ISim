#pragma once

// Buffers
#define MAX_TLB 10     // Max inputs in TLB
#define MAX_PROC 10    // Max process
#define MAX_PTE 16     // Total pages
#define MAX_PAGES 1024 // Total pages in levels
#define PGSIZE 4096    // Size os page

// Access
#define MMU_ACC_EXECUTE 0x001 // grant execute
#define MMU_ACC_WRITE 0x002   // grant write
#define MMU_ACC_READ 0x004    // grant read
#define MMU_ACC_SUPER 0x008   // grant root

// Errors
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

/**
 * @brief Check permission
 *
 * @param current permission from page
 * @param access  local permission
 * @return int32_t  MMU_OK | MMU_FORBIDEN_*
 */
int32_t checkPermission(const uint8_t& current, const uint8_t& access) {

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

struct TablePageEntry { // (PTE)
    TablePageEntry() = default;
    bool valid{false}, changed{false};
    uint8_t protection{0};
    uint32_t referenced{0}, framePage{0};
};