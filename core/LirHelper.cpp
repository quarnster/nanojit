/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is [Open Source Virtual Machine.].
 *
 * The Initial Developer of the Original Code is
 * Adobe System Incorporated.
 * Portions created by the Initial Developer are Copyright (C) 2004-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Adobe AS3 Team
 *   leon.sha@sun.com
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "avmplus.h"

#ifdef VMCFG_NANOJIT

#include "LirHelper.h"

#ifdef VMCFG_SHARK
#include <dlfcn.h> // for dlopen and dlsym
#endif

namespace avmplus
{

#ifdef NJ_VERBOSE
void AvmLogControl::printf( const char* format, ... )
{
    AvmAssert(core!=NULL);

    va_list vargs;
    va_start(vargs, format);

    char str[1024];
    VMPI_vsnprintf(str, sizeof(str), format, vargs);
    va_end(vargs);

    core->console << str;
}
#endif


} // end namespace avmplus

//
// The following methods implement Service Provider API's defined in
// nanojit, which must be implemented by the nanojit embedder.
//
namespace nanojit
{
    int StackFilter::getTop(LIns* /*br*/) {
        AvmAssert(false);
        return 0;
    }

    #ifdef NJ_VERBOSE
    void LInsPrinter::formatGuard(InsBuf*, LIns*) {
        AvmAssert(false);
    }
    void LInsPrinter::formatGuardXov(InsBuf*, LIns*) {
        AvmAssert(false);
    }

    const char* LInsPrinter::accNames[] = {
        "v",    // (1 << 0) == ACCSET_VARS
        "t",    // (1 << 1) == ACCSET_TAGS
        "o",    // (1 << 2) == ACCSET_OTHER
                  "?", "?", "?", "?", "?", "?", "?", "?",   //  3..10 (unused)
        "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",   // 11..20 (unused)
        "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",   // 21..30 (unused)
        "?"                                                 //     31 (unused)
    };
    #endif

    void* Allocator::allocChunk(size_t size, bool /* fallible */) {
        return mmfx_alloc(size);
    }

    void Allocator::freeChunk(void* p) {
        mmfx_free(p);
    }

    void Allocator::postReset() {
    }

#ifdef VMCFG_SHARK

#if defined VMCFG_64BIT
    static const char* JIT_SO_PATH = "/tmp/jit64.so";
#elif defined VMCFG_32BIT
    static const char* JIT_SO_PATH = "/tmp/jit32.so";
#else
#   error "unsupported system"
#endif

    // TODO: lock access to these statics.
    static void *sHandle = NULL;
    static char *sStart = NULL;
    static char *sEnd = NULL;
    static char *sCur = NULL;
    size_t overage = 0;

    void* CodeAlloc::allocCodeChunk(size_t nbytes)
    {
        if (!sHandle) {
            sHandle = dlopen(JIT_SO_PATH, RTLD_NOW);
            if (sHandle) {
                // Get the bounds of the code area by looking for known symbols,
                // then make that area writable.
                sStart = (char*) dlsym(sHandle, "_jitStart");
                sEnd = (char*) dlsym(sHandle, "_jitEnd");
                if (!mprotect(sStart, sEnd - sStart,
                              PROT_READ | PROT_WRITE | PROT_EXEC))
                    sCur = sStart;
            } else {
                const char *err = dlerror();
                fprintf(stderr, "dlopen error: %s\n", err);
                // will fall through to AVMPI_allocateCodeMemory below.
            }
        }

        if (sCur) {
            char *next = sCur + nbytes;
            if (next < sEnd) {
                char *alloc = sCur;
                sCur = next;
                return alloc;
            }
        }

        // Out of space in jit.so, fall back to AVMPI to get code mem.
        overage += nbytes;
        fprintf(stderr, "out of jit.so memory, allocated %luKB from system\n",
               overage / 1024);
        char* mem = (char*) AVMPI_allocateCodeMemory(nbytes);
        mprotect((maddr_ptr) mem, (unsigned int) nbytes,
                 PROT_EXEC | PROT_READ | PROT_WRITE);

        return mem;
    }

    void CodeAlloc::freeCodeChunk(void* addr, size_t nbytes)
    {
        if (addr < sStart || addr >= sEnd) {
            // Did not come from DL region.
            AVMPI_freeCodeMemory(addr, nbytes);
        }
    }

    void CodeAlloc::markCodeChunkExec(void* /*addr*/, size_t /*nbytes*/) {
    }

    void CodeAlloc::markCodeChunkWrite(void* /*addr*/, size_t /*nbytes*/) {
    }

#else // !VMCFG_SHARK

