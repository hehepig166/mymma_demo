#include "../include/ASTnode.h"
#include "../include/Integer.h"
#include "../include/Functions.h"
#include "../include/Variable.h"
#include <cstdio>
#include <string>
#include <map>


static Integer ZERO_INTEGER = Integer(0);
static Integer ONE_INTEGER = Integer(1);

/*=================================================================
  局部工具函数以及变量
=================================================================*/

static ASTnode *ComputeSons(ASTnode *node);

// 返回是否是基本元素
static bool IsAtom(const ASTnode *node);

// 返回是否是 True 或 False 或纯数字
static bool IsBool(const ASTnode *node);

// 返回是否是 True 或 数字0
static bool IsTrue(const ASTnode *node);

// 若是基本元素，则调用相应的函数输出它
static void PrintIfBasic(const ASTnode *node);

static void PrintFullForm(const ASTnode *node);

// 将 son 的儿子挂到 node 上，位置紧跟着 son，然后结构并删除 son
// 返回加完后最后一个儿子的位置。（若无儿子，返回 son->preNode）
static ASTnode *UpmountGrandSons(ASTnode *node, ASTnode *son);

static bool CheckFunction(const ASTnode *node);

static ASTnode *mergeLR(ASTnode *node)
{
    if (node->preNode) node->preNode->nxtNode = node;
    if (node->nxtNode) node->nxtNode->preNode = node;
    return node;
}



/*=================================================================
  对树节点统一的调度计算
=================================================================*/
ASTnode *Compute(ASTnode *node)
{
    if (!node) {return NULL;}


    // 暂时只需要处理函数
    // 由于还没建函数对应表，所以枚举比较字符串
    switch(node->nodeInfo->nodeType) {
        case NODETYPE_SYMBOL_FUNCTION: {
            std::string funName = node->nodeInfo->headName;
            if (funName == "FullForm") {
                return FullForm(node);
            }
            if (funName == "AtomQ") {
                return AtomQ(node);
            }
            if (funName == "Plus") {
                return Plus(node);
            }
            if (funName == "Set") {
                return Set(node);
            }
            if (funName == "Times") {
                return Times(node);
            }
            if (funName == "SetDelayed") {
                return SetDelayed(node);
            }
            if (funName == "Function") {
                return Function(node);
            }
            if (funName == "Apply") {
                return Apply(node);
            }
            if (funName == "If") {
                return If(node);
            }
            return ComputeSons(node);
            break;
        }
        case NODETYPE_SYMBOL_VARNAME: {
            // 临时性的代码
            ASTnode *tmp = VariableTable::FindVarNode(*((std::string*)(node->nodeVal)));
            if (!tmp) {
                return node;
            }
            tmp = Compute(Duplicate(tmp, node->preNode, node->nxtNode));
            Destroy(node);
            node = tmp;
            if (node->preNode) node->preNode->nxtNode = node;
            if (node->nxtNode) node->nxtNode->preNode = node;
            break;
        }
    }

    return node;
}



/*=================================================================
  对变量的赋值
=================================================================*/
// 立即赋值
ASTnode *Set(ASTnode *node)
{
    if (!node) {return NULL;}
    if (node->nodeInfo->sonCnt != 2) {
        printf("err: Wrong sonCnt <%d> in Set, must be 2.\n", node->nodeInfo->sonCnt);
        return node;
    }

    ASTnode *LHS = node->sonHead->nxtNode;
    ASTnode *RHS = LHS->nxtNode;

    // 左值必须是单一个变量名节点
    if (LHS->nodeInfo->nodeType != NODETYPE_SYMBOL_VARNAME) {
        printf("err: Wrong type <%d> in Set[[1]], must be NODETYPE_SYMBOL_VARNAME,\n", LHS->nodeInfo->nodeType);
        return node;
    }

    // 检验完成，将RHS分离出来，等一下准备取代 node
    RHS->preNode->nxtNode = RHS->nxtNode;
    RHS->nxtNode->preNode = RHS->preNode;
    node->nodeInfo->sonCnt--;
    RHS->preNode = node->preNode;
    RHS->nxtNode = node->nxtNode;

    // 获取变量名
    std::string varName = *((std::string*)(LHS->nodeVal));

    // 先化简右值
    RHS = Compute(RHS);

    // 检验通过，修改变量表
    // string varName = *((std::string)(LHS->nodeVal))
    VariableTable::SetVarNode(varName, RHS);

    // 重要！！解构原节点
    Destroy(node);

    return mergeLR(RHS);
}

