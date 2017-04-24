#include <iostream>
#include <list>
#include <fstream>
#include <stdlib.h>

using namespace std;

//prototypes
bool isExpr(string);
bool isStmt(string);
class Stmt;
class CompoundStmt;
class Except;

bool printDebug = false;

string getIndentation(int level){
	string ret = "";
	for(int i=1; i<=level; i++){
		ret += "\t";
	}
	return ret;
}


class Token{
	public:
		string value;
		int level;
};

class ASTNode{
	public:
		virtual string getType() = 0;
		virtual list<ASTNode*> getChildren() = 0;
		virtual void printNode(int level){
			cout << getIndentation(level);
			cout << "-----------------" << endl;
			cout << getIndentation(level);
			cout << "- " << getType() << ": " << endl;
			cout << getIndentation(level);
			cout << "-----------------" << endl;

		}

		int level;

};


class Module : public ASTNode{
	public:
		string getType(){
			return "Module";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			list<ASTNode*>::iterator itr;
			for(itr=body.begin(); itr != body.end(); itr++){
				if((*itr) != NULL){
					children.push_back((ASTNode*)*itr);
				}
			}
			return children;
		}

		list<ASTNode*> body;
};

class Expr : public ASTNode{
	virtual string getType() = 0;
	virtual list<ASTNode*> getChildren() = 0;
};

class Stmt : public ASTNode{
	virtual string getType() = 0;
	virtual list<ASTNode*> getChildren() = 0;
};


class Identifier : public Expr{
	public:
		string getType(){
			return "Identifier";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			return children;
		}	

		void printNode(int level){
			cout << getIndentation(level);
			cout << "-----------------" << endl;
			cout << getIndentation(level);
			cout << "- Indentifier: " << endl;
			cout << getIndentation(level);
			cout << "- Name: " << name << endl;
			cout << getIndentation(level);
			cout << "-----------------" << endl;
		}	

		string name;
};

class FunctionDef : public Stmt{
	public:
		string getType(){
			return "FunctionDef";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			children.push_back(name);
			children.push_back((ASTNode*)compoundStmt);
			return children;	
		}

		Identifier* name;
		CompoundStmt* compoundStmt;
};

class VariableDecl: public Stmt{
	public:
		string getType(){
			return "VariableDecl";
		}	

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			if(right){
				children.push_back(right);
			}
			return children;
		}

		Expr* right;
};

class For : public Stmt{
	public:
		string getType(){
			return "For";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			children.push_back((ASTNode*)compoundStmt);
			return children;
		}

		CompoundStmt* compoundStmt;

};

class While: public Stmt{
	public:
		string getType(){
			return "While";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			if(test){
				children.push_back(test);
			}
			children.push_back((ASTNode*)compoundStmt);
			return children;
		}

		Expr* test;
		CompoundStmt* compoundStmt;
};

class If: public Stmt{
	public:
		string getType(){
			return "If";
		}


		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			if(test){
				children.push_back(test);
			}
			children.push_back((ASTNode*)body);
			if(orelse){
				children.push_back((ASTNode*)orelse);
			}
			return children;
		}


		Expr* test;
		CompoundStmt* body;
		Stmt* orelse;	
};

class Bases: public Stmt{
	public:
		string getType(){
			return "Bases";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			list<Identifier*>::iterator itr;
			for(itr=data.begin(); itr != data.end(); itr++){
				if(*itr){
					children.push_back(*itr);
				}
			}
			return children;
		}

		list<Identifier*> data;
};

class ClassDef : public Stmt{
	public:
		string getType(){
			return "ClassDef";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			children.push_back(name);

			if(bases){
				children.push_back(bases);
			}

			list<ASTNode*>::iterator itr2;
			for(itr2=body.begin(); itr2 != body.end(); itr2++){
				if(*itr2){
					children.push_back(*itr2);
				}	
			}
			return children;
		}	

		Identifier* name;
		Bases* bases;
		list<ASTNode*> body;
};



class Import: public Stmt{
	public: 
		string getType(){
			return "Import";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			list<Identifier*>::iterator itr;
			for(itr=names.begin(); itr != names.end(); itr++){
				if(*itr){
					children.push_back(*itr);
				}
			}
			return children;
		}


		list<Identifier*> names;
};

