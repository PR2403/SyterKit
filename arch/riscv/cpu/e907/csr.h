/* SPDX-License-Identifier: GPL-2.0+ */

/**
 * @file csr.h
 * @brief Xuantie E907 extension CSR fields.
 */
#ifndef __RISCV_E907_CSR_H__
#define __RISCV_E907_CSR_H__

#include <csr.h>

/** @brief E907 MXSTATUS bit definitions. */
#define MXSTATUS_MM (1U << 15)
#define MXSTATUS_THEADISAEE (1U << 22)

/** @brief E907 MHCR bit definitions. */
#define MHCR_IE (1U << 0)
#define MHCR_DE (1U << 1)
#define MHCR_WA (1U << 2)
#define MHCR_WB (1U << 3)
#define MHCR_RS (1U << 4)
#define MHCR_BPE (1U << 5)
#define MHCR_BTE (1U << 12)

/** @brief E907 MHINT bit definitions. */
#define MHINT_D_PLD (1U << 2)
#define MHINT_AMR_0 (0b00U << 3)
#define MHINT_AMR_1 (0b01U << 3)
#define MHINT_AMR_2 (0b10U << 3)
#define MHINT_AMR_3 (0b11U << 3)
#define MHINT_IWPE (1U << 10)
#define MHINT_PREF_N_2 (0b00U << 13)
#define MHINT_PREF_N_4 (0b01U << 13)
#define MHINT_PREF_N_8 (0b10U << 13)
#define MHINT_PREF_N_16 (0b11U << 13)
#define MHINT_AEE (1U << 20)
#define MHINT_PCFIFO_FREEZE (1U << 24)

#endif /* __RISCV_E907_CSR_H__ */
