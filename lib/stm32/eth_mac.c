/*
 * Copyright (c) Michael Tharp <gxti@partiallystapled.com>
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include "common.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "net/tcpqueue.h"
#include "stm32/eth_mac.h"
#include "mii.h"
#include <string.h>

#define RX_BUFS 4
#define TX_BUFS 2
#define BUF_WORDS ((((MAC_BUF_SIZE - 1) | 3) + 1) / 4)

#define MFL_INIT    1
#define MFL_RUN     2
#define MFL_LINK    4

static SemaphoreHandle_t ethmac_tx_sem;
static uint32_t ethmac_queue_full_events;

static mac_desc_t rx_descs[RX_BUFS];
static mac_desc_t tx_descs[RX_BUFS];
static mac_desc_t *rx_ptr, *tx_ptr;
static uint32_t rx_bufs[RX_BUFS][BUF_WORDS];
static uint32_t tx_bufs[TX_BUFS][BUF_WORDS];
static uint8_t mac_flags;


void
smi_write(uint32_t reg, uint32_t value) {
    ETH->MACMIIDR = value;
    ETH->MACMIIAR = 0
        | BOARD_PHY_ADDRESS
        | (reg << 6)
        | ETH_MACMIIAR_CR_Div42
        | ETH_MACMIIAR_MW
        | ETH_MACMIIAR_MB
        ;
    while (ETH->MACMIIAR & ETH_MACMIIAR_MB) {}
}


uint32_t
smi_read(uint32_t reg) {
    /* FIXME: need nonblocking version of this */
    ETH->MACMIIAR = 0
        | BOARD_PHY_ADDRESS
        | (reg << 6)
        | ETH_MACMIIAR_CR_Div42
        | ETH_MACMIIAR_MB
        ;
    while (ETH->MACMIIAR & ETH_MACMIIAR_MB) {}
    return ETH->MACMIIDR;
}


uint8_t
smi_poll_link_status(void) {
    uint32_t maccr, bmsr, bmcr, lpa;
    maccr = ETH->MACCR;

    if (!(mac_flags & MFL_RUN)) {
        return 0;
    }

    (void)smi_read(MII_BMSR);
    bmsr = smi_read(MII_BMSR);
    bmcr = smi_read(MII_BMCR);
    lpa = smi_read(MII_LPA);

    if (bmcr & BMCR_ANENABLE) {
        if ((bmsr & (BMSR_LSTATUS | BMSR_RFAULT | BMSR_ANEGCOMPLETE))
                != (BMSR_LSTATUS | BMSR_ANEGCOMPLETE)) {
            mac_flags &= ~MFL_LINK;
            return 0;
        }
        if (lpa & (LPA_100HALF | LPA_100FULL | LPA_100BASE4)) {
            maccr |= ETH_MACCR_FES;
        } else {
            maccr &= ~ETH_MACCR_FES;
        }
        if (lpa & (LPA_10FULL | LPA_100FULL)) {
            maccr |= ETH_MACCR_DM;
        } else {
            maccr &= ~ETH_MACCR_DM;
        }
    } else {
        if (!(bmsr & BMSR_LSTATUS)) {
            mac_flags &= ~MFL_LINK;
            return 0;
        }
        if (bmcr & BMCR_SPEED100) {
            maccr |= ETH_MACCR_FES;
        } else {
            maccr &= ~ETH_MACCR_FES;
        }
        if (bmcr & BMCR_FULLDPLX) {
            maccr |= ETH_MACCR_DM;
        } else {
            maccr &= ~ETH_MACCR_DM;
        }
    }
    ETH->MACCR = maccr;
    mac_flags |= MFL_LINK;
    return 1;
}


void
smi_describe_link(char *buf) {
    uint32_t bmcr, bmsr, lpa;
    if (!(mac_flags & MFL_RUN)) {
        strcpy(buf, "Down");
        return;
    }
    bmsr = smi_read(MII_BMSR);
    bmcr = smi_read(MII_BMCR);
    lpa = smi_read(MII_LPA);
    if (bmcr & BMCR_ANENABLE) {
        if ((bmsr & (BMSR_LSTATUS | BMSR_RFAULT | BMSR_ANEGCOMPLETE))
                != (BMSR_LSTATUS | BMSR_ANEGCOMPLETE)) {
            strcpy(buf, "Down");
        } else {
            strcpy(buf, "Auto "); if (lpa & (LPA_100HALF | LPA_100FULL | LPA_100BASE4)) {
                strcat(buf, "100M ");
            } else {
                strcat(buf, "10M ");
            }
            if (lpa & (LPA_10FULL | LPA_100FULL)) {
                strcat(buf, "Full");
            } else {
                strcat(buf, "Half");
            }
        }
    } else {
        if (!(bmsr & BMSR_LSTATUS)) {
            strcpy(buf, "Down");
        } else {
            strcpy(buf, "Manual ");
            if (bmcr & BMCR_SPEED100) {
                strcat(buf, "100M ");
            } else {
                strcat(buf, "10M ");
            }
            if (bmcr & BMCR_FULLDPLX) {
                strcat(buf, "Full");
            } else {
                strcat(buf, "Half");
            }
        }
    }
    ASSERT(strlen(buf) < SMI_DESCRIBE_SIZE);
}


