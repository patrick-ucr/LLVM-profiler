/*
 * Authors: Name(s) <email(s)>
 * Hoang Anh Dau		hdau001@ucr.edu
 * Kittipat Apicharttrisorn	kapic001@ucr.edu
 * 
 */

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include <stack>
#include "llvm/IR/CFG.h"
#include <stdio.h>
#include <string.h>


using namespace llvm;
using namespace std;

 
namespace {
  // https://github.com/thomaslee/llvm-demo/blob/master/main.cc
  static Function* printf_prototype(LLVMContext& ctx, Module *mod) {
    vector<Type*> printf_arg_types;
    printf_arg_types.push_back(Type::getInt8PtrTy(ctx));
 
    FunctionType* printf_type = FunctionType::get(Type::getInt32Ty(ctx), printf_arg_types, true);
    Function *func = mod->getFunction("printf");
    if(!func)
      func = Function::Create(printf_type, Function::ExternalLinkage, Twine("printf"), mod);
    func->setCallingConv(CallingConv::C);
    return func;
  }
  
  struct CS201Profiling : public FunctionPass {
    static char ID;
    LLVMContext *Context;
    CS201Profiling() : FunctionPass(ID) {}
    GlobalVariable *BasicBlockPrintfFormatStr = NULL;
    GlobalVariable *TitleFormatStr = NULL;

    Function *printf_func = NULL;

    Module * currentModule;

    //----------------------------------
    
    // map between basic block and its counter
    DenseMap<BasicBlock*, GlobalVariable*> counterMap;
    // map between edge and its counter
    DenseMap<vector<BasicBlock*>*, GlobalVariable*> edgeMap;
	// map between backEdges and loops
	std::map<std::vector<std::string>, std::vector<std::vector<std::string>>> loopMap;
	// loops
	std::vector<std::vector<std::string>> loops; 
 
    //-------------------------------------------------------------------------------------------
    bool doInitialization(Module &M) {
      errs() << "\n---------Starting CS201 Profiling---------\n\n";
      Context = &M.getContext();

      currentModule = &M;

      const char *finalPrintString = "%s: %d\n";
      Constant *format_const = ConstantDataArray::getString(*Context, finalPrintString);
      BasicBlockPrintfFormatStr = new GlobalVariable(
					M, 
					llvm::ArrayType::get(llvm::IntegerType::get(*Context, 8), 
					strlen(finalPrintString)+1), 
					true, 
					llvm::GlobalValue::PrivateLinkage, 
					format_const, 
					"BasicBlockPrintfFormatStr");


      const char *finalPrintString_1 = "%s\n";
      Constant *format_const_1 = ConstantDataArray::getString(*Context, finalPrintString_1);
      TitleFormatStr = new GlobalVariable(
					M, 
					llvm::ArrayType::get(llvm::IntegerType::get(*Context, 8), 
					strlen(finalPrintString_1)+1), 
					true, 
					llvm::GlobalValue::PrivateLinkage, 
					format_const_1, 
					"TitleFormatStr");

      printf_func = printf_prototype(*Context, &M);
 
      errs() << "Module: " << M.getName() << "\n";
 
      return true;
    }
 
    //------------------------------------------------------------------------------------------
    bool doFinalization(Module &M) {
      errs() << "\n-------Finished CS201 Profiling----------\n\n";
      return false;
    }
    
