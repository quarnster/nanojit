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
 * The Original Code is [Open Source Virtual Machine].
 *
 * The Initial Developer of the Original Code is
 * Adobe System Incorporated.
 * Portions created by the Initial Developer are Copyright (C) 2004-2007
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Adobe AS3 Team
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

#include "nanojit.h"

namespace nanojit
{
    #ifdef FEATURE_NANOJIT

    #ifdef _DEBUG

    bool RegAlloc::isConsistent(Register r, LIns* i) const
    {
        return ( isFree(r) && !getActive(r)      && !i) ||
               (!isFree(r) &&  getActive(r) == i && i );
    }

    #endif /*DEBUG*/


    void RegAlloc::initialize(Assembler* a)
    {
        _assembler = a;
        _managed = nInitManagedRegisters(); // was nRegisterResetAll(_allocator);
        _free = _managed;

        // At start, should have some registers free and none active.
        NanoAssert( _assembler!=NULL );
        NanoAssert( _free != 0 );
        NanoAssert( ! activeMask() );
#ifdef NANOJIT_IA32
        debug_only( _assembler->_fpuStkDepth = 0; )
#endif
    }

    // Legend for register sets: A = allowed, P = preferred, F = free, S = SavedReg.
    //
    // Finds a register in 'setA___' to store the result of 'ins' (one from
    // 'set_P__' if possible), evicting one if necessary.  Doesn't consider
    // the prior state of 'ins'.
    //
    // Nb: 'setA___' comes from the instruction's use, 'set_P__' comes from its def.
    // Eg. in 'add(call(...), ...)':
    //     - the call's use means setA___==GpRegs;
    //     - the call's def means set_P__==rmask(retRegs[0]).
    //
    Register RegAlloc::allocReg(LIns* ins, RegisterMask setA___ , Register regClass )
    {
        Register r;
        RegisterMask set__F_ = _free;
        RegisterMask setA_F_ = setA___ & set__F_;
        RegisterMask set_P__ = nHint(ins);

        if (setA_F_) {
            RegisterMask set;
            RegisterMask set___S = SavedRegs;
            RegisterMask setA_FS = setA_F_ & set___S;
            RegisterMask setAPF_ = setA_F_ & set_P__;
            RegisterMask setAPFS = setA_FS & set_P__;

            if      (setAPFS) set = setAPFS;
            else if (setAPF_) set = setAPF_;
            else if (setA_FS) set = setA_FS;
            else              set = setA_F_;

            r = firstAvailableReg(ins, regClass, set); // Note: some platforms prefer lsReg, others msReg
#ifdef RA_REGISTERS_OVERLAP
            if (r == UnspecifiedReg) {
                //we may get here for composite regs, if the "prefered" set leaves only single regs; try again
                r = firstAvailableReg(ins, regClass, setA_F_);
            }
            if (r != UnspecifiedReg)
                return allocSpecificReg(ins, r);
            // fall through to else case below.
#else
            NanoAssert(r != UnspecifiedReg); // we should always find one
            return allocSpecificReg(ins, r);
#endif
        } 
        /* else */
        // Nothing free, steal one register.
        // LSRA says pick the one with the furthest use.
        LIns* vic = findVictim(setA___, ins, regClass);
        NanoAssert(vic->isInReg());
        r = vic->getReg();
        NanoAssert(_assembler);
        _assembler->evict(vic);
#ifdef RA_REGISTERS_OVERLAP
        // r may actually be wider than we need
        SET_REG_IN_MASK(r, setA_F_);
        r = firstAvailableReg(ins, regClass, setA_F_);
        NanoAssert(r !=  UnspecifiedReg );
#endif
        allocSpecificReg(ins, r);

        return r;
    }

    // Scan table for instruction with the lowest priority, meaning it is used
    // furthest in the future.
    LIns* RegAlloc::findVictim( RegisterMask allow, LIns* forIns /*= NULL*/, Register regClass /*= UnspecifiedReg*/ )
    {
        NanoAssert(allow);
        LIns *ins, *vic = 0;
        int allow_pri = 0x7fffffff;
        RegisterMask vic_set = allow & activeMask();
        for (Register r = lsReg(vic_set); vic_set; r = nextLsReg(vic_set, r))
        {
            ins = getActive(r);
            if(!ins) {
                NanoAssert(ins); //ins should really be non-null here, but let's be defensive
                continue;
            }

            int pri = canRemat(ins) ? 0 : getPriority(r);
#ifdef RA_REGISTERS_OVERLAP
            Register r1 = ins->getReg(); // may be wider than r
            if (forIns && firstAvailableReg(forIns, regClass, (_free | rmask(r1)) & allow) == UnspecifiedReg) {
                // evicting this instruction wouldn't help; find another one
                continue;
            }
#else
            (void) forIns; (void) regClass;
#endif
            if (!vic || pri < allow_pri) {
                vic = ins;
                allow_pri = pri;
            }
        }
        NanoAssert(vic != 0);
        return vic;
    }

    // Allocates a SPECIFIC register to the instruction; Asserts that it can
    // be allocated.
    // Always returns r
    Register RegAlloc::allocSpecificReg(LIns* ins, Register r)
    {
        NanoAssert(ins);
        NanoAssert(isFree(r));
        ins->setReg(r); 
#if RA_REGISTERS_OVERLAP
        RegisterMask rm = rmask(r);
        bool handle_r_separately = true;
        for (Register r1 = lsReg(rm); rm; r1 = nextLsReg(rm, r1))
        {
            NanoAssert(_active[REGNUM(r1)] == NULL);
            _active[REGNUM(r1)] = ins;
            useActive(r1);
            if(r==r1) handle_r_separately = false;
        }
        
        if(handle_r_separately)
#endif
        {
            NanoAssert(_active[REGNUM(r)] == NULL);
            _active[REGNUM(r)] = ins;
            useActive(r);
        }
        /* remove from free */
        CLR_REG_IN_MASK(r, _free);
        
        return r;
    }
    #endif /* FEATURE_NANOJIT */
}
