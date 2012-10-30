/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=4 sw=4 et tw=99:
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "BaselineFrameInfo.h"
#include "IonSpewer.h"

using namespace js;
using namespace js::ion;

bool
FrameInfo::init() {
    size_t nstack = script->nslots - script->nfixed;
    if (!stack.init(nstack))
        return false;

    return true;
}

void
FrameInfo::sync(StackValue *val)
{
    switch (val->kind()) {
      case StackValue::Stack:
        break;
      case StackValue::LocalSlot:
        masm.pushValue(addressOfLocal(val->localSlot()));
        break;
      case StackValue::Register:
        masm.pushValue(val->reg());
        break;
      case StackValue::Constant:
        masm.pushValue(val->constant());
        break;
      default:
        JS_NOT_REACHED("Invalid kind");
        break;
    }

    val->setStack();
}

void
FrameInfo::syncStack(uint32_t uses)
{
    JS_ASSERT(uses <= stackDepth());

    uint32_t depth = stackDepth() - uses;

    for (uint32_t i = 0; i < depth; i++) {
        StackValue *current = &stack[i];
        sync(current);
    }
}

void
FrameInfo::ensureInRegister(StackValue *val, ValueOperand dest, ValueOperand scratch)
{
    switch (val->kind()) {
      case StackValue::Constant:
        masm.moveValue(val->constant(), dest);
        break;
      case StackValue::LocalSlot:
        masm.loadValue(addressOfLocal(val->localSlot()), dest);
        break;
      case StackValue::Stack:
        masm.popValue(dest);
        break;
      case StackValue::Register: {
        ValueOperand reg = val->reg();
        if (reg.payloadReg() != dest.payloadReg())
            masm.mov(reg.payloadReg(), dest.payloadReg());
        if (reg.typeReg() != dest.typeReg())
            masm.mov(reg.typeReg(), dest.typeReg());
        break;
      }
      default:
        JS_NOT_REACHED("foo");
    }
}

void
FrameInfo::popRegsAndSync(uint32_t uses)
{
    // x86 has only 3 Value registers. Only support 2 regs here for now,
    // so that there's always a scratch Value register for reg -> reg
    // moves.
    JS_ASSERT(uses > 0);
    JS_ASSERT(uses <= 2);
    JS_ASSERT(uses <= stackDepth());

    syncStack(uses);

    switch (uses) {
      case 1:
        ensureInRegister(peek(-1), R0, R2);
        break;
      case 2:
        if (peek(-1)->kind() == StackValue::Register) {
            ensureInRegister(peek(-1), R1, R2);
            ensureInRegister(peek(-2), R0, R2);
        } else {
            ensureInRegister(peek(-2), R0, R2);
            ensureInRegister(peek(-1), R1, R2);
        }
        break;
      default:
        JS_NOT_REACHED("Unexpected use count");
    }

    spIndex -= uses;
}
