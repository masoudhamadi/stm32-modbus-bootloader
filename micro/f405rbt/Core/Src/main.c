#include "main.h"
#include "Modbus.h"
#include "flash.h"

extern volatile int bootloader_needs_reset;
extern volatile int app_need_start;

void SystemClock_Config(void) {
    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS;
    while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_5WS) {}
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR = (PWR->CR & ~PWR_CR_VOS) | PWR_CR_VOS;
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) {}
    RCC->CR |= RCC_CR_CSSON;
    RCC->PLLCFGR = (8U << RCC_PLLCFGR_PLLM_Pos) | (168U << RCC_PLLCFGR_PLLN_Pos) | RCC_PLLCFGR_PLLSRC_HSE;
    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2)) | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
}
static void MX_GPIO_Init(void) {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    GPIOC->BSRR = LL_GPIO_PIN_12 << 16U;
    GPIOC->MODER = (GPIOC->MODER & ~(3U << 24U)) | (1U << 24U);
    GPIOC->OTYPER &= ~(1U << 12U);
    GPIOC->OSPEEDR &= ~(3U << 24U);
    GPIOC->PUPDR &= ~(3U << 24U);
}
static void MX_CRC_Init(void){
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);
}
static void MX_USART3_UART_Init(void) {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
    GPIOC->MODER = (GPIOC->MODER & ~0x00F00000U) | 0x00A00000U;
    GPIOC->OTYPER &= ~0x00000C00U;
    GPIOC->OSPEEDR = (GPIOC->OSPEEDR & ~0x00F00000U) | 0x00F00000U;
    GPIOC->PUPDR &= ~0x00F00000U;
    GPIOC->AFR[1] = (GPIOC->AFR[1] & ~0x0000FF00U) | 0x00007700U;
    USART3->CR1 = 0U;
    USART3->CR2 = 0U;
    USART3->CR3 = 0U;
    USART3->BRR = (SystemCoreClock / 2U + 57600U) / 115200U;
    USART3->CR1 = USART_CR1_TE | USART_CR1_RE;
    USART3->CR1 |= USART_CR1_UE;
}

int main(void) {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));
    SystemClock_Config();
    MX_GPIO_Init();
    MX_CRC_Init();
    MX_USART3_UART_Init();
    Modbus_Init();
    while (!app_need_start) {
        FrameHandler();
        if (bootloader_needs_reset) NVIC_SystemReset();
    }
      FLASH->CR |= FLASH_CR_LOCK;
  __set_MSP(*(volatile uint32_t*) FLASH_APP_START_ADDR);
  ((void (*)(void)) (*(uint32_t*) (FLASH_APP_START_ADDR + 4)))();
}
