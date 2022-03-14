#ifndef HEHEPIG_MMA_ASTNODE
#define HEHEPIG_MMA_ASTNODE

#include <string>

/*
低4bit
    NUMBER: 数字，包括整型和实型
    SYMBOL: 符号，包括函数、变量名
    STRING: 字符串
其它位用作更细分（但本质上只有这三种元素）
*/
#define NODETYPEBITS                (0x0f)
#define NODETYPE_NUMBER             (0x01)
#define NODETYPE_SYMBOL             (0x02)
#define NODETYPE_STRING             (0x04)
#define NODETYPE_NUMBER_INTEGER     (0x11)
#define NODETYPE_NUMBER_RATIONAL    (0x21)
#define NODETYPE_NUMBER_REAL        (0x31)
#define NodeTYPE_NUMBER_COMPLEX     (0x41)
#define NODETYPE_SYMBOL_VARNAME     (0x12)
#define NODETYPE_SYMBOL_FUNCTION    (0x22)
#define NODETYPE_SYMBOL_TRUE        (0x32)
#define NODETYPE_SYMBOL_FALSE       (0x42)

#define VALTYPE_NULL        (0)
#define VALTYPE_INTEGER     (1)
#define VALTYPE_REAL        (2)
#define VALTYPE_CHARSTAR    (3)
#define VALTYPE_STRING      (4)
#define VALTYPE_ASTNODE     (5)




struct ASTnodeInfo {
    int nodeType;

    int valType;

    int sonCnt;
    
    std::string headName;
    int headID;
};

struct ASTnode {

    ASTnodeInfo *nodeInfo;

    // nodeVal 里面可以存任何类型的东西
    // 存储、释放是调用DestrpyNodeval或SetNodeVal根据 valType 实现的
    // 内部会调用 new, delete，会自动调用构造析构
    void *nodeVal;

    ASTnode *sonHead;
    ASTnode *sonTail;

    ASTnode *preNode;
    ASTnode *nxtNode;

    //~ASTnode();
};



// 调用该函数以后理应紧跟着SetNodeVal_move
ASTnode *CreateNode(int nodeType_, std::string headName_, const ASTnode *preNode_, const ASTnode *nxtNode_);

// 根据 valType 调用 delete 进行空间释放
void DestroyNodeVal(ASTnode *node);

// 会用 DestroyNodeVal 释放掉原来的空间，但不申请空间，直接将nodeVal_指针传给 node。
void SetNodeVal_move(ASTnode *node, int valType_, void *val_);

// 会用 DestroyNodeVal 释放掉原来的空间，并重新申请空间。
// 注意新空间的内容会直接用等于号复制。
void SetNodeVal(ASTnode *node, int valType_, const void *val_);

// 只是把指针给过去，并且会修改 *son_ 的 preNode, nxtNode
// 一般来说 son_ 应该已经处理好是个单独的根节点了，
// 不过 AddSon_move 还是会帮你把 son_ 的 preNode，nxtNode 接在一起 
void AppendSon_move(ASTnode *node, ASTnode *son_);

// node 在 preSon_ 后面加儿子 son_
// 返回加了之后的地址（也就是 son_）
// 传指针
// 请确保 preSon_ 是 node 的儿子
ASTnode *InsertSon_move(ASTnode *node, ASTnode *son_, ASTnode *preSon_);

// 解除 node 的一个儿子 son_，但不解构，返回 son_ 
// 请确保 son_ 是 node 的儿子
// 慎用！
ASTnode *UnmountSon(ASTnode *node, ASTnode *son_);

// 解构并删除 node 的一个儿子 son_
// 注意维护好 son_ 的 pre/nxt
// 请确保 son_ 是 node 的儿子
// 返回 son_->preNode
ASTnode *DeleteSon(ASTnode *node, ASTnode *son_);

// 下标从 1 开始
// 若没有那一项，则返回 NULL
ASTnode *GetSon(ASTnode *node, int index);

// 递归复制
ASTnode *Duplicate(const ASTnode *node, const ASTnode *preNode_, const ASTnode *nxtNode_);

// 递归解构
void Destroy(ASTnode *node);

#endif