// 延时赋值
ASTnode *SetDelayed(ASTnode *node)
{
    if (!node) {return NULL;}
    if (node->nodeInfo->sonCnt != 2) {
        printf("err: Wrong sonCnt <%d> in Set, must be 2.\n", node->nodeInfo->sonCnt);
        return node;
    }

    ASTnode *LHS = node->sonHead->nxtNode;
    ASTnode *RHS = LHS->nxtNode;

    // 左值必须是单一个变量名节点
    if (LHS->nodeInfo->nodeType != NODETYPE_SYMBOL_VARNAME) {
        printf("err: Wrong type <%d> in Set[[1]], must be NODETYPE_SYMBOL_VARNAME,\n", LHS->nodeInfo->nodeType);
        return node;
    }

    // 检验完成，将RHS分离出来，等一下准备取代 node
    RHS->preNode->nxtNode = RHS->nxtNode;
    RHS->nxtNode->preNode = RHS->preNode;
    node->nodeInfo->sonCnt--;
    RHS->preNode = node->preNode;
    RHS->nxtNode = node->nxtNode;

    // 获取变量名
    std::string varName = *((std::string*)(LHS->nodeVal));

    // SetDelayed不化简右值
    // RHS = Compute(RHS);

    // 检验通过，修改变量表
    // string varName = *((std::string)(LHS->nodeVal))
    VariableTable::SetVarNode(varName, RHS);

    // 重要！！解构原节点
    Destroy(node);

    return mergeLR(RHS);
}


/*=================================================================
  对树节点操作的函数，与用户可使用的函数一一对应
  返回值为新生成的节点或void
  应该将旧的节点解构（除非新节点子树里包含旧节点等特殊情况）
  !!!千万要注意!!!
  要把就节点的 preNode, nxtNode 信息复制到新节点中!!!
=================================================================*/

void Print(const ASTnode *node)
{
    PrintFullForm(node);
}

ASTnode *FullForm(ASTnode *node)
{
    return mergeLR(ComputeSons(node));
}

ASTnode *AtomQ(ASTnode *node)
{
    if (!node) {return NULL;}
    if (node->nodeInfo->sonCnt != 1) {
        printf("err: Wrone sonCnt <%d> in AtomQ, must be 1.\n",node->nodeInfo->sonCnt);
        return node;
    }

    node = ComputeSons(node);

    bool isAtom = IsAtom(node->sonHead->nxtNode);
    ASTnode *pre=node->preNode;
    ASTnode *nxt=node->nxtNode;
    Destroy(node);
    if (isAtom)
        return mergeLR(CreateNode(NODETYPE_SYMBOL_TRUE, "Symbol", pre, nxt));
    else
        return mergeLR(CreateNode(NODETYPE_SYMBOL_FALSE, "Symbol", pre, nxt));
}


ASTnode *Plus(ASTnode *node)
{
    if (!node) {return NULL;}
    node = ComputeSons(node);

    // 要是没有参数，则清除该节点，并返回数字0，注意要把左右节点信息存上
    if (node->nodeInfo->sonCnt == 0) {
        ASTnode *ret = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", node->preNode, node->nxtNode);
        Integer tmp0 = Integer(0);
        SetNodeVal(ret, VALTYPE_INTEGER, &tmp0);
        Destroy(node);
        return mergeLR(ret);
    }

    // 要是只有一个参数，则把这个儿子挂上来，并清除加法节点
    if (node->nodeInfo->sonCnt == 1) {
        ASTnode *ret = Duplicate(node->sonHead->nxtNode, node->preNode, node->nxtNode);
        Destroy(node);
        return mergeLR(ret);
    }

    // 要是有多个参数，先结合律，把同为Plus的儿子展开挂上来
    for (ASTnode *p = node->sonHead->nxtNode; p!=node->sonTail; p=p->nxtNode) {
        if (p->nodeInfo->nodeType == node->nodeInfo->nodeType &&
            p->nodeInfo->headName == node->nodeInfo->headName
        ) {
            p = UpmountGrandSons(node, p);
        }
    }


    // 合并同为整数的项
    Integer sumInteger=Integer(0);
    Integer tmpInteger;
    for (ASTnode *p = node->sonHead->nxtNode; p!=node->sonTail; p=p->nxtNode) {
        if (p->nodeInfo->nodeType == NODETYPE_NUMBER_INTEGER) {
            tmpInteger = *((Integer*)(p->nodeVal));
            sumInteger += tmpInteger;
            p = DeleteSon(node, p);
        }
    }

    if (sumInteger.val!=0) {
        ASTnode *tmp = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", NULL, NULL);
        SetNodeVal(tmp, VALTYPE_INTEGER, &sumInteger);
        AppendSon_move(node, tmp);
    }
    

    // 要是最后化简完只剩一项，那么就把那个儿子提出来，解构原节点并返回提出来的儿子节点
    // 要是剩下零项，说明消完了，创建一个单位元 0 返回。
    // 注意要保留住 pre/nxtNode 的信息
    switch(node->nodeInfo->sonCnt) {
        case 0: {
            ASTnode *tmp = node;
            node = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", tmp->preNode, tmp->nxtNode);
            SetNodeVal(node, VALTYPE_INTEGER, &ZERO_INTEGER);
            Destroy(tmp);
            return mergeLR(node);
            break;
        }
        case 1: {
            ASTnode *tmp = node;
            node = UnmountSon(tmp, tmp->sonHead->nxtNode, tmp->preNode, tmp->nxtNode);
            Destroy(tmp);
            return mergeLR(node);
            break;
        }
        default: {
            return mergeLR(node);
        }
    }
}