class CompoundStmt : public Stmt{
	public:
		string getType(){
			return "CompoundStmt";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			list<ASTNode*>::iterator itr;
			for(itr=body.begin(); itr != body.end(); itr++){
				if(*itr){
					children.push_back(*itr);
				}
			}
			return children;
		}


		list<ASTNode*> body;
};

class Return: public Stmt{
	public:
		string getType(){
			return "Return";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			if(value){
				children.push_back(value);
			}
			return children;
		}

		Expr* value;
};


class Assign: public Stmt{
	public:
		string getType(){
			return "Assign";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			list<Expr*>::iterator itr; 
			for(itr=targets.begin(); itr != targets.end(); itr++){
				if(*itr){
					children.push_back(*itr);
				}
			}
			if(value){
				children.push_back(value);
			}
			return children;
		}


		list<Expr*> targets;
		Expr* value;	
};

class AugAssign: public Stmt{
	public:
		string getType(){
			return "AugAssign";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			if(target){
				children.push_back(target);
			}
			if(value){
				children.push_back(value);
			}
			return children;
		}


		Expr* target;
		Expr* value;
};

class Raise: public Stmt{
	string getType(){
		return "Raise";
	}
	
	list<ASTNode*> getChildren(){
		list<ASTNode*> children;
		return children;
	}
};

class Exec: public Stmt{
	public:
		string getType(){
			return "Exec";	
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			return children;
		}
};

class Try: public Stmt{
	public:
		string getType(){
			return "Try";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			children.push_back(body);
			children.push_back((ASTNode*)except);
			return children;
		}

		CompoundStmt* body;
		Except* except;
};

class Except: public Stmt{
	public:
		string getType(){
			return "Except";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			children.push_back(body);
			return children;	
		}

		CompoundStmt* body;
};

class Call: public Expr{
	public:
		string getType(){
			return "Call";	
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			return children;
		}

		void printNode(int level){
			cout << getIndentation(level);
			cout << "-----------------" << endl;
			if(obj.size() > 0){
				cout << getIndentation(level);
				cout << "- Object: " << obj << endl;
			}
			cout << getIndentation(level);
			cout << "- Calling func: " << func << endl;
			cout << getIndentation(level);
			cout << "-----------------" << endl;
		}

		string func;
		string obj;
		int level;
};

class BinOp: public Expr{
	public:
		string getType(){
			return "BinOp";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			if(left){
				children.push_back(left);
			}
			if(right){
				children.push_back(right);
			}
			return children;
		}


		Expr* left;
		Expr* right;
};

class UnaryOp: public Expr{
	public:
		string getType(){
			return "UnaryOp";
		}


		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			if(operand){
				ASTNode* cast = (ASTNode*) operand;
				children.push_back(cast);
			}
			return children;
		}


		Expr* operand;		
};

class Comparison: public Expr{
	public:
		string getType(){
			return "Comparison";
		}

		list<ASTNode*> getChildren(){
			list<ASTNode*> children;
			if(left){
				children.push_back(left);		
			}

			list<Expr*>::iterator itr;
			for(itr=comparators.begin(); itr != comparators.end(); itr++){
				if(*itr){
					children.push_back(*itr);
				}
			}

			return children;
		}

		Expr* left;
		list<Expr*> comparators;
		int level;
};

class Parser{
	public:
		Parser(const string& filename): file(filename.c_str()) {} ;


		Module* parseModule(){
			getToken();
			Module* module = new Module();
			module->level = 1;
			while(getLookaheadToken()->value != "END"){
				if(isExpr(getLookaheadToken()->value)){
					module->body.push_back(parseExpr());
				}else if(isStmt(getLookaheadToken()->value)){
					module->body.push_back(parseStmt());
				}else{
					cerr << "PARSE ERROR: parsed token that is not an EXPR or a STMT" << endl;	
					exit(1);
				}
			}
			return module;
		}

