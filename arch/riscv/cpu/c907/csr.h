/* SPDX-License-Identifier: GPL-2.0+ */

/**
 * @file csr.h
 * @brief Xuantie C907 extension CSR fields.
 */
#ifndef __RISCV_C907_CSR_H__
#define __RISCV_C907_CSR_H__

#include <csr.h>

/** @brief C907 MXSTATUS bit definitions. */
#define MXSTATUS_MM (1U << 15)
#define MXSTATUS_UCME (1U << 16)
#define MXSTATUS_CLINTEE (1U << 17)
#define MXSTATUS_MAEE (1U << 21)
#define MXSTATUS_THEADISAEE (1U << 22)

#define MXSTATUS_C9XX                                                                     \
	(MXSTATUS_MM | MXSTATUS_UCME | MXSTATUS_CLINTEE | MXSTATUS_MAEE | MXSTATUS_THEADISAEE)

/** @brief C907 MHCR bit definitions. */
#define MHCR_IE (1U << 0)
#define MHCR_DE (1U << 1)
#define MHCR_WA (1U << 2)
#define MHCR_WB (1U << 3)
#define MHCR_RS (1U << 4)
#define MHCR_BPE (1U << 5)
#define MHCR_IBPE (1U << 7)
#define MHCR_WBR (1U << 8)
#define MHCR_L0BTB (1U << 12)

/** @brief C907 MCOR bit definitions. */
#define MCOR_CACHE_SEL_ICACHE (0b01U << 0)
#define MCOR_CACHE_SEL_DCACHE (0b10U << 0)
#define MCOR_CACHE_SEL_BOTH (0b11U << 0)
#define MCOR_INV (1U << 4)
#define MCOR_CLR (1U << 5)
#define MCOR_BHT_INV (1U << 16)
#define MCOR_TLB_INV (1U << 19)

/** @brief C907 MHINT bit definitions. */
#define MHINT_D_PLD (1U << 2)
#define MHINT_AMR_0 (0b00U << 3)
#define MHINT_AMR_1 (0b01U << 3)
#define MHINT_AMR_2 (0b10U << 3)
#define MHINT_AMR_3 (0b11U << 3)
#define MHINT_IPLD (1U << 8)
#define MHINT_IWPE (1U << 10)
#define MHINT_D_DIS_2 (0b00U << 13)
#define MHINT_D_DIS_4 (0b01U << 13)
#define MHINT_D_DIS_8 (0b10U << 13)
#define MHINT_D_DIS_16 (0b11U << 13)
#define MHINT_ECC (1U << 20)

#endif /* __RISCV_C907_CSR_H__ */
