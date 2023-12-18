#include "lexer.h"
using namespace std;
int line_num = 1;
string token_text;
keyword n[IDENTIFIER] = {
        {"auto",AUTO},
        {"break",BREAK},
        {"case",CASE},
        {"char",CHAR},
        {"const",CONST},
        {"continue",CONTINUE},
        {"default",DEFAULT},
        {"do",DO},
        {"double",DOUBLE},
        {"else",ELSE},
        {"enum",ENUM},
        {"extern",EXTERN},
        {"float",FLOAT},
        {"for",FOR},
        {"goto",GOTO},
        {"if",IF},
        {"int",INT},
        {"long",LONG},
        {"register",REGISTER},
        {"return",RETURN},
        {"short",SHORT},
        {"signed",SIGNED},
        {"sizeof",SIZEOF},
        {"static",STATIC},
        {"struct",STRUCT},
        {"switch",SWITCH},
        {"typedef",TYPEDEF},
        {"union",UNION},
        {"unsigned",UNSIGNED},
        {"void",VOID},
        {"volatile",VOLATILE},
        {"while",WHILE},
        {"include",INCLUDE},
        {"define",DEFINE},
};

using namespace std;
// 对于发生错误的专门处理
static int error(char c, ifstream & file){
    file.putback(c);
    return ERROR_TOKEN;
}
// 对于数字后缀的专门处理
static int u_suffix(char c,ifstream& file){
    token_text.push_back(c);
    file.get(c);
    if(c=='l' || c=='L'){
        token_text.push_back(c);
        return UNSIGNED_LONG_CONST;
    }else{
        file.putback(c);
        return UNSIGNED_CONST;
    }
}
static int int_l_suffix(char c){
    token_text.push_back(c);
    return LONG_CONST;
}
static int f_suffix(char c){
    token_text.push_back(c);
    return FLOAT_CONST;
}
static int double_l_suffix(char c){
    token_text.push_back(c);
    return LONG_DOUBLE_CONST;
}
static int e_suffix(char & c, ifstream & file){
    token_text.push_back(c);
    file.get(c);
    if(c>='0' && c<='9'){
        do {
            token_text.push_back(c);
            file.get(c);
        } while(c>='0' && c<='9');
        if(c=='f' || c=='F'){
            return f_suffix(c);
        } else if(c=='l' || c=='L'){
            return double_l_suffix(c);
        } else{
            file.putback(c);
            return DOUBLE_CONST;
        }
    } else if (c == '+' || c == '-') {
        token_text.push_back(c);
        file.get(c);
        if(c>='0' && c<='9'){
            do {
                token_text.push_back(c);
                file.get(c);
            } while(c>='0' && c<='9');
            if(c=='f' || c=='F'){
                return f_suffix(c);
            } else if(c=='l' || c=='L'){
                return double_l_suffix(c);
            } else{
                file.putback(c);
                return DOUBLE_CONST;
            }
        } else {
            return error(c, file);
        }
    }
    else return error(c, file);
}
static int point_withoutNum(char & c, ifstream & file){
    do {
        token_text.push_back(c);
        file.get(c);
    }while(c>='0' && c<='9');
    if(c=='f' || c=='F'){
        return f_suffix(c);
    }else if(c=='l' || c=='L'){
        return double_l_suffix(c);
    }else if(c=='e' || c=='E'){
        return e_suffix(c,file);
    }else{
        file.putback(c);
        return DOUBLE_CONST;
    }
}
static int numbers(char & c, ifstream & file){
    if(c=='+' || c=='-'){
        // 数字前面带有符号
        if(c=='+'){ // 如果是正数的加号自动删掉，不管了，但是如果是identifier的那么不能删除
            while(file.get(c) && c==' ');
            if(!((c>='0' && c<='9') || c=='.')){
                // 说明是一个identifer
                file.putback(c);
                token_text.push_back('+');
                return PLUS;
            }
        }else{
            while(file.get(c) && c==' ');
            if((c>='0' && c<='9') || c=='.'){
                token_text.push_back('-');
            }else{
                file.putback(c);
                token_text.push_back('-');
                return MINUS;
            }
        }
    }
    // 如果是正常的数字，那么就是非0开头
    if(c>'0' && c<='9'){//数字开头不能是0
        token_text.push_back(c);
        file.get(c);
        if(c>='0' && c<='9'){
            do{
                token_text.push_back(c);
                file.get(c);
            }while(c>='0' && c<='9');
            if(c=='u' || c=='U'){
                // u/U后缀
                return u_suffix(c,file);
            }
            else if(c=='l' || c=='L'){
                // l/L后缀
                return int_l_suffix( c);
            }
            else if(c=='.'){
                return point_withoutNum(c,file);
            }
            else if(c=='e' || c=='E'){
                return e_suffix(c,file);
            }else{
                file.putback(c);
                return INT_CONST;
            }
        }
        else if(c=='.'){
            // 小数点，但是可能没有数字
            return point_withoutNum(c,file);
        }
        else if(c=='u' || c=='U'){
            return u_suffix(c,file);
        }
        else if(c=='l' || c=='L'){
            return int_l_suffix( c);
        }
        else if(c=='e' || c=='E'){
            return e_suffix(c,file);
        }
        else if((c>='a' && c<='z') || (c>='Z' && c<='Z') || c=='_'){
            return error(c,file);
        }
        else{
            file.putback(c);
            return INT_CONST;
        }
    }
    // 小数可能是0开头
    if(c=='0'){ // 开头是0
        token_text.push_back(c);
        file.get(c);
        if(c=='x' || c=='X'){ // 十六进制的情况
            token_text.push_back(c);
            file.get(c);
            if((c>='0' && c<='9') || (c>='A' && c<='F') || (c>='a' && c<='f')){
                do {
                    token_text.push_back(c);
                    file.get(c);
                }while((c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F'));
                if(c=='u' || c=='U') return u_suffix(c,file);
                else if(c=='l' || c=='L')  return int_l_suffix( c);
                else if ((c>='g' && c<='z') || (c>='G' && c<='Z')) return error(c,file);
                else{
                    file.putback(c);
                    return INT_CONST;
                }
            }
            else return error(c,file);
        }
        else if(c>='0' && c<='9'){ // 八进制的情况
            do {
                token_text.push_back(c);
                file.get(c);
            }while(c>='0' && c<='7');
            if(c=='u' || c=='U') return u_suffix(c,file);
            else if(c=='l' || c=='L')  return int_l_suffix( c);
            else if((c>='a' && c<='z') || c == '_' || (c == '8' || c == '9')) return error(c,file);
            else{
                file.putback(c);
                return INT_CONST;
            }
        }
        else if(c=='.') return point_withoutNum(c,file);
        else if(c=='u' || c=='U') return u_suffix(c,file);
        else if(c=='l' || c=='L') return int_l_suffix( c);
        else if((c>='a' && c<='z') || c>='A' && c<='Z' || c=='_' || (c=='8' || c=='9')) return error(c,file);
        else{
            file.putback(c);
            return INT_CONST;
        }
    }
    // 小数可能省略0
    if(c=='.'){ // 出现小数点后必须出现数字
        token_text.push_back(c);
        file.get(c);
        if(c>='0' && c<='9'){
            do {
                token_text.push_back(c);
                file.get(c);
            }while(c>='0' && c<='9');
            if(c=='f' || c=='F'){
                return f_suffix(c);
            }
            else if(c=='l' || c=='L'){
                return double_l_suffix(c);
            }
            else if(c=='e' || c=='E'){
                return e_suffix(c,file);
            }else{
                file.putback(c);
                return DOUBLE_CONST;
            }
        }
        else{
            return error(c,file);
        }
    }
    return 0; // 防止出现意外
}
// 反正最后才判断是不是符号类，不需要传index和c的引用了，
static int symbols(char c, ifstream & file){
    // 判断符号
    switch(c){
        case '=':
            token_text.push_back(c);
            file.get(c);
            if(c=='='){
                // 相等
                token_text.push_back(c);
                return EQUAL;
            }else{
                // 赋值
                file.putback(c);
                return ASSIGN;
            }
        case '{':
            token_text.push_back(c);
            return LL;
        case '}':
            token_text.push_back(c);
            return RL;
        case '[':
            token_text.push_back(c);
            return LM;
        case ']':
            token_text.push_back(c);
            return RM;
        case '(':
            token_text.push_back(c);
            return LS;
        case ')':
            token_text.push_back(c);
            return RS;
        case ';':
            token_text.push_back(c);
            return SEMI_COLUMN;
        case ',':
            token_text.push_back(c);
            return COMMA;
        case '#':
            token_text.push_back(c);
            return POUND;
        case '>':
            token_text.push_back(c);
            file.get(c);
            if(c=='='){
                token_text.push_back(c);
                return MORE_EQUAL;
            }else{
                file.putback(c);
                return MORE;
            }
        case '<':
            token_text.push_back(c);
            file.get(c);
            if(c=='='){
                token_text.push_back(c);
                return LESS_EQUAL;
            }else{
                file.putback(c);
                return LESS;
            }
        case '!':
            token_text.push_back(c);
            file.get(c);
            if(c=='='){
                token_text.push_back(c);
                return UNEQUAL;
            }else{
                return error(c,file);
            }
        case '&':
            token_text.push_back(c);
            file.get(c);
            if(c=='&'){
                token_text.push_back(c);
                return AND;
            }else{
                return error(c,file);
            }
        case '|':
            token_text.push_back(c);
            file.get(c);
            if(c=='|'){
                token_text.push_back(c);
                return OR;
            }else{
                return error(c,file);
            }
        case '+':
            token_text.push_back(c);
            return PLUS;
        case '-':
            token_text.push_back(c);
            return MINUS;
        case '*':
            token_text.push_back(c);
            return MULTIPLY;
        case '%':
            token_text.push_back(c);
            return MOD;
        case '/':
            token_text.push_back(c);
            file.get(c);
            if(c=='/'){ // 说明是行注释
                do{
                    token_text.push_back(c);
                    file.get(c);
                }while(c!='\n' && c!=EOF);
                if(c=='\n'){
                    file.putback(c);
                    return LINE_NOTE;
                }
            }
            else if(c=='*'){ // 说明是块注释
                // 临时判断是否是*_/的情况
                while(true){
                    token_text.push_back(c);
                    file.get(c);
                    if(c=='\n'){
                        line_num++;
                    }else if(c==EOF){
                        return error(c,file);
                    }else if(c=='*'){
                        token_text.push_back(c);
                        file.get(c);
                        if(c=='/'){ // 说明这里是块注释的结尾了
                            token_text.push_back(c);
                            return BLOCK_NOTE;
                        }
                    }
                }
            }
            else{
                // 说明只是一个除号
                file.putback(c);
                return DIVIDE;
            }
        case '\'':
            token_text.push_back(c);
            file.get(c);
            if(c=='\\'){ // 可能是一个转义字符
                token_text.push_back(c);
                file.get(c);
                if(c>='0' && c<='9'){ // 数字转义
                    do{
                        token_text.push_back(c);
                        file.get(c);
                    }while(c>='0' && c<='9');
                    if(c=='\''){
                        token_text.push_back(c);
                        return CHAR_CONST;
                    }
                    else return error( c,file);
                }
                else if(c=='\''){
                    token_text.push_back(c);
                    file.get(c);
                    if(c=='\''){
                        token_text.push_back(c);
                        return CHAR_CONST;
                    }
                    else return error(c,file);
                }
                else{ // 可能是字母转义
                    token_text.push_back(c);
                    file.get(c);
                    if(c=='\''){
                        token_text.push_back(c);
                        return CHAR_CONST;
                    }
                    else{
                        do {
                            token_text.push_back(c);
                            file.get(c);
                        }while(c!='\'' && c!='\n' && c!=EOF); // 如果一直没有遇到另一半，也不要一直往后，遇到换行什么的就直接报错就可以了
                        if(c=='\'') return ERROR_TOKEN;
                        else return error(c,file);
                    }
                }
            }
            else if(c=='\''){ // '\''的情况
                file.get(c);
                if(c=='\'') return ERROR_TOKEN;
                else return error(c,file);
            }
            else{
                token_text.push_back(c);
                file.get(c);
                if(c=='\''){
                    token_text.push_back(c);
                    return CHAR_CONST;
                }
                else{
                    do{
                        token_text.push_back(c);
                        file.get(c);
                    }while(c!='\'' && c!='\n' && c!=EOF);
                    if(c=='\'') return ERROR_TOKEN;
                    else return error(c,file);
                }
            }
        case '"':
            token_text.push_back(c);
            while((file.get(c)) && c != '"'){
                token_text.push_back(c);
                if(c=='\\') {
                    file.get(c);
                    if (c == '"') {
                        do {
                            token_text.push_back(c);
                            file.get(c);
                        } while (c != '"' && c != '\n' && c != EOF);
                        if (c == '"') {
                            token_text.push_back(c);
                            return STRING_CONST;
                        } else return error(c, file);
                    } else if (c == '\n') {
                        line_num++;
                        token_text.push_back(c);
                    } else token_text.push_back(c);
                }
                else if(c=='\n'){
                    line_num++;
                    do {
                        token_text.push_back(c);
                        file.get(c);
                    } while (c!='"' && c!='\n' && c!=EOF);
                    if(c=='"') return ERROR_TOKEN;
                    else return error(c,file);
                }
                else if(c==EOF) return error(c,file);
            }

            token_text.push_back(c);
            return STRING_CONST;
        default:
            if(file.eof()) return EOF;
            else {
                // 虽然是错误的字符，但是仍然应当保存并且打印
                token_text.push_back(c);
                return ERROR_TOKEN;
            }
    }
}
int getToken(ifstream &file){
    token_text.clear();
//    token_text.reserve(100);
    char c; // 暂时存&读取单个字符
    while (file.get(c) && (c == ' ' || c == '\n' || c == '\t' || c == EOF)){
        // 开头可能会有一堆不重要的东西，将直接跳过，生成的中间文件不需要这些
        if(c=='\n') line_num++;
        if(c==EOF) return EOF; // 空文件的话直接立刻返回eof就好
    }
    // 判断标识符或者关键字
    if((c>='a' && c<='z') || (c>='A' && c<='Z')){
        do{
            token_text.push_back(c);
            file.get(c);
        }while((c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_' || (c>='0' && c<='9') || c=='.');
        file.putback(c);
        for(auto & j : n){
            if(token_text == j.key) {
                return j.enum_key;
            }
        }
        // 不是关键字就一定是标识符了
        return IDENTIFIER;
    }
    // 标识符可能是下划线开头
    if(c=='_'){
        do{
            token_text.push_back(c);
            file.get(c);
        }while((c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_' || (c>='0' && c<='9') || c=='.');

        file.putback(c);
        return IDENTIFIER;
    }
    // 判断数字
    if((c>='0' && c<='9') || c=='.' ||
    c=='+' || c=='-'){
        return numbers(c,file);
    }
    // 如果以上都不是，说明就是其它符号了
    // 当然，因为char和string都需要有'或者"，所以将放在符号里面讨论相关情况
    return symbols(c,file);
}
