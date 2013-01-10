/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "common.h"
#include <iostream>

using namespace nanojit;
using namespace std;

#if defined(AVMPLUS_ARM) && defined(VMCFG_NEON)
float4_t vsqrtq_f32( float4_t q_x )
{
    const float4_t q_step_0 = vrsqrteq_f32( q_x );
    // step
    const float4_t q_step_parm0 = vmulq_f32( q_x, q_step_0 );
    const float4_t q_step_result0 = vrsqrtsq_f32( q_step_parm0, q_step_0 );
    // step
    const float4_t q_step_1 = vmulq_f32( q_step_0, q_step_result0 );
    const float4_t q_step_parm1 = vmulq_f32( q_x, q_step_1 );
    const float4_t q_step_result1 = vrsqrtsq_f32( q_step_parm1, q_step_1 );
    // take the res
    const float4_t q_step_2 = vmulq_f32( q_step_1, q_step_result1 );
    // mul by x to get sqrt, not rsqrt
    return vmulq_f32( q_x, q_step_2 );
}
#endif

/* Allocator SPI implementation. */

void*
nanojit::Allocator::allocChunk(size_t nbytes, bool /*fallible*/)
{
    void *p = malloc(nbytes);
    if (!p)
        exit(1);
    return p;
}

void
nanojit::Allocator::freeChunk(void *p) {
    free(p);
}

void
nanojit::Allocator::postReset() {
}

/* LIR SPI implementation */

int
nanojit::StackFilter::getTop(LIns*)
{
    return 0;
}

#if defined NJ_VERBOSE
void
nanojit::LInsPrinter::formatGuard(InsBuf *buf, LIns *ins)
{
    RefBuf b1, b2;
    LasmSideExit *x = (LasmSideExit *)ins->record()->exit;
    VMPI_snprintf(buf->buf, buf->len,
            "%s: %s %s -> line=%ld (GuardID=%03d)",
            formatRef(&b1, ins),
            lirNames[ins->opcode()],
            ins->oprnd1() ? formatRef(&b2, ins->oprnd1()) : "",
            (long)x->line,
            ins->record()->profGuardID);
}

void
nanojit::LInsPrinter::formatGuardXov(InsBuf *buf, LIns *ins)
{
    RefBuf b1, b2, b3;
    LasmSideExit *x = (LasmSideExit *)ins->record()->exit;
    VMPI_snprintf(buf->buf, buf->len,
            "%s = %s %s, %s -> line=%ld (GuardID=%03d)",
            formatRef(&b1, ins),
            lirNames[ins->opcode()],
            formatRef(&b2, ins->oprnd1()),
            formatRef(&b3, ins->oprnd2()),
            (long)x->line,
            ins->record()->profGuardID);
}

const char*
nanojit::LInsPrinter::accNames[] = {
    "o",    // (1 << 0) == ACCSET_OTHER
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",   //  1..10 (unused)
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",   // 11..20 (unused)
    "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",   // 21..30 (unused)
    "?"                                                 //     31 (unused)
};
#endif


#ifdef DEBUG
void ValidateWriter::checkAccSet(LOpcode op, LIns* base, int32_t disp, AccSet accSet)
{
    (void)op;
    (void)base;
    (void)disp;
    NanoAssert(accSet == ACCSET_OTHER);
}
#endif