void
mac_set_hwaddr(const uint8_t *hwaddr) {
    if (!(mac_flags & MFL_RUN)) {
        return;
    }
    ETH->MACA0HR = ((uint32_t)hwaddr[5] <<  8)
                |  ((uint32_t)hwaddr[4] <<  0);
    ETH->MACA0LR = ((uint32_t)hwaddr[3] << 24)
                |  ((uint32_t)hwaddr[2] << 16)
                |  ((uint32_t)hwaddr[1] <<  8)
                |  ((uint32_t)hwaddr[0] <<  0);
}


void
mac_start(void) {
    int i;
    if (mac_flags & MFL_RUN) {
        HALT();
    }
    if (!(mac_flags & MFL_INIT)) {
        ASSERT((ethmac_tx_sem = xSemaphoreCreateBinary()));
        mac_flags |= MFL_INIT;
    }
    RCC->AHBRSTR |=  RCC_AHBRSTR_ETHMACRST;
    RCC->AHBRSTR &= ~RCC_AHBRSTR_ETHMACRST;
    RCC->AHBENR |= RCC_AHBENR_ETHMACEN
        | RCC_AHBENR_ETHMACTXEN
        | RCC_AHBENR_ETHMACRXEN;

    /* Configure DMA */
    for (i = 0; i < RX_BUFS; i++) {
        rx_descs[i].des0 = STM32_RDES0_OWN;
        rx_descs[i].des1 = STM32_RDES1_RCH | MAC_BUF_SIZE;
        rx_descs[i].des_buf = (uint8_t*)rx_bufs[i];
        rx_descs[i].des_next = &rx_descs[(i+1) % RX_BUFS];
    }
    for (i = 0; i < TX_BUFS; i++) {
        tx_descs[i].des0 = STM32_TDES0_TCH;
        tx_descs[i].des1 = 0;
        tx_descs[i].des_buf = (uint8_t*)tx_bufs[i];
        tx_descs[i].des_next = &tx_descs[(i+1) % TX_BUFS];
    }
    rx_ptr = &rx_descs[0];
    tx_ptr = &tx_descs[0];
    ETH->DMABMR |= ETH_DMABMR_SR;
    while (ETH->DMABMR & ETH_DMABMR_SR) {}
    ETH->DMARDLAR = (uint32_t)rx_ptr;
    ETH->DMATDLAR = (uint32_t)tx_ptr;

    /* MAC configuration */
    ETH->MACFFR = ETH_MACFFR_PAM;
    ETH->MACFCR = 0;
    ETH->MACVLANTR = 0;

    ETH->MACA0HR = 0x0000FFFF;
    ETH->MACA0LR = 0xFFFFFFFF;
    ETH->MACA1HR = 0x0000FFFF;
    ETH->MACA1LR = 0xFFFFFFFF;
    ETH->MACA2HR = 0x0000FFFF;
    ETH->MACA2LR = 0xFFFFFFFF;
    ETH->MACA3HR = 0x0000FFFF;
    ETH->MACA3LR = 0xFFFFFFFF;
    ETH->MACHTHR = 0;
    ETH->MACHTLR = 0;

    ETH->MACCR = 0
#if STM32_IP_CHECKSUM_OFFLOAD
        | ETH_MACCR_IPCO
#endif
        | ETH_MACCR_RE
        | ETH_MACCR_TE
        | ETH_MACCR_FES
        | ETH_MACCR_DM
        ;

    /* Reset PHY */
    smi_write(MII_BMCR, BMCR_RESET);
    while (smi_read(MII_BMCR) & BMCR_RESET) {}

    /* Enable DMA */
    ETH->DMASR = ETH->DMASR;
    ETH->DMAIER = 0
        | ETH_DMAIER_NISE
        | ETH_DMAIER_RIE
        | ETH_DMAIER_TIE
        ;
    ETH->DMABMR = 0
        | ETH_DMABMR_AAB
        | ETH_DMABMR_RDP_1Beat
        | ETH_DMABMR_PBL_1Beat
        ;
    ETH->DMAOMR = ETH_DMAOMR_FTF;
    while(ETH->DMAOMR & ETH_DMAOMR_FTF) {}
    ETH->DMAOMR = 0
        | ETH_DMAOMR_DTCEFD
        | ETH_DMAOMR_RSF
        | ETH_DMAOMR_TSF
        | ETH_DMAOMR_ST
        | ETH_DMAOMR_SR
        ;

    mac_flags |= MFL_RUN;
    NVIC_SetPriority(ETH_IRQn, IRQ_PRIO_ETH);
    NVIC_EnableIRQ(ETH_IRQn);
}


void
mac_stop(void) {
    if (!(mac_flags & MFL_RUN)) {
        return;
    }
    DISABLE_IRQ();
    NVIC_DisableIRQ(ETH_IRQn);
    smi_write(MII_BMCR, smi_read(MII_BMCR) | BMCR_PDOWN);
    RCC->AHBENR &= ~RCC_AHBENR_ETHMACEN
        & ~RCC_AHBENR_ETHMACTXEN
        & ~RCC_AHBENR_ETHMACRXEN;
    mac_flags &= ~(MFL_RUN | MFL_LINK);
    ENABLE_IRQ();
}


