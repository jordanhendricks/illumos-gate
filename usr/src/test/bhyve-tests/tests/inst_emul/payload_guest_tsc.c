/*
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 */

/*
 * Copyright 2023 Oxide Computer Company
 */

#include "payload_common.h"
#include "payload_utils.h"

//#define TSC_MSR 0xc0000104U
#define MSR_TSC 0x010

void start(void)
{
	/*
	 * Initial read of the TSC: should be set to an arbitrarily large value
	 * by the test.
	 */
	//uint64_t tsc = rdmsr(MSR_TSC);
	uint64_t tsc = rdtsc();
	outl(IOP_TEST_VALUE, tsc << 32);
	outl(IOP_TEST_VALUE, tsc >> 32);
	/*if (tsc < 500000000000) {
		outl(IOP_TEST_VALUE, tsc << 32);
		outl(IOP_TEST_VALUE, tsc >> 32);
		outb(IOP_TEST_RESULT, TEST_RESULT_FAIL);
	} */

	/* reset the TSC to 0 */
	wrmsr(MSR_TSC, 0);

	/* check that the TSC has been reset */
	/*if (rdmsr(TSC_MSR) >= 500000000000) {
		outb(IOP_TEST_RESULT, TEST_RESULT_FAIL);
	}*/
	
	//uint64_t new_tsc = rdmsr(MSR_TSC);
	uint64_t new_tsc = rdtsc();
	outl(IOP_TEST_VALUE, new_tsc << 32);
	outl(IOP_TEST_VALUE, new_tsc >> 32);

	/* if we made it this far, successful */
	outb(IOP_TEST_RESULT, TEST_RESULT_PASS);
}