    //------------------------------------------------------------------------------------------
	bool runOnFunction(Function &F) override {
      	errs() << '\n' <<  "Function: " << F.getName() << '\n';

      	//------------------------------------------------------
      	int numBBs = -1; //for the first BB to be b0
      	for( Function::iterator BB = F.begin(), IE = F.end(); BB != IE; ++BB){
      		numBBs++;
        	string s = to_string(numBBs);
        	BB->setName('b' + s);
      	}
 
      	for(auto &BB: F) {
            // initialize the count of each basic block
		    counterMap[&BB] = new GlobalVariable(
									*currentModule,Type::getInt32Ty(*Context), 
									false, 
									GlobalValue::InternalLinkage, 
									ConstantInt::get(Type::getInt32Ty(*Context), 0), 
									"eachBlockCounter" );       
 		    runOnBasicBlock(BB);
      	}

		// loop finding
		std::map<std::string, std::vector<std::string>> predMap;
		std::map<std::string, std::vector<std::string>> doms = buildDominatorTree(F, numBBs, predMap);
		errs() << "\nDominatorSets:\n";
		printDoms(doms);
		std::vector<std::string>  backEdges = getBackEdges(F, doms);
		loops = findLoops(backEdges, predMap);
		errs() << "\nLoops:\n";
		printLoops();
		// map backEdges and loops
		loopMap[backEdges] = loops;	
	
		// edge profiling
		vector<vector<BasicBlock*>*> edges = findEdges(F);
		instrumentEdge(F, edges);

		for (auto &BB : F) {
			if(F.getName().equals("main") && isa<ReturnInst>(BB.getTerminator())) {
    	    	addFinalPrintf(
					BB, Context, 
					counterMap, edgeMap, 
					BasicBlockPrintfFormatStr, TitleFormatStr, 
					printf_func);
	    	}
		}
		return true; // since runOnBasicBlock has modified the program
    }
 
    //----------------------------------
    bool runOnBasicBlock(BasicBlock &BB) {
    	errs() << "\nBasicBlock:\n";
        errs() <<  BB.getName() << ":\n"; 

      	for (pred_iterator PI = pred_begin(&BB), E = pred_end(&BB); PI != E; ++PI){
			string name = (*PI)->getName();
			errs() << "preds %" << name << '\n';
      	}

		// Will insert the generated instructions BEFORE the first BB instruction
		IRBuilder<> IRB(BB.getFirstInsertionPt());  

 		Value *loadAddr = IRB.CreateLoad(counterMap[&BB]);
		Value *addAddr = IRB.CreateAdd(ConstantInt::get(Type::getInt32Ty(*Context), 1), loadAddr);
      	IRB.CreateStore(addAddr, counterMap[&BB]);
      

		// print out the instructions in each basic block
      	for (auto &I: BB)
        	errs() << I << "\n";
 
		return true;
    }


    //------------------------------------------------------------------------------
    // Find edge 
    
    string createEdgeName(BasicBlock* bb1, BasicBlock* bb2) {
		return string(bb1->getName()) + " -> " + string(bb2->getName());
    }

    vector<vector<BasicBlock*>*> findEdges(Function &F){
		vector<vector<BasicBlock*>*> edges;
		// Find edge by identifing successor of each BB
		errs() << "\n";
		for (Function::iterator BB = F.begin(), IE = F.end(); BB!= IE; ++BB){
        	TerminatorInst *TInst = BB->getTerminator();
			for (unsigned I = 0, NSucc = TInst->getNumSuccessors(); I < NSucc; ++I){
				BasicBlock *Succ = TInst->getSuccessor(I);
				vector<BasicBlock*>* edge = new vector<BasicBlock*>(2);
				(*edge)[0] = BB;
				(*edge)[1] = Succ;
				edges.push_back(edge);
				string edgeName = createEdgeName(BB, Succ);
				edgeMap[edge] = new GlobalVariable(
										*currentModule, 
										Type::getInt32Ty(*Context), 
										false, 
										GlobalValue::InternalLinkage, 
										ConstantInt::get(Type::getInt32Ty(*Context), 0), 
										"edgeCount");
			}
		}
		return edges;
    }


