#include "Instrument.h"

// #include "llvm/IR/DerivedTypes.h"

using namespace llvm;

namespace instrument {

/*
 * Implement your instrumentation for dynamic symbolic execution engine
 */
bool Instrument::runOnFunction(Function &F) {
  LLVMContext &Cxt = F.getContext();
  // Function to be instrumented
  auto initFunc = F.getParent()->getOrInsertFunction(DSEInitFunctionName,
                                                     Type::getVoidTy(Cxt));
  auto allocaFunc = F.getParent()->getOrInsertFunction(
      DSEAllocaFunctionName, Type::getVoidTy(Cxt), Type::getInt32Ty(Cxt),
      Type::getInt32PtrTy(Cxt));
  auto storeFunc = F.getParent()->getOrInsertFunction(
      DSEStoreFunctionName, Type::getVoidTy(Cxt), Type::getInt32PtrTy(Cxt));
  auto loadFunc = F.getParent()->getOrInsertFunction(
      DSELoadFunctionName, Type::getVoidTy(Cxt), Type::getInt32Ty(Cxt),
      Type::getInt32PtrTy(Cxt));
  auto constFunc = F.getParent()->getOrInsertFunction(
      DSEConstFunctionName, Type::getVoidTy(Cxt), Type::getInt32Ty(Cxt));
  auto regFunc = F.getParent()->getOrInsertFunction(
      DSERegisterFunctionName, Type::getVoidTy(Cxt), Type::getInt32Ty(Cxt));
  auto icmpFunc = F.getParent()->getOrInsertFunction(
      DSEICmpFunctionName, Type::getVoidTy(Cxt), Type::getInt32Ty(Cxt),
      Type::getInt32Ty(Cxt));
  auto brFunc = F.getParent()->getOrInsertFunction(
      DSEBranchFunctionName, Type::getVoidTy(Cxt), Type::getInt32Ty(Cxt),
      Type::getInt32Ty(Cxt), Type::getInt32Ty(Cxt));
  auto binOpFunc = F.getParent()->getOrInsertFunction(
      DSEBinOpFunctionName, Type::getVoidTy(Cxt), Type::getInt32Ty(Cxt),
      Type::getInt32Ty(Cxt));

  // Iterate over the program
  for (Function::iterator bb = F.begin(); bb != F.end(); ++bb) {
    std::string bbName = bb->getName().str();
    if (bbName == "entry") {
      auto beginInst = bb->begin();
      IRBuilder<> builder(&*beginInst);
      builder.SetInsertPoint(&*bb, ++builder.GetInsertPoint());
      builder.CreateCall(initFunc);
    }

    for (BasicBlock::iterator ii = bb->begin(); ii != bb->end(); ++ii) {
      if (AllocaInst *curInst = dyn_cast<AllocaInst>(&*ii)) {
        IRBuilder<> builder(&*curInst);
        builder.SetInsertPoint(&*bb, ++builder.GetInsertPoint());

        Value *callArgs[] = {builder.getInt32(getRegisterID(curInst)), curInst};

        builder.CreateCall(allocaFunc, callArgs);
      } else if (StoreInst *curInst = dyn_cast<StoreInst>(&*ii)) {
        IRBuilder<> builder(&*curInst);
        builder.SetInsertPoint(&*bb, ++builder.GetInsertPoint());

        Value *op0 = curInst->getOperand(0);
        Value *op1 = curInst->getOperand(1);
        if (Constant *cOp0 = dyn_cast<Constant>(op0)) {
          // Insert __DSE_Const__
          Value *constArgs = {op0};
          Value *args = {op1};
          builder.CreateCall(constFunc, constArgs);
          builder.CreateCall(storeFunc, args);
        } else {
          // Insert __DSE_Register__
          Value *regArgs = {builder.getInt32(getRegisterID(op0))};
          Value *args = {op1};
          builder.CreateCall(regFunc, regArgs);
          builder.CreateCall(storeFunc, args);
        }
      } else if (LoadInst *curInst = dyn_cast<LoadInst>(&*ii)) {
        IRBuilder<> builder(&*curInst);
        builder.SetInsertPoint(&*bb, ++builder.GetInsertPoint());

        Value *op0 = curInst->getOperand(0);
        Value *callArgs[] = {builder.getInt32(getRegisterID(curInst)), op0};

        builder.CreateCall(loadFunc, callArgs);
      } else if (ICmpInst *curInst = dyn_cast<ICmpInst>(&*ii)) {
        IRBuilder<> builder(&*curInst);
        builder.SetInsertPoint(&*bb, ++builder.GetInsertPoint());

        Value *op0 = curInst->getOperand(0);
        Value *op1 = curInst->getOperand(1);

        if (Constant *cop0 = dyn_cast<Constant>(op0)) {
          // Insert __DSE_Const__
          Value *constArgs[] = {op0};
          builder.CreateCall(constFunc, constArgs);
        } else {
          // Insert __DSE_Register__
          Value *regArgs = {builder.getInt32(getRegisterID(op0))};
          Value *args = {op0};
          builder.CreateCall(regFunc, regArgs);
        }

        if (Constant *cop1 = dyn_cast<Constant>(op1)) {
          // Insert __DSE_Const__
          Value *constArgs[] = {op1};
          builder.CreateCall(constFunc, constArgs);
        } else {
          // Insert __DSE_Register__
          Value *regArgs = {builder.getInt32(getRegisterID(op1))};
          Value *args = {op1};
          builder.CreateCall(regFunc, regArgs);
        }

        // Insert __DSE_Icmp__
        Value *icmpArgs[] = {builder.getInt32(getRegisterID(curInst)),
                             builder.getInt32(curInst->getSignedPredicate())};
        // Value *icmpArgs[] = {builder.getInt32(getRegisterID(curInst)),
        //  builder.getInt32(curInst->getOpcode())};
        builder.CreateCall(icmpFunc, icmpArgs);
      } else if (BinaryOperator *curInst = dyn_cast<BinaryOperator>(&*ii)) {
        IRBuilder<> builder(&*curInst);
        builder.SetInsertPoint(&*bb, ++builder.GetInsertPoint());

        Value *op0 = curInst->getOperand(0);
        Value *op1 = curInst->getOperand(1);

        if (Constant *cop0 = dyn_cast<Constant>(op0)) {
          // Insert __DSE_Const__
          Value *constArgs[] = {op0};
          builder.CreateCall(constFunc, constArgs);
        } else {
          // Insert __DSE_Register__
          Value *regArgs = {builder.getInt32(getRegisterID(op0))};
          Value *args = {op0};
          builder.CreateCall(regFunc, regArgs);
        }
        if (Constant *cop1 = dyn_cast<Constant>(op1)) {
          // Insert __DSE_Const__
          Value *constArgs[] = {op1};
          builder.CreateCall(constFunc, constArgs);
        } else {
          // Insert __DSE_Register__
          Value *regArgs = {builder.getInt32(getRegisterID(op1))};
          Value *args = {op1};
          builder.CreateCall(regFunc, regArgs);
        }

        // Insert __DSE_BinOp__
        Value *binArgs[] = {builder.getInt32(getRegisterID(curInst)),
                            builder.getInt32(curInst->getOpcode())};
        builder.CreateCall(binOpFunc, binArgs);
      } else if (BranchInst *curInst = dyn_cast<BranchInst>(&*ii)) {
        if (curInst->isConditional()) {
          ICmpInst *icmp = dyn_cast<ICmpInst>(curInst->getOperand(0));
          BasicBlock *trueBr = curInst->getSuccessor(0);
          BasicBlock *falseBr = curInst->getSuccessor(1);
          // Create new basic block
          BasicBlock *trueBrTmp =
              BasicBlock::Create(Cxt, trueBr->getName() + ".tmp", &F);
          BasicBlock *falseBrTmp =
              BasicBlock::Create(Cxt, falseBr->getName() + ".tmp", &F);
          // Insert function
          // True Cond
          IRBuilder<> trueBrBuilder(trueBrTmp);
          trueBrBuilder.SetInsertPoint(trueBrTmp, trueBrTmp->begin());
          Value *trueArgs[] = {trueBrBuilder.getInt32(getBranchID(curInst)),
                               trueBrBuilder.getInt32(getRegisterID(icmp)),
                               trueBrBuilder.getInt32(1)};
          trueBrBuilder.CreateCall(brFunc, trueArgs);
          trueBrBuilder.CreateBr(trueBr);
          // False Cond
          IRBuilder<> falseBrBuilder(falseBrTmp);
          falseBrBuilder.SetInsertPoint(falseBrTmp, falseBrTmp->begin());
          Value *falseArgs[] = {falseBrBuilder.getInt32(getBranchID(curInst)),
                                falseBrBuilder.getInt32(getRegisterID(icmp)),
                                falseBrBuilder.getInt32(0)};
          falseBrBuilder.CreateCall(brFunc, falseArgs);
          falseBrBuilder.CreateBr(falseBr);

          curInst->setSuccessor(0, trueBrTmp);
          curInst->setSuccessor(1, falseBrTmp);
        }
      } else {
      }
    }
  }
  return true;
}

char Instrument::ID = 1;
static RegisterPass<Instrument> X(
    "Instrument", "Instrumentations for Dynamic Symbolic Execution", false,
    false);

}  // namespace instrument
