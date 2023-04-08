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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <libgen.h>
#include <assert.h>
#include <errno.h>
#include <err.h>

#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/debug.h>
#include <sys/vmm.h>
#include <sys/vmm_data.h>
#include <sys/vmm_dev.h>
#include <vmmapi.h>

#include "in_guest.h"

int
main(int argc, char *argv[])
{
	const char *test_suite_name = basename(argv[0]);
	struct vmctx *ctx = NULL;
	int err;

	ctx = test_initialize(test_suite_name);

	err = test_setup_vcpu(ctx, 0, MEM_LOC_PAYLOAD, MEM_LOC_STACK);
	if (err != 0) {
		test_fail_errno(err, "Could not initialize vcpu0");
	}

	const int vmfd = vm_get_device_fd(ctx);

	/* Read timing data to get baseline guest timing values */
	struct vdi_timing_info_v1 timing_info;
	struct vm_data_xfer xfer = {
		.vdx_class = VDC_VMM_TIMING,
		.vdx_version = 1,
		.vdx_len = sizeof (struct vdi_timing_info_v1),
		.vdx_data = &timing_info,
	};
	if (ioctl(vmfd, VM_DATA_READ, &xfer) != 0) {
		errx(EXIT_FAILURE, "VMM_DATA_READ of timing info failed");
	}

	/* Change the guest TSC to a much larger value */
	uint64_t expect_tsc = 500000000000;
	timing_info.vt_guest_tsc = expect_tsc;
	if (ioctl(vmfd, VM_DATA_WRITE, &xfer) != 0) {
		int error;
		error = errno;
		if (error == EPERM) {
			warn("VMM_DATA_WRITE got EPERM: is "
			    "vmm_allow_state_writes set?");
		}
		errx(EXIT_FAILURE, "VMM_DATA_WRITE of timing info failed");
	}

	struct vm_entry ventry = { 0 };
	struct vm_exit vexit = { 0 };

	do {
		const enum vm_exit_kind kind =
		    test_run_vcpu(ctx, 0, &ventry, &vexit);
		switch (kind) {
		case VEK_REENTR:
			break;
		case VEK_TEST_PASS:
			test_pass();
			break;
		case VEK_TEST_FAIL:
			test_fail_vmexit(&vexit);
			break;
		case VEK_UNHANDLED:
		default:
			test_fail_vmexit(&vexit);
			break;
		}
	} while (true);
}
