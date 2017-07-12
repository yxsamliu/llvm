//===------ SelectAcceleratorCode.cpp - Select only accelerator code ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares and defines a pass which selects only code which is
// expected to be run by an accelerator i.e. referenced directly or indirectly
// (through a fully inlineable call-chain) by a [[hc]] function. To support
// subsequent processing, it also marks all identified functions as AlwaysInline
// thus making it possible to use only the AlwaysInliner without resorting to a
// more expensive full Inliner pass.
//
//===----------------------------------------------------------------------===//
#include "llvm/Analysis/InlineCost.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <unordered_set>

using namespace llvm;
using namespace std;

namespace
{
    class SelectAcceleratorCode : public ModulePass {
        unordered_set<Function*> hcCallees_;

        void findAllHCCallees_(const Function &f, Module &M)
        {
            for (auto&& x : f) {
                for (auto&& y : x) {
                    if (y.getOpcode() == Instruction::Call) {
                        auto g = cast<CallInst>(y).getCalledFunction();
                        if (g) {
                            auto t =
                                hcCallees_.insert(M.getFunction(g->getName()));
                            if (t.second) findAllHCCallees_(*g, M);
                        }
                    }
                }
            }
        }

        template<typename T>
        static
        void erase_(T& x)
        {
            x.dropAllReferences();
            x.replaceAllUsesWith(UndefValue::get(x.getType()));
            x.eraseFromParent();
        }

        template<typename F, typename G, typename P>
        bool eraseIf_(F f, G l, P p) const
        {
            bool r = false;

            auto it = f();
            while (it != l()) {
                it->removeDeadConstantUsers();
                if (p(*it)) {
                    erase_(*it);
                    r = true;
                    it = f();
                }
                else ++it;
            }

            return r;
        }

        bool eraseNonHCFunctions_(Module &M) const
        {
            return eraseIf_(
                [&]() { return M.begin(); },
                [&]() { return M.end(); },
                [&, this](const Function& x) {
                    return hcCallees_.count(M.getFunction(x.getName())) == 0;
                });
        }

        bool eraseDeadGlobals_(Module &M) const
        {
            return eraseIf_(
                [&]() { return M.global_begin(); },
                [&]() { return M.global_end(); },
                [](const Constant& x) { return !x.isConstantUsed(); });
        }

        bool eraseDeadAliases_(Module &M)
        {
            return eraseIf_(
                [&]() { return M.alias_begin(); },
                [&]() { return M.alias_end(); },
                [](const Constant& x) { return !x.isConstantUsed(); });
        }

        static
        bool alwaysInline_(Function& x)
        {
            if (!x.hasFnAttribute(Attribute::AlwaysInline)) {
                if (x.hasFnAttribute(Attribute::NoInline)) {
                    x.removeFnAttr(Attribute::NoInline);
                }
                x.addFnAttr(Attribute::AlwaysInline);

                return false;
            }

            return true;
        }
    public:
        static char ID;
        SelectAcceleratorCode() : ModulePass{ID} {}

        bool doInitialization(Module &M) override
        {   // TODO: this may represent a valid analysis pass.
            for (auto&& x : M.functions()) {
                if (x.hasFnAttribute("HC")) {
                    auto t = hcCallees_.insert(M.getFunction(x.getName()));
                    if (t.second) findAllHCCallees_(x, M);
                }
            }

            return false;
        }

        bool runOnModule(Module &M) override
        {
            bool r = eraseNonHCFunctions_(M);

            r = eraseDeadGlobals_(M) || r;

            M.dropTriviallyDeadConstantArrays();

            r = eraseDeadAliases_(M) || r;

            return r;
        }

        bool doFinalization(Module& M) override
        {
            bool r = false;
            std::for_each(M.begin(), M.end(), [&](Function& x) {
                if (!isInlineViable(x) && !x.isIntrinsic()) {
                    M.getContext().diagnose(DiagnosticInfoUnsupported{
                        x, "The function cannot be inlined."});
                }

                r = !alwaysInline_(x);
            });

            return r;
        }
    };
    char SelectAcceleratorCode::ID = 0;

    static RegisterPass<SelectAcceleratorCode> X{
        "select-accelerator-code",
        "Selects only the code that is expected to run on an accelerator, "
        "ensuring that it can be lowered by AMDGPU.",
        false,
        false};
}