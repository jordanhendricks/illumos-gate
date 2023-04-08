#include <sys/asm_linkage.h>
#include "payload_common.h"

ENTRY(start)
	// EDX:EAX
	rdtsc
	movl %edx, %esi
	// low
	mov    $IOP_TEST_VALUE, %dx
	// already in eax
	outb    (%dx)
	// high
	movl %esi, %eax
	outb    (%dx)
	hlt
SET_SIZE(start)