    void* CodeAlloc::allocCodeChunk(size_t nbytes) {
        return AVMPI_allocateCodeMemory(nbytes);
    }

    void CodeAlloc::freeCodeChunk(void* addr, size_t nbytes) {
        AVMPI_freeCodeMemory(addr, nbytes);
    }

    void CodeAlloc::markCodeChunkExec(void* addr, size_t nbytes) {
        //printf("protect   %d %p\n", (int)nbytes, addr);
        AVMPI_makeCodeMemoryExecutable(addr, nbytes, true); // RX
    }

    void CodeAlloc::markCodeChunkWrite(void* addr, size_t nbytes) {
        //printf("unprotect %d %p\n", (int)nbytes, addr);
        AVMPI_makeCodeMemoryExecutable(addr, nbytes, false); // RW
    }
#endif

#ifdef DEBUG
    // Note this method should only be called during debug builds.
    bool CodeAlloc::checkChunkMark(void* addr, size_t nbytes, bool isExec) {
        bool b = true;

        // iterate over each page checking permission bits
        size_t psize = VMPI_getVMPageSize();
        uintptr_t last = alignTo((uintptr_t)addr + nbytes-1, psize);
        uintptr_t start = (uintptr_t)addr;
        for( uintptr_t n=start; b && n<=last; n += psize) {
#ifdef AVMPLUS_WIN32
            /* windows */
            MEMORY_BASIC_INFORMATION buf;
            VMPI_memset(&buf, 0, sizeof(MEMORY_BASIC_INFORMATION));
            SIZE_T sz = VirtualQuery((LPCVOID)n, &buf, sizeof(buf));
            NanoAssert(sz > 0);
            b = isExec ? buf.Protect == PAGE_EXECUTE_READ
                       : buf.Protect == PAGE_READWRITE;
            NanoAssert(b);
#elif defined(__MACH30__) && !defined(VMCFG_SHARK)
            /* mach / osx */
            vm_address_t vmaddr = (vm_address_t)addr;
            vm_size_t vmsize = psize;
            vm_region_basic_info_data_64_t inf;
            mach_msg_type_number_t infoCnt = sizeof(vm_region_basic_info_data_64_t);
            mach_port_t port;
            VMPI_memset(&inf, 0, infoCnt);
            kern_return_t err = vm_region_64(mach_task_self(), &vmaddr, &vmsize, VM_REGION_BASIC_INFO_64, (vm_region_info_t)&inf, &infoCnt, &port);
            NanoAssert(err == KERN_SUCCESS);
            b = isExec ? inf.protection == (VM_PROT_READ | VM_PROT_EXECUTE)
                       : inf.protection == (VM_PROT_READ | VM_PROT_WRITE);
            NanoAssert(b);
#else
            (void)psize;
            (void)last;
            (void)start;
            (void)n;
            (void)isExec;
#endif
        }
        return b;
    }

    void ValidateWriter::checkAccSet(LOpcode op, LIns* base, int32_t disp, AccSet accSet)
    {
        (void)disp;
        LIns* vars = checkAccSetExtras ? (LIns*)checkAccSetExtras[0] : 0;
        LIns* tags = checkAccSetExtras ? (LIns*)checkAccSetExtras[1] : 0;

        // not enough to check base == xxx , since cse or lirbuffer may split (ld/st,b,d) into (ld/st,(addp,b,d),0)
        bool isTags = (base == tags) ||
                      ( (base->opcode() == LIR_addp) && (base->oprnd1() == tags) );
        bool isVars = (base == vars) ||
                      ( (base->opcode() == LIR_addp) && (base->oprnd1() == vars) );
        bool isUnknown = !isTags && !isVars;

        bool ok;

        NanoAssert(accSet != ACCSET_NONE);
        switch (accSet) {
        case avmplus::ACCSET_VARS:   ok = isVars;        break;
        case avmplus::ACCSET_TAGS:   ok = isTags;        break;
        case avmplus::ACCSET_OTHER:  ok = isUnknown;     break;
        default:
            // This assertion will fail if any single-region AccSets aren't covered
            // by the switch -- only multi-region AccSets should be handled here.
            AvmAssert(compressAccSet(accSet).val == MINI_ACCSET_MULTIPLE.val);
            ok = true;
            break;
        }

        if (!ok) {
            InsBuf b1, b2;
            printer->formatIns(&b1, base);
            VMPI_snprintf(b2.buf, b2.len, "but the base pointer (%s) doesn't match", b1.buf);
            errorAccSet(lirNames[op], accSet, b2.buf);
         }
    }
#endif

}

#endif // VMCFG_NANOJIT
