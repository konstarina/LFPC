// AST

namespace {
    class ExprAST {
    public:
        virtual ~ExprAST() {}
    };

    class NumberExprAST : public ExprAST {
    public:
        string Val;
    public:
        NumberExprAST(string Val) : Val(Val) {}
    };

    class VariableExprAST : public ExprAST {
    public:
        std::string Name;

    public:
        VariableExprAST(const std::string &Name) : Name(Name) {}
    };

    class UnaryExprAST : public ExprAST {
    public:
        char Opcode;
        ExprAST *Operand;
    public:
        UnaryExprAST(char opcode, ExprAST *operand)
                : Opcode(opcode), Operand(operand) {}

    };

    class BinaryExprAST : public ExprAST {
    public:char Op;
        ExprAST *LHS, *RHS;

    public:
        BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs)
                : Op(op), LHS(lhs), RHS(rhs) {}
    };

    class CallExprAST : public ExprAST {
    public:
        std::string Callee;
        std::vector<ExprAST*> Args;

    public:
        CallExprAST(const std::string &callee, std::vector<ExprAST*> &args)
                : Callee(callee), Args(args) {}
    };

    class IfExprAST : public ExprAST {
    public:
        ExprAST *Stm, *Execute, *ElseStm;
    public:
        IfExprAST(ExprAST *stm, ExprAST *execute, ExprAST *elseStm)
                : Stm(stm), Execute(execute), ElseStm(elseStm) {}

    };

    class ForExprAST : public ExprAST {
    public:
        std::string VarName;
        ExprAST *From, *To, *Code;
    public:
        ForExprAST(const std::string &varname, ExprAST *from, ExprAST *to,
                     ExprAST *code)
                : VarName(varname), From(from), To(to), Code(code) {}
    };

    class VarExprAST : public ExprAST {
    public:
    public:
        std::vector<std::pair<std::string, ExprAST*> > VarNames;
    public:
        VarExprAST(const std::vector<std::pair<std::string, ExprAST*> > &varnames
                   )
                : VarNames(varnames) {}
  };

    class PrototypeAST {
        std::string Name;
        std::vector<std::string> Args;
        bool isOperator;
        unsigned Precedence;
    public:
        PrototypeAST(const std::string &name, const std::vector<std::string> &args,
                     bool isoperator = false, unsigned prec = 0)
                : Name(name), Args(args), isOperator(isoperator), Precedence(prec) {}

        bool isUnaryOp() const { return isOperator && Args.size() == 1; }
        bool isBinaryOp() const { return isOperator && Args.size() == 2; }

        char getOperatorName() const {
            assert(isUnaryOp() || isBinaryOp());
            return Name[Name.size()-1];
        }
        unsigned getBinaryPrecedence() const { return Precedence; }
    };

    class FunctionAST : public ExprAST {
    public:
        PrototypeAST *Proto;
        ExprAST *Body;
    public:
        FunctionAST(PrototypeAST *proto, ExprAST *body)
                : Proto(proto), Body(body) {}
    };
}