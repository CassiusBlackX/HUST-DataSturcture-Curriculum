#include "SymbolTable.h"
SymbolTable::SymbolTable() {
    SymbolTable::initStringTranslate();
    SymbolTable::initTokenKindTranslate();
}
void SymbolTable::clear() {
    externVariables.clear();
    functions.clear();
    localVariables.clear();
}
void SymbolTable::addExternVariable(const std::string & name, const std::string & type) {
    if(stringTranslate.find(type)!=stringTranslate.end())
        externVariables[name] = stringTranslate.at(type);
}
void SymbolTable::addLocalVariable(const std::string & name, const std::string & type){
    localVariables[name] = stringTranslate.at(type);
}
void SymbolTable::addPreDefinition(const std::string & name,const token_kind kind){
    if(tokenKindTranslate.find(kind)!=tokenKindTranslate.end())
        preDefinition[name] = tokenKindTranslate.at(kind);
}
void SymbolTable::addFunction(const std::string & name, const std::string & returnType){ // 重载一个只知道返回值的版本
    token_kind kind = AUTO; // 先随便赋值一个绝对不可能的类型
    if(stringTranslate.find(returnType)!=stringTranslate.end()){
        kind = stringTranslate.at(returnType);
    }
    FunctionInfo info;
    if(functions.find(name)!=functions.end())
        info = functions.at(name);
    else  info = FunctionInfo(kind);
    functions[name] = info;
}
void SymbolTable::functionDeclared(const std::string & name){
    if(functions.find(name)!=functions.end()){
        functions.at(name).declared = true;
    }
}
void SymbolTable::addFunctionFormParameter(const std::string & funcName, const std::string & paramType){
    if(functions.find(funcName)!=functions.end()){
        if(stringTranslate.find(paramType)!=stringTranslate.end()){
            token_kind kind = stringTranslate.at(paramType);
            functions.at(funcName).parameterTypes.push_back(kind);
        }
    }
}
token_kind SymbolTable::constTranslate(int kind){
    return tokenKindTranslate.at(kind);
}
bool SymbolTable::checkFunctionFormParamType(const std::string & funcName, int index,const std::string & param){
    if(functions.find(funcName)!=functions.end()){
        if(isVariableDeclared(param)){
            token_kind paramType = AUTO; // 先随便赋一个不可能的
            if(isLocalVariableDeclared(param)){
                paramType = getLocalType(param);
            }
            else if(isExternVariableDeclared(param)){
                paramType = getExternType(param);
            }
            else{
                paramType = getPreDefinitionType(param);
            }
            if(*(functions.at(funcName).parameterTypes.begin()+index)==paramType){
                return true;
            }
        }

    }
    return false;
}
bool SymbolTable::checkFunctionDeCDef(const std::string & funcName,int index,const std::string & paramType){
    if(functions.find(funcName)!=functions.end() &&
       isFunctionDeclared(funcName) &&
       stringTranslate.find(paramType)!=stringTranslate.end()){
        token_kind kind = stringTranslate.at(paramType);
        if(*(functions.at(funcName).parameterTypes.begin()+index)==kind){
            return true;
        }
    }
    return false;
}
[[nodiscard]] bool SymbolTable::isVariableDeclared(const std::string & name) const{ // 保证在这个方法里面不会修改任何成员变量的值
    return externVariables.find(name) != externVariables.end() ||
    localVariables.find(name) != localVariables.end() ||
    preDefinition.find(name) != preDefinition.end();
}
[[nodiscard]] bool SymbolTable::isExternVariableDeclared(const std::string & name) const {
    return externVariables.find(name) != externVariables.end();
}
[[nodiscard]] bool SymbolTable::isLocalVariableDeclared(const std::string & name) const {
    return localVariables.find(name) != localVariables.end();
}
[[nodiscard]] bool SymbolTable::isPreDefinitionDeclared(const std::string & name) const {
    return preDefinition.find(name) != preDefinition.end();
}
bool SymbolTable::isFunctionDeclared(const std::string & name){
    if(functions.find(name)!=functions.end()){
        if(functions.at(name).declared){
            return true;
        }
    }
    return false;
}
[[nodiscard]] token_kind SymbolTable::getExternType(const std::string & name) const {
    return externVariables.at(name);
}
[[nodiscard]] token_kind SymbolTable::getLocalType(const std::string & name) const {
    return localVariables.at(name);
}
[[nodiscard]] token_kind SymbolTable::getPreDefinitionType(const std::string & name) const {
    return preDefinition.at(name);
}
[[nodiscard]] token_kind SymbolTable::getFunctionReturnType(const std::string & name) const {
    return functions.at(name).returnType;
}
void SymbolTable::initStringTranslate(){
    stringTranslate["char"] = CHAR;
    stringTranslate["int"] = INT;
    stringTranslate["float"] = FLOAT;
    stringTranslate["double"] = DOUBLE;
    stringTranslate["long"] = LONG;
    stringTranslate["short"] = SHORT;
    stringTranslate["signed"] = SIGNED;
    stringTranslate["unsigned"] = UNSIGNED;
    stringTranslate["void"] = VOID;
}
void SymbolTable::initTokenKindTranslate(){
    tokenKindTranslate[INT_CONST] = INT;
    tokenKindTranslate[UNSIGNED_CONST] = UNSIGNED;
    tokenKindTranslate[LONG_CONST] = LONG;
    tokenKindTranslate[UNSIGNED_LONG_CONST] = LONG;
    tokenKindTranslate[DOUBLE_CONST] = DOUBLE;
    tokenKindTranslate[FLOAT_CONST] = FLOAT;
    tokenKindTranslate[LONG_DOUBLE_CONST] = DOUBLE;
    tokenKindTranslate[CHAR_CONST] = CHAR;
    // 字符串不在考虑范围内
}














