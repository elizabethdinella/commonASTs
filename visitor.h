class ASTNode;

class Visitor{
	public:
		virtual void visit(ASTNode* n);
		
		virtual int getFor() const = 0;
		virtual int getForbiddenFuncCall() const = 0;
}; 
