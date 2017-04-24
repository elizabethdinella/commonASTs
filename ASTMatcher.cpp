#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/ASTConsumer.h"
// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "clang/AST/RecursiveASTVisitor.h"
#include <vector>


using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;
using namespace std;


//Global list of files to include in our AST traversal
vector<string> includeList;

bool debugPrint = false;

bool isFlowControl(const Stmt* parent, const Decl* x){
	if(parent != NULL && strcmp(parent->getStmtClassName(), "ForStmt") == 0){
		auto childItr = parent->child_begin();
		int count = 0;
		while((childItr != parent->child_end()) && count <= 3){
			if((*childItr != 0) && childItr->getLocStart() == x->getLocStart()){
				return true;
			}
			count++;
			childItr++;
		}
	}

	return false;
}

bool isFlowControl(const Stmt* parent, const Stmt* x){
	if(parent != NULL && strcmp(parent->getStmtClassName(), "ForStmt") == 0){
		auto childItr = parent->child_begin();
		int count = 0;
		while((childItr != parent->child_end()) && count <= 3){
			if((*childItr != 0) && childItr->getLocStart() == x->getLocStart()){
				return true;
			}
			count++;
			childItr++;
		}
	}

	return false;
}

class ASTMatcherVisitor : public RecursiveASTVisitor<ASTMatcherVisitor> {
	public:
		explicit ASTMatcherVisitor(ASTContext *Context){
			this->Context = Context;
		}

		const Stmt* getStmtParent(const Stmt *s){
			const Stmt* ret = NULL;
			if(!s) {
				return ret;	
			}
			const ASTContext::DynTypedNodeList parents = Context->getParents(*s);
			if(parents.size() > 0){
				ret = parents[0].get<Stmt>();
			}
			return ret;
		}

		const Stmt* getStmtParent(const Decl *d){
			const Stmt* ret = NULL;
			if(!d) {
				return ret;	
			}
			const ASTContext::DynTypedNodeList parents = Context->getParents(*d);
			if(parents.size() > 0){
				ret = parents[0].get<Stmt>();
			}
			return ret;
		}


		const Decl* getDeclParent(const Decl* d){
			const Decl* ret = NULL;
			if(!d){
				return ret;
			}

			const ASTContext::DynTypedNodeList parents = Context->getParents(*d);
			if(parents.size() > 0){
				ret = parents[0].get<Decl>();
			}
			return ret;
		}

		const Decl* getDeclParent(const Stmt* s){
			const Decl* ret = NULL;
			if(!s){
				return ret;
			}
			const ASTContext::DynTypedNodeList parents = Context->getParents(*s);
			if(parents.size() > 0){
				ret = parents[0].get<Decl>();
			}
			return ret;
		}


		bool DeclHelper(Decl *D){
			const Stmt* parent = getStmtParent(D);
			const Stmt* parentsParent = getStmtParent(parent);

			if(isFlowControl(parentsParent, D)){
				return false;
			}


			if(parent != NULL && strcmp(parent->getStmtClassName(), "CXXCatchStmt") == 0){
				return true;					
			}


			SourceManager &sm = Context->getSourceManager();
			SourceLocation loc = D->getLocation();

			StringRef filenameRef = sm.getFilename(loc);
			string filename = filenameRef.str();	

			vector<string>::iterator fileItr = find(includeList.begin(), includeList.end(), filename);
			if(fileItr == includeList.end() && filename.size() != 0){
				return false;
			}else if(filename.size() == 0){
				return true;
			}


			string output = "";
			string node = D->getDeclKindName();
			int intLevel = getLevelDecl(D);	int intNextLevel = intLevel+1;	
			int intNextNextLevel = intLevel+2; int intPrevLevel = intLevel-1;
			string level; string nextLevel;
			string nextNextLevel; string prevLevel;
			stringstream ss; stringstream ss2; stringstream ss3; stringstream ss4;
			ss << intLevel;
			level = ss.str();
			ss2 << intNextLevel;
			nextLevel = ss2.str();
			ss3 << intPrevLevel;
			prevLevel = ss3.str();
			ss4 << intNextNextLevel;
			nextNextLevel = ss4.str();

			if(node == "Var"){
				output += "<variableDecl, " + prevLevel +  ">";
			}else if(node == "Function"){
				FunctionDecl* FD = (FunctionDecl*) D; 
				output += "<functionDef," + level +">";
				//add function name to the output
				output += "\n<name: " + FD->getNameInfo().getAsString()
					+ "," + nextLevel + ">";

			}else if(node == "CXXRecord"){
				const Decl* parent = getDeclParent(D);	
				if(parent && strcmp(parent->getDeclKindName(), "CXXRecord") != 0){
					CXXRecordDecl* CD = (CXXRecordDecl*) D;
					output += "<classDef," + level + ">";
					output += "\n<name: " + CD->getNameAsString() + "," + nextLevel + ">";
					output += "\n<bases," + nextLevel + ">";

					CXXRecordDecl::base_class_iterator basesItr =  CD->bases_begin();
					while(basesItr != CD->bases_end()){
						QualType qt = basesItr->getType();
						output +=  "\n<base: " +  qt.getBaseTypeIdentifier()->getName().str();
						output +=  "," + nextNextLevel + ">";
						basesItr++;
					}

					auto vBasesItr = CD->vbases_begin();
					while(vBasesItr != CD->vbases_end()){
						QualType qt = vBasesItr->getType();
						output +=  "\n<base: " +  qt.getBaseTypeIdentifier()->getName().str();
						output +=  "," + nextNextLevel + ">";
						vBasesItr++;
					}

				}
			}else if(node == "CXXConstructor"){
				CXXConstructorDecl* CD = (CXXConstructorDecl*) D;
				if(!CD->isImplicit()){
					output += "<functionDef," + level +">";
					//add function name to the output
					output += "\n<name: " + CD->getNameInfo().getAsString()
						+ "," + nextLevel + ">";
				}
			}else if(node == "CXXMethod"){
				CXXMethodDecl* CM = (CXXMethodDecl*) D;
				if(!CM->isImplicit()){
					output += "<functionDef," + level +">";
					//add function name to the output
					output += "\n<name: " + CM->getNameInfo().getAsString()
						+ "," + nextLevel + ">";
				}
			}else{

				if(debugPrint){
					output += "<";
					output += node;
					output += ">";
				}else{
					//	output += "<NULL>";
				}
			}

			if(output.size() != 0){
				cout << output << endl;
			}

			return true;
		}



