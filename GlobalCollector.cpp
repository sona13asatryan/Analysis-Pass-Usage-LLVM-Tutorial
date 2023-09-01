#include "llvm/Analysis/GlobalCollector.h"

using namespace llvm;

AnalysisKey GlobalCollectorAnalysis::Key;

std::map<StringRef, unsigned int> GlobalCollectorAnalysis::run (Module &M, ModuleAnalysisManager &MAM) {
	std::map<StringRef, unsigned int> result;
	unsigned int count;
        for (llvm::Module::global_iterator ii = M.global_begin(); ii != M.global_end(); ++ii) {
                GlobalVariable* gv = &(*ii);
                count = 0;
                for (llvm::Value::use_iterator jj = gv->use_begin(); jj != gv->use_end(); ++jj) { 
                        ++count;
                }
                result[gv->getName()] = count;  
        }
	return result;
}

PreservedAnalyses GlobalCollectorPrinterPass::run (Module &M, ModuleAnalysisManager &MAM) {
	OS << "YOUR PASS WORKED!\n";
	return PreservedAnalyses::all();
}
