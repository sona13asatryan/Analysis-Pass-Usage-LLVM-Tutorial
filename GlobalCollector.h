#ifndef LLVM_LIB_ANALYSIS_GLOBAL_COLLECTOR
#define LLVM_LIB_ANALYSIS_GLOBAL_COLLECTOR

#include "llvm/IR/PassManager.h"
#include <map>

namespace llvm {
	class GlobalCollectorPrinterPass : public PassInfoMixin<GlobalCollectorPrinterPass> {
  			raw_ostream &OS;

		public:
  			explicit GlobalCollectorPrinterPass(raw_ostream &OS) : OS(OS) {}
			PreservedAnalyses run (Module &M, ModuleAnalysisManager &MAM);
  	
	};
	
	class GlobalCollectorAnalysis : public AnalysisInfoMixin<GlobalCollectorAnalysis> {
					
			friend AnalysisInfoMixin<GlobalCollectorAnalysis>;
			static AnalysisKey Key;
		public:
			using Result = std::map<StringRef, unsigned int>;	
			Result run(Module &M, ModuleAnalysisManager &);
	};
}

#endif // LLVM_LIB_ANALYSIS_GLOBAL_COLLECTOR
