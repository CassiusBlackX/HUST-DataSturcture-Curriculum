#include "preprocess.h"
#include "lexer.h"
define_data data_Def[10]; // 用于存储define宏定义的内容
include_data data_Inculd[10]; // 用于存储include文件包含的被容
int dataDefNum; // 宏定义的个数
using namespace std;
status preProcess(ifstream & fp){
    int w;// 接受gettoken读取的返回值
    int defineIndex=0; // 宏定义的个数
    int includeIndex=0; // include个数
    int preLineNum = 1; // 用于记录换行情况
    char container; // 暂时存储字符判断结尾处的分号
    int a,b; // 比较行数
    bool flag = false; // 判断语句中是否出现过define定义
    string filename = "../output/C_mid_file.txt" ;// 中间文件
    ofstream mid_fp(filename);
    w = getToken(fp);
    do {
        if(w==POUND){
            // 说明是#开头的预处理指令
            w = getToken(fp);
            if(w==DEFINE){
                string preDefinition;
                w = getToken(fp);
                a = line_num;
                if(w==ERROR_TOKEN) return ERROR;
                else if(w == SEMI_COLUMN) return ERROR;
                else if(w==POUND) return ERROR;
                else{
                    data_Def[defineIndex].string = token_text + " ";
                    preDefinition = token_text;
                }
                w = getToken(fp);
                b = line_num;
                if(w==ERROR_TOKEN) return ERROR;
                else if(w == SEMI_COLUMN) return ERROR;
                else if(w==POUND) return ERROR;
                else{
                    symbolTable.addPreDefinition(preDefinition,static_cast<token_kind>(w));
                    data_Def[defineIndex++].string = token_text + " ";
                }
                if(a!=b) return ERROR;
                dataDefNum = defineIndex;
                mid_fp<<endl;
                w = getToken(fp);
                preLineNum = line_num;
                continue;
            }
            else if(w==INCLUDE){
                w = getToken(fp);
                if(w==ERROR_TOKEN) return ERROR;
                else if(w==STRING_CONST){
                    data_Inculd[includeIndex++].string = token_text + " ";
                    if(fp.get(container) && container!=';') {
                        fp.putback(container);
                        mid_fp<<endl;
                        w = getToken(fp);
                        preLineNum = line_num;
                        continue;
                    }
                    else return ERROR;
                }
                else if(w==LESS){
                    w = getToken(fp);
                    a = line_num;
                    if(w != IDENTIFIER) return ERROR;
                    else{
                        w = getToken(fp);
                        b = line_num;
                        if(w!=MORE) return ERROR;
                        if(a!=b) return ERROR;
                        if(fp.get(container) && container!=';') {
                            fp.putback(container);
                            w = getToken(fp);
                            for(int i=0;i<line_num-preLineNum;i++)
                                mid_fp<<endl;
                            preLineNum = line_num;
                            continue;
                        }else return ERROR;
                    }
                }
            }
            else return ERROR;
        }
        dataDefNum = defineIndex;
        if(w!=POUND){
            if(w == IDENTIFIER){ // 是标识符的时候，判断是不是define的类型
                for(int i=0; i < dataDefNum; i++){
                    if(token_text == data_Def[i].identifier){
                        if(preLineNum != line_num){
                            mid_fp << "\n" << data_Def[i].string << " ";
                            flag = true;
                        }
                        else{
                            mid_fp << data_Def[i].string << " ";
                            flag = true;
                        }
                    }
                }
                if(flag) flag = false;
                else{
                    if(preLineNum != line_num){
                        for(int i=0;i<line_num-preLineNum;i++){
                            mid_fp<<endl;
                        }
                    }
                    mid_fp<<token_text<<" ";

                }
            }
            else if(w == LINE_NOTE){
                for(int m=0;m<line_num-preLineNum;m++){
                    mid_fp<<endl;
                }
                preLineNum = line_num;
                w = getToken(fp);
                continue;
            }
            else if(w == BLOCK_NOTE){
                for(int m=0;m< line_num - preLineNum; m++){
                    mid_fp<<endl;
                }
                preLineNum = line_num;
                w = getToken(fp);
                continue;
            }
            else if(w==ERROR_TOKEN){
                if(preLineNum != line_num){
                    for(int i=0;i<line_num-preLineNum;i++){
                        mid_fp<<endl;
                    }
                }
                mid_fp<<token_text;
            }
            else{
                if(preLineNum != line_num){
                    for(int i=0;i<line_num-preLineNum;i++){
                        mid_fp<<endl;
                    }
                }
                mid_fp<<token_text<<" ";
            }
        }
        preLineNum = line_num;
        w = getToken(fp);
    }while(w!=EOF && !fp.eof());
    mid_fp.close();
    return OK;
}