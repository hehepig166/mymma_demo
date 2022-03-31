#include "../include/ASTnode.h"
#include "../include/Integer.h"
#include "../include/Functions.h"
#include "../include/Variable.h"
#include "../include/Tools.h"
#include <cstdio>
#include <string>
#include <map>

extern VariableTable globalVariableTable;

static Integer ZERO_INTEGER = Integer(0);
static Integer ONE_INTEGER = Integer(1);
static Integer NEG_ONE_INTEGER = Integer(-1);

/*=================================================================
  局部工具函数以及变量
=================================================================*/

static void PrintHelp();

static ASTnode *ComputeSons(ASTnode *node);

// 返回是否是基本元素
static bool IsAtom(const ASTnode *node);

// 返回是否是 True 或 False 或纯数字
static bool IsBool(const ASTnode *node);

// 返回是否是 True 或 数字0
static bool IsTrue(const ASTnode *node);

static bool IsNumber(const ASTnode *node);

// 是否可比
static bool _Comparable(const ASTnode *n1, const ASTnode *n2);
// 二元判等于，使用前应先判断_Comparable
static bool _Equal(const ASTnode *n1, const ASTnode *n2);

// 若是基本元素，则调用相应的函数输出它
static void PrintIfBasic(const ASTnode *node);

static void PrintFullForm(const ASTnode *node);

// 将 son 的儿子挂到 node 上，位置紧跟着 son，然后结构并删除 son
// 返回加完后最后一个儿子的位置。（若无儿子，返回 son->preNode）
static ASTnode *UpmountGrandSons(ASTnode *node, ASTnode *son);

static bool CheckFunction(const ASTnode *node);

