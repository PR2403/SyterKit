/* SPDX-License-Identifier: GPL-2.0+ */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <types.h>

#include <mmu.h>
#include <timer.h>

#include <c906/csr.h>

#include <common.h>
#include <log.h>

#define L1_CACHE_BYTES (64) /**< Size of L1 cache line in bytes. */
#define C906_MCOR_INIT                                                                    \
	(MCOR_CACHE_SEL_BOTH | MCOR_INV | MCOR_BHT_INV | MCOR_BTB_INV)
#define C906_MHCR_INIT                                                                    \
	(MHCR_WA | MHCR_WB | MHCR_RS | MHCR_BPE | MHCR_BTB | MHCR_WBR)
#define C906_MHINT_INIT                                                                   \
	(MHINT_D_PLD | MHINT_AMR_1 | MHINT_IPLD | MHINT_IWPE | MHINT_D_DIS_16)

_Static_assert(C906_MCOR_INIT == 0x30013U, "unexpected C906 MCOR initialization");
_Static_assert(C906_MHCR_INIT == 0x17cU, "unexpected C906 MHCR initialization");
_Static_assert(C906_MHINT_INIT == 0x630cU, "unexpected C906 MHINT initialization");

/**
 * @file cache.c
 * @brief C906 cache-control implementation used by the RISC-V MMU layer.
 *
 * The C906 exposes cache controls through machine CSRs rather than the
 * standard RISC-V cache-management instructions. This file centralizes the
 * required CSR setup and line/all-cache maintenance operations, including
 * the instruction synchronization barriers needed after each operation.
 */

/**
 * @brief Insert a data synchronization barrier.
 *
 * The C906 @c fence.i sequence drains prior instruction effects before code
 * or data is observed through a newly configured cache state.
 */
void data_sync_barrier(void)
{
	asm volatile("fence.i");
}

/**
 * @brief Initialize the cache configuration.
 *
 * The writes select the C906 cache operation mode, enable hit-control
 * features, configure machine status bits, and set implementation-specific
 * prefetch hints. These values are part of the C906 programming model.
 */
void cache_init(void)
{
	csr_write(mcor, C906_MCOR_INIT);
	csr_write(mhcr, C906_MHCR_INIT);
	csr_set(mxstatus, MXSTATUS_C9XX);
	/*
	 * C906 user manual 16.1.7.5: DPLD is bit 2, AMR is bits 4:3,
	 * IPLD is bit 8, IWPE is bit 9 and D_DIS is bits 14:13.
	 */
	csr_write(mhint, C906_MHINT_INIT);
}

/**
 * @brief Enable the data cache.
 *
 * The write sets the C906 data-cache enable field in @c mhcr. Existing cache
 * configuration from ::cache_init is preserved by the hardware register.
 */
void dcache_enable(void)
{
	csr_set(mhcr, MHCR_DE);
}

/**
 * @brief Enable the instruction cache.
 *
 * The set operation enables instruction fetches from the C906 instruction
 * cache without disturbing the data-cache control bits.
 */
void icache_enable(void)
{
	csr_set(mhcr, MHCR_IE);
}

/**
 * @brief Enable the SV39 MMU with cache initialization.
 *
 * Initialization is ordered so cache mode and status are programmed before
 * either cache is enabled. The routine does not install a page table; it only
 * enables the cache controls used with an already configured MMU.
 */
void mmu_enable(void)
{
	cache_init();
	dcache_enable();
	icache_enable();
}

/* The first address is rounded down to a 64-byte line. The operation writes
 * back dirty lines and finishes with sync.i so subsequent instruction/data
 * accesses observe the completed maintenance. */
void flush_dcache_range(uint64_t start, uint64_t end)
{
	register uint64_t i asm("a0") = start & ~(L1_CACHE_BYTES - 1);
	for (; i < end; i += L1_CACHE_BYTES)
		asm volatile("dcache.cpa a0");
	asm volatile("sync.i");
}

/* The range is rounded down to a cache-line boundary and each covered line is
 * invalidated with the C906 physical-address operation before synchronization. */
void invalidate_dcache_range(uint64_t start, uint64_t end)
{
	register uint64_t i asm("a0") = start & ~(L1_CACHE_BYTES - 1);
	for (; i < end; i += L1_CACHE_BYTES)
		asm volatile("dcache.ipa a0");
	asm volatile("sync.i");
}

/**
 * @brief Write back every C906 data-cache line.
 *
 * The @c dcache.call instruction cleans all levels of the data cache. The
 * hardware instruction includes the C906 completion ordering required before
 * another agent or the instruction stream consumes the modified memory.
 */
void flush_dcache_all(void)
{
	asm volatile("dcache.call");
}

/**
 * @brief Invalidate every C906 data-cache line.
 *
 * The @c dcache.ciall operation discards all data-cache lines, so subsequent
 * reads refill from memory rather than observing stale cached contents.
 */
void invalidate_dcache_all(void)
{
	asm volatile("dcache.ciall");
}
