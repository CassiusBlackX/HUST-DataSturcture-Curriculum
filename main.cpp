#include "lexer.h"
#include "parser.h"
#include "preprocess.h"
#include "profunction.h"
#include "printfile.h"

/*
 * 新语法限定：
 *  1. 外部变量只能声明或者在声明的时候赋初值，不能先声明了外部变量以后又能够在所有函数体外部赋值 =》理由：全局变量居然在函数外面随便被改动，不太好
 *  2. 局部变量只能先集中声明再集中进行操作，不能声明和操作混杂 =》理由：变量就不可以被同名覆盖了，也不用考虑作用域相关的问题，涉及到的变量都已经在刚刚进入函数的时候声明好了
 *  *  不允许声明和操作混杂的情况
 *  eg`
 *      int x;int y;
 *      x = 30; y = 20;
 *  ` 正确
 *  `   int x; x = 30;
 *      int y; y = 20;
 *  ` 错误
 *      补充： 根据以上规则，也不允许for(int i=0;i<10;i++)中在for的括号内再声明变量
 *  2. 该语言只支持一维数组的声明
 *  3. 数组只能声明，不能操作，因为下标运算符并没有被包含在要求的功能内
 *      补充： 数组声明[]内必须有数字
 *  4. 数据类型里面将不考虑long long，unsigned long, unsigned short之类的类型（反正也从来没有用到过这些）
 *  5. 类型判断不会自动类型转换，只要不是同一类型就直接报错
 *  6. 布尔类型的双目运算符操作后的结果也将和两个操作数同类型（符合上一条）
 *  7. return后面如果是有括号的表达式将不负责判断return的值是否与声明一致（其实聪明一点说可以说成return不支持后面的表达式第一个字符为括号）
 *  8. 如果出现了变量由函数返回值赋值的情况，打印语法树的时候将打印value值为`)`，以指代此处为函数
 *  9. extern, const之类的关键字虽然可以词法识别，但是语法不识别，因此将报错
 *  10. void类型的函数没有返回值检查
 *  11. 其它函数但是没有return也不检查
 *  12. 出现错误符号的时候，预编译仍然显示成功，即不至于进行不下去，后面语法检查的时候仍然会报错
 *  13. 不会作用域判断
 */
