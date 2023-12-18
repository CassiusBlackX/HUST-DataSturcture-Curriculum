//
// Created by 张奥林 on 2023/8/29.
//
#pragma once
#ifndef DSEA2_SYMBOLTABLE_H
#define DSEA2_SYMBOLTABLE_H
#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "lexer.h"
struct FunctionInfo{
    token_kind returnType;
    std::vector<token_kind> parameterTypes;
    bool declared;
    void initParameterV(){
        parameterTypes.reserve(2);
    }
    FunctionInfo(token_kind kind){
        returnType = kind;
        initParameterV();
        declared = false;
    }
    FunctionInfo () {}
};
class SymbolTable {
    // 为了功能的集中，实际上这个类不只是记录变量、函数
public:
    SymbolTable();
    void clear();
    void addExternVariable(const std::string & name, const std::string & type) ;
    void addLocalVariable(const std::string & name, const std::string & type);
    void addPreDefinition(const std::string & name,const token_kind kind);
    /*
    // 一个没有用的全能add
    void addFunction(const std::string & name, const std::string & returnType, const std::vector<std::string> & parameterTypes){
        FunctionInfo info;
        if(stringTranslate.find(returnType)!=stringTranslate.end()){
            info.returnType = stringTranslate.at(returnType);
        }
        for(const auto & paramType : parameterTypes){
            info.parameterTypes.push_back(stringTranslate.at(paramType));
        }
        functions[name] = info;
    }
     */
    void addFunction(const std::string & name, const std::string & returnType);
    void functionDeclared(const std::string & name);
    void addFunctionFormParameter(const std::string & funcName, const std::string & paramType);
    token_kind constTranslate(int kind);
    bool checkFunctionFormParamType(const std::string & funcName, int index,const std::string & param);
    bool checkFunctionDeCDef(const std::string & funcName,int index,const std::string & paramType);
    [[nodiscard]] bool isVariableDeclared(const std::string & name) const;
    [[nodiscard]] bool isExternVariableDeclared(const std::string & name) const ;
    [[nodiscard]] bool isLocalVariableDeclared(const std::string & name) const ;
    [[nodiscard]] bool isPreDefinitionDeclared(const std::string & name) const ;
    /*
     // 函数不一定需要被定义，但是一定要被声明过
    [[nodiscard]] bool isFunctionDefined(const std::string & name) const {
        return functions.find(name) != functions.end();
    }
     */
    bool isFunctionDeclared(const std::string & name);
    [[nodiscard]] token_kind getExternType(const std::string & name) const ;
    [[nodiscard]] token_kind getLocalType(const std::string & name) const ;
    [[nodiscard]] token_kind getPreDefinitionType(const std::string & name) const ;
    [[nodiscard]] token_kind getFunctionReturnType(const std::string & name) const ;
private:
    std::map<std::string, token_kind> externVariables; // 存储外部变量名和类型
    std::map<std::string, token_kind> localVariables; // 存储形参和局部变量
    std::map<std::string, token_kind> preDefinition; // 存储define的变量，
    std::map<std::string, FunctionInfo> functions; // 存储函数名和返回类型
    std::map<std::string,token_kind> stringTranslate;
    std::map<int,token_kind> tokenKindTranslate;
    void initStringTranslate();
    void initTokenKindTranslate();
};
#endif //DSEA2_SYMBOLTABLE_H
