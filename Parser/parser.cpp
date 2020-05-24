#include <iostream>
#include <string>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>
#include<fstream>
#include <assert.h>
#include <memory>


// Parser


static int CurTok;
static int getNextToken() { return CurTok = arrayOfStruct[++x].id; }
static std::map<char, int> BinopPrecedence;
static int GetTokPrecedence() {
    if (!isascii(CurTok))
        return -1;
    int TokPrec = BinopPrecedence[CurTok];
    if (TokPrec <= 0)
        return -1;
    return TokPrec;
}
ExprAST *Error(const char *Str) { fprintf(stderr, "Error: %s\n", Str);return 0;}
PrototypeAST *ErrorP(const char *Str) { Error(Str); return 0; }
FunctionAST *ErrorF(const char *Str) { Error(Str); return 0; }

static ExprAST *ParseExpression();
static ExprAST *ParseIdentifierExpr() {
    std::string IdName = arrayOfStruct[x].value;
    getNextToken();

    if (CurTok != '(')
        return new VariableExprAST(IdName);
    getNextToken();
    std::vector<ExprAST*> Args;
    if (CurTok != ')') {
        while (true) {
            ExprAST *Arg = ParseExpression();
            if (!Arg) return 0;
            Args.push_back(Arg);
            if (CurTok == ')') break;
            if (CurTok != ',')
                return Error("Expected ')' or ',' in argument list");
            getNextToken();
        }
    }
    getNextToken();
    return new CallExprAST(IdName, Args);
}
static ExprAST *ParseNumberExpr() {
    ExprAST *Result = new NumberExprAST(arrayOfStruct[x].value);
    getNextToken();
    return Result;
}
static ExprAST *ParseParenExpr() {
    getNextToken();
    ExprAST *V = ParseExpression();
    if (!V) return 0;
    if (CurTok != ')')
        return Error("Expected ')'");
    getNextToken();
    return V;
}

static ExprAST *ParseIfExpr() {
    getNextToken();
    ExprAST *Stm = ParseExpression();
    if (!Stm) return 0;
    if (CurTok != tok_execute)
        return Error("Expected execute");
    getNextToken();
    ExprAST *Execute = ParseExpression();
    if(Execute == 0) return 0;
    if (CurTok != tok_else)
        return Error("Expected else");
    getNextToken();
    ExprAST *ElseStm = ParseExpression();
    if (!ElseStm) return 0;
    return new IfExprAST(Stm, Execute, ElseStm);
}
static ExprAST *ParseForExpr() {
    getNextToken();
    if (CurTok != tok_identifier)
        return Error("Expected identifier after for");
    std::string IdName = IdentifierStr;
    getNextToken();
    if (CurTok != tok_from)
        return Error("Expected from after for");
    getNextToken();
    ExprAST *From = ParseExpression();
    if (From == 0) return 0;
    if (CurTok != tok_to)
        return Error("Expected to after from intial value");
    getNextToken();
    ExprAST *To = ParseExpression();
    if (To == 0) return 0;
    ExprAST *Code = ParseExpression();
    if (Code == 0) return 0;
    return new ForExprAST(IdName, From, To, Code);
}

static ExprAST *ParseVarExpr() {
    getNextToken();
    std::vector<std::pair<std::string, ExprAST*> > VarNames;
    if (CurTok != tok_identifier)
        return Error("Expected identifier after variable");

    while (1) {
        std::string Name = arrayOfStruct[x].value;
        getNextToken();
        ExprAST *Init = 0;
        if (CurTok == '=') {
            getNextToken();
            Init = ParseExpression();
            if (Init == 0) return 0;
        }
        VarNames.push_back(std::make_pair(Name, Init));
        if (CurTok != ',') break;
        getNextToken();
        if (CurTok != tok_identifier)
            return Error("Expected identifier list after var");
    } return new VarExprAST(VarNames);
}
static ExprAST *ParsePrimary() {
    switch (CurTok) {
        default: return Error("Unknown - expecting an expression");
        case tok_identifier:
            return ParseIdentifierExpr();
        case tok_number:
            return ParseNumberExpr();
        case '(':
            return ParseParenExpr();
        case tok_if:
            return ParseIfExpr();
        case tok_for:
            return ParseForExpr();
        case tok_variable:
            return ParseVarExpr();
    }
}
static ExprAST *ParseUnary() {
    if (!isascii(CurTok) || CurTok == '(' || CurTok == ',')
        return ParsePrimary();

    int Opc = CurTok;
    getNextToken();
    if (ExprAST *Operand = ParseUnary())
        return new UnaryExprAST(Opc, Operand);
    return 0;
}