void
ETH_IRQHandler(void) {
    uint32_t dmasr;
    BaseType_t wakeup = 0;
    dmasr = ETH->DMASR;
    ETH->DMASR = dmasr;
    if (dmasr & ETH_DMASR_RS) {
        void *qmsg = NULL;
        if (!xQueueSendFromISR(tcpip_queue, &qmsg, &wakeup)) {
            ethmac_queue_full_events++;
        }
    }
    if (dmasr & ETH_DMASR_TS) {
        xSemaphoreGiveFromISR(ethmac_tx_sem, &wakeup);
    }
    portEND_SWITCHING_ISR(wakeup);
}


static mac_desc_t *
mac_get_tx_descriptor_once(void) {
    mac_desc_t *tdes;
    tdes = tx_ptr;
    if (tdes->des0 & (STM32_TDES0_OWN | STM32_TDES0_LOCKED)) {
        return NULL;
    }
    tdes->des0 |= STM32_TDES0_LOCKED;
    tdes->size = MAC_BUF_SIZE;
    tdes->offset = 0;
    tx_ptr = tdes->des_next;
    return tdes;
}


mac_desc_t *
mac_get_tx_descriptor(uint32_t timeout) {
    mac_desc_t *tdes;
    if (timeout) {
        timeout += xTaskGetTickCount();
    }
    while (1) {
        if (!(mac_flags & MFL_LINK) || (timeout && xTaskGetTickCount() > timeout)) {
            return NULL;
        }
        if ((tdes = mac_get_tx_descriptor_once()) != NULL) {
            return tdes;
        }
        xSemaphoreTake(ethmac_tx_sem,
                timeout ? (timeout - xTaskGetTickCount()) : portMAX_DELAY);
    }
}


uint16_t
mac_write_tx_descriptor(mac_desc_t *tdes, const uint8_t *buf, uint16_t size) {
    if (size > tdes->size - tdes->offset) {
        size = tdes->size - tdes->offset;
    }
    if (size > 0) {
        memcpy(tdes->des_buf + tdes->offset, buf, size);
        tdes->offset = tdes->offset + size;
    }
    return size;
}


void
mac_release_tx_descriptor(mac_desc_t *tdes) {
    DISABLE_IRQ();
    tdes->des1 = tdes->offset;
    tdes->des0 = 0
        | STM32_TDES0_CIC(STM32_IP_CHECKSUM_OFFLOAD)
        | STM32_TDES0_IC
        | STM32_TDES0_LS
        | STM32_TDES0_FS
        | STM32_TDES0_TCH
        | STM32_TDES0_OWN
        ;
    if ((ETH->DMASR & ETH_DMASR_TPS) == ETH_DMASR_TPS_Suspended) {
        ETH->DMASR   = ETH_DMASR_TBUS;
        ETH->DMATPDR = ETH_DMASR_TBUS;
    }
    ENABLE_IRQ();
}


mac_desc_t *
mac_get_rx_descriptor(void) {
    if (!(mac_flags & MFL_RUN)) {
        return NULL;
    }
    while (1) {
        uint32_t des0 = rx_ptr->des0;
        if (des0 & STM32_RDES0_OWN) {
            break;
        }
        if (!(des0 & (STM32_RDES0_AFM | STM32_RDES0_ES)) /* Filter match, no error */
#if STM32_IP_CHECKSUM_OFFLOAD
                && ( !(des0 & STM32_RDES0_FT) /* Not ethernet */
                    || !(des0 & (STM32_RDES0_IPHCE | STM32_RDES0_PCE)) ) /* FCS ok */
#endif
                && (des0 & STM32_RDES0_FS) && (des0 & STM32_RDES0_LS)) {
            /* Valid frame */
            mac_desc_t *ret = rx_ptr;
            rx_ptr = rx_ptr->des_next;
            ret->size = ((des0 & STM32_RDES0_FL_MASK) >> 16) - 4;
            ret->offset = 0;
            return ret;
        }
        /* Invalid frame, release it now */
        rx_ptr->des0 = STM32_RDES0_OWN;
        rx_ptr = rx_ptr->des_next;
    }
    return NULL;
}


uint16_t
mac_read_rx_descriptor(mac_desc_t *rdes, uint8_t *buf, uint16_t size) {
    if (size > rdes->size - rdes->offset) {
        size = rdes->size - rdes->offset;
    }
    if (size > 0) {
        memcpy(buf, rdes->des_buf + rdes->offset, size);
        rdes->offset += size;
    }
    return size;
}


void
mac_release_rx_descriptor(mac_desc_t *rdes) {
    rdes->des0 = STM32_RDES0_OWN;
    if ((ETH->DMASR & ETH_DMASR_RPS) == ETH_DMASR_RPS_Suspended) {
        ETH->DMASR   = ETH_DMASR_RBUS;
        ETH->DMARPDR = ETH_DMASR_RBUS;
    }
}
