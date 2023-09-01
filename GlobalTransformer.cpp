#include "llvm/Transforms/Utils/GlobalTransformer.h"

using namespace llvm;

PreservedAnalyses GlobTrans::run (Module &M, ModuleAnalysisManager &MAM) {
	std::map<StringRef, unsigned int> var = MAM.getResult<GlobalCollectorAnalysis>(M);
	for (auto i = var.begin(); i != var.end(); ++i) {
		if (i->second > 2) {
			errs() << "The global " << i->first << " has " << i->second << " uses!\n";
		}
	}
	return PreservedAnalyses::all();
}