ASTnode *Times(ASTnode *node)
{
    if (!node) {return NULL;}
    node = ComputeSons(node);

    // 要是没有参数，则清除该节点，并返回数字0，注意要把左右节点信息存上
    if (node->nodeInfo->sonCnt == 0) {
        ASTnode *ret = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", node->preNode, node->nxtNode);
        Integer tmp0 = Integer(1);
        SetNodeVal(ret, VALTYPE_INTEGER, &tmp0);
        Destroy(node);
        return mergeLR(ret);
    }

    // 要是只有一个参数，则把这个儿子挂上来，并清除加法节点
    if (node->nodeInfo->sonCnt == 1) {
        ASTnode *ret = Duplicate(node->sonHead->nxtNode, node->preNode, node->nxtNode);
        Destroy(node);
        return mergeLR(ret);
    }

    // 要是有多个参数，先结合律，把同为Plus的儿子展开挂上来
    for (ASTnode *p = node->sonHead->nxtNode; p!=node->sonTail; p=p->nxtNode) {
        if (p->nodeInfo->nodeType == node->nodeInfo->nodeType &&
            p->nodeInfo->headName == node->nodeInfo->headName
        ) {
            p = UpmountGrandSons(node, p);
        }
    }


    // 合并同为整数的项
    Integer productInteger=Integer(1);
    Integer tmpInteger;
    for (ASTnode *p = node->sonHead->nxtNode; p!=node->sonTail; p=p->nxtNode) {
        if (p->nodeInfo->nodeType == NODETYPE_NUMBER_INTEGER) {
            tmpInteger = *((Integer*)(p->nodeVal));
            productInteger *= tmpInteger;
            p = DeleteSon(node, p);
        }
    }
    if (productInteger.val!=1) {
        ASTnode *tmp = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", NULL, NULL);
        SetNodeVal(tmp, VALTYPE_INTEGER, &productInteger);
        AppendSon_move(node, tmp);
    }
    

    // 要是最后化简完只剩一项，那么就把那个儿子提出来，解构原节点并返回提出来的儿子节点
    // 要是剩下零项，说明消完了，创建一个单位元 1 返回。
    // 注意要保留住 pre/nxtNode 的信息
    switch(node->nodeInfo->sonCnt) {
        case 0: {
            ASTnode *tmp = node;
            node = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", tmp->preNode, tmp->nxtNode);
            SetNodeVal(node, VALTYPE_INTEGER, &ONE_INTEGER);
            Destroy(tmp);
            return mergeLR(node);
            break;
        }
        case 1: {
            ASTnode *tmp = node;
            node = UnmountSon(tmp, tmp->sonHead->nxtNode, tmp->preNode, tmp->nxtNode);
            Destroy(tmp);
            return mergeLR(node);
            break;
        }
        default: {
            return mergeLR(node);
        }
    }
}


ASTnode *Function(ASTnode *node)
{
    return node;
}


