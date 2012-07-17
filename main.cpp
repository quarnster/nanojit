#include <stdio.h>
#include <stdint.h>
#include "nanojit.h"

using namespace nanojit;
const uint32_t CACHE_SIZE_LOG2 = 20;

int main()
{
    LogControl lc;
    bool optimize = true;
#ifdef DEBUG
    lc.lcbits = LC_ReadLIR | LC_Native;
#else
    lc.lcbits = 0;
#endif
    Allocator alloc;
    CodeAlloc codeAlloc;
    Config config;
    Assembler assm(codeAlloc, alloc, alloc, &lc, config);



    LirBuffer *buf = new (alloc) LirBuffer(alloc);
    LirBufWriter out(buf, config);

#ifdef DEBUG
    LInsPrinter p(alloc, 1024);
    buf->printer = &p;
#endif

    Fragment f(NULL verbose_only(, 0));
    f.lirbuf = buf;


    out.ins0(LIR_start);
    LIns *two = out.insImmI(2);
    LIns *firstParam = out.insParam(0, 0);
    LIns *result = out.ins2(LIR_addi, firstParam, two);
    f.lastIns = out.ins1(LIR_reti, result);

    assm.compile(&f, alloc, optimize verbose_only(, &p));

    typedef int32_t (*AddTwoFn)(int32_t);
    AddTwoFn fn = reinterpret_cast<AddTwoFn>(f.code());
    printf("2 + 5 = %d\n", fn(5));
    return 0;
}

