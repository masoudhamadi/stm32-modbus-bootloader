
#include "Modbus.h"
#include "flash.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"

#define MB_RX_SIZE          255U
#define MB_FC03             3U
#define MB_FC16             16U
#define MB_FC16_MAX_REGS    123U
#define EraseFlash          1U
#define ProgramFlash        2U
#define GetChecksumFlash    3U
#define ResetDevice         4U
#define AppNeedStart        5U
#define FLASH_CMD_REG       0U

uint16_t gRegs[123];
static uint8_t rx[MB_RX_SIZE], tx[255];
static uint16_t rx_count;
static uint32_t fw_size_bytes;

volatile int bootloader_needs_reset = 0;
volatile int app_need_start;

static uint16_t calcCRC(const uint8_t *buf, uint16_t len) {
    uint16_t crc = 0xFFFFU;
    while (len--) {
        crc ^= *buf++;
        for (uint8_t i = 0U; i < 8U; i++)
            crc = (crc & 1U) ? (crc >> 1) ^ 0xA001U : crc >> 1;
    }
    return crc;
}

static uint16_t word(uint8_t H, uint8_t L) {
    return ((uint16_t)H << 8) | L;
}

static uint8_t valid_register_range(uint16_t start, uint16_t count, uint16_t max_count) {
    return count != 0U && count <= max_count &&
           (uint32_t)start + count <= 123U;
}

static void transmit(const uint8_t *buf, uint16_t len) {
#if defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN)
    LL_GPIO_SetOutputPin(MODBUS_DE_PORT, MODBUS_DE_PIN);
#endif
    for (uint16_t i = 0U; i < len; i++) {
        while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
        LL_USART_TransmitData8(USART3, buf[i]);
    }
    while (!LL_USART_IsActiveFlag_TC(USART3)) {}
#if defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN)
    LL_GPIO_ResetOutputPin(MODBUS_DE_PORT, MODBUS_DE_PIN);
#endif
}

static void send_response(uint16_t len) {
    uint16_t crc = calcCRC(tx, len);
    tx[len++] = (uint8_t)crc;
    tx[len++] = (uint8_t)(crc >> 8);
    transmit(tx, len);
}


void Modbus_Init(void) {
    rx_count = 0U;
}

void FrameHandler(void) {
    while (LL_USART_IsActiveFlag_RXNE(USART3)) {
        uint8_t data = LL_USART_ReceiveData8(USART3);
        if (rx_count >= sizeof(rx)) rx_count = 0U;
        rx[rx_count++] = data;
        if (rx_count < 2U) continue;

        if (rx[1] == MB_FC03) {
            if (rx_count < 8U) continue;
            uint16_t crc = calcCRC(rx, 6U);
            if (rx[0] == MODBUS_SLAVE_ID &&
                rx[6] == (uint8_t)crc && rx[7] == (uint8_t)(crc >> 8)) {
                uint16_t start = word(rx[2], rx[3]);
                uint16_t count = word(rx[4], rx[5]);
                if (valid_register_range(start, count, 125U)) {
                    tx[0] = rx[0];
                    tx[1] = MB_FC03;
                    tx[2] = (uint8_t)(count * 2U);
                    for (uint16_t i = 0U; i < count; i++) {
                        uint16_t val = gRegs[start + i];
                        tx[3U + i * 2U] = (uint8_t)(val >> 8);
                        tx[4U + i * 2U] = (uint8_t)val;
                    }
                    send_response(3U + count * 2U);
                }
            }
            rx_count = 0U;
            return;
        }

        if (rx[1] == MB_FC16) {
            if (rx_count < 9U) continue;
            uint16_t count = word(rx[4], rx[5]);
            uint8_t byte_count = rx[6];
            if (count == 0U || count > MB_FC16_MAX_REGS ||
                byte_count != count * 2U) {
                rx_count = 0U;
                return;
            }
            uint16_t expected = 9U + byte_count;
            if (rx_count < expected) continue;
            uint16_t crc = calcCRC(rx, expected - 2U);
            if (rx[0] == MODBUS_SLAVE_ID &&
                rx[expected - 2U] == (uint8_t)crc &&
                rx[expected - 1U] == (uint8_t)(crc >> 8)) {
                uint16_t start = word(rx[2], rx[3]);
                if (valid_register_range(start, count, MB_FC16_MAX_REGS)) {
                    if (start == FLASH_CMD_REG) {
                        uint8_t cmd = (uint8_t)word(rx[7], rx[8]);
                        uint8_t status = FLASH_ERR;
                        uint32_t offset, crc_value;
                        uint16_t data_len;
                        switch (cmd) {
                        case EraseFlash:
                            fw_size_bytes = 0U;
                            status = flash_erase_app();
                            gRegs[FLASH_CMD_REG] = status;
                            break;
                        case ProgramFlash:
                            if (count < 4U) {
                                rx_count = 0U;
                                return;
                            }
                            offset = ((uint32_t)word(rx[9], rx[10]) << 16) |
                                     word(rx[11], rx[12]);
                            data_len = (uint16_t)((count - 3U) * 2U);
                            if ((offset & 1U) != 0U) {
                                rx_count = 0U;
                                return;
                            }
                            status = flash_write_app_data(offset, &rx[13], (uint8_t)data_len);
                            if (status == FLASH_NOERR && offset + data_len > fw_size_bytes)
                                fw_size_bytes = offset + data_len;
                            gRegs[FLASH_CMD_REG] = status;
                            break;
                        case GetChecksumFlash:
                            flash_lock();
                            crc_value = flash_get_app_crc(fw_size_bytes);
                            gRegs[0] = (uint16_t)crc_value;
                            gRegs[1] = (uint16_t)(crc_value >> 16);
                            break;
                        case ResetDevice:
                            gRegs[FLASH_CMD_REG] = 0U;
                            bootloader_needs_reset = 1;
                            break;
                        case AppNeedStart:
                            gRegs[FLASH_CMD_REG] = 0U;
                            app_need_start = 1;
                            break;
                        default:
                            rx_count = 0U;
                            return;
                        }
                    } else {
                        for (uint16_t i = 0U; i < count; i++)
                            gRegs[start + i] = word(rx[7U + i * 2U], rx[8U + i * 2U]);
                    }
                    tx[0] = rx[0];
                    tx[1] = MB_FC16;
                    tx[2] = rx[2];
                    tx[3] = rx[3];
                    tx[4] = rx[4];
                    tx[5] = rx[5];
                    send_response(6U);
                }
            }
            rx_count = 0U;
            return;
        }
        rx_count = 0U;
        return;
    }
}