using namespace std;
int main(){
    string filename;
    int choice = 1;
    int w;
    CTree T;
    ifstream fp,mid_fp;
    extern define_data data_Def[10]; // 用于存储define宏定义的内容
    extern int line_num;
    extern string token_text;
    int error_line[100] = {-1}; // 记录错误行数
    int error_line_num = 0; // 记录错误总个数
Choosefile:
    system("clear");
    printf("input the name of the file\n");
    filename = "../test/file2.txt";
//    cin>>filename;
    fp.open(filename);
    if(!fp.is_open()){
        printf("\ncannot open the file,choose again");
        system("pause");
        system("clear");
        getchar();
        getchar();
        goto Choosefile;
    }
    else fp.close();
    while(choice){
        printf("Menu of Advanced Language Source Code Processing Tool\n");
        printf("1. Lexical Analysis\n2. Syntax Analysis\n3. Indentation Formatting\n4. Select File\n0. Exit Program\nPlease choose:\n");
        cin >> choice;
//        choice = 1;
        system("clear");
        switch(choice){
            case 1:
                error_line_num = 0;
                line_num = 1; // 行初始化为1
                fp.open(filename);
                if(!fp.is_open()){
                    printf("cannot open the file, please choose again");
                    getchar();
                    getchar();
                    break;
                }
                printf("compiling...\n");
                if(preProcess(fp)){
                    printf("compiled succeeded!,press here to continue...\n");
                    getchar();
                    getchar();
                }else{
                    printf("compiled failed. exit. please check the errors\n");
                    getchar();
                    getchar();
                    return 0;
                }
                mid_fp.open("../output/C_mid_file.txt");
                line_num = 1;
                system("clear");
                printf("\n");
                printf("\tToken Type\t\t\tToken Value\n");
                while(!mid_fp.eof()){
                    w = getToken(mid_fp);
                    if(w>=AUTO && w<=DEFINE){
                        cout<<"\tkeyword\t\t\t\t\t"<<token_text<<endl;
                    }
                    switch(w){
                        case POUND:
                            cout<<"\tpound\t\t\t\t"<<token_text<<endl;
                            break;
                        case IDENTIFIER:
                            cout<<"\tidentifier\t\t\t\t"<<token_text<<endl;
                            break;
                        case INT_CONST:
                            cout<<"\tint const\t\t\t\t"<<token_text<<endl;
                            break;
                        case UNSIGNED_CONST:
                            cout<<"\tunsigned const\t\t\t\t"<<token_text<<endl;
                            break;
                        case LONG_CONST:
                            cout<<"\tlong const\t\t\t\t"<<token_text<<endl;
                            break;
                        case UNSIGNED_LONG_CONST:
                            cout<<"\tunsigned long const\t\t\t"<<token_text<<endl;
                            break;
                        case DOUBLE_CONST:
                            cout<<"\tdouble const\t\t\t\t"<<token_text<<endl;
                            break;
                        case FLOAT_CONST:
                            cout<<"\tfloat const\t\t\t\t"<<token_text<<endl;
                            break;
                        case LONG_DOUBLE_CONST:
                            cout<<"\tlong double const\t\t\t"<<token_text<<endl;
                            break;
                        case CHAR_CONST:
                            cout<<"\tchar const\t\t\t\t"<<token_text<<endl;
                            break;
                        case STRING_CONST:
                            cout<<"\tstring const\t\t\t\t"<<token_text<<endl;
                            break;
                        case ASSIGN:
                            cout<<"\tassign\t\t\t\t\t"<<token_text<<endl;
                            break;
                        case PLUS:
                            cout<<"\tplus\t\t\t\t"<<token_text<<endl;
                            break;
                        case MINUS:
                            cout<<"\tminus\t\t\t\t"<<token_text<<endl;
                            break;
                        case MULTIPLY:
                            cout<<"\tmultiply\t\t\t\t"<<token_text<<endl;
                            break;
                        case DIVIDE:
                            cout<<"\tdivide\t\t\t\t"<<token_text<<endl;
                            break;
                        case MOD:
                            cout<<"\tmod\t\t\t\t"<<token_text<<endl;
                            break;
                        case MORE:
                        case LESS:
                        case EQUAL:
                        case UNEQUAL:
                        case MORE_EQUAL:
                        case LESS_EQUAL:
                            cout<<"\trelational operator\t\t\t\t"<<token_text<<endl;
                            break;
                        case AND:
                            cout<<"\tlogical and\t\t\t\t"<<token_text<<endl;
                            break;
                        case OR:
                            cout<<"\tlogical or\t\t\t\t"<<token_text<<endl;
                            break;
                        case LS:
                            cout<<"\tleft parenthesis\t\t\t"<<token_text<<endl;
                            break;
                        case RS:
                            cout<<"\tright parenthesis\t\t\t"<<token_text<<endl;
                            break;
                        case LM:
                            cout<<"\tleft bracket\t\t\t\t"<<token_text<<endl;
                            break;
                        case RM:
                            cout<<"\tright bracket\t\t\t\t"<<token_text<<endl;
                            break;
                        case LL:
                            cout<<"\tleft opening brace\t\t\t"<<token_text<<endl;
                            break;
                        case RL:
                            cout<<"\tright opening brace\t\t\t"<<token_text<<endl;
                            break;
                        case SEMI_COLUMN:
                            cout<<"\tsemi column\t\t\t\t"<<token_text<<endl;
                            break;
                        case COMMA:
                            cout<<"\tcomma\t\t\t\t\t"<<token_text<<endl;
                            break;
                        case ERROR_TOKEN:
                            error_line[error_line_num] = line_num;
                            error_line_num++;
                            break;
                    }
                }
                printf("\n");
                printf("Error List Total Errors: %d \n",error_line_num);
                if(error_line_num){
                    printf("\n");
                    printf("\tNumber\tLine Number\n");
                    for(int i=0;i<error_line_num;i++){
                        printf("\t%d\t\t%d\n",i+1,error_line[i]);
                    }
                }
                fp.close();
                mid_fp.close();
                printf("press here to continue\n");
                getchar();
                break;
            case 2:
                printf("pre_compiling...\n");
                line_num = 1;
                fp.open(filename);
                if(preProcess(fp)){
                    printf("pre_compiled succeeded, press here to continue\n");
                    // todo debug change
                    getchar();
                    getchar();
                }else{
                    printf("pre_compiled failed,exit,check the errors\n");
                    getchar();
                    getchar();
                    return 0;
                }
                mid_fp.open("../output/C_mid_file.txt");
                line_num = 1;
                system("clear");
                if(!program(mid_fp,T)){
                    printf("errors occur! error line:%d\n",line_num);
                    printf("press here to continue...\n");
                    getchar();
                    break;
                }
                traverseTree(T, printTree);
                printf("\npress here to continue...\n");
                fp.close();
                mid_fp.close();
                getchar();
                break;
            case 3:
                line_num = 1;
                fp.open(filename);
                if(!preProcess(fp)){
                    printf("pre_compiled failed,exit,check the errors\n");
                    getchar();
                    getchar();
                    return 0;
                }
                line_num = 1;
                mid_fp.open("../output/C_mid_file.txt");
                if(!program(mid_fp,T)){
                    printf("there occurs grammar errors in the program. failed to print with indent\n");
                    getchar();
                    getchar();
                    break;
                }else{
                    printf("the grammar of the program is correct\n");
                    fp.close();
                    fp.open(filename);
                    PrintFile(fp);
                    printf("indent edit file succeeded\n");
                    getchar();
                    getchar();
                    fp.close();
                    mid_fp.close();
                    break;
                }
            case 4:
                goto Choosefile;
            default:
                return 0;
        }
    }
    return 1;
}
