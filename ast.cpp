#include "ast.h"
#include <iostream>
#include <sstream>
#include <set>
#include "asm.h"
#include <map>
const char * floatTemps[] = {"$f0",
                            "$f1",
                            "$f2",
                            "$f3",
                            "$f4",
                            "$f5",
                            "$f6",
                            "$f7",
                            "$f8",
                            "$f9",
                            "$f10",
                            "$f11",
                            "$f12",
                            "$f13",
                            "$f14",
                            "$f15",
                            "$f16",
                            "$f17",
                            "$f18",
                            "$f19",
                            "$f20",
                            "$f21",
                            "$f22",
                            "$f23",
                            "$f24",
                            "$f25",
                            "$f26",
                            "$f27",
                            "$f28",
                            "$f29",
                            "$f30",
                            "$f31"
                        };

#define FLOAT_TEMP_COUNT 32
set<string> intTempMap;
set<string> floatTempMap;
map<string, int> codeGenerationVars;
list<Expr*>::iterator exprs;
list<Statement*>:: iterator stmts;

extern Asm assemblyFile;

int globalStackPointer = 0;

string getFloatTemp(){
    for (int i = 0; i < FLOAT_TEMP_COUNT; i++)
    {
        if(floatTempMap.find(floatTemps[i]) == floatTempMap.end()){
            floatTempMap.insert(floatTemps[i]);
            return string(floatTemps[i]);
        }
    }
    cout<<"No more float registers!"<<endl;
    return "";
}

void releaseFloatTemp(string temp){
    floatTempMap.erase(temp);
}

int counter = 0;
string newLabel(string prefix){
    stringstream ss;
    ss << prefix <<"_"<<counter;
    counter++;
    return ss.str();
}

void FloatExpr::genCode(Code &code){
    string temp = getFloatTemp();
    code.place = temp;
    stringstream ss;
    ss << "li.s " << temp <<", "<< this->number <<endl;
    code.code = ss.str();
}

void SubExpr::genCode(Code &code){
    Code rightCode, leftCode;
    stringstream ss;
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    releaseFloatTemp(rightCode.place);
    releaseFloatTemp(leftCode.place);
    string result = getFloatTemp();
    ss << leftCode.code << endl << rightCode.code << endl << "sub.s " << result << ", " << leftCode.place << ", " << rightCode.place << endl;
    code.place = result;
    code.code = ss.str();
}

void DivExpr::genCode(Code &code){
    Code rightCode, leftCode;
    stringstream ss;
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    releaseFloatTemp(rightCode.place);
    releaseFloatTemp(leftCode.place);
    string result = getFloatTemp();
    ss << leftCode.code << endl << rightCode.code << endl << "div.s " << result << ", " << leftCode.place << ", " << rightCode.place << endl;
    code.place = result;
    code.code = ss.str();
}

void IdExpr::genCode(Code &code){
    if(floatTempMap.find(this->id) == floatTempMap.end()){
        stringstream ss;
        string temp = getFloatTemp();
        code.place = temp;
        ss << "l.s " << temp << ", " << codeGenerationVars[this->id] << "($sp)" << endl;
        code.code = ss.str();
    }
}

string ExprStatement::genCode(){
    Code code;
    this->expr->genCode(code);
    releaseFloatTemp(code.place);
    return code.code;
}

string IfStatement::genCode(){
    Code code;
    string endLabel = newLabel("endif");
    this->conditionalExpr->genCode(code);
    stringstream ss;
    ss << code.code << endl
    << "bc1f " << endLabel << endl;

    stmts = this->trueStatement.begin();

    while (stmts != this->trueStatement.end())
    {
        ss << (*stmts)->genCode() << endl;
        stmts++;
    }

    stmts = this->falseStatement.begin();
    while(stmts != this->falseStatement.end()){
        ss << (*stmts)->genCode() << endl;
        stmts++;
    }
    
    ss << endLabel << ":" << endl;
    releaseFloatTemp(code.place);

    return ss.str();
}

void MethodInvocationExpr::genCode(Code &code){
    
}

string AssignationStatement::genCode(){
    Code rightCode, assignationCode;
    stringstream ss;
    this->value->genCode(rightCode);
    ss << rightCode.code << endl;
    string nombre = this->id;
    //recorrer todas las expressions
    exprs = this->expressions.begin();

    while(exprs != this->expressions.end()){
        (*exprs)->genCode(assignationCode);
        ss << assignationCode.code << endl;
        exprs++;
        releaseFloatTemp(assignationCode.place);
    }
    codeGenerationVars[nombre] = globalStackPointer;
    ss << "s.s " << rightCode.place << ", " << codeGenerationVars[nombre] << "($sp) " << endl;
    globalStackPointer+=4;
    releaseFloatTemp(rightCode.place); 


    return ss.str();
}

void GteExpr::genCode(Code &code){
    Code rightCode, leftCode;
    stringstream ss;
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    ss << leftCode.code << endl << rightCode.code << endl;
    releaseFloatTemp(rightCode.place);
    releaseFloatTemp(leftCode.place);
    ss << "c.lt.s " << rightCode.place << ", " << leftCode.place << endl;
    
    code.code = ss.str();
}

void LteExpr::genCode(Code &code){
    Code rightCode, leftCode;
    stringstream ss;
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    ss << leftCode.code << endl << rightCode.code << endl;
    releaseFloatTemp(rightCode.place);
    releaseFloatTemp(leftCode.place);
    ss << "c.lt.s " << rightCode.place << ", " << leftCode.place << endl;

    code.code = ss.str();
}

void EqExpr::genCode(Code &code){
    Code rightCode, leftCode;
    stringstream ss;
    this->expr1->genCode(leftCode);
    this->expr2->genCode(rightCode);
    ss << leftCode.code << endl << rightCode.code << endl;
    releaseFloatTemp(rightCode.place);
    releaseFloatTemp(leftCode.place);
    ss << "c.eq.s " << rightCode.place << ", " << leftCode.place << endl;

    code.code = ss.str();
}

void ReadFloatExpr::genCode(Code &code){
    //no hacer
}

string PrintStatement::genCode(){
    Code code;
    stringstream asciiLabel;
    string label = newLabel("string");
    asciiLabel << label <<": .asciiz " << this->id << ""<<endl;
    assemblyFile.data += asciiLabel.str();

    exprs = this->expressions.begin();
    stringstream ss;
    while (exprs != this->expressions.end())
    {
        (*exprs)->genCode(code);
        ss << code.code <<endl;
        exprs++;
     ss << "mov.s $f12, "<< code.place<<endl
        << "li $v0, 2"<<endl
        << "syscall"<<endl;
        releaseFloatTemp(code.place);
    }

    return ss.str();
}

string ReturnStatement::genCode(){
    return "Return statement code generation\n";
}

string MethodDefinitionStatement::genCode(){
    return "Method definition code generation\n";
}