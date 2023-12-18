#include "profunction.h"
// 树的相关函数
/*
 * 数组风格的定义的时候的函数，但是这样不能动态分配空间，有点太浪费了
status insertChild(CTree& T, int p, int i, const CTree & c) {
    if (p < 0 || p >= T.n)
        return ERROR;
    if (!c.n)
        return ERROR;
    int j = T.n;
    CTNode::Child* prior = T.nodes[p].firstchild;
    CTNode::Child* cur = prior;
    CTNode::Child* t;
    int k = 0;
    while (k < c.n) {
        T.nodes[j] = c.nodes[k++];
        for (t = T.nodes[j].firstchild; t; t = t->next)
            t->child += T.n;
        j++;
    }
    t = new CTNode::Child(T.n + c.r);
    t->next = nullptr;
    T.n += c.n;
    if (i == 1) {
        if (prior) {
            t->next = prior;
            prior = t;
        } else prior = t;
        T.nodes[p].firstchild = prior;
    } else if (i < 1)
        return ERROR;
    else {
        k = 1;
        while (k < i && cur) {
            k++;
            prior = cur;
            cur = cur->next;
        }
        if (cur) {
            prior->next = t;
            t->next = cur;
        } else if (k == i) {
            prior->next = t;
            t->next = nullptr;
        } else
            return ERROR;
    }
    return OK;
}
*/

status insertChild(CTree& T, int p, int i, const CTree& c) {
    // 检查父节点索引 p 是否在目标树 T 的有效范围内。
    if (p < 0 || p >= T.n)
        return ERROR;
    // 检查子树 c 是否为空。
    if (!c.n)
        return ERROR;
    // 获取目标树 T 的当前大小。
    int j = T.n;
    // 获取父节点的第一个子节点的指针。
    CTNode::Child* prior = T.nodes[p].firstchild;
    // 初始化用于遍历子节点的指针。
    CTNode::Child* cur = prior;
    CTNode::Child* t;
    int k = 0;
    // 将子树 c 的节点插入目标树 T。
    while (k < c.n) {
        T.nodes.insert(T.nodes.begin() + j, c.nodes[k++]);
        // 更新新插入节点的子节点索引。
        for (t = T.nodes[j].firstchild; t; t = t->next){
            t->child += T.n;
        }
        j++;
    }
    // 创建一个新的子节点，以维护与父节点的关系。
    t = new CTNode::Child(T.n + c.r);
    t->next = nullptr;
    // 更新目标树的大小。
    T.n += c.n;
    // 根据 i 的值，确定在父节点的子节点中插入新的子节点的位置。
    if (i == 1) {
        if (prior) {
            t->next = prior;
            prior = t;
        } else
            prior = t;
        // 更新父节点的第一个子节点指针。
        T.nodes[p].firstchild = prior;
    } else if (i < 1)
        return ERROR;
    else {
        k = 1;
        // 找到适合插入新子节点的位置。
        while (k < i && cur) {
            k++;
            prior = cur;
            cur = cur->next;
        }
        if (cur) {
            // 在 prior 和 cur 之间插入新的子节点。
            prior->next = t;
            t->next = cur;
        } else if (k == i) {
            // 在末尾插入新的子节点。
            prior->next = t;
            t->next = nullptr;
        } else
            return ERROR;
    }
    return OK;
}