    void instrumentEdge(Function &F, vector<vector<BasicBlock*>*> edges){
		
		unsigned numEdges = edges.size();
		for (unsigned i = 0; i < numEdges; ++i) {
			vector<BasicBlock*>* edgeBB = edges[i];
			BasicBlock* bb1 = (*edgeBB)[0];
			BasicBlock* bb2 = (*edgeBB)[1];		
			string edgeName = createEdgeName(bb1, bb2);		
        	TerminatorInst *TInst = bb1->getTerminator();
    		BasicBlock* pseudoBB = BasicBlock::Create(*Context, " ", &F);

 			unsigned numSucc = TInst->getNumSuccessors();
            for (unsigned j = 0; j < numSucc; j++) {
            	if (TInst->getSuccessor(j) == bb2) {
                	TInst->setSuccessor(j, pseudoBB);
                    break;
                }
            }

			BranchInst::Create(bb2, pseudoBB);
			IRBuilder<> IRB(pseudoBB->getFirstInsertionPt());

			Value *loadAddr = IRB.CreateLoad(edgeMap[edgeBB]);

			Value *addAddr = IRB.CreateAdd(
									ConstantInt::get(Type::getInt32Ty(*Context), 1), 
									loadAddr);
			IRB.CreateStore(addAddr, edgeMap[edgeBB]); 
		}
    }

    // end of edge profiling


    //--------------------------------------------------------------------------------------------
    // Find dominator trees
	map<string, vector<string>> buildDominatorTree(Function &F, int total, map<string, vector<string>> &predMap){
    	map<string, vector<string>> doms;
		//initialize doms[bb] to include all bbs except the first bb, doms[b0] = b0
		vector<string> all_bb;
		for (Function::iterator bb = F.begin(), IE = F.end(); bb != IE; ++bb){
			all_bb = performUnion(all_bb, {bb->getName()});
		}
		int initialize_counter = 0;
		for (Function::iterator bb = F.begin(), IE = F.end(); bb != IE; ++bb){
			initialize_counter++;
		
			if (initialize_counter == 1)
				doms[bb->getName()] = {bb->getName()};
			else {
				doms[bb->getName()] = all_bb;
			}
		}
		//iterate to have correct dominator sets shrinked from initialization
       	bool cont = true;
       	while(cont){
		cont = false;
		for (Function::iterator bb = F.begin(), IE = F.end(); bb != IE; ++bb){
			int counter = 0;
			vector<string> v0 = {bb->getName()};
			vector<string> v1;
			vector<string> preds;
			for (pred_iterator PI = pred_begin(bb), E = pred_end(bb); PI != E; PI++){
				preds.push_back((*PI)->getName());
				counter++;
				string name = (*PI)->getName();
				if(counter == 1){
					v1 = doms[name];
				}
				else{
					v1 = intersection(v1, doms[name]);
				}
			
			}
			predMap[bb->getName()] = preds;
			v1 = performUnion(v0, v1);
			vector<string> oldv1 = doms[bb->getName()];
			if (v1.size() != oldv1.size()){
				cont = true;
			}
			doms[bb->getName()] = v1;
			}
		}
       	return doms;
    } 

	std::vector<std::vector<std::string>> findLoops(std::vector<std::string> backEdges,std::map<std::string, std::vector<std::string> > predMap)
    {
        std::vector<std::vector<std::string>> loops;
        for (std::vector<std::string>::iterator it = backEdges.begin() ; it != backEdges.end(); ++it)
        {
            std::string backEdge = *it;
            std::string start_be = backEdge.substr(0,2);
            std::string end_be = backEdge.substr(3,5);
            std::vector<std::string> this_loop; 
			this_loop.push_back(end_be);
            std::stack<std::string> st;
			st.push(start_be);
            while (!st.empty())
            {
                std::string ele = st.top();
                st.pop();
				bool isInLoop = false;
				for (std::vector<std::string>::iterator it = this_loop.begin() ; it != this_loop.end() ; ++it){
					if (*it == ele)
						isInLoop = true;
				}
				if (!(isInLoop))
					this_loop.push_back(ele);
                std::vector<std::string> pred = predMap[ele];
				isInLoop = false;
                for (std::vector<std::string>::iterator it_pred = pred.begin() ; it_pred != pred.end(); ++it_pred)
                {
					for (std::vector<std::string>::iterator it = this_loop.begin() ; it != this_loop.end() ; ++it){
					if (*it == *it_pred)
						isInLoop = true;
					}
					if (!(isInLoop))
						st.push(*it_pred);
                }
            }
            loops.push_back(this_loop);
        }
        return loops;
    }

