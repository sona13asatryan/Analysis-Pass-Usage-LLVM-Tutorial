#ifndef GLOBAL_TRANSFORMER_H
#define GLOBAL_TRANSFORMER_H

#include "llvm/Analysis/GlobalCollector.h"
#include "llvm/IR/PassManager.h"

using namespace llvm;

class GlobTrans : public PassInfoMixin<GlobTrans> {
	public:
		PreservedAnalyses run (Module &M, ModuleAnalysisManager &MAM);
};

#endif // GLOBAL_TRANSFORMER_H
