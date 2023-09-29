// 用于一个字符一个字符的读取并且分析单词本身的种类
enum token_kind{AUTO,BREAK,CASE,
    CHAR,
    CONST,
    CONTINUE,
    DEFAULT,
    DO,
    DOUBLE,
    ELSE,
    ENUM,
    EXTERN,
    FLOAT,
    FOR,
    GOTO,
    IF,
    INT,
    LONG,
    REGISTER,
    RETURN,
    SHORT,
    SIGNED,
    SIZEOF,
    STATIC,
    STRUCT,
    SWITCH,
    TYPEDEF,
    UNION,
    UNSIGNED,
    VOID,
    VOLATILE, //用于告诉编译器不要对被声明为 volatile 的变量进行优化，以确保对该变量的访问不被编译器优化为从寄存器中读取，而是从内存中实际读取或写入。这在多线程、中断处理、硬件寄存器访问等场景中非常有用。
    WHILE,
    INCLUDE,
    DEFINE,
    IDENTIFIER,
    INT_CONST,
    UNSIGNED_CONST,
    LONG_CONST,
    UNSIGNED_LONG_CONST,
    DOUBLE_CONST,
    FLOAT_CONST,
    LONG_DOUBLE_CONST,
    CHAR_CONST,
    STRING_CONST,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MOD,
    MORE,
    LESS,
    EQUAL,
    UNEQUAL,
    MORE_EQUAL,
    LESS_EQUAL,
    AND,
    OR,
    ASSIGN,
    LL,
    RL,
    LM,
    RM,
    LS,
    RS,
    LINE_NOTE,
    BLOCK_NOTE,
    SEMI_COLUMN,
    COMMA,
    ERROR_TOKEN,
    POUND,
};

//
// Created by 张奥林 on 2023/8/25.
//
#pragma once
#ifndef DSEA2_LEXER_H
#define DSEA2_LEXER_H

#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
typedef struct keyword{
    char key[10];
    int enum_key;
}keyword;

extern keyword n[IDENTIFIER];  // 储存各类关键字
extern std::string token_text;// 暂时存储常量
extern int line_num; // 检测运行行数
int getToken(std::ifstream & file);


#endif //DSEA2_LEXER_H
