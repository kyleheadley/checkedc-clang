#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace clang::tooling;

std::string LastFn;
std::map<std::string,std::set<std::string>> FnGraph;

void FindLoops(
    // std::map<std::string,std::set<std::string>> *FnGraph,
    std::vector<std::string> *Path,
    std::set<std::string> *Found,
    std::string Start) {
  if (Found->count(Start)) {
    return;
  }
  auto *Branches = &FnGraph[Start];
  for (auto itr = Branches->begin(); itr != Branches->end(); ++itr) {
    bool Loop = false;
    for (unsigned long i = 0; i < Path->size(); ++i) {
      if ( (*Path)[i] == *itr) {
        Loop = true;
      }
      if (Loop) {
        Found->insert((*Path)[i]);
      }
    }
    if (!Loop) {
      Path->push_back(*itr);
      FindLoops(Path,Found,*itr);
      Path->pop_back();
    }
  }
}

class InterviewASTVisitor
        : public RecursiveASTVisitor<InterviewASTVisitor> {
public:
    explicit InterviewASTVisitor(ASTContext *Context)
            : Context(Context) {}

    bool VisitFunctionDecl(FunctionDecl *Decl) {
      LastFn = Decl->getDeclName().getAsString();
      std::set<std::string> empty;
      FnGraph.insert({LastFn, empty});
      // llvm::errs() << Decl->getDeclName().getAsString() << "\n";
      return true;
    }

    bool VisitCallExpr(CallExpr *Call) {
      std::string CallName = Call->getDirectCallee()->getDeclName().getAsString();
      FnGraph[LastFn].insert(CallName);
      // llvm::errs() << "call: " << Call->getDirectCallee()->getDeclName().getAsString() << "\n";
      return true;
    }

private:
    ASTContext *Context;
};

class InterviewASTConsumer : public clang::ASTConsumer {
public:
    explicit InterviewASTConsumer(ASTContext *Context)
            : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());

        std::vector<std::string> Path;
        std::set<std::string> Found;
        for (auto itr = FnGraph.begin(); itr != FnGraph.end(); ++itr) {
          FindLoops(&Path,&Found,itr->first);
        }

        llvm::errs() << "Syntactic Recursive Functions: \n";
        for (auto i = Found.begin(); i != Found.end(); ++i) {
          llvm::errs() << *i << "\n";
        }

//        // Debug out
//        for (auto itr = FnGraph.begin(); itr != FnGraph.end(); ++itr) {
//          llvm::errs() << itr->first << "\n";
//          for (auto i = itr->second.begin(); i != itr->second.end(); ++i) {
//            llvm::errs() << "call: " << *i << "\n";
//          }
//        }
    }

private:
    InterviewASTVisitor Visitor;
};

class InterviewAction : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        return std::unique_ptr<clang::ASTConsumer>(
                new InterviewASTConsumer(&Compiler.getASTContext()));
    }
};

static llvm::cl::OptionCategory MyToolCategory("interview-template");
int main(int argc, const char **argv) {
    CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
    return Tool.run(newFrontendActionFactory<InterviewAction>().get());
}