		Stmt* parseStmt(){
			Token* t = getToken();
			string compVal("name:");
			if(printDebug){
				cout << "Parsing Stmt: " << t->value << endl;
			}
			if(t->value == "functionDef"){
				return (Stmt*) parseFunctionDef(t->level);
			}else if(t->value == "classDef"){
				return (Stmt*) parseClassDef(t->level);
			}else if(t->value == "compoundStmt"){
				return (Stmt*) parseCompoundStmt();
			}else if(t->value == "return"){
				return (Stmt*) parseReturn(t->level);
			}else if(t->value == "assignment"){
				return (Stmt*) parseAssign(t->level);
			}else if(t->value == "augAssign"){
				return (Stmt*) parseAugAssign(t->level);
			}else if(t->value == "forLoop"){
				return (Stmt*) parseFor(t->level);
			}else if(t->value == "whileLoop"){
				return (Stmt*) parseWhile(t->level);
			}else if(t->value == "ifStatement"){
				return (Stmt*) parseIf(t->level);
			}else if(t->value == "importing"){
				return (Stmt*) parseImport(t->level);	
			}else if(t->value == "exec"){
				return (Stmt*) parseExec();
			}else if(t->value == "raisingException"){
				return (Stmt*) parseRaise();
			}else if(t->value == "try"){
				return (Stmt*) parseTry();
			}else if(t->value == "except"){
				return (Stmt*) parseExcept();
			}else if(t->value == "variableDecl"){
				return (Stmt*) parseVariableDecl(t->level);
			}else if(t->value == "END"){
				return NULL;
			}else if(t->value.compare(0, compVal.length(), compVal) == 0){
				return (Stmt*) parseIdentifier();
			}else{
				cerr << "parseStmt -> PARSE ERROR: did not expect token: " << t->value << endl;
				exit(1);
			}

			return NULL;
		}

		Expr* parseExpr(){
			Token* t = getToken();
			string compVal("calling func");
			string objCompVal("object:");
			if(printDebug){
				cout << "Parsing Expr: " << t->value << endl;
			}
			if(t->value == "binaryOp"){
				return (Expr*) parseBinOp(t->level);
			}else if(t->value == "unaryOp"){
				return (Expr*) parseUnaryOp(t->level);
			}else if(t->value == "comparison"){
				return (Expr*) parseComparison(t->level);
			}else if(t->value.compare(0, compVal.length(), compVal) == 0){
				return (Expr*) parseCallingFunc(t->value, t->level);
			}else if(t->value.compare(0, objCompVal.length(), objCompVal) == 0){
				return (Expr*) parseObjectCallingFunc(t->value);	
			}else if(t->value == "END"){
				return NULL;
			}else{
				cerr << "parseExpr -> PARSE ERROR: did not expect token: " << t->value << endl;
				exit(1);
			}

			return NULL;
		}

		FunctionDef* parseFunctionDef(int level){
			FunctionDef* fd = new FunctionDef();			
			fd->level = level;
			fd->name = parseIdentifier();
			fd->compoundStmt = parseCompoundStmt();
			return fd;
		}

		Bases* parseBases(){
			Bases* b = new Bases();
			Token* t = getToken();
			if(t->value != "bases"){
				cerr << "PARSE ERROR: expected \"bases\" but recieved token: " << t->value << endl;
				exit(1);
			}

			while(getLookaheadToken()->level > t->level){
				b->data.push_back(parseIdentifier());	
			}

			return b;
		}


		ClassDef* parseClassDef(int level){
			ClassDef* cd = new ClassDef();
			cd->name = parseIdentifier();
			cd->bases = parseBases();
			cd->body = parseBody(level);	
			return cd;
		}

		Identifier* parseIdentifier(){
			Identifier* i = new Identifier();
			Token* t = getToken();	
			i->level = t->level;
			string val = t->value;
			size_t pos = t->value.find(":");
			if(pos == string::npos){
				cerr << "ERROR: illformated <name: > xml" << endl;
				exit(1);
			}

			i->name = t->value.substr(pos+1);
			return i;
		}

		Call* parseObjectCallingFunc(string val){
			Call* c = new Call();

			size_t pos = val.find(":");
			size_t semiPos = val.find(";");
			if(pos == string::npos || semiPos == string::npos){
				cerr << "ERROR: illformated <callingFunc: > xml" << endl;
				exit(1);
			}

			c->obj = val.substr(pos+2, (semiPos-(pos+2)));
			val = val.substr(pos+2);
			pos = val.find(":");
			if(pos == string::npos){
				cerr << "ERROR: illformated <callingFunc: > xml" << endl;
				exit(1);
			}


			c->func = val.substr(pos+2);
			return c;
		}


		Call* parseCallingFunc(string val, int level){
			Call* c = new Call();
			c->level = level;
			size_t pos = val.find(":");
			if(pos == string::npos){
				cerr << "ERROR: illformated <callingFunc: > xml" << endl;
				exit(1);
			}
			c->func = val.substr(pos+2);
			return c;
		}

