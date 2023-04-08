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

#include <sys/asm_linkage.h>
#include "payload_common.h"

ENTRY(start)
	/* check that the TSC is >= to expected value */
	rdtsc
	shld $32, %rdx, %rax
	movq $500000000000, %rdi
	cmpq %rdi, %rax
	movw    $IOP_TEST_RESULT, %dx
	movb    $TEST_RESULT_PASS, %al
	outb    (%dx)

	jnge		fail
	movw    $IOP_TEST_RESULT, %dx
	movb    $TEST_RESULT_PASS, %al
	outb    (%dx)
	hlt

fail:
	movw    $IOP_TEST_RESULT, %dx
	movb    $TEST_RESULT_FAIL, %al
	outb    (%dx)
	hlt
SET_SIZE(start)
