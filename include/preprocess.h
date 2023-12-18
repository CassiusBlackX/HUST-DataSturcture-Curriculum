//
// Created by 张奥林 on 2023/8/25.
//
#pragma once
#ifndef DSEA2_PREPROCESS_H
#define DSEA2_PREPROCESS_H
#include <cstdio>
#include <iostream>
#include <string>
#include "SymbolTable.h"
typedef int status;
#define OK 1
#define ERROR 0
typedef struct define_data{
    // 用于存储define宏定义数据
    std::string identifier;
    std::string string;
}define_data;
typedef struct include_data{
    std::string string;
}include_data;
extern SymbolTable symbolTable;
extern define_data data_Def[10]; // 用于存储define宏定义的内容
extern include_data data_Inculd[10]; // 用于存储include文件包含的被容
// todo 换一个名字
extern int dataDefNum;
status preProcess(std::ifstream &fp);
#endif //DSEA2_PREPROCESS_H
