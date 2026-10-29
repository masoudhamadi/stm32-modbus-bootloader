#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>


#define FLASH_PAGE_SIZE (0x400)
#define FLASH_APP_START_ADDR (FLASH_BASE + 0x00004400)
#define FLASH_APP_MAX_ADDR (FLASH_BASE + 0x00010000)

typedef enum {
  FLASH_NOERR, FLASH_ERR
} flash_error_t;

flash_error_t flash_erase_app(void);
flash_error_t flash_write_app_data(uint32_t offset, uint8_t *data, uint8_t size);
uint32_t flash_get_app_crc(uint32_t fw_size_bytes);
flash_error_t flash_lock(void);

#endif /* FLASH_H */
