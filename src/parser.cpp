#include "parser.h"
using namespace std;
int w; // 获得gettoken函数的返回值即读入的单词种类编码
string kind; // 存储类型关键字
string tokenText0;// 存储第一个函数名或者变量名
string funcName; // 专门用来在定义函数的时候存储函数的名字
SymbolTable symbolTable; // 用于存储所有已经生命力的变量名、函数名及其类型
int indent0 = 0; // 初始化缩进
queue<print> printList; //用于方便打印缩进
//  分析对于没有声明过的变量就进行操作的问题--> 目前想法： 不考虑同名变量，也不考虑变量作用域，只要声明了就用
status program(ifstream & fp, CTree & T){
    CTree c;
    struct print elem = {indent0,line_num};
    printList.push(elem);
    w = getToken(fp);
    if(!externDefineList(fp, c)) return ERROR; // 调用外部定义序列函数
    T = CTree(1,0);
    T.nodes[0] = CTNode("program",0);
    insertChild(T, T.r, 1, c); // 将c子树插入到语法树中
    symbolTable.clear();
    return OK;
}
status externDefineList(ifstream & fp, CTree &T){
    CTree c;
    status flag;
    if(w==EOF) return INFEASIBLE;
    T = CTree(1,0);
    T.nodes[0] = CTNode("extern definition list",0);
    if(!externDefinition(fp, c)) return ERROR;
    insertChild(T, T.r, 1, c); // 处理一个外部定义，得到一棵子树，作为根的第一棵子树
    flag = externDefineList(fp, c);
    if(flag==OK) insertChild(T, T.r, 2, c); // 得到的子树作为第二棵子树
    if(flag==ERROR) return ERROR;
    return OK;
}
status externDefinition(ifstream & fp, CTree & T){
    status flag;
    if(w!=INT && w!=LONG && w!=SHORT && w!=SIGNED && w!=UNSIGNED && w!=FLOAT && w!=DOUBLE && w!=CHAR && w!=VOID ) return ERROR;
    kind = token_text; // 保存类型关键字
    w = getToken(fp);
    if(w != IDENTIFIER) return ERROR;
    tokenText0 = token_text; // 保存第一个变量名或函数名到tokenText0
    w = getToken(fp);
    if(w!=LS){ // 说明不是一个方程，而是一个外部变量
        flag = externVariableDefinition(fp, T); // 调用外部变量定义子程序
    }
    else {
        symbolTable.addFunction(tokenText0,kind);
        flag = funcDef(fp,T); // 调用函数定义子程序
    }
    if(!flag) return ERROR;
    return OK;
}
status externVariableDefinition(ifstream & fp, CTree & T){
    CTree p;
    T = CTree(1,0);
    T.nodes[0] = CTNode("extern variable definition:",1);
    CTree c = CTree(1,0); // 生成外部变量类型节点
    c.nodes[0] = CTNode("kind: "+kind,1);
    if(!insertChild(T, T.r, 1, c)) return ERROR; // c作为T的第一个孩子
    if(!variableList(fp, p)) return ERROR;
    if(!insertChild(T, T.r, 2, p)) return ERROR; // p作为T的第二个孩子
    return OK;
}
status variableList(ifstream & fp, CTree & T){
    CTree c; // c树用来构建此变量节点
    CTree t; // t树用来构建可能存在的下一变量节点
    T = CTree(1,0);// 生成变量序列节点
    T.nodes[0] = CTNode("variable sequence",0);
    if(w==LM){ // 区分是一个数组还是一个单纯的变量
        tokenText0 += "["; // 只考虑一维数组的情况
        if((w= getToken(fp)) == INT_CONST){
            tokenText0 += token_text;
            if((w= getToken(fp)) == RM){
                // 成功识别到了一维数组
                tokenText0 += "]";
                c = CTree(1,0); // 创建数组节点
                c.nodes[0] = CTNode("Array:"+tokenText0,1);// 初始时，tokenText0保存了第一个变量名
                w = getToken(fp);
            }else return ERROR;
        }
        else return ERROR;
    }
    else{  // 说明不是一个数组，而是一个单纯的变量
        c = CTree(1,0); // 创建变量节点
        c.nodes[0] = CTNode("value:"+tokenText0,1);
        // -+-+-+-+-+-+-+-
        symbolTable.addExternVariable(tokenText0, kind);
        // -+-+-+-+-+-+-+-
    }
    if(w==ASSIGN){
        c.nodes[0].data += "=";
        w = getToken(fp);
        status flag = expression(fp,t,SEMI_COLUMN);
        if(!flag) return ERROR;
        if(!insertChild(c,c.r,1,t)) return ERROR;
    }
    if(!insertChild(T, T.r, 1, c)) return ERROR; // 识别变量的节点作为T的第一个孩子
//    if(w == SEMI_COLUMN){
    // 自己凭感觉觉得的
    if(w == SEMI_COLUMN || w==POUND){ // w==POUND 说明w是经过了expression函数出来的
        w = getToken(fp);
        return OK;
    }
    w = getToken(fp);
    if(w != IDENTIFIER) return ERROR; // 如果w不是标识符则报错，反之则后面还有第二个变量
    tokenText0 = token_text;
    w = getToken(fp);
    if(!variableList(fp, t))return ERROR;
    if(!insertChild(T, T.r, 2, t))return ERROR;
    return OK;
}
status funcDef(ifstream & fp,CTree & T){
    CTree c; // c节点为函数返回值类型
    CTree p; // p节点为函数名节点
    CTree q; // q节点为参数序列节点
    CTree f; // f节点函数体节点
    CTree s; // s为可能的复合语句节点

    T = CTree(1,0); // 生成函数定义节点T
    T.nodes[0] = CTNode("function definition:",1);
    c = CTree(1,0); // 生成函数返回值类型节点c
    c.nodes[0] = CTNode("stringTranslate:"+kind,1);
    if(!insertChild(T, T.r, 1, c)) return ERROR;
    w = getToken(fp);
    // 函数括号内可能无参数，或者是void，可能是参数序列，其它情况则报错
    if(w!=RS && w!=VOID && w!=INT && w!=LONG && w!=SHORT && w!=SIGNED && w!=UNSIGNED && w!=FLOAT && w!=DOUBLE && w!=CHAR){
        return ERROR;
    }
    p = CTree(1,0); // 生成函数名节点
    p.nodes[0] = CTNode("function name:"+tokenText0,1);
    funcName = tokenText0;
    if(w==RS || w==VOID){ // 判断void情况或者参数序列情况
        if(w==VOID){
            w = getToken(fp);
            if(w!=RS) return ERROR;
        }
    }else{
        int index = 0;
        if(!paramList(fp, q,index)) return ERROR;
        if(!insertChild(p, p.r, 1, q)) return ERROR;
    }
    if(!insertChild(T, T.r, 2, p)) return ERROR;
    w = getToken(fp);
    if(w != SEMI_COLUMN && w != LL) return ERROR;
    f = CTree(1,0); // 生成函数体节点
    f.nodes[0] = CTNode("function body: ",1);
    if(w==LL){ // 存在函数体则判断复合句，说明是函数定义
        if(!compoundStatement(fp, s)) return ERROR;
        if(!insertChild(f, f.r, 1, s)) return ERROR;
        if(!insertChild(T, T.r, 3, f)) return ERROR;
    }
    //如果是函数声明则直接返回OK
    // 自己的感觉，如果是函数声明，那么就有可能接下来仍然是函数声明，所以这里应当再使用getToken一次才对
    else{
        w = getToken(fp);
    }
    symbolTable.functionDeclared(funcName);
    return OK;
}
status paramList(ifstream & fp, CTree & T,int & index){
    CTree c; // c用于生成形參子树
    CTree p; // p用于生成可能出现的下一个形參序列
    T = CTree(1,0);
    T.nodes[0] = CTNode("parameter sequence:",0);
    if(!formParDef(fp, c,index)) return ERROR;
    if(!insertChild(T, T.r, 1, c)) return ERROR;
    w = getToken(fp);
    if(w!=RS && w!=COMMA) return ERROR;
    if(w==COMMA){
        w = getToken(fp);
        if(!paramList(fp, p,index)) return ERROR;
        insertChild(T, T.r, 2, p);
    }
    return OK;
}
status formParDef(ifstream & fp, CTree & T,int & index){
    // w此前已经读取了第一个形參类型
    CTree c; // 用于生成形參类型节点
    CTree p; // 用于生成形參变量节点
    T = CTree(1,0); // 生成形參节点
    T.nodes[0] = CTNode("parameter:",1);
    if(w!=INT && w!=LONG && w!=SHORT && w!= SIGNED && w!=UNSIGNED && w!=FLOAT && w!=DOUBLE && w!=CHAR) return ERROR;
    c = CTree(1,0); // 生成形參类节点
    c.nodes[0] = CTNode("kind:"+token_text,1);
    string formParameterType = token_text;
    insertChild(T, T.r, 1, c);
    w = getToken(fp);
    if(w != IDENTIFIER) return ERROR;
    if(symbolTable.isFunctionDeclared(funcName)){
        // 这个函数已经被声明过了，那么现在就是要定义这个函数了
        if(!symbolTable.checkFunctionDeCDef(funcName,index,formParameterType)){
            // 函数定义时候的变量类型与函数声明的类型不一致，报错
            return ERROR;
        }
    }
    else{
        symbolTable.addFunctionFormParameter(funcName,formParameterType);
    }
    symbolTable.addLocalVariable(token_text, formParameterType); // 不管声明没有声明过，都需要添加到局部变量中
    p = CTree(1,0); // 生成形參变量节点
    p.nodes[0] = CTNode("value:"+token_text,1);
    insertChild(T, T.r, 2, p);
    index++;
    return OK;
}
status compoundStatement(ifstream & fp, CTree & T){
    CTree c; // c用于生成局部变量定义子树
    CTree p; // p用于生成语句序列子树
    status flag;
    T = CTree(1,0);
    T.nodes[0] = CTNode("compound sentence:",1);
    // 注意其中局部变量说明和语句序列都可以为空
    w = getToken(fp);
    print elem = {++indent0,line_num};
    printList.push(elem); // 添加缩进量
    if(w==INT || w==LONG || w==SHORT || w==SIGNED || w==UNSIGNED || w==FLOAT || w==CHAR){
        if(!localVariableList(fp, c)) return ERROR;
        if(!insertChild(T, T.r, 1, c)) return ERROR;
        flag = statementList(fp, p);
        if(!flag) return ERROR;
        if(flag==OK){
            if(!insertChild(T, T.r, 2, p)) return ERROR;
        }
    }else{
        flag = statementList(fp, p);
        if(!flag) return ERROR;
        if(flag==OK){
            if(!insertChild(T, T.r, 1, p)) return ERROR;
        }
    }
    elem = {--indent0,line_num};
    printList.push(elem);
    if(w!=RL) return ERROR;
    w = getToken(fp);
    return OK;
}
status localVariableList(ifstream & fp, CTree & T){
    CTree c; // c生成局部变量定义子树
    CTree p; // p生成可能存在的下一个局部变量序列子树
    status flag;
    if(w!=INT && w!= LONG && w!=SHORT && w!=SIGNED && w!=UNSIGNED && w!=FLOAT && w!=DOUBLE && w!=CHAR) return INFEASIBLE;
    // 读到的后继单词不为类型说明符的时候，变量定义序列结束
    T = CTree(1,0);// 生成局部变量定义序列节点
    T.nodes[0] = CTNode("local variable definition sequence:",0);
    if(!localVariableDefinition(fp, c)) return ERROR;
    if(!insertChild(T, T.r, 1, c)) return ERROR;
    flag = localVariableList(fp, p);
    if(flag==OK) insertChild(T, T.r, 2, p);
    if(!flag) return ERROR;
    return OK;
}
status localVariableDefinition(ifstream & fp, CTree & T){
    CTree c; // c生成局部变量类型节点
    CTree p; // p生成变量序列子树
    if(w!=INT && w!=LONG && w!=SHORT && w!=SIGNED && w!=UNSIGNED && w!=FLOAT && w!=DOUBLE && w!=CHAR) return ERROR;
    T = CTree(1,0); // 生成局部变量定义节点
    T.nodes[0] = CTNode("local variable definition:",1);
    c = CTree(1,0); // 生成局部变量类型节点
    kind = token_text;
    c.nodes[0] = CTNode("kind:"+token_text,1);
    if(!insertChild(T, T.r, 1, c)) return ERROR;
    w = getToken(fp);
    if(w != IDENTIFIER) return ERROR;
    tokenText0 = token_text;
    w = getToken(fp);
    if(!variableList2(fp, p)) return ERROR;
    if(!insertChild(T, T.r, 2, p)) return ERROR;
    return OK;
}
status variableList2(ifstream & fp, CTree & T){
    CTree c; // c树用来构建此变量节点
    CTree t; // t树用来构建可能存在的下一变量节点
    T = CTree(1,0);// 生成变量序列节点
    T.nodes[0] = CTNode("variable sequence",0);
    if(w==LM){ // 区分是一个数组还是一个单纯的变量
        tokenText0 += "["; // 只考虑一维数组的情况
        if((w= getToken(fp)) == INT_CONST){
            tokenText0 += token_text;
            if((w= getToken(fp)) == RM){
                // 成功识别到了一维数组
                tokenText0 += "]";
                c = CTree(1,0); // 创建数组节点
                c.nodes[0] = CTNode("Array:"+tokenText0,1);// 初始时，tokenText0保存了第一个变量名
                w = getToken(fp);
            }else return ERROR;
        }
        else return ERROR;
    }
    else{  // 说明不是一个数组，而是一个单纯的变量
        c = CTree(1,0); // 创建变量节点
        c.nodes[0] = CTNode("value:"+tokenText0,1);
        // -+-+-+-+-+-+-+-
        symbolTable.addLocalVariable(tokenText0, kind);
        // -+-+-+-+-+-+-+-
    }
    if(w==ASSIGN){
        c.nodes[0].data += "=";
        w = getToken(fp);
        status flag = expression(fp,t,SEMI_COLUMN);
        if(!flag) return ERROR;
        if(!insertChild(c,c.r,1,t)) return ERROR;
    }
    if(!insertChild(T, T.r, 1, c)) return ERROR; // 识别变量的节点作为T的第一个孩子
//    if(w == SEMI_COLUMN){
    // 自己凭感觉觉得的
    if(w == SEMI_COLUMN || w==POUND){ // w==POUND 说明w是经过了expression函数出来的
        w = getToken(fp);
        return OK;
    }
    w = getToken(fp);
    if(w != IDENTIFIER) return ERROR; // 如果w不是标识符则报错，反之则后面还有第二个变量
    tokenText0 = token_text;
    w = getToken(fp);
    if(!variableList(fp, t))return ERROR;
    if(!insertChild(T, T.r, 2, t))return ERROR;
    return OK;
}
status statementList(ifstream & fp, CTree & T){
    CTree c; // c生成语句树
    CTree p; // p生成可能出现的语句序列树
    status flag;
    flag = statement(fp, c);
    if(flag==INFEASIBLE) return INFEASIBLE;
    if(!flag)return ERROR;
    else{
        T = CTree(1,0); // 生成语句序列节点
        T.nodes[0] = CTNode("sentence sequence:",0);
        insertChild(T, T.r, 1, c);
        flag = statementList(fp, p);
        if(!flag) return ERROR;
        if(flag==OK){
            if(!insertChild(T, T.r, 2, p)) return ERROR;
        }
    }
    return OK;
}
status statement(ifstream & fp, CTree & T){
    CTree c, p, q, k;
    print elem;
    if(w==IF){ // 分析条件语句，p用于生成表达式树，q用于生成if模块子句树，k用于生成else模块子句树
        w = getToken(fp);
        if(w!=LS) return ERROR;
        w = getToken(fp);
        if(w==RS) return ERROR;
        if(!expression(fp, p, RS)) return ERROR;
        c = CTree(1,0);
        c.nodes[0] = CTNode("condition:",1);
        insertChild(c, c.r, 1, p);
        w = getToken(fp);
        if(w==LL){
            if(!compoundStatement(fp, q)) return ERROR;
        }
        else{
            elem = {++indent0,line_num};
            printList.push(elem);
            if(!statement(fp, q)) return ERROR;
            elem = {--indent0,line_num};
            printList.push(elem);
        }
        p = CTree(1,0);
        p.nodes[0] = CTNode("IF clause:",1);
        insertChild(p, p.r, 1, q);
        if(w==ELSE){
            w = getToken(fp);
            if(w==LL){
                if(!compoundStatement(fp, k)) return ERROR;
            }else{
                elem = {++indent0,line_num};
                printList.push(elem);
                if(!statement(fp, k)) return ERROR;
                elem = {--indent0,line_num};
                printList.push(elem);
            }
            q = CTree(1,0); //生成else语句子树
            q.nodes[0] = CTNode("ELSE clause:",1);
            insertChild(q, q.r, 1, k);
            T = CTree(1,0);
            T.nodes[0] = CTNode("if-else clause:",1);
            insertChild(T, T.r, 1, c);
            insertChild(T, T.r, 2, p);
            insertChild(T, T.r, 3, q);
        }else{
            T = CTree(1,0);
            T.nodes[0] = CTNode("if statement:",1);
            insertChild(T, T.r, 1, c);
            insertChild(T, T.r, 2, p);
        }
        return OK;
    }
    else if(w==LL){
        if(!compoundStatement(fp, T)) return ERROR;
        return OK;
    }
    else if(w==FOR){
        T = CTree(1,0);
        T.nodes[0] = CTNode("for statement:",1);
        c = CTree(1,0);
        c.nodes[0] = CTNode("initial expression:",1);
        insertChild(T, T.r, 1, c);
        c = CTree(1,0);
        c.nodes[0] = CTNode("Termination condition:",1);
        insertChild(T, T.r, 2, c);
        c = CTree(1,0);
        c.nodes[0] = CTNode("loop expression:",1);
        insertChild(T, T.r, 3, c);
        c = CTree(1,0);
        c.nodes[0] = CTNode("for clause:",1);
        insertChild(T, T.r, 4, c);
        w = getToken(fp);
        if(w!=LS) return ERROR;
        w = getToken(fp);
        if(!expression(fp, c, SEMI_COLUMN)) return ERROR;
        insertChild(T, T.nodes[0].firstchild->child, 1, c);
        w = getToken(fp);
        if(w == SEMI_COLUMN) return ERROR;
        if(!expression(fp, c, SEMI_COLUMN)) return ERROR;
        insertChild(T, T.nodes[0].firstchild->next->child, 1, c);
        w = getToken(fp);
        if(!expression(fp, c, RS)) return ERROR;
        insertChild(T, T.nodes[0].firstchild->next->next->child, 1, c);
        w = getToken(fp);
        if(w==LL){
            if(!compoundStatement(fp, c)) return ERROR;
        }
        else{
            elem = {++indent0,line_num};
            printList.push(elem);
            if(!statement(fp, c)) return ERROR;
            elem = {--indent0,line_num};
            printList.push(elem);
        }
        insertChild(T, T.nodes[0].firstchild->next->next->next->child, 1, c);
        return OK;
    }
    else if(w==WHILE){
        w = getToken(fp);
        if(w!=LS) return ERROR;
        w = getToken(fp);
        if(w==RS) return ERROR;
        if(!expression(fp, c, RS)) return ERROR;
        w = getToken(fp);
        if(w==LL){
            if(!compoundStatement(fp, p)) return ERROR;
        }
        else{
            elem = {++indent0,line_num};
            printList.push(elem);
            if(!statement(fp, p)) return ERROR;
            elem = {--indent0,line_num};
            printList.push(elem);
        }
        T = CTree(1,0);
        T.nodes[0] = CTNode("while statement:",1);
        insertChild(T, T.r, 1, c);
        insertChild(T, T.r, 2, p);
        return OK;
    }
    else if(w==CONTINUE){
        T = CTree(1,0);
        T.nodes[0] = CTNode("continue statement:",1);
        w = getToken(fp);
        if(w != SEMI_COLUMN) return ERROR;
        w = getToken(fp);
        return OK;
    }
    else if(w==BREAK){
        T = CTree(1,0);
        T.nodes[0] = CTNode("break statement:",1);
        w = getToken(fp);
        if(w != SEMI_COLUMN) return ERROR;
        w = getToken(fp);
        return OK;
    }
    else if(w==RETURN){
        T = CTree(1,0);
        T.nodes[0] = CTNode("return statement:",1);
        w = getToken(fp);
        token_kind returnType = symbolTable.getFunctionReturnType(funcName);
        if(w!=LS){ // return后面的第一个词不是`(`，也不是声明时候理论上要返回的类型；则报错
            token_kind identifierType = AUTO; // 随便先赋值一个绝对不可能的
            if(w==IDENTIFIER){
                // 首先寻找局部变量，再看是不是外部变量，最后再看是不是预定义
                // 这样，当局部变量和外部变量同名的时候，就会优先（也只考虑）局部变量
                if(symbolTable.isLocalVariableDeclared(token_text))
                    identifierType = symbolTable.getLocalType(token_text);
                else if(symbolTable.isExternVariableDeclared(token_text))
                    identifierType = symbolTable.getExternType(token_text);
                else if(symbolTable.isPreDefinitionDeclared(token_text))
                    identifierType = symbolTable.getPreDefinitionType(token_text);
            }
            else if(w>=INT_CONST && w<=CHAR_CONST){
                identifierType = symbolTable.constTranslate(w);
            }
            if(returnType != identifierType) return ERROR;
        }

        if(w != SEMI_COLUMN) {
            if(!expression(fp,c,SEMI_COLUMN)) return ERROR;
            insertChild(T, T.r, 1, c);
        }
        w = getToken(fp);
        return OK;
        }
    else if(w==LS){
        if(!expression(fp, T, RS)) return ERROR;
        w = getToken(fp);
        return OK;
    }
    else if(w == IDENTIFIER || w == INT_CONST || w == UNSIGNED_CONST || w == LONG_CONST || w == UNSIGNED_LONG_CONST || w == DOUBLE_CONST
             || w==FLOAT_CONST || w== LONG_DOUBLE_CONST || w==CHAR_CONST){
        if(!expression(fp, T, SEMI_COLUMN)) return ERROR;
        w = getToken(fp);
        return OK;
    }
    else if(w==RL){
        return INFEASIBLE;
    }
    else if (w == SEMI_COLUMN){
        T = CTree(1,0);
        T.nodes[0] = CTNode("empty statement:",1);
        w = getToken(fp);
        return OK;
    }else return ERROR;
}
status expression(ifstream & fp, CTree & T, int endSymbol) {
    // 在考虑了变量声明的同时赋值的情况，但是目前只针对分号类型，不支持逗号类型。
    // 好消息是不用考虑逗号运算符
    // 已经读入了第一个单词到w了
    stack<CTree*> op; // 运算符栈
    stack<CTree*> op_num; //操作数栈
    vector<token_kind> expre; // 用来判断表达式的identifier是否类型一致
    CTree *node;
    CTree *child1;
    CTree *child2;
    node = new CTree(1,0); // 设立起止符号
    node->nodes[0].data = "#";
    node->nodes[0].firstchild = nullptr;
    child1 = new CTree();
    child2 = new CTree(); // 先随便指个空结构体，防止空指针的出现
    T = CTree(1,0); // 生成表达式节点
    T.nodes[0] = CTNode("expression:",1);
    bool error = false;
    op.push(node); // 初始化，将起止符#入栈
    while ((w != POUND || node->nodes[0].data != "#") && !error) { // 当运算符栈栈顶不是起止符号，并且没有错误的时候
        if (w == IDENTIFIER || w == INT_CONST || w == UNSIGNED_CONST || w == LONG_CONST || w == UNSIGNED_LONG_CONST || w == DOUBLE_CONST || w == FLOAT_CONST || w == LONG_DOUBLE_CONST || w == CHAR_CONST) {
            if(w==IDENTIFIER){
                if(!symbolTable.isVariableDeclared(token_text) &&
                !symbolTable.isFunctionDeclared(token_text)){
                    // 如果identifier既不是已经声明过的变量，也不是已经声明过的函数，那么就直接报错了
                    return ERROR;
                }
                if(symbolTable.isLocalVariableDeclared(token_text)){
                    expre.push_back(symbolTable.getLocalType(token_text));
                }
                else if(symbolTable.isExternVariableDeclared(token_text)){
                    expre.push_back(symbolTable.getExternType(token_text));
                }
                else if(symbolTable.isPreDefinitionDeclared(token_text)){
                    expre.push_back(symbolTable.getPreDefinitionType(token_text));
                }
                else if(symbolTable.isFunctionDeclared(token_text)) {
                    expre.push_back(symbolTable.getFunctionReturnType(token_text));
                    string funcNameHere = token_text;
                    int index = 0;
                    while(w!=RS){
                        w = getToken(fp);
                        if(w==IDENTIFIER){ // 说明调用了函数，而且函数是有形参的
                            if(!symbolTable.checkFunctionFormParamType(funcNameHere,index,token_text)){
                                return ERROR;
                            }
                            index++;
                        }
                    }
                }
                else{
                    expre.push_back(symbolTable.getFunctionReturnType(token_text));
                }
            }
            else{ // 说明是常量
                expre.push_back(symbolTable.constTranslate(w));
            }
            node = new CTree(1,0);
            node->nodes[0] = CTNode("value: "+token_text,1);
            op_num.push(node); // 根据w生成一个节点，节点指针进栈op_num
            w = getToken(fp);
        } else if (w == ASSIGN) { // 处理赋值操作
            if (!op.empty() && op.top()->nodes[0].data == "=") {
                op.pop();
            } else {
                node = new CTree(1,0);
                node->nodes[0] = CTNode("=",1);
                op.push(node);
            }
            w = getToken(fp);
        } else if (w == PLUS || w == MINUS || w == MULTIPLY || w == DIVIDE || w == MOD || w == LS || w == RS || ((w >= MORE) && (w <= LESS_EQUAL)) || w == AND || w == OR || w == POUND) {
            node = op.top();
            if (w == POUND) token_text = "#";
            switch (precede(node->nodes[0].data, token_text)) {
                case '<':
                    node = new CTree(1,0);
                    node->nodes[0] = CTNode(token_text,1);
                    op.push(node); // 根据w生成一个节点，节点指针进栈opn
                    w = getToken(fp);
                    break;
                case '=':
                    if (op.empty()) {
                        error = true;
                    } else {
                        op.pop();
                    }
                    w = getToken(fp);
                    break; // 去括号
                case '>':
                    if (op_num.empty()) {
                        error = true;
                    } else {
                        child2 = op_num.top();
                        op_num.pop();
                    }
                    if (op_num.empty()) {
                        error = true;
                    } else {
                        child1 = op_num.top();
                        op_num.pop();
                    }
                    if (op.empty()) {
                        error = true;
                    } else {
                        node = op.top();
                        op.pop();
                    }
                    // 根据运算符退栈得到的运算符node和操作数的节点指针child1和child2，完成建立生成一个运算符的节点，节点指针进栈opn
                    // 因为是先取的2，所以要先动1
                    insertChild(*node, node->r, 1, *child1);
                    insertChild(*node, node->r, 2, *child2);
                    op_num.push(node);
                    break;
                default:
                    if (w == endSymbol) w = POUND; // 遇到结束标记分号，w被替换成#
                    else error = true;
            }
        }
        else if (w == endSymbol) {
            // 说明到表达式的结束部分了
            w = POUND;
            bool allEqual = all_of(expre.begin()+1, expre.end(),
                                   [firstValue = expre.front()](token_kind value){return value==firstValue;});
            if(!allEqual) return ERROR;
        }
        else error = true;
        node = op.top();
    }
    if (error) return ERROR;
    node = op_num.top();
    insertChild(T, T.r, 1, *node);
    return OK;
}

