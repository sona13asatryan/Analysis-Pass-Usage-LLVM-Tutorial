# Analysis-Pass-Usage-LLVM-Tutorial
This tutorial contains step by step description of how to implement a little analysis, which collects global variables and the number of their uses in a map, and to use the data from an outer "trasnformer" pass, the task of which is to output the globals with more than 2 uses (the key is not to transform the data, but just to use it from the "outside").
