#include "flash.h"
#include "stm32f4xx.h"
#include <stddef.h>
#include <stdbool.h>

#include <stdint.h>

static inline bool flash_is_locked(void) { return (FLASH->CR & FLASH_CR_LOCK) != 0U; }
static inline bool flash_is_busy(void) { return (FLASH->SR & FLASH_SR_BSY) != 0U; }

static flash_error_t flash_unlock(void) {
    if (!flash_is_locked()) return FLASH_NOERR;
    FLASH->KEYR = 0x45670123U;
    FLASH->KEYR = 0xCDEF89ABU;
    return flash_is_locked() ? FLASH_ERR : FLASH_NOERR;
}

static uint8_t flash_read_byte(uint32_t address) { return *(__IO uint8_t *)address; }
static uint16_t flash_read_hword(uint32_t address) { return *(__IO uint16_t *)address; }

static flash_error_t flash_write_hword(uint32_t address, uint16_t data) {
    while (flash_is_busy()) {}
    FLASH->SR = FLASH_SR_EOP | FLASH_SR_OPERR | FLASH_SR_WRPERR |
                FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR;
    FLASH->CR = (FLASH->CR & ~(FLASH_CR_PSIZE | FLASH_CR_SER | FLASH_CR_MER)) |
                FLASH_CR_PG | FLASH_CR_PSIZE_1;
    *(__IO uint16_t *)address = data;
    while (flash_is_busy()) {}
    FLASH->CR &= ~FLASH_CR_PG;
    if ((FLASH->SR & (FLASH_SR_OPERR | FLASH_SR_WRPERR | FLASH_SR_PGAERR |
                      FLASH_SR_PGPERR | FLASH_SR_PGSERR)) != 0U) return FLASH_ERR;
    return flash_read_hword(address) == data ? FLASH_NOERR : FLASH_ERR;
}
static flash_error_t flash_erase_sector(uint8_t sector) {
    if (sector > 11U || flash_unlock() == FLASH_ERR) return FLASH_ERR;
    while (flash_is_busy()) {}
    FLASH->SR = FLASH_SR_EOP | FLASH_SR_OPERR | FLASH_SR_WRPERR |
                FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR;
    FLASH->CR = (FLASH->CR & ~(FLASH_CR_SNB | FLASH_CR_PSIZE | FLASH_CR_PG |
                               FLASH_CR_SER | FLASH_CR_MER)) |
                FLASH_CR_SER | ((uint32_t)sector << FLASH_CR_SNB_Pos) |
                FLASH_CR_PSIZE_1;
    FLASH->CR |= FLASH_CR_STRT;
    while (flash_is_busy()) {}
    FLASH->CR &= ~FLASH_CR_SER;
    return (FLASH->SR & (FLASH_SR_OPERR | FLASH_SR_WRPERR | FLASH_SR_PGAERR |
                         FLASH_SR_PGPERR | FLASH_SR_PGSERR)) != 0U ? FLASH_ERR : FLASH_NOERR;
}
flash_error_t flash_erase_app(void) {
    uint8_t first_sector = 0U;
    uint8_t last_sector = 11U;
    uint32_t start = FLASH_APP_START_ADDR;
    uint32_t end = FLASH_APP_MAX_ADDR;
    if (start < 0x08000000U || end <= start || end > 0x08100000U) return FLASH_ERR;
    while (first_sector < 11U) {
        uint32_t next = first_sector < 4U ? 0x08000000U + ((uint32_t)first_sector + 1U) * 0x4000U :
                        first_sector == 4U ? 0x08020000U :
                        0x08020000U + ((uint32_t)first_sector - 4U) * 0x20000U;
        if (start < next) break;
        first_sector++;
    }
    last_sector = first_sector;
    while (last_sector < 11U) {
        uint32_t next = last_sector < 3U ? 0x08000000U + ((uint32_t)last_sector + 1U) * 0x4000U :
                        last_sector == 3U ? 0x08010000U :
                        last_sector == 4U ? 0x08020000U :
                        0x08020000U + ((uint32_t)last_sector - 4U) * 0x20000U;
        if (end <= next) break;
        last_sector++;
    }
    for (uint8_t sector = first_sector; sector <= last_sector; sector++)
        if (flash_erase_sector(sector) != FLASH_NOERR) return FLASH_ERR;
    return flash_lock();
}
flash_error_t flash_write_app_data(uint32_t offset, uint8_t *data, uint8_t size) {
    uint32_t address;
    uint16_t hword;
    if (data == NULL || size == 0U ||
        offset >= FLASH_APP_MAX_ADDR - FLASH_APP_START_ADDR ||
        size > FLASH_APP_MAX_ADDR - FLASH_APP_START_ADDR - offset) return FLASH_ERR;
    if (flash_unlock() == FLASH_ERR) return FLASH_ERR;
    for (uint32_t i = 0U; i < size; i += 2U) {
        address = FLASH_APP_START_ADDR + offset + i;
        hword = data[i];
        if (i + 1U < size) hword |= (uint16_t)data[i + 1U] << 8;
        else hword |= 0xFF00U;
        if (flash_write_hword(address, hword) != FLASH_NOERR) {
            flash_lock();
            return FLASH_ERR;
        }
    }
    return flash_lock();
}
flash_error_t flash_lock(void) {
    if (flash_is_locked()) return FLASH_NOERR;
    FLASH->CR |= FLASH_CR_LOCK;
    return flash_is_locked() ? FLASH_NOERR : FLASH_ERR;
}

uint32_t flash_get_app_crc(uint32_t fw_size_bytes) {
    uint32_t address = FLASH_APP_START_ADDR;
    uint32_t words = fw_size_bytes / 4U;
    uint32_t remain = fw_size_bytes % 4U;
    if (fw_size_bytes > FLASH_APP_MAX_ADDR - FLASH_APP_START_ADDR) return 0U;
    CRC->CR = CRC_CR_RESET;
    for (uint32_t i = 0U; i < words; ++i, address += 4U)
        CRC->DR = *(volatile const uint32_t *)address;
    if (remain != 0U) {
        uint32_t data = 0xFFFFFFFFU;
        for (uint32_t i = 0U; i < remain; ++i)
            ((uint8_t *)&data)[i] = flash_read_byte(address + i);
        CRC->DR = data;
    }
    return CRC->DR;
}