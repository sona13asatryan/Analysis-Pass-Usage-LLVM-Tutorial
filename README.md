# Analysis-Usage-LLVM-Tutorial (NEW PASS MANAGER)
This tutorial contains step by step description of how to implement a little analysis, which collects global variables and the number of their uses in a map, and to use the data from an outer "trasnformer" pass, the task of which is to output the globals with more than 2 uses (the key is not to transform the data, but just to use it from the "outside").

# Introduction
One of the main tools LLVM provides are passes, which give the opportunity to analyze, collect and transform data. Thus, passes can be grouped according to being analyses, main task of which is to collect data, and being transformer ones, which, on the contrary, are allowed to do various modifications and manipulations. The point is, that such transformer passes often need some data collected beforehand, to make their changes, therefore they use analyses. Maybe you wonder, why do analyses need to be separate from transformer passes ? The answer is, that passes are executed in some order, and there is high likelihood, that if a certain pass starts its execution the result of the analysis required from it is already invalid, so it must be executed again. Secondly, one analysis can be a essential for many passes, so isolating the concept of an analysis helps to get rid of code repetition.

# Analysis
Let's create the analysis first, as we need to use it afterwards. We need to locate our cpp file in llvm-project/llvm/lib/Analysis/ directory. I named the file GlobalCollector.cpp. We should include the name of our file in the CMakeLists.txt file of the same directory in add_llvm_component_library command to make it build with LLVM.  

The header file is to be placed in llvm-project/llvm/include/llvm/Analysis/ directory.  

