//
// Created by 张奥林 on 2023/8/25.
//
#pragma once
#ifndef DSEA2_PARSER_H
#define DSEA2_PARSER_H
#include <queue>
#include <stack>
#include "lexer.h"
#include "profunction.h"
#include "SymbolTable.h"
using namespace std;
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
struct print{
    int indent;
    int linenum;
};
typedef int status;
extern int indent0; // 记录源代码的缩紧值
extern queue<print>printList; // 存储各个打印格式单元
extern int w; // 获取gettoken函数返回值即读入的单词种类编码
extern string kind; // 存取类型关键字
extern string tokenText0; // 存取变量名或函数名
extern SymbolTable symbolTable; // 用来存储所有已经声明了的变量名、函数名及其类型
status program(ifstream & fp, CTree & T);  // 语法单位<程序>的子程序
status externDefineList(ifstream &  fp, CTree &T); // 语法单位<外部定义序列>的子程序
status externDefinition(ifstream &  fp, CTree & T); // 语法单位<外部定义>的子程序
status externVariableDefinition(ifstream & fp, CTree & T); // 语法单位<外部变量定义>的子程序
status variableList(ifstream &  fp, CTree & T); // 语法单位<变量序列>的子程序
status variableList2(ifstream & fp, CTree & T);
status funcDef(ifstream &  fp,CTree & T); // 语法单位<函数定义>的子程序
status paramList(ifstream &  fp, CTree & T,int & index); // 语法单位<形参序列>的子程序
status formParDef(ifstream &  fp, CTree & T, int & index); // 语法单位<形参定义>的子程序
status compoundStatement(ifstream &  fp, CTree & T); // 语法单位<复合语句>的子程序
status localVariableList(ifstream &  fp, CTree & T); // 语法单位<局部变量定义序列>的子程序
status localVariableDefinition(ifstream &  fp, CTree & T); // 语法单位<局部变量定义>的子程序
status statementList(ifstream & fp, CTree & T); // 语法单位<语句序列>的子程序
status statement(ifstream &  fp, CTree & T); // 语法单位<语句>的子程序
status expression(ifstream &  fp, CTree & T, int endSymbol); // 语法单位<表达式>的子程序
char precede(const string & a, const string & b); // 比较a与b的优先级
status printTree(const string & data, int indent); // 打印缩进

#endif //DSEA2_PARSER_H
