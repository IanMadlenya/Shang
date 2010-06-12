/*
* Copyright: 2008 by Nadav Rotem. all rights reserved.
* IMPORTANT: This software is supplied to you by Nadav Rotem in consideration
* of your agreement to the following terms, and your use, installation, 
* modification or redistribution of this software constitutes acceptance
* of these terms.  If you do not agree with these terms, please do not use, 
* install, modify or redistribute this software. You may not redistribute, 
* install copy or modify this software without written permission from 
* Nadav Rotem. 
*/
#include "VTargetMachine.h"
#include "llvm/CallingConv.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Module.h"
#include "llvm/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/TypeSymbolTable.h"
#include "llvm/Intrinsics.h"
#include "llvm/IntrinsicInst.h"
#include "llvm/InlineAsm.h"
#include "llvm/Analysis/ConstantsScanner.h"
#include "llvm/Analysis/FindUsedTypes.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/CodeGen/IntrinsicLowering.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Target/TargetRegistry.h "
#include "llvm/Target/TargetData.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Support/CFG.h"
#include "llvm/Support/GetElementPtrTypeIterator.h"
#include "llvm/Support/InstVisitor.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Target/Mangler.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Config/config.h"
#include <algorithm>
#include <sstream>
#include "llvm/Support/raw_ostream.h"

#include <llvm/ADT/DenseMap.h>
#include "llvm/Transforms/Utils/Cloning.h"

#include "HWAtom.h"
#include "RTLWriter.h"
#include "TestBenchWriter.h"
#include "vbe/ResourceConfig.h"

using namespace llvm;
using namespace esyn;

extern "C" void LLVMInitializeVerilogBackendTarget() { 
  // Register the target.
  RegisterTargetMachine<VTargetMachine> X(TheVBackendTarget);
}

using std::string;
using std::stringstream;
using llvm::TargetData; //JAWAD
namespace {
class VWriter : public FunctionPass {
  llvm::raw_ostream &Out;
 
public:
  static char ID;
  VWriter(llvm::raw_ostream &o) : FunctionPass((intptr_t)&ID),Out(o) {}
  
  /// @name Pass Interface.
  //{
  void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<TargetData>();//JAWAD
    AU.addRequired<VLang>();
    //AU.addRequired<ListSchedDriver>();
    AU.setPreservesAll();
  }

  bool runOnFunction(Function &F);
  //}
};
} // namespace

char VWriter::ID = 0;

bool VWriter::runOnFunction(Function &F) { 
  TargetData *TD =  &getAnalysis<TargetData>();//JAWAD
  VLang &vlang = getAnalysis<VLang>();
//  RTLWriter DesignWriter(vlang, TD);
//
//  ListSchedDriver &LSD = getAnalysis<ListSchedDriver>();
//
//  Out<<
//    "/*       This module was generated by c-to-verilog.com\n"
//    " * THIS SOFTWARE IS PROVIDED BY www.c-to-verilog.com ''AS IS'' AND ANY\n"
//    " * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n"
//    " * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n"
//    " * DISCLAIMED. IN NO EVENT SHALL c-to-verilog.com BE LIABLE FOR ANY\n"
//    " * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n"
//    " * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES)\n"
//    " * \n"
//    " * Found a bug? email info@c-to-verilog.com \n"
//    " */\n\n\n";
//
//  DesignWriter.emitModuleDecl(Out, &F);
//  DesignWriter.emitRegistersDecl(Out, LSD);
//  DesignWriter.emitStateDefs(Out, LSD);
//
//  DesignWriter.emitAssignPart(Out, LSD);
//
//  vlang.emitAlwaysffBegin(Out);
//  DesignWriter.emitResetAllRegs(Out, F, LSD);
//  vlang.emitEndReset(Out);
//
//  vlang.emitCommentBegin(Out) << "Datapath \n";
//  for (ListSchedVector::iterator I = LSD.begin(), E = LSD.end();
//      I != E; ++I)
//    DesignWriter.emitBBDatapath(Out, *I);
//
//  vlang.emitCommentBegin(Out) << "Control \n";
//  vlang.emitCaseBegin(Out);
//  // First of all, the idle state
//  vlang.emitCaseStateBegin(Out, "state_idle");
//  vlang.emitIfBegin(Out, "start");
//  vlang.indent(Out) << "eip <= " <<
//    vlang.GetValueName(&(F.getEntryBlock())) << 0 <<";\n"; //header
//  vlang.emitEnd(Out);
//  vlang.emitEnd(Out);
//
//  for (ListSchedVector::iterator I = LSD.begin(), E = LSD.end();
//      I != E; ++I)
//    DesignWriter.emitBBControl(Out, *I);
//
//  vlang.emitEndCase(Out);
//  vlang.emitEndAlwaysff(Out);
//  vlang.emitEndModule(Out);
//#if 0
//  Out<<"\n\n// -- Library components --  \n";
//  Out<<DesignWriter.createBinOpModule("mul","*",ResourceConfig::getResConfig("delay_mul"));
//  Out<<DesignWriter.createBinOpModule("div","/",ResourceConfig::getResConfig("delay_div"));
//  Out<<DesignWriter.createBinOpModule("shl","<<",ResourceConfig::getResConfig("delay_shl"));
//  Out<<DesignWriter.getBRAMDefinition(ResourceConfig::getResConfig("mem_wordsize"),
//                                      ResourceConfig::getResConfig("membus_size"));
//  
//#endif
  return false;
}


//===----------------------------------------------------------------------===//
//                       External Interface declaration
//===----------------------------------------------------------------------===//

bool VTargetMachine::addPassesToEmitWholeFile(PassManager &PM,
                                              formatted_raw_ostream &Out,
                                              CodeGenFileType FileType,
                                              CodeGenOpt::Level OptLevel,
                                              bool DisableVerify) {
    if (FileType != TargetMachine::CGFT_AssemblyFile) return true;

    // Resource config
    PM.add(new ResourceConfig());
    // Add the language writer.
    PM.add(new VLang());
    //
    PM.add(new HWAtomInfo());
    //
    PM.add(new VWriter(Out));
    PM.add(new TestbenchWriter(Out));
    return false;
}
