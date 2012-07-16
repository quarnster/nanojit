#include <stdio.h>
#include <stdint.h>
//#include "jsapi.h"
//#include "jstracer.h"
#include "nanojit.h"

using namespace nanojit;
const uint32_t CACHE_SIZE_LOG2 = 20;

int main()
{
    LogControl lc;
#ifdef DEBUG
    lc.lcbits = LC_ReadLIR | LC_Assembly;
#else
    lc.lcbits = 0;
#endif
/*
    // Set up the basic Nanojit objects.
    Allocator *alloc = new VMAllocator();
    CodeAlloc *codeAlloc = new CodeAlloc();
    Assembler *assm = new (&gc) Assembler(*codeAlloc, *alloc, &core, &lc);
    Fragmento *fragmento =
        new (&gc) Fragmento(&core, &lc, CACHE_SIZE_LOG2, codeAlloc);
    LirBuffer *buf = new (*alloc) LirBuffer(*alloc);

    #ifdef DEBUG
    fragmento->labels = new (*alloc) LabelMap(*alloc, &lc);
    buf->names = new (*alloc) LirNameMap(*alloc, fragmento->labels);
    #endif

    // Create a Fragment to hold some native code.
    Fragment *f = fragmento->getAnchor((void *)0xdeadbeef);
    f->lirbuf = buf;
    f->root = f;

    // Create a LIR writer
    LirBufWriter out(buf);

    // Write a few LIR instructions to the buffer: add the first parameter
    // to the constant 2.
    out.ins0(LIR_start);
    LIns *two = out.insImm(2);
    LIns *firstParam = out.insParam(0, 0);
    LIns *result = out.ins2(LIR_add, firstParam, two);
    out.ins1(LIR_ret, result);

    // Emit a LIR_loop instruction.  It won't be reached, but there's
    // an assertion in Nanojit that trips if a fragment doesn't end with
    // a guard (a bug in Nanojit).
    LIns *rec_ins = out.insSkip(sizeof(GuardRecord) + sizeof(SideExit));
    GuardRecord *guard = (GuardRecord *) rec_ins->payload();
    memset(guard, 0, sizeof(*guard));
    SideExit *exit = (SideExit *)(guard + 1);
    guard->exit = exit;
    guard->exit->target = f;
    f->lastIns = out.insGuard(LIR_loop, out.insImm(1), rec_ins);

    // Compile the fragment.
    compile(assm, f, *alloc verbose_only(, fragmento->labels));
    if (assm->error() != None) {
        fprintf(stderr, "error compiling fragment\n");
        return 1;
    }
    printf("Compilation successful.\n");

    // Call the compiled function.
    typedef JS_FASTCALL int32_t (*AddTwoFn)(int32_t);
    AddTwoFn fn = reinterpret_cast<AddTwoFn>(f->code());
    printf("2 + 5 = %d\n", fn(5));
*/
    return 0;
}