static ExprAST *ParseBinOpRHS(int ExprPrec, ExprAST *LHS) {
    while (1) {
        int TokPrec = GetTokPrecedence();
        if (TokPrec < ExprPrec)
            return LHS;
        int BinOp = CurTok;
        getNextToken();
        ExprAST *RHS = ParseUnary();
        if (!RHS) return 0;
        int NextPrec = GetTokPrecedence();
        if (TokPrec < NextPrec) {
            RHS = ParseBinOpRHS(TokPrec+1, RHS);
            if (RHS == 0) return 0;
        }
    LHS = new BinaryExprAST(BinOp, LHS, RHS);
    }
}
static ExprAST *ParseExpression() {
    ExprAST *LHS = ParseUnary();
    if (!LHS) return 0;

    return ParseBinOpRHS(0, LHS);
}
static PrototypeAST *ParsePrototype() {
    std::string FnName;

    unsigned Kind = 0;
    unsigned BinaryPrecedence = 30;
    switch (CurTok) {
        default:
            return ErrorP("Expected Name Of The Fucntion");
        case tok_identifier:
            FnName = IdentifierStr;
            Kind = 0;
            getNextToken();
            break;
        case tok_unary:
            getNextToken();
            if (!isascii(CurTok))
                return ErrorP("Expected Unary Operator");
            FnName = "unary";
            FnName += (char)CurTok;
            Kind = 1;
            getNextToken();
            break;
        case tok_binary:
            getNextToken();
            if (!isascii(CurTok))
                return ErrorP("Expected Binary Operator");
            FnName = "binary";
            FnName += (char)CurTok;
            Kind = 2;
            getNextToken();
            if (CurTok == tok_number) {
                if (NumVal < 1 || NumVal > 100)
                    return ErrorP("Out of range 1-100");
                BinaryPrecedence = (unsigned)NumVal;
                getNextToken();
            }
            break;
    }
    if (CurTok != '(')
        return ErrorP("Expected '(' in prototype");
    std::vector<std::string> ArgNames;
    while (getNextToken() == tok_identifier)
        ArgNames.push_back(IdentifierStr);
    if (CurTok != ')')
        return ErrorP("Expected ')' in prototype");
        getNextToken();
    if (Kind && ArgNames.size() != Kind)
        return ErrorP("Invalid number of operands for operator");
    return new PrototypeAST(FnName, ArgNames, Kind != 0, BinaryPrecedence);
}

static FunctionAST *ParseDefinition() {
    getNextToken();
    PrototypeAST *Proto = ParsePrototype();
    if (Proto == 0) return 0;
    if (ExprAST *E = ParseExpression())
        return new FunctionAST(Proto, E);
    return 0;
}
vector<ExprAST*> exparr;
static FunctionAST *ParseTopLevelExpr() {
    if (ExprAST *E = ParseExpression()) {
        auto Proto = new PrototypeAST("", std::vector<std::string>());
        auto func =new FunctionAST(Proto, E);
        exparr.push_back(func);
        return func;
    }
    return 0;
}
static PrototypeAST *ParseExtern() {
    getNextToken();
    return ParsePrototype();
}
//===----------------------------------------------------------------------===//
// Top-Level parsing
//===----------------------------------------------------------------------===//
static void HandleDefinition() {
    auto def = ParseDefinition();
    if (def) {
    } else {
        getNextToken();
    }
}
static void HandleExtern() {
    if (ParseExtern()) {
    } else {
        getNextToken();
    }
}
static void HandleTopLevelExpression() {
    if (ParseTopLevelExpr()) {
    } else {
        getNextToken();
    }
}