		list<ASTNode*> parseBody(int level){
			list<ASTNode*> body;

			Token* lt = getLookaheadToken();
			while(lt->level > level){
				if(isExpr(lt->value)){
					body.push_back(parseExpr());
				}else if(isStmt(lt->value)){
					body.push_back(parseStmt());
				}else{
					cerr << "ERROR: Attempted to add value which is not an EXPR or STMT" << endl;
				}

				lt = getLookaheadToken();
			}
			return body;
		}

		CompoundStmt* parseCompoundStmt(){
			if(printDebug){
				cout << "parsing compoundStmt" << endl;
			}
			Token* t = getToken();
			if(t->value != "compoundStmt" && t->value != "elseStatement" /*&& t->value != "ifStatement"*/){
				cerr << "PARSE ERROR: expected compoundStmt token, but parsed: " << t->value << endl;
				exit(1);
			}

			CompoundStmt* cs = new CompoundStmt();
			cs->level = t->level;
			while(getLookaheadToken()->level > t->level /*&& getLookaheadToken()->value != "END"*/){
				if(isExpr(getLookaheadToken()->value)){
					cs->body.push_back(parseExpr());	
				}else if(isStmt(getLookaheadToken()->value)){
					cs->body.push_back(parseStmt());	
				}else{
					cerr << "ERROR: Attempted to add value which is not an EXPR or STMT" << endl;	
				}
			}	
			return cs;
		}


		Return* parseReturn(int level){
			Return* ret= new Return();
			ret->level = level;
			if(getLookaheadToken()->level > level){ 
				ret->value = parseExpr();	
			}else{
				ret->value = NULL;
			}
			return ret;
		}


		Assign* parseAssign(int level){
			Assign* assign = new Assign();
			assign->level = level;
			while(getLookaheadToken()->level > level /*&& getLookaheadToken()->value != "END"*/) {
				assign->targets.push_back(parseExpr());
			}
			return assign;
		}

		AugAssign* parseAugAssign(int level){
			AugAssign* augAssign = new AugAssign();
			augAssign->level = level;
			/*if(getLookaheadToken()->value != "END"){
			  augAssign->target = parseExpr();
			  }else{*/
			augAssign->target = NULL;
			//}

			/*if(getLookaheadToken()->value != "END"){
			  augAssign->value = parseExpr();
			  }else{*/
			augAssign->value = NULL;
			//}

			return augAssign;
		}


		For* parseFor(int level){
			For* f = new For();
			f->level = level;
			f->compoundStmt = parseCompoundStmt();
			return f;
		}

		While* parseWhile(int level){
			While* w = new While();
			w->level = level;
			if(getLookaheadToken()->level > level && getLookaheadToken()->value != "compoundStmt"){
				w->test = parseExpr();
			}
			w->compoundStmt = parseCompoundStmt();
			return w;
		}


		If* parseIf(int level){
			If* i = new If();
			i->level = level;

			if(getLookaheadToken()->level > level && getLookaheadToken()->value != "compoundStmt"){
				if(printDebug){
					cout << "parsing If's test: " << getLookaheadToken()->value << endl;
				}

				i->test = parseExpr();
			}

			if(printDebug){
				cout << "parsing If's compoundStmt: " << getLookaheadToken()->value << endl;
			}

			i->body = parseCompoundStmt();

			if(getLookaheadToken()->level > level && getLookaheadToken()->value == "elseStatement"){
				i->orelse = parseCompoundStmt();
			}else if(getLookaheadToken()->level > level && getLookaheadToken()->value == "ifSatement"){
				Token* t = getToken();
				i->orelse = parseIf(t->level);
			}else{
				i->orelse = NULL;
			}
			return i;
		}

		Exec* parseExec(){
			Exec* e = new Exec();
			return e;
		}

		Raise* parseRaise(){
			Raise* r = new Raise();
			return r;
		}

		Try* parseTry(){
			Try* t = new Try();
			t->body = parseCompoundStmt();
			t->except = parseExcept();		
			return t;
		}

		Except* parseExcept(){
			Token* t = getToken();
			if(t->value != "except"){
				cerr << "PARSE ERROR: expected token: \"except\" but recieved token: " << t->value << endl;
			}
			Except* e = new Except();
			e->body = parseCompoundStmt();
			return e;
		}

		VariableDecl* parseVariableDecl(int level){
			VariableDecl* vd = new VariableDecl();
			vd->level = level;

			if(getLookaheadToken()->level > level){
				vd->right = parseExpr();
			}

			return vd;
		}

