/*
 * Copyright (c) Michael Tharp <gxti@partiallystapled.com>
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <task.h>
#include <stm32/dma.h>


const dma_ch_t dma_streams[DMA_STREAMS] = {
    { DMA1_Channel1, DMA1_Channel1_IRQn,   0,  &DMA1->IFCR, 0  },
    { DMA1_Channel2, DMA1_Channel2_IRQn,   1,  &DMA1->IFCR, 4  },
    { DMA1_Channel3, DMA1_Channel3_IRQn,   2,  &DMA1->IFCR, 8  },
    { DMA1_Channel4, DMA1_Channel4_IRQn,   3,  &DMA1->IFCR, 12 },
    { DMA1_Channel5, DMA1_Channel5_IRQn,   4,  &DMA1->IFCR, 16 },
    { DMA1_Channel6, DMA1_Channel6_IRQn,   5,  &DMA1->IFCR, 20 },
    { DMA1_Channel7, DMA1_Channel7_IRQn,   6,  &DMA1->IFCR, 24 },

    { DMA2_Channel1, DMA2_Channel1_IRQn,   7,  &DMA2->IFCR, 0  },
    { DMA2_Channel2, DMA2_Channel2_IRQn,   8,  &DMA2->IFCR, 4  },
    { DMA2_Channel3, DMA2_Channel3_IRQn,   9,  &DMA2->IFCR, 8  },
#ifdef STM32F10X_CL
    { DMA2_Channel4, DMA2_Channel4_IRQn,   10, &DMA2->IFCR, 12 },
    { DMA2_Channel5, DMA2_Channel5_IRQn,   11, &DMA2->IFCR, 16 },
#else
    { DMA2_Channel4, DMA2_Channel4_5_IRQn, 10, &DMA2->IFCR, 12 },
    { DMA2_Channel5, DMA2_Channel4_5_IRQn, 11, &DMA2->IFCR, 16 },
#endif
};

#ifndef STM32F10X_CL
uint8_t dma2_ch4 = 0, dma2_ch5 = 0;
#endif

static dma_isr_t isr_funcs[DMA_STREAMS];
static void *isr_params[DMA_STREAMS];


void
dma_allocate(const dma_ch_t *ch, uint32_t irq_priority, dma_isr_t func, void *param)
{
    isr_funcs[ch->index] = func;
    isr_params[ch->index] = param;
    dma_disable(ch);
    ch->ch->CCR = 0;
    NVIC_SetPriority(ch->vector, irq_priority);
    NVIC_EnableIRQ(ch->vector);
#ifndef STM32F10X_CL
    if (ch->ch == DMA2_Channel4) dma2_ch4 = 1;
    else if (ch->ch == DMA2_Channel5) dma2_ch5 = 1;
#endif
}


void
dma_release(const dma_ch_t *ch)
{
#ifdef STM32F10X_CL
    NVIC_DisableIRQ(ch->vector);
#else
    if (ch->vector != DMA2_Channel4_5_IRQn) {
        NVIC_DisableIRQ(ch->vector);
    } else {
        if (ch->ch == DMA2_Channel4) dma2_ch4 = 0;
        else if (ch->ch == DMA2_Channel5) dma2_ch5 = 0;

        if (!dma2_ch4 && !dma2_ch5)
            NVIC_DisableIRQ(ch->vector);
    }
#endif
    dma_disable(ch);
}


static inline void
dma_service_irq(DMA_TypeDef *dma, const dma_ch_t *ch)
{
    uint32_t flags;

    flags = (dma->ISR >> ch->ishift) & 0xF;
    dma->IFCR = 0xF << ch->ishift;
    if (isr_funcs[ch->index])
        isr_funcs[ch->index](isr_params[ch->index], flags);
}


void
DMA1_Channel1_IRQHandler(void)
{
    dma_service_irq(DMA1, &dma_streams[0]);
}


void
DMA1_Channel2_IRQHandler(void)
{
    dma_service_irq(DMA1, &dma_streams[1]);
}


void
DMA1_Channel3_IRQHandler(void)
{
    dma_service_irq(DMA1, &dma_streams[2]);
}


void
DMA1_Channel4_IRQHandler(void)
{
    dma_service_irq(DMA1, &dma_streams[3]);
}


void
DMA1_Channel5_IRQHandler(void)
{
    dma_service_irq(DMA1, &dma_streams[4]);
}


void
DMA1_Channel6_IRQHandler(void)
{
    dma_service_irq(DMA1, &dma_streams[5]);
}


void
DMA1_Channel7_IRQHandler(void)
{
    dma_service_irq(DMA1, &dma_streams[6]);
}


void
DMA2_Channel1_IRQHandler(void)
{
    dma_service_irq(DMA2, &dma_streams[7]);
}


void
DMA2_Channel2_IRQHandler(void)
{
    dma_service_irq(DMA2, &dma_streams[8]);
}


void
DMA2_Channel3_IRQHandler(void)
{
    dma_service_irq(DMA2, &dma_streams[9]);
}


void
DMA2_Channel4_5_IRQHandler(void)
{
    if ((DMA2->ISR >> dma_streams[10].ishift) & 1)
        dma_service_irq(DMA2, &dma_streams[10]);
    if ((DMA2->ISR >> dma_streams[11].ishift) & 1)
        dma_service_irq(DMA2, &dma_streams[11]);
}


void
DMA2_Channel4_IRQHandler(void)
{
    dma_service_irq(DMA2, &dma_streams[10]);
}


void
DMA2_Channel5_IRQHandler(void)
{
    dma_service_irq(DMA2, &dma_streams[11]);
}

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