ASTnode *Apply(ASTnode *node)
{
    if (!node) {return NULL;}

    if (node->nodeInfo->sonCnt != 2) {
        printf("Wrong sonCnt <%d> in Apply, must be 2.\n", node->nodeInfo->sonCnt);
    }

    //node->sonHead->nxtNode = Compute(node->sonHead->nxtNode);
    node = ComputeSons(node);
    //node->sonHead->nxtNode->nxtNode = mergeLR(Compute(node->sonHead->nxtNode->nxtNode));

    //PrintFullForm(node);
    //puts("");
    //getchar();

    ASTnode *s1 = node->sonHead->nxtNode;
    ASTnode *s2 = s1->nxtNode;
    if (CheckFunction(s1)) {
        int fail=0;
        ASTnode *nameList = s1->sonHead->nxtNode;
        ASTnode *valList = s2;
        fail |= nameList->nodeInfo->sonCnt != valList->nodeInfo->sonCnt;

        // 保存变量信息并替换变量
        std::map<std::string, ASTnode*> tmpVarMap;
        ASTnode *pName = nameList->sonHead->nxtNode;
        ASTnode *pVal = valList->sonHead->nxtNode;
        for (; !fail && pName!=nameList->sonTail; pName=pName->nxtNode, pVal=pVal->nxtNode) {
            if (pName->nodeInfo->nodeType == NODETYPE_SYMBOL_VARNAME) {
                std::string str = *((std::string*)(pName->nodeVal));
                if (tmpVarMap.find(str)==tmpVarMap.end()) {
                    tmpVarMap[str]=Duplicate(VariableTable::FindVarNode(str), NULL, NULL);
                    VariableTable::SetVarNode(str, pVal);
                }
                else {  //说明变量表有重复
                    printf("err: same var %s in Apply.Function.\n", str.c_str());
                    fail = 1;
                }
            }
            else {
                fail=1;
                break;
            }
        }

        // 计算
        if (!fail) {
            ASTnode *tmpNode = Duplicate(s1->sonHead->nxtNode->nxtNode, node->preNode, node->nxtNode);
            Destroy(node);
            tmpNode = Compute(tmpNode);
            node = tmpNode;
        }

        // 复原变量信息
        std::map<std::string, ASTnode*>::iterator iter;
        for (iter = tmpVarMap.begin(); iter!=tmpVarMap.end(); iter++) {
            if (iter->second) {
                VariableTable::SetVarNode(iter->first, iter->second);
                Destroy(iter->second);
            }
            else {
                VariableTable::EraseVar(iter->first);
            }
        }

        return mergeLR(node);
    }
    else if (s1->nodeInfo->nodeType == NODETYPE_SYMBOL_VARNAME) {
        if (s1->nodeInfo->nodeType == NODETYPE_SYMBOL_VARNAME) {    // 直接替换
            // Apply[Plus, {1,2,3}];
            ASTnode *ret = CreateNode(NODETYPE_SYMBOL_FUNCTION, *((std::string*)s1->nodeVal), node->preNode, node->nxtNode);
            ASTnode *i=s2->sonHead->nxtNode;
            ASTnode *tmpi;
            while (i!=s2->sonTail) {
                tmpi=i->nxtNode;
                AppendSon_move(ret, UnmountSon(s1, i, NULL, NULL));
                i=tmpi;
            }
            Destroy(node);
            node = ret;
        }
        return mergeLR(Compute(node));
    }

    return mergeLR(node);
}


ASTnode *If(ASTnode *node)
{
    if (!node) return NULL;

    // 检查
    if (node->nodeInfo->sonCnt != 3) {
        return mergeLR(Compute(node));
    }

    ASTnode *tmp;

    // 先计算判断
    node->sonHead->nxtNode = Compute(node->sonHead->nxtNode);

    // 若不是可判断的布尔型（暂时定为数字或True或False）
    tmp = node->sonHead->nxtNode;
    if (!IsBool(tmp)) {
        return mergeLR(ComputeSons(node));
    }

    if (IsTrue(tmp)) {
        tmp= UnmountSon(node, node->sonHead->nxtNode->nxtNode, node->preNode, node->nxtNode);
    }
    else {
        tmp= UnmountSon(node, node->sonHead->nxtNode->nxtNode->nxtNode, node->preNode, node->nxtNode);
    }
    tmp= Compute(tmp);
    Destroy(node);
    return mergeLR(tmp);
}


/*=================================================================
  本地函数
=================================================================*/

ASTnode *ComputeSons(ASTnode *node)
{
    if (!node) {return NULL;}


    ASTnode *p, *pL, *pR;
    for (p=node->sonHead->nxtNode; p!=node->sonTail; p=pR) {
        pL = p->preNode;
        pR = p->nxtNode;
        p = Compute(p);
        if (pL) pL->nxtNode = p;
        if (pR) pR->preNode = p;
    }
    return node;
}


