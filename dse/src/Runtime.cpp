#include <iostream>

#include "SymbolicInterpreter.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"

extern SymbolicInterpreter SI;

/*
 * Implement your transfer functions.
 */
extern "C" void __DSE_Alloca__(int R, int *Ptr) {
  SI.getMemory().insert(std::make_pair(
      Address(R),
      SI.getContext().int_const(std::to_string(uintptr_t(Ptr)).c_str())));
}

extern "C" void __DSE_Store__(int *X) {
  z3::expr top = SI.getStack().top();
  SI.getStack().pop();

  // If top is a reg
  if (top.to_string()[0] == 'R') {
    top = SI.getMemory().at(Address(top));
  }

  // store
  SI.getMemory().insert(std::make_pair(Address(X), top));
}

extern "C" void __DSE_Load__(int Y, int *X) {
  z3::expr val = SI.getMemory().at(Address(X));

  SI.getMemory().insert(std::make_pair(Address(Y), val));
}

extern "C" void __DSE_ICmp__(int R, int Op) {
  // Op1
  z3::expr op1 = SI.getStack().top();
  SI.getStack().pop();
  if (op1.to_string()[0] == 'R') {
    op1 = SI.getMemory().at(Address(op1));
  }

  // Op0
  z3::expr op0 = SI.getStack().top();
  SI.getStack().pop();
  if (op0.to_string()[0] == 'R') {
    op0 = SI.getMemory().at(Address(op0));
  }

  z3::expr cmpRes = (op0 == op1);

  if (Op == llvm::CmpInst::ICMP_EQ) {
    cmpRes = (op0 == op1);
    SI.getMemory().insert(std::make_pair(Address(R), cmpRes));
  } else if (Op == llvm::CmpInst::ICMP_NE) {
    cmpRes = (op0 != op1);
    SI.getMemory().insert(std::make_pair(Address(R), cmpRes));
  } else if (Op == llvm::CmpInst::ICMP_SGE) {
    cmpRes = (op0 >= op1);
    SI.getMemory().insert(std::make_pair(Address(R), cmpRes));
  } else if (Op == llvm::CmpInst::ICMP_SLE) {
    cmpRes = (op0 <= op1);
    SI.getMemory().insert(std::make_pair(Address(R), cmpRes));
  } else if (Op == llvm::CmpInst::ICMP_SGT) {
    cmpRes = (op0 > op1);
    SI.getMemory().insert(std::make_pair(Address(R), cmpRes));
  } else if (Op == llvm::CmpInst::ICMP_SLT) {
    cmpRes = (op0 < op1);
    SI.getMemory().insert(std::make_pair(Address(R), cmpRes));
  }
}

extern "C" void __DSE_BinOp__(int R, int Op) {
  // Op1
  z3::expr op1 = SI.getStack().top();
  SI.getStack().pop();
  if (op1.to_string()[0] == 'R') {
    op1 = SI.getMemory().at(Address(op1));
  }

  // Op0
  z3::expr op0 = SI.getStack().top();
  SI.getStack().pop();
  if (op0.to_string()[0] == 'R') {
    op0 = SI.getMemory().at(Address(op0));
  }

  // Store res into mem
  z3::expr res = (op0 + op1);
  if (Op == llvm::BinaryOperator::Add) {
    res = (op0 + op1);
    SI.getMemory().insert(std::make_pair(Address(R), res));
  } else if (Op == llvm::BinaryOperator::Sub) {
    res = (op0 - op1);
    SI.getMemory().insert(std::make_pair(Address(R), res));
  } else if (Op == llvm::BinaryOperator::Mul) {
    res = (op0 * op1);
    SI.getMemory().insert(std::make_pair(Address(R), res));
  } else if (Op == llvm::BinaryOperator::SDiv) {
    res = (op0 / op1);
    SI.getMemory().insert(std::make_pair(Address(R), res));
  } else if (Op == llvm::BinaryOperator::SRem) {
    res = (op0 % op1);
    SI.getMemory().insert(std::make_pair(Address(R), res));
  }
}