char precede(const string & a, const string & b){
    int i,j; // 指示运算符对应的编号
    // 定义一个二维数组，用于存放优先级
    // 人工的智能～～～
    char precede[13][13] =
            {   //				            +		    -		    *		     /		     %		    （	    	 ）	    	 =		    >和<	  ==和!=		    #		    &&		        ||
                    /* + */			'>',	'>',	'<',	'<',	'<',	'<',	'>',	'?',	'>',		'>',		'>',	    '>',	'>',
                    /* - */			'>',	'>',	'<',	'<',	'<',	'<',	'>',	'?',	'>',		'>',		'>',	    '>',	'>',
                    /* * */			'>',	'>',	'>',	'>',	'>',	'<',	'>',	'?',	'>',		'>',		'>',	    '>',	'>',
                    /* / */			'>',	'>',	'>',	'>',	'>',	'<',	'>',	'?',	'>',		'>',		'>',	    '>',	'>',
                    /* % */			'>',	'>',	'<',	'<',	'<',	'<',	'>',	'?',	'>',		'>',		'>',	    '>',	'>',
                    /* ( */			'<',	'<',	'<',	'<',	'<',	'<',	'=',	'?',	'<',		'<',		'>',	    '<',	'<',
                    /* ) */			'>',	'>',	'>',	'>',	'>',	'>',	'?',	'?',	'>',		'>',		'>',	    '>',	'>',
                    /* = */			'<',	'<',	'<',	'<',	'<',	'<',	'?',	'<',	'<',		'<',		'>',	    '<',	'<',
                    /* >和< */		'<',	'<',	'<',	'<',	'<',	'<',	'>',	'?',	'>',		'>',		'>',	    '>',	'>',
                    /* ==和!= */	'<',	'<',	'<',	'<',	'<',	'<',	'>',	'?',	'<',		'>',		'>',	    '>',	'>',
                    /* # */			'<',	'<',	'<',	'<',	'<',	'<',	'?',	'<',	'<',		'<',		'=',	'<','<',
                    /* && */		'<',	'<',	'<',	'<',	'<',	'<',	'>',	'>',	'<',		'<',		'>',	'>',	'>',
                    /* || */		'<',	'<',	'<',	'<',	'<',	'<',	'>',	'>',	'<',		'<',		'>',	'>','>'
            };
    switch(a[0]){
        case '+':
            i = 0;
            break;
        case '-':
            i = 1;
            break;
        case '*':
            i = 2;
            break;
        case '/':
            i = 3;
            break;
        case '%':
            i = 4;
            break;
        case '(':
            i = 5;
            break;
        case ')':
            i = 6;
            break;
        case '=':
            if(a[1]=='=') i = 9;
            else i = 7;
            break;
        case '<':
        case '>':
            i = 8;
            break;
        case '!':
            if(a[1]=='=') i = 9;
            else return '?';
            break;
        case '#':
            i = 10;
            break;
        case '&':
            if(a[1]=='&') i = 11;
            else return '?';
            break;
        case '|':
            if(a[1] =='|') i = 12;
            else return '?';
            break;
        default:
            return '?';
    }
    switch (b[0])
    {
        case '+':
            j = 0;
            break;
        case '-':
            j = 1;
            break;
        case '*':
            j = 2;
            break;
        case '/':
            j = 3;
            break;
        case '%':
            j = 4;
            break;
        case '(':
            j = 5;
            break;
        case ')':
            j = 6;
            break;
        case '=':
            if (b[1] == '=') j = 9;
            else j = 7;
            break;
        case '>':
        case '<':
            j = 8;
            break;
        case '!':
            if (b[1] == '=') j = 9;
            else return '?';
            break;
        case '#':
            j = 10;
            break;
        case '&':
            if (b[1] == '&') j = 11;
            else return '?';
            break;
        case '|':
            if (b[1] == '|') j = 12;
            else return '?';
            break;
        default:
            return '?';
    }
    return precede[i][j];
}
status printTree(const string & data, int indent){
    for(int i=0;i<indent;i++){
        cout<<"\t";
    }
    cout<<data<<endl;
    return OK;
}