static bool IsAtom(const ASTnode *node)
{
    if (!node) {return false;}
    const int nt = node->nodeInfo->nodeType;
    
    switch(nt & NODETYPEBITS) {
        case NODETYPE_NUMBER:
        case NODETYPE_STRING:
            return true;
        case NODETYPE_SYMBOL:
            // 这里应该直接判断 head 是不是 "symbol" 就行的
            // 但是感觉不应该直接用字符串，以后还是会弄个数据结构把 head 映射到整数上
            // 所以先用 VALTYPE 判断一下吧
            switch(nt) {
                case NODETYPE_SYMBOL_VARNAME:
                case NODETYPE_SYMBOL_TRUE:
                case NODETYPE_SYMBOL_FALSE:
                    return true;
            }
    }

    return false;
}

static bool IsBool(const ASTnode *node)
{
    if (!node) return false;
    if (node->nodeInfo->nodeType == NODETYPE_SYMBOL_TRUE) return true;
    if (node->nodeInfo->nodeType == NODETYPE_SYMBOL_FALSE) return true;
    if ((node->nodeInfo->nodeType & NODETYPEBITS) == NODETYPE_NUMBER) return true;
    return false;
}

static bool IsTrue(const ASTnode *node)
{
    if (!node) return false;
    if (node->nodeInfo->nodeType == NODETYPE_SYMBOL_TRUE)
        return true;
    
    if (node->nodeInfo->nodeType == NODETYPE_NUMBER_INTEGER) {
        return !(*((Integer*)(node->nodeVal))==ZERO_INTEGER);
    }

    return false;
}

static void PrintIfBasic(const ASTnode *node)
{
    if (!IsAtom(node)) {return;}
    switch(node->nodeInfo->nodeType) {
        case NODETYPE_NUMBER_INTEGER:
            if (node->nodeVal)
                ((Integer*)(node->nodeVal))->print();
            else
                printf("??");
            break;
        case NODETYPE_SYMBOL_TRUE:
            printf("True");
            break;
        case NODETYPE_SYMBOL_FALSE:
            printf("False");
            break;
        case NODETYPE_SYMBOL_VARNAME:
            printf("%s",((std::string*)(node->nodeVal))->c_str());
            break;
    }
}

void PrintFullForm(const ASTnode *node)
{
    if (!node) {return;}

    if (IsAtom(node)) {
        PrintIfBasic(node);
    }
    else {
        if (node->nodeInfo->headName != "PureFunction")
            printf("%s", node->nodeInfo->headName.c_str());
        else {
            PrintFullForm((ASTnode*)(node->nodeVal));
        }
        printf("[");

        for (const ASTnode *i = node->sonHead->nxtNode; i!=node->sonTail; i=i->nxtNode) {
            PrintFullForm(i);
            if (i->nxtNode != node->sonTail) {
                printf(", ");
            }
        }

        printf("]");
    }
}

static ASTnode *UpmountGrandSons(ASTnode *node, ASTnode *son)
{
    if (!node || !son) {return node;}
    ASTnode *ret = son->preNode;
    son = UnmountSon(node, son, NULL, NULL);
    ASTnode *p = son->sonHead->nxtNode;
    ASTnode *pnxt;
    while (p!=son->sonTail) {
        pnxt = p->nxtNode;
        p=UnmountSon(son, p, NULL, NULL);
        InsertSon_move(node, p, ret);
        ret = ret->nxtNode;
        p = pnxt;
    }
    Destroy(son);
    return mergeLR(ret);
}

static bool CheckFunction(const ASTnode *node)
{
    if (!node) {return false;}

    if (node->nodeInfo->nodeType != NODETYPE_SYMBOL_FUNCTION) {
        return false;
    }
    if (node->nodeInfo->headName != "Function") {
        return false;
    }

    if (node->nodeInfo->sonCnt==2) {
        const ASTnode *s1 = node->sonHead->nxtNode;
        if (s1->nodeInfo->nodeType != NODETYPE_SYMBOL_FUNCTION || s1->nodeInfo->headName != "List") {
            printf("Wrong nodeType <%d> in Function #1, must be a list.", s1->nodeInfo->nodeType);
            return false;
        }
        for (ASTnode *i = s1->sonHead->nxtNode; i!=s1->sonTail; i=i->nxtNode) {
            if (i->nodeInfo->nodeType != NODETYPE_SYMBOL_VARNAME) {
                printf("Wrong nodeType <%d> in Function #1#, must be a symbol.", i->nodeInfo->nodeType);
                return false;
            }
        }
    }
    else {
        printf("Wrong sonCnt <%d> in Function, must be 2.\n", node->nodeInfo->sonCnt);
        return false;
    }
    return true;
}