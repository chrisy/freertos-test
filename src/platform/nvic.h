/** NVIC vector definition
 * \file src/platform/nvic.h
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef _NVIC_H
#define _NVIC_H

#include <stdint.h>

struct nvic {
    uint32_t    *stack_top;
    void        (*Reset_Handler)(void);
    void        (*NMI_Handler)(void);
    void        (*HardFault_Handler)(void);
    void        (*MemManage_Handler)(void);
    void        (*BusFault_Handler)(void);
    void        (*UsageFault_Handler)(void);
    void        (*unknown1)(void);
    void        (*unknown2)(void);
    void        (*unknown3)(void);
    void        (*unknown4)(void);
    void        (*SVC_Handler)(void);
    void        (*DebugMon_Handler)(void);
    void        (*unknown5)(void);
    void        (*PendSV_Handler)(void);
    void        (*SysTick_Handler)(void);
    void        (*WWDG_IRQHandler)(void);
    void        (*PVD_IRQHandler)(void);
    void        (*TAMPER_IRQHandler)(void);
    void        (*RTC_IRQHandler)(void);
    void        (*FLASH_IRQHandler)(void);
    void        (*RCC_IRQHandler)(void);
    void        (*EXTI0_IRQHandler)(void);
    void        (*EXTI1_IRQHandler)(void);
    void        (*EXTI2_IRQHandler)(void);
    void        (*EXTI3_IRQHandler)(void);
    void        (*EXTI4_IRQHandler)(void);
    void        (*DMA1_Channel1_IRQHandler)(void);
    void        (*DMA1_Channel2_IRQHandler)(void);
    void        (*DMA1_Channel3_IRQHandler)(void);
    void        (*DMA1_Channel4_IRQHandler)(void);
    void        (*DMA1_Channel5_IRQHandler)(void);
    void        (*DMA1_Channel6_IRQHandler)(void);
    void        (*DMA1_Channel7_IRQHandler)(void);
    void        (*ADC1_2_IRQHandler)(void);
    void        (*USB_HP_CAN1_TX_IRQHandler)(void);
    void        (*USB_LP_CAN1_RX0_IRQHandler)(void);
    void        (*CAN1_RX1_IRQHandler)(void);
    void        (*CAN1_SCE_IRQHandler)(void);
    void        (*EXTI9_5_IRQHandler)(void);
    void        (*TIM1_BRK_IRQHandler)(void);
    void        (*TIM1_UP_IRQHandler)(void);
    void        (*TIM1_TRG_COM_IRQHandler)(void);
    void        (*TIM1_CC_IRQHandler)(void);
    void        (*TIM2_IRQHandler)(void);
    void        (*TIM3_IRQHandler)(void);
    void        (*TIM4_IRQHandler)(void);
    void        (*I2C1_EV_IRQHandler)(void);
    void        (*I2C1_ER_IRQHandler)(void);
    void        (*I2C2_EV_IRQHandler)(void);
    void        (*I2C2_ER_IRQHandler)(void);
    void        (*SPI1_IRQHandler)(void);
    void        (*SPI2_IRQHandler)(void);
    void        (*USART1_IRQHandler)(void);
    void        (*USART2_IRQHandler)(void);
    void        (*USART3_IRQHandler)(void);
    void        (*EXTI15_10_IRQHandler)(void);
    void        (*RTCAlarm_IRQHandler)(void);
    void        (*USBWakeUp_IRQHandler)(void);
    void        (*TIM8_BRK_IRQHandler)(void);
    void        (*TIM8_UP_IRQHandler)(void);
    void        (*TIM8_TRG_COM_IRQHandler)(void);
    void        (*TIM8_CC_IRQHandler)(void);
    void        (*ADC3_IRQHandler)(void);
    void        (*FSMC_IRQHandler)(void);
    void        (*SDIO_IRQHandler)(void);
    void        (*TIM5_IRQHandler)(void);
    void        (*SPI3_IRQHandler)(void);
    void        (*UART4_IRQHandler)(void);
    void        (*UART5_IRQHandler)(void);
    void        (*TIM6_IRQHandler)(void);
    void        (*TIM7_IRQHandler)(void);
    void        (*DMA2_Channel1_IRQHandler)(void);
    void        (*DMA2_Channel2_IRQHandler)(void);
    void        (*DMA2_Channel3_IRQHandler)(void);
    void        (*DMA2_Channel4_5_IRQHandler)(void);
    void        (*unknown6)(void);
    void        (*unknown7)(void);
    void        (*unknown8)(void);
    void        (*unknown9)(void);
    void        (*unknown10)(void);
    void        (*unknown11)(void);
    void        (*unknown12)(void);
    void        (*unknown13)(void);
    void        (*unknown14)(void);
    void        (*unknown15)(void);
    void        (*unknown16)(void);
    void        (*unknown17)(void);
    void        (*unknown18)(void);
    void        (*unknown19)(void);
    void        (*unknown20)(void);
    void        (*unknown21)(void);
    void        (*unknown22)(void);
    void        (*unknown23)(void);
    void        (*unknown24)(void);
    void        (*unknown25)(void);
    void        (*unknown26)(void);
    void        (*unknown27)(void);
    void        (*unknown28)(void);
    void        (*unknown29)(void);
    void        (*unknown30)(void);
    void        (*unknown31)(void);
    void        (*unknown32)(void);
    void        (*unknown33)(void);
    void        (*unknown34)(void);
    void        (*unknown35)(void);
    void        (*unknown36)(void);
    void        (*unknown37)(void);
    void        (*unknown38)(void);
    void        (*unknown39)(void);
    void        (*unknown40)(void);
    void        (*unknown41)(void);
    void        (*unknown42)(void);
    void        (*unknown43)(void);
    void        (*unknown44)(void);
    void        (*unknown45)(void);
    void        (*unknown46)(void);
    void        (*unknown47)(void);
    void        (*unknown48)(void);
    void        (*unknown49)(void);
//    void            (*BootRAM)(void);
};

#ifdef USE_NVIC_VECTOR
extern struct nvic _nvic_vector;
#endif  /* USE_NVIC_VECTOR */

#endif  /* _NVIC_H */

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