		bool TraverseDecl(Decl *D) {

			bool continueTraversing = DeclHelper(D);
			if(continueTraversing){
				RecursiveASTVisitor<ASTMatcherVisitor>::TraverseDecl(D); // Forward to base class
			}

			return true;
		}


		void StmtHelper(Stmt *x){
			bool condition = false;
			bool isElse = false;
			if(x != NULL){
				string output = "";
				int intLevel = getLevelStmt(x);
				int intNextLevel = intLevel+1;
				string level;
				string nextLevel;
				stringstream ss;
				ss << intLevel;
				level = ss.str();
				stringstream ss2;
				ss2 << intNextLevel;
				nextLevel = ss2.str();

				const Stmt* parent = getStmtParent(x);
				if(isFlowControl(parent, x)){
					return;
				}

				if(parent != NULL && strcmp(parent->getStmtClassName(), "IfStmt") == 0){
					//suppress condition
					const Stmt* firstChild = NULL;

					auto children = parent->children();
					for(const Stmt* child : children){
						if(child != NULL){
							firstChild = child;
							break;
						}
					}

					if(firstChild != NULL  && x->getLocStart() == firstChild->getLocStart()){
						condition = true;
					}

					//find if else
					const IfStmt* ifstmt = (IfStmt*) parent;
					const Stmt* elseStmt = ifstmt->getElse();
					if(x == elseStmt){
						isElse = true;
					}

				}else if(parent != NULL && strcmp(parent->getStmtClassName(), "WhileStmt") == 0){
					const Stmt* firstChild = NULL;

					auto children = parent->children();
					for(const Stmt* child : children){
						if(child != NULL){
							firstChild = child;
							break;
						}
					}

					if(firstChild != NULL  && x->getLocStart() == firstChild->getLocStart()){
						condition = true;
					}

				}
				string node = x->getStmtClassName();
				//cout << node << endl;
				if(node == "ReturnStmt"){
					output += "<return";
				}else if(node == "ForStmt"){
					output += "<forLoop";
				}else if(node == "WhileStmt"){
					output += "<whileLoop";
				}else if(node == "IfStmt"){
					output += "<ifStatement";
				}else if(node == "CXXMemberCallExpr"){
					CXXMemberCallExpr* ce = (CXXMemberCallExpr*) x;
					Expr* obj = ce->getImplicitObjectArgument();
					
					CallExpr* expr = (CallExpr*) x;

					output += "<object: ";
					QualType qt = obj->getType();
					output += qt.getBaseTypeIdentifier()->getName().str();
					output += "; calling func: ";
					output += expr->getDirectCallee()->getNameInfo().getAsString();

				}else if(node == "CallExpr"){
					CallExpr* expr = (CallExpr*) x;
					output += "<calling func: ";
					output += expr->getDirectCallee()->getNameInfo().getAsString();
				}else if(node == "CXXConstructExpr"){

					CXXConstructExpr* ce = (CXXConstructExpr*) x;

					Decl* CD = ce->getConstructor();

					SourceManager &sm = Context->getSourceManager();
					SourceLocation loc = CD->getLocation();

					StringRef filenameRef = sm.getFilename(loc);
					string filename = filenameRef.str();	

					vector<string>::iterator fileItr = find(includeList.begin(), includeList.end(), filename);
					if(fileItr != includeList.end()){
						CXXMethodDecl* MD =  ce->getConstructor();
						output += "<calling func: ";
						output += MD->getNameInfo().getAsString();
					}


				}else if(node == "BinaryOperator"){
					BinaryOperator* binaryOp = (BinaryOperator*) x;
					//cout << binaryOp->getOpcodeStr().str() << endl;
					if(binaryOp->isAssignmentOp()){
						output += "<assignment";
					}else if(binaryOp->isComparisonOp()){
						output += "<comparison";
					}else{
						output += "<binaryOp";
					}
				}else if(node == "UnaryOperator"){
					UnaryOperator* uo = (UnaryOperator*) x;
					string op = uo->getOpcodeStr(uo->getOpcode()).str();
					if(op != "-"){
						output += "<unaryOp";
					}
				}else if(node == "CompoundAssignOperator"){
					output += "<augAssign";
				}else if(node == "CompoundStmt"){
					if(isElse){
						output += "<elseStatement";
					}else{
						output += "<compoundStmt";
					}
				}else if(node == "CXXThrowExpr"){
					output += "<raisingException";
				}else if(node == "CXXTryStmt"){
					output += "<try";
				}else if(node == "CXXCatchStmt"){
					output += "<except";
					/*}else if(node == "StringLiteral"){
					  if(parent->getStmtClassName() == "ImplicitCastExpr"){
					  output += "<assignment";
					  }*/
			}else if(node == "CXXOperatorCallExpr"){
				CXXOperatorCallExpr* ce = (CXXOperatorCallExpr*) x;
				if(ce->isAssignmentOp()){
					output += "<assignment";
				}
			}else{
				if(debugPrint){
					output += "<";
					output += node;
					output += ">";
				}else{
					//	output += "<NULL>";
				}

			}

			if(output.size() != 0 /*&& !condition*/){
				/*if(isElse){
				  output += ", " + nextLevel + ">";
				  }else{*/
				output += ", " + level + ">";
				//}
				//outputIndentation(getLevelStmt(x));
				cout << output << endl;
			}
			}

		}