We'll use a common approach of combining the analysis itself with its printer pass in the same file. As passes are derived form PassInfoMixin template class, with need to include the header "llvm/IR/PassManager.h" in our header file. Besides, analyses are particularly derived from AnalysisInfoMixin template class, which, in turn is also derived from the abovementioned one. To learn more you can visit [this](https://llvm.org/doxygen/structllvm_1_1PassInfoMixin.html) .  
The pass and the anlaysis itself represent classes, which are highly preferred to be included in the llvm namespace, for both not polluting the global namespace and achive our goal of integrating our entities into LLVM. Next, we define the analysis class (GlobalCollectorAnalysis) and add 2 important lines. The first one is, 
```c++ 
friend AnalysisInfoMixin<GlobalCollectorAnalysis>;
```
which makes it possible for 
```c++ 
AnalysisInfoMixin<GlobalCollectorAnalysis>
```
to access private members of our class. The second one is, 
```c++ 
static AnalysisKey Key;
``` 
which is the exact private member required by 
```c++  
AnalaysisInfoMixin<GlobalCollectorAnalysis> .
```
Concerning AnalysisKey LLVM says the following:
``` c++
template <typename DerivedT>
struct AnalysisInfoMixin : PassInfoMixin<DerivedT> {
  /// Returns an opaque, unique ID for this analysis type.
  ///
  /// This ID is a pointer type that is guaranteed to be 8-byte aligned and thus
  /// suitable for use in sets, maps, and other data structures that use the low
  /// bits of pointers.
  ///
  /// Note that this requires the derived type provide a static \c AnalysisKey
  /// member called \c Key.
  ///
  /// FIXME: The only reason the mixin type itself can't declare the Key value
  /// is that some compilers cannot correctly unique a templated static variable
  /// so it has the same addresses in each instantiation. The only currently
  /// known platform with this limitation is Windows DLL builds, specifically
  /// building each part of LLVM as a DLL. If we ever remove that build
  /// configuration, this mixin can provide the static key as well.
  static AnalysisKey *ID() {
    static_assert(std::is_base_of<AnalysisInfoMixin, DerivedT>::value,
                  "Must pass the derived type as the template argument!");
    return &DerivedT::Key;
  }
};
```
Upcoming step is to understand how is analysis run and how the result should be transmitted. Every analysis ought to have a run method, which must return the result. It's needed to define what the reult is, usually it appears to be some data structure. In our case, we want to collect the names of all global variables and the number of their uses respectively. I chose a map to organize the data, therefore the line
```c++
 using Result = std::map<StringRef, unsigned int>;
```
specifies the result, which keys are strings representing the name of the global, and the value — number of uses. The analysis will be executed due to it run function, so 
we declare in our GlobalCollectorAnalysis 
```c++
 Result run(Module &M, ModuleAnalysisManager &);
```
Take a look on the parameters. As you might already know, passes are supposed to run on specific IR units ( e.g. Modules, Functions, Loops, etc. ).  
To know more you can visit [this](https://llvm.org/docs/WritingAnLLVMPass.html#pass-classes-and-requirements) , but be attentive, the document deals with  ${\color{red} Legacy \space Pass \space Manager}$ and not the new one, whereas the basic concepts remain the same.  
As globals belong to the module unit, we need to use Module and ModuleAnalysisManager respectively.  
Time for printer pass. It must be derived from PassInfoMixin class and have a member of raw_ostream& type, as the task of a printer pass is to output. We define the constructor and another run function, which is appropriate for transform passes. As you can see it's return type is PreservedAnalyses which indicates the fact, that after the execution of this pass none of the analyses will become invalid. That's it for the header file GlobalCollector.h Let's move on to the cpp one.  

At the beginning we'll add the GlobalCollector.h header 
```c++
  #include "llvm/Analysis/GlobalCollector.h" .
```
Afterwards add the line,
```c++
  using namespace llvm;
```
as function that we use are already part of the llvm namespace. Next, we define our static data member in the global scope and proceed to the definition of the run function ehich is the core part of the analysis. Here, we iterate over the global variables of the module and than — over their uses. We use the method [getName](https://llvm.org/doxygen/classllvm_1_1Value.html#adb5c319f5905c1d3ca9eb5df546388c5) to save the name of the global.  
Here, we also define our PrinterPass's run method, which just outputs the map of all the globals and their uses.  

The only thing left is to register the analysis and the pass. To reach that goal, you need to redact the llvm-project/llvm/lib/Passes/PassRegistry.def file by adding
```c++
  MODULE_ANALYSIS("my-analysis", GlobalCollectorAnalysis())                                                                                                                  //
  //
  .
  .
  .
  //
  MODULE_PASS("my-analysis-pass", GlobalCollectorPrinterPass(dbgs()))
```
Also, don't forget to add the header to llvm-project/llvm/lib/Passes/PassBuilder.cpp. Now it's all done. You can invoke your pass by writing 
```
opt -passes=my-analysis-pass sample.ll
```
command in terminal. Analysis can't be called separately, that's why the PrinterPass is used.
  
# Transformation Pass
In our case, the transformation pass intends to print all the global variables' data, which uses are more than 2. This time, we also need a header file and a cpp file. I named them GlobalTransformer.h and GlobalTransformer.cpp respectively. The header must be located in llvm-project/llvm/include/llvm/Transforms/Utils, the cpp — llvm-project/llvm/lib/Transforms/Utils directories. To register our pass we need to perform the same steps as above in file PassRegistry.def and PassBuilder.cpp.  
In the header file, we need to include the following, 
```c++
  #include "llvm/Analysis/GlobalCollector.h"                                                                                                                                       #include "llvm/IR/PassManager.h"
```
as we want to use the analysis we've written before. There's nothing new to say here, just repetitions of all the necessary steps, which have been discussed above.  

Now, the cpp file. The main part is to use the analysis, so we are not going to deepen to much into details, you can always take a look on the files.  
So, the method, which allows to take the results of the analysis is called getResult, defined in "llvm/IR/PassManager.h":
```c++
  /// Get the result of an analysis pass for a given IR unit.
  /// Runs the analysis if a cached result is not available.
  template <typename PassT>
  typename PassT::Result &getResult(IRUnitT &IR, ExtraArgTs... ExtraArgs) {
    assert(AnalysisPasses.count(PassT::ID()) &&
           "This analysis pass was not registered prior to being queried");
    ResultConceptT &ResultConcept =
        getResultImpl(PassT::ID(), IR, ExtraArgs...);
 
    using ResultModelT =
        detail::AnalysisResultModel<IRUnitT, PassT, typename PassT::Result,
                                    PreservedAnalyses, Invalidator>;
 
    return static_cast<ResultModelT &>(ResultConcept).Result;
  }
```
Thus, we simply add the following in line to our run function
```c++
  std::map<StringRef, unsigned int> var = MAM.getResult<GlobalCollectorAnalysis>(M);
```
and ta-da! Done!