		Import* parseImport(int level){
			Import* i = new Import();
			i->level = level;
			while(getLookaheadToken()->level > level /*&& getLookaheadToken()->value != "END"*/){
				i->names.push_back(parseIdentifier());
			}

			return i;
		}


		BinOp* parseBinOp(int level){
			BinOp* bo = new BinOp();
			bo->level = level;
			if(getLookaheadToken()->level > level /*&& getLookaheadToken()->value != "END"*/){
				bo->left = parseExpr();
			}else{
				bo->left = NULL;
			}

			if(getLookaheadToken()->level > level /*&& getLookaheadToken()->value != "END"*/){
				bo->right = parseExpr();
			}else{
				bo->right = NULL;
			}

			return bo;
		}

		Comparison* parseComparison(int level){
			Comparison* cp = new Comparison();

			cp->level = level;
			if(getLookaheadToken()->level > level){
				if(printDebug){
					cout << "parsing compare's left: " << getLookaheadToken()->value;
				}
				cp->left = parseExpr();
			}
			while(getLookaheadToken()->level > level){
				if(printDebug){
					cout << "parsing comparators " << getLookaheadToken()->value;
				}

				cp->comparators.push_back(parseExpr());
			}

			return cp;
		}

		UnaryOp* parseUnaryOp(int level){
			UnaryOp* uo = new UnaryOp();
			uo->level = level;
			if(getLookaheadToken()->level > level /*&& getLookaheadToken()->value != "END"*/){
				uo->operand = parseExpr();
			}else{
				uo->operand = NULL;
			}
			return uo;
		}


		Token* getToken(){
			Token* token = new Token();
			file >> ws;
			string word;
			getline(file,word);
			if(word.size() == 0){
				token->value = "END";
				token->level = -1;
				return token;
			}

			token->value = getWord(word);
			token->level = getLevel(word);
			return token;
		}

		Token* getLookaheadToken(){
			Token* token = new Token();
			string word;
			int len = file.tellg();
			file >> ws;
			getline(file,word);
			if(word.size() == 0){
				token->value = "END";
				token->level = -1;
				return token;
			}

			token->value = getWord(word);

			token->level = getLevel(word);
			file.seekg(len ,std::ios_base::beg);	
			return token;

		}

	private:
		ifstream file;

		string getWord(string word){
			size_t pos = word.find(",");	
			if(pos == string::npos){
				cout << "ERROR. Word: " << word << " not in format \",level\". Exiting" << endl;
				exit(1);
			}

			return word.substr(1,pos-1);
		}

		int getLevel(string word){

			size_t pos = word.find(",");
			if(pos ==  string::npos){
				cout << "ERROR. Word: " << word << " not in format \",level\". Exiting" << endl;

				exit(1);
			}

			string level = word.substr(pos+1, word.size()-1);
			return atoi(level.c_str());
		}
};


bool isExpr(string val){
	if(printDebug){
		cout << "checking if " << val << " is an expr" << endl;
	}

	string objCompVal("object:");
	string compVal("calling func");
	return val == "binaryOp" || val == "unaryOp" || val == "comparison"||
		(val.compare(0, compVal.length(), compVal) == 0) || val.compare(0, objCompVal.length(), objCompVal) == 0;
}

bool isStmt(string val){
	if(printDebug){
		cout << "checking if " << val << " is a stmt" << endl;
	}

	string compVal("name");
	return val == "functionDef" || val == "classDef" || val == "compoundStmt" || val == "return" || val == "assignment" || val == "augAssign" || val == "forLoop" || val == "whileLoop" || val == "ifStatement" || val == "importing" || val == "exec" || val == "variableDecl" || val == "try" || val == "except" || val == "raisingException" || val.compare(0, compVal.length(), compVal) == 0;
}


void printAST(ASTNode* node, int level=0){

	list<ASTNode*> children = node->getChildren();
	node->printNode(level);

	list<ASTNode*>::iterator itr;
	for(itr=children.begin(); itr!= children.end(); itr++){
		printAST(*itr, level+1);

	}

}

int main(int argc, char** argv){

	if(argc < 2){
		cerr << "ERROR: no input file specified" << endl;
		exit(1);
	}

	Parser parser(argv[1]);
	Module* m = parser.parseModule();
	printAST(m);
}