static void MainLoop() {
    while (true) {
        switch (CurTok) {
            case tok_eof:
                return;
            case ';':
                getNextToken();
                break;
            case tok_function:
                HandleDefinition();
                break;
            case tok_extern:
                HandleExtern();
                break;
            default:
                HandleTopLevelExpression();
                break;
        }
    }
}

void mostrestrictive(ExprAST* q,int level){
    auto e=dynamic_cast<BinaryExprAST*>(q);
    auto var = dynamic_cast<VariableExprAST*>(q);
    auto call = dynamic_cast<CallExprAST*>(q);
    auto number = dynamic_cast<NumberExprAST*>(q);
    auto func = dynamic_cast<FunctionAST *>(q);
    auto unary = dynamic_cast<UnaryExprAST *>(q);
    auto var2 = dynamic_cast<VarExprAST*>(q);
    auto if_fun = dynamic_cast<IfExprAST*>(q);
    auto for_fun = dynamic_cast<ForExprAST*>(q);
    for (int i = 0; i <level;i++){
        cout<<'\t';
    }

    if (e){
        cout<<"Operator ->  "<<e->Op<<endl; mostrestrictive(e->LHS,level+1); mostrestrictive(e->RHS,level+1);
    } else if(var)
    {
        cout<<"Identifier ->"<<var->Name<<endl;
    } else if(call)
    {
        cout<<"Method -> "<<call->Callee<<endl;
        for(int i=0;i<call->Args.size();i++){
            for (int j = 0; j <level;j++){
                cout<<'\t';
            }
            cout<<"Parameter ->"<<i<<":"<<endl; mostrestrictive(call->Args[i],level+1);
        }
    } else if(number){
        cout<<"Value -> "<<number->Val<<endl;
    }else if(func){ mostrestrictive(func->Body,level);
    } else if(unary){
        cout<<unary->Opcode; mostrestrictive(unary->Operand,level);
    }else if(var2){
        for(int j=0;j<var2->VarNames.size();j++){
            for (int j = 0; j <level;j++){
                cout<<'\t';
            }
            cout<<"Initialization of -> "<<var2->VarNames[j].first<<endl; mostrestrictive(var2->VarNames[j].second,level);        }
    }
    else if (if_fun){
        cout<<"Condition -> "<<endl; mostrestrictive(if_fun->Stm,level);
        cout<<"Else ->"<<endl; mostrestrictive(if_fun->ElseStm,level);
    }
    else if (for_fun){
        cout<<"Code: ->";  mostrestrictive(for_fun->Code,level);
        cout<<"From -> "; mostrestrictive(for_fun->From,level);
        cout<<"To - > "; mostrestrictive(for_fun->To,level); }
    else{ cout<<"null"<<endl;}
    level++;
}
void printfunc(ExprAST *a){
    int level=0; mostrestrictive(a,level);
}
int main()
{
    BinopPrecedence['='] = 5;
    BinopPrecedence['<'] = 10;
    BinopPrecedence['>'] = 10;
    BinopPrecedence['+'] = 20;
    BinopPrecedence['-'] = 20;
    BinopPrecedence['*'] = 40;  // highest.
    BinopPrecedence['/'] = 40;  // highest.

    int tok;
    while(!fin.eof()){
        tok=gettok();
        tmpStructTok.id= tok;
    arrayOfStruct.push_back(tmpStructTok);
    }
    tmpStructTok.id=tok_eof;
    tmpStructTok.value="EOF";
    arrayOfStruct.push_back(tmpStructTok);

    getNextToken();
    MainLoop();
    for(int i=0;i<exparr.size();i++) {
        printfunc(exparr[i]);
    }
    return 0;
}