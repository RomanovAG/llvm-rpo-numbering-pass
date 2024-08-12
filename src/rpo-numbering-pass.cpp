#include <functional>
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

void
VisitFunction(const Function &Func)
{
	/* if bool == true, node is open, otherwise it's closed but still visited since it's in the map */
	DenseMap<const BasicBlock *, bool> visited;
	TinyPtrVector<const BasicBlock *> order;
	DenseMap<const BasicBlock *, const BasicBlock *> back_edges;

	std::function<void(const BasicBlock *)>
	dfs = [&](const BasicBlock *BB) -> void
	{
		visited.insert(std::make_pair(BB, true));
		
		const Instruction *Terminator = BB->getTerminator();
		for (unsigned i = 0, n = Terminator->getNumSuccessors(); i < n; i++)
		{
			BasicBlock *Successor = Terminator->getSuccessor(i);

			const auto &visited_successor = visited.find(Successor);
			if (visited_successor == visited.end())
			{
				dfs(Successor);
			}
			else if (visited_successor->second == true)
			{
				/* if open node encountered then back edge found */
				back_edges.insert(std::make_pair(BB, Successor));
			}
		}

		order.push_back(BB);
		visited[BB] = false;
	};

	/* start from entry basic block */
	dfs( &(Func.getEntryBlock()) );

	outs() << "\nFunction name: " << Func.getName() << "\n";
	outs() << "\tIndex of Basic Block / Number of Instructions\n"
		  "\t     ┃ ┏━━━━━━━━━━━━━━━━━┛\n"
		  "\t     ᐯ ᐯ\n";
	outs() << "\tRPO: ";	/* First BB should be 0 */
	for (auto it = order.rbegin(); it != order.rend(); it++)
	{
		outs() << (*it)->getNumber() << "(" << (*it)->size() << ") ";
	}
	outs() << "\n";
	for (const auto &edge : back_edges)
	{
		outs()	<< "\tFound back edge: " 
			<< edge.first->getNumber() << "(" << edge.first->size() << ")" 
			<< " -> " 
			<< edge.second->getNumber() << "(" << edge.second->size() << ")" 
			<< "\n";
	}
}

struct RPONumberingPass : PassInfoMixin<RPONumberingPass> 
{
	PreservedAnalyses 
	run(Function &Function, FunctionAnalysisManager &AnalysisManager) 
	{
		VisitFunction(Function);
		return (PreservedAnalyses::all());
	}

	static bool 
	isRequired(void) 
	{ 
		return (true); 
	}
};
} /* namespace */

void
CallBackForPassBuilder(PassBuilder &PB)
{
	PB.registerPipelineParsingCallback(
		(	[](
			StringRef Name,
			FunctionPassManager &FPM,
			ArrayRef<PassBuilder::PipelineElement>
			) -> bool
			{
				if (Name == "rpo-numbering-pass")
				{
					FPM.addPass(RPONumberingPass());
					return (true);
				}
				else
				{
					return (false);
				}
			}
		)
	);
} /* CallBackForPassBuider */

PassPluginLibraryInfo 
getRPONumberingPassPluginInfo(void)
{
	uint32_t     APIversion =  LLVM_PLUGIN_API_VERSION;
	const char * PluginName =  "rpo-numbering-pass";
	const char * PluginVersion =  LLVM_VERSION_STRING;
    
	PassPluginLibraryInfo info = 
	{ 
		APIversion, 
		PluginName, 
		PluginVersion, 
		CallBackForPassBuilder
	};

	return (info);
} /* getRPONumberingPassPluginInfo */

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() 
{
	return (getRPONumberingPassPluginInfo());
} /* llvmGetPassPluginInfo */