// 最好在每个函数 return 时都 mergeLR 一下
static ASTnode *mergeLR(ASTnode *node)
{
    if (!node) return NULL;
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
            if (funName == "Power") {
                return Power(node);
            }
            if (funName == "Set") {
                return Set(node);
            }
            if (funName == "Times") {
                return Times(node);
            }
            if (funName == "Quotient") {
                return Quotient(node);
            }
            if (funName == "Mod") {
                return Mod(node);
            }
            if (funName == "Divide") {
                return Divide(node);
            }
            if (funName == "Subtract") {
                return Subtract(node);
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
            if (funName == "Flatten") {
                return Flatten(node);
            }
            if (funName == "Equal") {
                return Equal(node);
            }
            if (funName == "Less") {
                return Less(node);
            }
            if (funName == "Greater") {
                return Greater(node);;
            }
            if (funName == "Sort") {
                return Sort(node);
            }


            if (funName == "Help") {
                PrintHelp();
            }

            if (globalVariableTable.FindVarNode(node->nodeInfo->headName)) {
                ASTnode *nodeApply = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Apply", node->preNode, node->nxtNode);
                ASTnode *nodeHead = CreateNode(NODETYPE_SYMBOL_VARNAME, "Symbol", NULL, NULL);
                SetNodeVal(nodeHead, VALTYPE_STRING, &(node->nodeInfo->headName));
                node->nodeInfo->headName = "List";
                node->preNode = node->nxtNode = NULL;   //重要
                AppendSon_move(nodeApply, nodeHead);
                AppendSon_move(nodeApply, node);
                node=nodeApply;
                return Compute(mergeLR(node));
            }

            return ComputeSons(node);
            break;
        }
        case NODETYPE_SYMBOL_VARNAME: {
            // 临时性的代码
            ASTnode *tmp = globalVariableTable.FindVarNode(*((std::string*)(node->nodeVal)));
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
    RHS = UnmountSon(node, RHS, node->preNode, node->nxtNode);

    // 获取变量名
    std::string varName = *((std::string*)(LHS->nodeVal));

    // 先化简右值
    RHS = Compute(RHS);

    // 检验通过，修改变量表
    // string varName = *((std::string)(LHS->nodeVal))
    globalVariableTable.SetVarNode(varName, RHS);

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
    RHS = UnmountSon(node, RHS, node->preNode, node->nxtNode);

    // 获取变量名
    std::string varName = *((std::string*)(LHS->nodeVal));

    // SetDelayed不化简右值
    // RHS = Compute(RHS);

    // 检验通过，修改变量表
    // string varName = *((std::string)(LHS->nodeVal))
    globalVariableTable.SetVarNode(varName, RHS);

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

ASTnode *Equal(ASTnode *node)
{
    if (!node) {return NULL;}
    int flag=1;
    // 1: True
    // 2: False
    // 3: 保留

    node = ComputeSons(node);

    ASTnode *tmp = node->sonHead->nxtNode;
    for (ASTnode *p = tmp->nxtNode; p!=node->sonTail;) {
        ASTnode *nxt = p->nxtNode;
        if (!_Comparable(tmp, p)) {
            flag = 3;
            break;
        }
        if (!_Equal(tmp, p)) {
            flag = 2;
            break;
        }
        p=nxt;
    }

    ASTnode *ret;
    if (flag == 1) {
        ret = CreateNode(NODETYPE_SYMBOL_TRUE, "Symbol", node->preNode, node->nxtNode);
    }
    else if (flag == 2) {
        ret = CreateNode(NODETYPE_SYMBOL_FALSE, "Symbol", node->preNode, node->nxtNode);
    }
    else {
        ret = node;
        node = NULL;
    }

    Destroy(node);    
    return mergeLR(ret);
}


ASTnode *Less(ASTnode *node)
{
    if (!node) return NULL;
    int flag=1;
    // 1: True
    // 2: False
    // 3: 保留

    node = ComputeSons(node);

    if (node->nodeInfo->sonCnt >= 2) {
        for (ASTnode *p=node->sonHead->nxtNode; p->nxtNode!=node->sonTail; p=p->nxtNode) {
            if (!_Comparable(p, p->nxtNode)) {
                flag = 3;
                break;
            }
            if (!(*p < *(p->nxtNode))) {
                flag = 2;
                break;
            }
        }
    }

    ASTnode *ret;
    if (flag == 1) {
        ret = CreateNode(NODETYPE_SYMBOL_TRUE, "Symbol", node->preNode, node->nxtNode);
    }
    else if (flag == 2) {
        ret = CreateNode(NODETYPE_SYMBOL_FALSE, "Symbol", node->preNode, node->nxtNode);
    }
    else {
        ret = node;
        node = NULL;
    }

    Destroy(node);    
    return mergeLR(ret);
}

ASTnode *Greater(ASTnode *node)
{
    if (!node) return NULL;
    int flag=1;
    // 1: True
    // 2: False
    // 3: 保留

    node = ComputeSons(node);

    if (node->nodeInfo->sonCnt >= 2) {
        for (ASTnode *p=node->sonHead->nxtNode; p->nxtNode!=node->sonTail; p=p->nxtNode) {
            if (!_Comparable(p, p->nxtNode)) {
                flag = 3;
                break;
            }
            if ((*p < *(p->nxtNode)) || *p == *(p->nxtNode)) {
                flag = 2;
                break;
            }
        }
    }

    ASTnode *ret;
    if (flag == 1) {
        ret = CreateNode(NODETYPE_SYMBOL_TRUE, "Symbol", node->preNode, node->nxtNode);
    }
    else if (flag == 2) {
        ret = CreateNode(NODETYPE_SYMBOL_FALSE, "Symbol", node->preNode, node->nxtNode);
    }
    else {
        ret = node;
        node = NULL;
    }

    Destroy(node);    
    return mergeLR(ret);
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
    


    node = Tools_SortSons(node);    //20220325



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



    // 20220329
    /* 把不是Power的套一个Power[#, 1]&，再合并底数相同的 */
    // 套Power
    for (ASTnode *p = node->sonHead->nxtNode; p!=node->sonTail; p=p->nxtNode) {
        if (p->nodeInfo->headName != "Power") {
            ASTnode *pre = p->preNode;
            p = UnmountSon(node, p, NULL, NULL);
            ASTnode *tmp = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Power", NULL, NULL);
            AppendSon_move(tmp, p);
            ASTnode *tmpOne = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", NULL, NULL);
            SetNodeVal(tmpOne, VALTYPE_INTEGER, &ONE_INTEGER);
            AppendSon_move(tmp, tmpOne);
            p = InsertSon_move(node, tmp, pre);
        }
    }

    // 排序，让底数相同的在一起
    node = Tools_SortSons(node);
    // 合并底数相同的
    for (ASTnode *p = node->sonHead->nxtNode; p!=node->sonTail && p->nxtNode!=node->sonTail; p=p->nxtNode) {
        ASTnode *nxt = p->nxtNode;
        if (*(GetSon(p, 1)) == *(GetSon(nxt, 1))) {
            ASTnode *tmpPlus = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Plus", NULL, NULL);
            AppendSon_move(tmpPlus, UnmountSon(p, p->sonHead->nxtNode->nxtNode, NULL, NULL));
            while (nxt != node->sonTail && (*(GetSon(p, 1)) == *(GetSon(nxt, 1)))) {
                ASTnode *tmp = nxt->nxtNode;
                nxt = UnmountSon(node, nxt, NULL, NULL);
                AppendSon_move(tmpPlus, UnmountSon(nxt, GetSon(nxt, 2), NULL, NULL));
                Destroy(nxt);
                nxt = tmp;
            }
            AppendSon_move(p, tmpPlus);
        }
        p = Compute(p);
    }
    node = ComputeSons(node);
    

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


    node = Tools_SortSons(node);    //20220325



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


ASTnode *Power(ASTnode *node)
{
    if (!node) return NULL;

    node = ComputeSons(node);

    // 若没参数，返回 Integer 1
    if (node->nodeInfo->sonCnt == 0) {
        ASTnode *ret = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", node->preNode, node->nxtNode);
        Destroy(node);
        SetNodeVal(ret, VALTYPE_INTEGER, &ONE_INTEGER);
        return mergeLR(ret);
    }

    // 若只有一个参数，返回这个参数
    if (node->nodeInfo->sonCnt == 1) {
        ASTnode *ret = UnmountSon(node, node->sonHead->nxtNode, node->preNode, node->nxtNode);
        Destroy(node);
        return mergeLR(ret);
    }

    // 若大于两个参数，递归
    // Power[x,y,z,...] = Power[x, Power[y, z, ...]]
    if (node->nodeInfo->sonCnt > 2) {
        ASTnode *ret = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Power", node->preNode, node->nxtNode);
        AppendSon_move(ret, UnmountSon(node, GetSon(node, 1), NULL, NULL));
        AppendSon_move(ret, Compute(node));
        return mergeLR(Compute(ret));
    }

    // 以下保证了两个参数

    ASTnode *pExp = node->sonHead->nxtNode->nxtNode;
    
    // 要是指数不是Integer，则保持原样返回
    if (pExp->nodeInfo->nodeType != NODETYPE_NUMBER_INTEGER) {
        return mergeLR(node);
    }

    // 以下前提 “指数是 Integer ”

    // X^0 = 1
    if (*((Integer*)(pExp->nodeVal)) == ZERO_INTEGER) {
        ASTnode *ret = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", node->preNode, node->nxtNode);
        Destroy(node);
        SetNodeVal(ret, VALTYPE_INTEGER, &ONE_INTEGER);
        return mergeLR(ret);
    }

    // X^1 = X
    if (*((Integer*)(pExp->nodeVal)) == ONE_INTEGER) {
        ASTnode *ret = UnmountSon(node, GetSon(node, 1), node->preNode, node->nxtNode);
        Destroy(node);
        return mergeLR(ret);
    }

    // 底数是Integer，指数为非负整数，可计算
    if (ZERO_INTEGER<*((Integer*)(pExp->nodeVal)) && node->sonHead->nxtNode->nodeInfo->nodeType == NODETYPE_NUMBER_INTEGER) {
        Integer x = *((Integer*)(node->sonHead->nxtNode->nodeVal));
        Integer k = *((Integer*)(pExp->nodeVal));
        Integer ans = ONE_INTEGER;
        while (ZERO_INTEGER < k) {
            if ((k&ONE_INTEGER) == ONE_INTEGER)
                ans*=x;
            k>>=ONE_INTEGER;
            x*=x;
        }
        ASTnode *ret = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", node->preNode, node->nxtNode);
        SetNodeVal(ret, VALTYPE_INTEGER, &ans);
        Destroy(node);
        return mergeLR(ret);
    }

    // (a*b*c)^e = a^e * b^e * c^e
    if (node->sonHead->nxtNode->nodeInfo->headName == "Times") {
        ASTnode *ret = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Times", node->preNode, node->nxtNode);
        ASTnode *p = node->sonHead->nxtNode;
        while (p->nodeInfo->sonCnt) {
            ASTnode *tmp = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Power", NULL, NULL);
            AppendSon_move(tmp, UnmountSon(p, p->sonHead->nxtNode, NULL, NULL));
            AppendSon_move(tmp, Duplicate(pExp, NULL, NULL));
            AppendSon_move(ret, tmp);
        }
        Destroy(node);
        return Times(ret);
    }

    // (a^b)^c = a^(b*c)
    if (node->sonHead->nxtNode->nodeInfo->headName == "Power") {
        ASTnode *tmpExp = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Times", NULL, NULL);
        AppendSon_move(tmpExp, UnmountSon(GetSon(node, 1), GetSon(GetSon(node, 1), 2), NULL, NULL));
        AppendSon_move(tmpExp, UnmountSon(node, GetSon(node, 2), NULL, NULL));
        AppendSon_move(node, tmpExp);
        return Compute(node);
    }

    return node;
}



ASTnode *Quotient(ASTnode *node)
{
    if (!node) {return NULL;}
    node = ComputeSons(node);

    // 检查，只接收两个 Integer
    if (node->nodeInfo->sonCnt != 2) {
        return mergeLR(node);
    }
    if (node->sonHead->nxtNode->nodeInfo->nodeType != NODETYPE_NUMBER_INTEGER) {
        return mergeLR(node);
    }
    if (node->sonHead->nxtNode->nxtNode->nodeInfo->nodeType != NODETYPE_NUMBER_INTEGER) {
        return mergeLR(node);
    }

    ASTnode *ret = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", node->preNode, node->nxtNode);
    node->preNode = node->nxtNode = NULL;
    Integer a1 = *((Integer*)(node->sonHead->nxtNode->nodeVal));
    Integer a2 = *((Integer*)(node->sonHead->nxtNode->nxtNode->nodeVal));
    a1/=a2;
    SetNodeVal(ret, VALTYPE_INTEGER, &a1);
    Destroy(node);

    return mergeLR(ret);
}

ASTnode *Mod(ASTnode *node)
{
    if (!node) {return NULL;}
    node = ComputeSons(node);

    // 检查，只接收两个 Integer
    if (node->nodeInfo->sonCnt != 2) {
        return mergeLR(node);
    }
    if (node->sonHead->nxtNode->nodeInfo->nodeType != NODETYPE_NUMBER_INTEGER) {
        return mergeLR(node);
    }
    if (node->sonHead->nxtNode->nxtNode->nodeInfo->nodeType != NODETYPE_NUMBER_INTEGER) {
        return mergeLR(node);
    }

    ASTnode *ret = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", node->preNode, node->nxtNode);
    node->preNode = node->nxtNode = NULL;
    Integer a1 = *((Integer*)(node->sonHead->nxtNode->nodeVal));
    Integer a2 = *((Integer*)(node->sonHead->nxtNode->nxtNode->nodeVal));
    a1%=a2;
    SetNodeVal(ret, VALTYPE_INTEGER, &a1);
    Destroy(node);

    return mergeLR(ret);
}


ASTnode *Divide(ASTnode *node)
{
    if (!node) return NULL;

    // 只接收两个参数，否则报错
    if (node->nodeInfo->sonCnt != 2) {
        printf("Err: wrong sonCnt<%d> in Divide, must be 2.\n", node->nodeInfo->sonCnt);
        return node;
    }

    // a/b => a*(b^-1)
    ASTnode *ret = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Times", node->preNode, node->nxtNode);
    AppendSon_move(ret, UnmountSon(node, GetSon(node, 1), NULL, NULL));
    ASTnode *tmp = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Power", NULL, NULL);
    AppendSon_move(tmp, UnmountSon(node, GetSon(node, 1), NULL, NULL));
    ASTnode *tmpn1 = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", NULL, NULL);
    SetNodeVal(tmpn1, VALTYPE_INTEGER, &NEG_ONE_INTEGER);
    AppendSon_move(tmp, tmpn1);
    AppendSon_move(ret, tmp);
    Destroy(node);


    ret = Compute(ret);
    return ret;
}


ASTnode *Subtract(ASTnode *node)
{
    if (!node) return NULL;

    // 只接收两个参数，否则报错
    if (node->nodeInfo->sonCnt != 2) {
        printf("Err: wrong sonCnt<%d> in Subtract, must be 2.\n", node->nodeInfo->sonCnt);
        return node;
    }

    // a/b => a*(b^-1)
    ASTnode *ret = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Plus", node->preNode, node->nxtNode);
    AppendSon_move(ret, UnmountSon(node, GetSon(node, 1), NULL, NULL));
    ASTnode *tmp = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Times", NULL, NULL);
    AppendSon_move(tmp, UnmountSon(node, GetSon(node, 1), NULL, NULL));
    ASTnode *tmpn1 = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", NULL, NULL);
    SetNodeVal(tmpn1, VALTYPE_INTEGER, &NEG_ONE_INTEGER);
    AppendSon_move(tmp, tmpn1);
    AppendSon_move(ret, tmp);
    Destroy(node);


    ret = Compute(ret);
    return ret;
}



ASTnode *Function(ASTnode *node)
{
    /*
    if (!node) {return NULL;}
    if (!CheckFunction(node)) return node;

    ASTnode *nameList = node->sonHead->nxtNode;
    ASTnode *expr = nameList->nxtNode;
    std::map<std::string, ASTnode*> tmpVarMap;
    int fail=0;
    for (ASTnode *p = nameList->sonHead->nxtNode; !fail && p!=nameList->sonTail; p=p->nxtNode) {
        if (p->nodeInfo->nodeType == NODETYPE_SYMBOL_VARNAME) {
            std::string str = *((std::string*)(p->nodeVal));
            if (tmpVarMap.find(str)==tmpVarMap.end()) {
                tmpVarMap[str]=Duplicate(globalVariableTable.FindVarNode(str), NULL, NULL);
                globalVariableTable.SetVarNode(str, NULL);
            }
            else {  //说明有重复变量
                printf("err: same var %s in Function\n", str.c_str());
                fail=1;
            }
        }
        else {
            fail =1;
            break;
        }
    }

    if (!fail) {
        expr = mergeLR(Compute(expr));
    }

    std::map<std::string, ASTnode*>::iterator iter;
    for (iter = tmpVarMap.begin(); iter!=tmpVarMap.end(); iter++) {
        if (iter->second) {
            globalVariableTable.SetVarNode(iter->first, iter->second);
            Destroy(iter->second);
        }
        else {
            globalVariableTable.EraseVar(iter->first);
        }
    }
    */
    return mergeLR(node);
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
        // 只要valList不比nameList短就行，多出来的话从前往后取需要的
        fail |= nameList->nodeInfo->sonCnt > valList->nodeInfo->sonCnt;

        // 保存变量信息并替换变量
        std::map<std::string, ASTnode*> tmpVarMap;
        ASTnode *pName = nameList->sonHead->nxtNode;
        ASTnode *pVal = valList->sonHead->nxtNode;
        for (; !fail && pName!=nameList->sonTail; pName=pName->nxtNode, pVal=pVal->nxtNode) {
            if (pName->nodeInfo->nodeType == NODETYPE_SYMBOL_VARNAME) {
                std::string str = *((std::string*)(pName->nodeVal));
                if (tmpVarMap.find(str)==tmpVarMap.end()) {
                    tmpVarMap[str]=Duplicate(globalVariableTable.FindVarNode(str), NULL, NULL);
                    globalVariableTable.SetVarNode(str, pVal);
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
                globalVariableTable.SetVarNode(iter->first, iter->second);
                Destroy(iter->second);
            }
            else {
                globalVariableTable.EraseVar(iter->first);
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

    // 若不是可判断的布尔型（暂时定为数字或True或False），则不计算
    tmp = node->sonHead->nxtNode;
    if (!IsBool(tmp)) {
        return mergeLR(node);
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


ASTnode *Flatten(ASTnode *node)
{
    if (!node) return NULL;
    node = ComputeSons(node);

    // 检查，若是对List节点Flatten，则可以
    if (node->nodeInfo->headName == "List") {
        for (ASTnode *p = node->sonHead->nxtNode; p!=node->sonTail; p=p->nxtNode) {
            p = Compute(p);
            if (p->nodeInfo->headName == "List") {
                p = Flatten(p);
                p = UpmountGrandSons(node, p);
            }
        }

        return mergeLR(node);
    }

    // 检查，若是对Flatten函数节点Flatten，儿子必须是一个List
    if (node->nodeInfo->sonCnt != 1 || node->sonHead->nxtNode->nodeInfo->headName != "List") {
        puts("warning: Flatten just accept one List!!");
        return node;
    }
    ASTnode *ret = UnmountSon(node, node->sonHead->nxtNode, node->preNode, node->nxtNode);
    Destroy(node);
    ret = Flatten(ret);
    return mergeLR(ret);
}


ASTnode *Sort(ASTnode *node)
{
    if (!node) return NULL;
    node = ComputeSons(node);

    // Sort[List]
    if (node->nodeInfo->sonCnt == 1) {
        ASTnode *ret = UnmountSon(node, node->sonHead->nxtNode, node->preNode, node->nxtNode);
        Destroy(node);
        ret = Tools_SortSons(ret);
        return ret;
    }
    else if (node->nodeInfo->sonCnt == 2) {
        ASTnode *ret = UnmountSon(node, node->sonHead->nxtNode, node->preNode, node->nxtNode);
        ASTnode *cmp = UnmountSon(node, node->sonHead->nxtNode, node->preNode, node->nxtNode);
        Destroy(node);
        ret = Tools_SortSons(ret, cmp);
        Destroy(cmp);
        return ret;
    }

    return node;
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

static bool IsNumber(const ASTnode *node)
{
    if ((node->nodeInfo->nodeType & NODETYPEBITS) == NODETYPE_NUMBER) {
        return true;
    }
    return false;
}


static bool _Comparable(const ASTnode *n1, const ASTnode *n2)
{
    if (IsNumber(n1) && IsNumber(n2)) {
        return true;
    }

    return false;
}

static bool _Equal(const ASTnode *n1, const ASTnode *n2)
{
    if (!n1 || !n2) {return false;}

    // 先判断nodeInfo是否相等
    if (*(n1->nodeInfo) != *(n2->nodeInfo)) {
        return false;
    }

    // 然后就分情况讨论了
    if (n1->nodeInfo->nodeType == NODETYPE_NUMBER_INTEGER) {
        return (*((Integer*)(n1->nodeVal)) == *((Integer*)(n2->nodeVal)));
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













void PrintHelp()
{
    puts("Set[varName, expr]");
    puts("SetDelayed[varName, expr]");
    puts("FullForm[expr]");
    puts("AtomQ[expr]");
    puts("Equal[expr1, expr2, ...]");
    puts("Less[expr1, expr2, ...]");
    puts("Greater[expr1, expr2, ...]");
    puts("Plus[expr1, expr2, ...]");
    puts("Times[expr1, expr2, ...]");
    puts("Power[expr1, expr2, ...]");
    puts("Quotient[divisor, dividend]");
    puts("Mod[divisor, dividend]");
    puts("Divide[divisor, dividend]");
    puts("Subtract[a, b]");
    puts("List[expr1, expr2, ...]");
    puts("Function[List, expr]");
    puts("Apply[func, List]");
    puts("If[judgeExpr, trueExpr, falseExpr]");
    puts("Flatten[List]");
    puts("Sort[List]");
    puts("Sort[List, func]");
    puts("Help[]");
    puts("Exit");
}