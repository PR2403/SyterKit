/* SPDX-License-Identifier: GPL-2.0+ */

/**
 * @file cache.c
 * @brief C907 L1 cache control.
 *
 * C907 uses the T-Head C9xx cache CSRs and address-based maintenance
 * instructions.  Cache maintenance is done purely with the T-Head cache
 * instructions (dcache.call/iall/cpa/ipa) plus a sync.is barrier.  The
 * memory-mapped L2 controller is deliberately not used: it is not
 * wired up on this SoC, and polling its status register spins
 * forever.
 */

#include <stddef.h>
#include <stdint.h>
#include <types.h>

#include <cache.h>
#include <c907/csr.h>

#define L1_CACHE_BYTES (64U)
#define C907_MCOR_INIT (MCOR_CACHE_SEL_BOTH | MCOR_INV | MCOR_BHT_INV)
#define C907_MHCR_INIT                                                                     \
	(MHCR_WA | MHCR_WB | MHCR_RS | MHCR_BPE | MHCR_IBPE | MHCR_WBR | MHCR_L0BTB)
#define C907_MHINT_INIT                                                                    \
	(MHINT_D_PLD | MHINT_AMR_1 | MHINT_IPLD | MHINT_IWPE | MHINT_D_DIS_16 | MHINT_ECC)

_Static_assert(C907_MCOR_INIT == 0x10013U, "unexpected C907 MCOR initialization");
_Static_assert(C907_MHCR_INIT == 0x11bcU, "unexpected C907 MHCR initialization");
_Static_assert(C907_MHINT_INIT == 0x10650cU, "unexpected C907 MHINT initialization");

/**
 * @brief Insert the C907 instruction/data synchronization barrier.
 */
void data_sync_barrier(void)
{
	asm volatile("sync.is" ::: "memory");
}

/**
 * @brief Initialize C907 cache controls and prefetch settings.
 */
void cache_init(void)
{
	/* Invalidate both L1 caches and the branch history table before enabling. */
	csr_write(mcor, C907_MCOR_INIT);
	csr_write(mhcr, C907_MHCR_INIT);
	csr_set(mxstatus, MXSTATUS_C9XX);
	/*
	 * C907 user manual 19.1.9.6: DPLD is bit 2, AMR is bits 4:3,
	 * IPLD is bit 8, IWPE is bit 10, D_DIS is bits 14:13 and ECC is
	 * bit 20. Keep ECC enabled as the previous initialization did; unlike
	 * E907, bit 20 does not control accurate exception reporting on C907.
	 */
	csr_write(mhint, C907_MHINT_INIT);
}

/**
 * @brief Enable the C907 data cache.
 */
void dcache_enable(void)
{
	csr_set(mhcr, MHCR_DE);
}

/**
 * @brief Disable the C907 data cache.
 */
void dcache_disable(void)
{
	csr_clear(mhcr, MHCR_DE);
}

/**
 * @brief Enable the C907 instruction cache.
 */
void icache_enable(void)
{
	csr_set(mhcr, MHCR_IE);
}

/**
 * @brief Initialize the C907 cache interface without enabling an MMU.
 */
void mmu_enable(void)
{
	cache_init();
	dcache_enable();
	icache_enable();
}

/* Clean a C907 L1 cache range. The range is inclusive start / exclusive end.
 * Doxygen documentation lives on the declaration in arch/riscv/include/cache.h. */
void flush_dcache_range(uint64_t start, uint64_t end)
{
	uintptr_t address = (uintptr_t)start & ~(L1_CACHE_BYTES - 1U);
	uintptr_t limit = (uintptr_t)end;

	for (; address < limit; address += L1_CACHE_BYTES)
		asm volatile("dcache.cpa %0" : : "r"(address) : "memory");
	data_sync_barrier();
}

/* Invalidate a C907 L1 cache range. The range is inclusive start / exclusive end.
 * Doxygen documentation lives on the declaration in arch/riscv/include/cache.h. */
void invalidate_dcache_range(uint64_t start, uint64_t end)
{
	uintptr_t address = (uintptr_t)start & ~(L1_CACHE_BYTES - 1U);
	uintptr_t limit = (uintptr_t)end;

	for (; address < limit; address += L1_CACHE_BYTES)
		asm volatile("dcache.ipa %0" : : "r"(address) : "memory");
	data_sync_barrier();
}

/**
 * @brief Clean all C907 L1 data-cache lines.
 */
void flush_dcache_all(void)
{
	asm volatile("dcache.call" ::: "memory");
	data_sync_barrier();
}

/**
 * @brief Invalidate all C907 L1 data-cache lines.
 */
void invalidate_dcache_all(void)
{
	asm volatile("dcache.iall" ::: "memory");
	data_sync_barrier();
}