		std::vector<std::string> getBackEdges(Function &F, std::map<std::string, std::vector<std::string> > doms){
			std::vector<std::string>  backEdges;
			for (Function::iterator bb = F.begin(), IE = F.end(); bb != IE; ++bb)
			{
				for (succ_iterator SI = succ_begin(bb), E = succ_end(bb); SI != E; ++SI)
				{
					bool backEdge = isBackEdge((*SI)->getName(), doms[bb->getName()]);
					if (backEdge)
					{
						std::string start = bb->getName();
						std::string end = (*SI)->getName();
						std::string be  = start +" "+ end;
						backEdges.push_back(be);
					}
				}
			}
			return backEdges;
		}

		bool isBackEdge(std::string succ, std::vector<std::string> dom)
		{
			for (std::vector<std::string>::iterator it = dom.begin() ; it != dom.end(); ++it)
			{
				std::string cur_element = *it;
				if (cur_element==succ)
				{
                return true;
				}
			}
			return false;
		}

		vector<string> intersection(vector<string> v1, vector<string> v2){
			vector<string> v3;
			sort(v1.begin(), v1.end());
			sort(v2.begin(), v2.end());
			set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v3));
			return v3;
		}

		vector<string> performUnion(vector<string> v1, vector<string> v2){
			vector<string> v3;
			sort(v1.begin(), v1.end());
			sort(v2.begin(), v2.end());
			set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v3));
			return v3;
		}
 
		void printDoms(map<string, vector<string>> doms){
			for(map<string, vector<string>>::iterator it=doms.begin(); it!=doms.end(); ++it){
				vector<string> vec = it->second;
				errs() << "DomSet[" << it->first << "] " << "=>";
				for(vector<string>::iterator it1 = vec.begin(); it1 != vec.end(); ++it1){
					errs() << ' ' << *it1 << ", ";
				}
				errs() << '\n';
			}	
		}

		void printLoops(void) {
			for (std::vector<std::vector<std::string>>::iterator lb=loops.begin(); lb!=loops.end(); ++lb){
				for (std::vector<std::string>::iterator vb=lb->begin(); vb!=lb->end(); ++vb) 
					errs() <<  *vb << ", ";
			}
		}

		string createLoopName(std::vector<std::vector<std::string>> loop){
			string loopName;
			int count = 0;
			for(std::vector<std::vector<std::string>>::iterator it = loop.begin(); it!=loop.end(); ++it){
				for(std::vector<std::string>::iterator its = it->begin(); its!= it->end(); ++its){
					if (count == 0){
						loopName = *its;
					}
					else{	
						loopName = loopName + " " + *its;
					}
					++count;
				}
			}
			return loopName;
		}
	// end of find dominator tree and loops

 
    /*----------------------------------
    ** Rest of this code is needed to: printf("%s: %d\n", string, count); 
	**		to the end of main, just BEFORE the return statement
	*/
    void addFinalPrintf(
			BasicBlock& BB, 
			LLVMContext *Context, 
			DenseMap<BasicBlock*, GlobalVariable*> counterMap, 
			DenseMap<vector<BasicBlock*>*, GlobalVariable*> edgeMap,
			GlobalVariable *var,
			GlobalVariable *var_2, 
			Function *printf_func) {
		
		IRBuilder<> builder(BB.getTerminator()); // Insert BEFORE the final statement
      	std::vector<Constant*> indices;
      	Constant *zero = Constant::getNullValue(IntegerType::getInt32Ty(*Context));
      	indices.push_back(zero);
      	indices.push_back(zero);
      	Constant *var_ref = ConstantExpr::getGetElementPtr(var, indices);
      	Constant *var_ref_2 = ConstantExpr::getGetElementPtr(var_2, indices);

	// Print out Basic Block Profiling
	const string blockTitle_1 = "\nBASIC BLOCK PROFILING";
	Value *blockTitleName_1 = builder.CreateGlobalStringPtr(*(new StringRef(&blockTitle_1[0])));
	CallInst *call = builder.CreateCall2(printf_func, var_ref_2, blockTitleName_1);
	call->setTailCall(false);

      	for (DenseMap<BasicBlock*, 
				GlobalVariable*>::iterator it = counterMap.begin(), 
				en = counterMap.end()
						; it != en; it++ ){
		
			BasicBlock* entryBB = it->first;
			GlobalVariable* c = it->second;
			//Value *blockCounter = builder.CreateLoad(counterMap[entryBB]);	
			Value *blockCounter = builder.CreateLoad(c);	
			Value *blockName = builder.CreateGlobalStringPtr(entryBB->getName());

        	CallInst *call = builder.CreateCall3(printf_func, var_ref, blockName, blockCounter);
      		call->setTailCall(false);
      	}

	// to print EDGE PROFILING
	const string blockTitle = "\nEDGE PROFILING";
	Value *blockTitleName = builder.CreateGlobalStringPtr(*(new StringRef(&blockTitle[0])));
	call = builder.CreateCall2(printf_func, var_ref_2, blockTitleName);
	call->setTailCall(false);

	// Print out Edge Profiling
      	for (DenseMap<vector<BasicBlock*>*, 
				GlobalVariable*>::iterator it = edgeMap.begin(), 
				en = edgeMap.end()
						; it != en; it++ ){
		
			vector<BasicBlock*>* edge = it->first;
			const string edgeNameString =  createEdgeName((*edge)[0], (*edge)[1]);


			GlobalVariable* c = it->second;

			Value *edgeCounter = builder.CreateLoad(c);	
			Value *edgeName = builder.CreateGlobalStringPtr(*(new StringRef(&edgeNameString[0])));

        	CallInst *call = builder.CreateCall3(printf_func, var_ref, edgeName, edgeCounter);
      		call->setTailCall(false);
      	}

	// to print LOOP PROFILING
	// don't know how to remove the number placeholder in the format string yet
	const string loopTitle = "\nLOOP PROFILING";
	Value *loopTitleName = builder.CreateGlobalStringPtr(*(new StringRef(&loopTitle[0])));
	call = builder.CreateCall2(printf_func, var_ref_2, loopTitleName);
	call->setTailCall(false);
	// iterate loopMap
	for(std::map<std::vector<std::string>, std::vector<std::vector<std::string>>>::iterator it = loopMap.begin(); it!=loopMap.end(); ++it){
		// for each backEdges
		std::vector<std::string> be = it->first;
		std::string be_start, be_end; 
		// to have begin node and end node of back edges
		for (std::vector<std::string>::iterator itv = be.begin();itv!=be.end();++itv){
			be_start = itv->substr(0,2);
			be_end = itv->substr(3,5);
		}
		for (DenseMap<vector<BasicBlock*>*, 
				GlobalVariable*>::iterator ite = edgeMap.begin(), 
				ene = edgeMap.end()
						; ite != ene; ite++ ){
		
			vector<BasicBlock*>* edge = ite->first;
			// check if backEdges matches an edge in edgeProfiling
			if ( (be_start == (*edge)[0]->getName()) && (be_end == (*edge)[1]->getName()) ){
				std::vector<std::vector<std::string>> it2 = it->second;
				const string loopNameString = createLoopName(it2);
				// get back edge count from edge profiling
				GlobalVariable* countLoop = ite->second;
				Value *loopCounter = builder.CreateLoad(countLoop);	
				Value *loopName = builder.CreateGlobalStringPtr(*(new StringRef(&loopNameString[0])));
				call = builder.CreateCall3(printf_func, var_ref,loopName,loopCounter);
				call->setTailCall(false);
			}
      	}
		
	}
    }
  };
}
 

char CS201Profiling::ID = 0;
static RegisterPass<CS201Profiling> X("pathProfiling", "CS201Profiling Pass", false, false);

