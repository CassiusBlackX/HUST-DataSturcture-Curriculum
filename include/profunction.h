//
// Created by 张奥林 on 2023/8/25.
//
#pragma once
#ifndef DSEA2_PROFUNCTION_H
#define DSEA2_PROFUNCTION_H
#include <cstdio>
#include <string>
#include <stack>
#include <queue>
#include <vector>

using namespace std;
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
typedef int status;
// 树的相关说明
struct CTNode {
    string data;
    int indent;
    struct Child {
        int child;
        Child* next;
        Child(int c) : child(c), next(nullptr) {}
    };
    Child* firstchild;
    CTNode() : data(""), indent(0), firstchild(nullptr) {}
    CTNode(const string& s, int i) : data(s), indent(i), firstchild(nullptr) {}
};

struct CTree {
    vector<CTNode> nodes;
    int n, r;
    CTree(int _n,int _r) : n(_n), r(_r), nodes(_n+1) {}
    CTree() : n(0), r(0) {
        initializeNodes(); // 如果是空构造的话，随便分配一个100的空间
        // 虽然似乎这样也没有什么意义
    }
    void initializeNodes() {
        nodes.assign(100, CTNode("",0));
    }
};

/*
struct CTree {
    CTNode* nodes;
    int n, r;
    CTree(int _n,int _r) : n(_n), r(_r), nodes(nullptr) {
        initializeNodes(); // Call the function to initialize nodes
    }
    CTree() : n(0), r(0), nodes(nullptr) {}
    void initializeNodes() {
        nodes = new CTNode[1000];
        for (int i = 0; i < 1000; ++i) {
            nodes[i] = CTNode("", 0); // Use the CTNode constructor to initialize
        }
    }
};
*/
status insertChild(CTree & T, int p, int i, const CTree & c);
status getParent(const CTree & T, int child, int & parent);
//void traverseTree(CTree T, status(*visit)(const string &, int));
// T是一个很大的东西了，如果使用拷贝的话可能太占资源了，所以改成const引用
void traverseTree(const CTree & T, status(*visit)(const string &, int));


#endif //DSEA2_PROFUNCTION_H