status getParent(const CTree & T, int child, int& parent) {
    for (int i = 0; i < T.n; i++) {
        CTNode::Child* t = T.nodes[i].firstchild;
        while (t) {
            if (t->child == child) {
                parent = i;
                return OK;
            }
            t = t->next;
        }
    }
    return INFEASIBLE;
}
/*
 * 数组风格的遍历
void traverseTree(const CTree & T, status (*visit)(const string &, int)) {
    stack<int> stack;
    int status = 0;
    int i, j;
    int indent = 0;
    int parent;
    CTNode::Child* t;
    int visited[1000] = {0};
    if (!T.n) return ;
    for (i = 0; i < T.n; i++) {
        indent = 0;
        if (!visited[i]) {
            stack.push(i);
            visited[i] = 1;
            if (getParent(T, i, parent) == OK)
                indent += T.nodes[parent].indent;
            if (T.nodes[i].indent)
                visit(T.nodes[i].data, indent);
            t = T.nodes[i].firstchild;
            while (t && !status) {
                while (visited[t->child] && t->next)
                    t = t->next;
                if (!visited[t->child]) {
                    visited[t->child] = 1;
                    if (T.nodes[t->child].firstchild)
                        stack.push(t->child);
                    getParent(T, t->child, parent);
                    indent += T.nodes[parent].indent;
                    if (T.nodes[t->child].indent)
                        visit(T.nodes[t->child].data, indent);
                    t = T.nodes[t->child].firstchild;
                    status = 0;
                } else
                    status = 1;
            }
        }
        while (!stack.empty()) {
            j = stack.top();
            indent -= T.nodes[j].indent;
            t = T.nodes[j].firstchild;
            status = 0;
            while (t && !status) {
                while (visited[t->child] && t->next)
                    t = t->next;
                if (!visited[t->child]) {
                    visited[t->child] = 1;
                    if (T.nodes[t->child].firstchild)
                        stack.push(t->child);
                    getParent(T, t->child, parent);
                    indent += T.nodes[parent].indent;
                    if (T.nodes[t->child].indent)
                        visit(T.nodes[t->child].data, indent);
                    t = T.nodes[t->child].firstchild;
                    status = 0;
                } else
                    status = 1;
            }
            if (t) stack.pop();
        }
    }
}
*/

void traverseTree(const CTree& T, status (*visit)(const string&, int)) {
    stack<int> stack; // 使用栈来辅助遍历
    int status = 0;
    int i, j;
    int indent = 0;
    int parent;
    CTNode::Child* t;
    vector<bool> visited(T.nodes.size() + 10, false); // 预分配一些内存用于标记是否访问过节点
    if (!T.n) return;
    for (i = 0; i < T.n; i++) {
        indent = 0;
        if (!visited[i]) {
            stack.push(i);
            visited[i] = true;
            // 获取当前节点的父节点，以确定缩进值
            if (getParent(T, i, parent) == OK)
                indent += T.nodes[parent].indent;
            // 如果当前节点有缩进，执行访问操作
            if (T.nodes[i].indent)
                visit(T.nodes[i].data, indent);
            t = T.nodes[i].firstchild;
            while (t && !status) {
                while (visited[t->child] && t->next)
                    t = t->next;
                if (!visited[t->child]) {
                    visited[t->child] = true;
                    // 如果子节点还有孩子，将其加入栈中以后续处理
                    if (T.nodes[t->child].firstchild)
                        stack.push(t->child);
                    // 获取子节点的父节点，更新缩进值
                    getParent(T, t->child, parent);
                    indent += T.nodes[parent].indent;
                    // 如果子节点有缩进，执行访问操作
                    if (T.nodes[t->child].indent)
                        visit(T.nodes[t->child].data, indent);
                    t = T.nodes[t->child].firstchild;
                    status = 0;
                } else
                    status = 1;
            }
        }
        // 处理栈中的节点
        while (!stack.empty()) {
            j = stack.top();
            indent -= T.nodes[j].indent;
            t = T.nodes[j].firstchild;
            status = 0;
            while (t && !status) {
                while (visited[t->child] && t->next)
                    t = t->next;
                if (!visited[t->child]) {
                    visited[t->child] = true;
                    if (T.nodes[t->child].firstchild)
                        stack.push(t->child);
                    getParent(T, t->child, parent);
                    indent += T.nodes[parent].indent;
                    if (T.nodes[t->child].indent)
                        visit(T.nodes[t->child].data, indent);
                    t = T.nodes[t->child].firstchild;
                    status = 0;
                } else
                    status = 1;
            }
            // 如果 t 不为空，将节点从栈中弹出
            if (t) stack.pop();
        }
    }
}