		bool TraverseStmt(Stmt *x) {
			StmtHelper(x);
			RecursiveASTVisitor<ASTMatcherVisitor>::TraverseStmt(x);
			return true;
		}


		int getLevelDecl(const Decl *D, int level=0){
			if(D == NULL){
				return level;
			}
			const Stmt* stmt;

			const Decl* decl = getDeclParent(D);
			//TODO: FIX THIS FORMATTING
			if(decl == NULL){
				stmt = getStmtParent(D); 
				level = getLevelStmt(stmt, level);

			}

			return getLevelDecl(decl, level+1);
		}



		int getLevelStmt(const Stmt *S, int level=0){
			if(S == NULL){
				return level;
			}

			//const ASTContext::DynTypedNodeList parents = Context->getParents(*S);
			const Stmt* parent = getStmtParent(S);
			// parents[0].get<Stmt>();
			if(parent == NULL){

				const Decl* decl = getDeclParent(S);
				level = getLevelDecl(decl, level);
			}

			return getLevelStmt(parent, level+1);
		}


	private:
		ASTContext *Context;
};

//AST CONSUMER - an interface that provides actions on the AST
class astConsumer : public clang::ASTConsumer{
	public:
		explicit astConsumer(ASTContext *Context) : Visitor(Context) {}

		virtual void HandleTranslationUnit(clang::ASTContext &Context){
			// Traversing the translation unit decl via a RecursiveASTVisitor
			// will visit all nodes in the AST.
			Visitor.TraverseDecl(Context.getTranslationUnitDecl());
		}

	private:
		//Declare a RecursiveASTVisitor
		ASTMatcherVisitor Visitor;

};


//FRONTEND ACTION - allows excecution of actions as part of the compilation
class ASTMatcherAction : public clang::ASTFrontendAction {
	public:

		//create an astConsumer to perform actions on the AST
		virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
				clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
			return std::unique_ptr<clang::ASTConsumer>(
					new astConsumer(&Compiler.getASTContext()));
		}
};


int main(int argc, char** argv){
	includeList.push_back("input.cc");	

	for(int i=1; i<argc; i++){
		includeList.push_back(argv[i]);
	}


	if (argc > 1) {
		ifstream f(argv[1]);
		if(!f.good()){
			cerr << "can't open: " << argv[1] << endl;
		}
		stringstream buffer;
		buffer << f.rdbuf();
		cout << "<module,1>" << endl;
		cout << "<importing,2>" << endl;
		clang::tooling::runToolOnCode(new ASTMatcherAction, buffer.str());
	}

}

