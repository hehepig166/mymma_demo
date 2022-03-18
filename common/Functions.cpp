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
  �ֲ����ߺ����Լ�����
=================================================================*/

static ASTnode *ComputeSons(ASTnode *node);

// �����Ƿ��ǻ���Ԫ��
static bool IsAtom(const ASTnode *node);

// �����Ƿ��� True �� False ������
static bool IsBool(const ASTnode *node);

// �����Ƿ��� True �� ����0
static bool IsTrue(const ASTnode *node);

// ���ǻ���Ԫ�أ��������Ӧ�ĺ��������
static void PrintIfBasic(const ASTnode *node);

static void PrintFullForm(const ASTnode *node);

// �� son �Ķ��ӹҵ� node �ϣ�λ�ý����� son��Ȼ��ṹ��ɾ�� son
// ���ؼ�������һ�����ӵ�λ�á������޶��ӣ����� son->preNode��
static ASTnode *UpmountGrandSons(ASTnode *node, ASTnode *son);

static bool CheckFunction(const ASTnode *node);

// �����ÿ������ return ʱ�� mergeLE һ��
static ASTnode *mergeLR(ASTnode *node)
{
    if (node->preNode) node->preNode->nxtNode = node;
    if (node->nxtNode) node->nxtNode->preNode = node;
    return node;
}



/*=================================================================
  �����ڵ�ͳһ�ĵ��ȼ���
=================================================================*/
ASTnode *Compute(ASTnode *node)
{
    if (!node) {return NULL;}


    // ��ʱֻ��Ҫ������
    // ���ڻ�û��������Ӧ������ö�ٱȽ��ַ���
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
            if (funName == "Quotient") {
                return Quotient(node);
            }
            if (funName == "Mod") {
                return Mod(node);
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

            if (VariableTable::FindVarNode(node->nodeInfo->headName)) {
                ASTnode *nodeApply = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Apply", node->preNode, node->nxtNode);
                ASTnode *nodeHead = CreateNode(NODETYPE_SYMBOL_VARNAME, "Symbol", NULL, NULL);
                SetNodeVal(nodeHead, VALTYPE_STRING, &(node->nodeInfo->headName));
                node->nodeInfo->headName = "List";
                node->preNode = node->nxtNode = NULL;   //��Ҫ
                AppendSon_move(nodeApply, nodeHead);
                AppendSon_move(nodeApply, node);
                node=nodeApply;
                return Compute(mergeLR(node));
            }

            return ComputeSons(node);
            break;
        }
        case NODETYPE_SYMBOL_VARNAME: {
            // ��ʱ�ԵĴ���
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
  �Ա����ĸ�ֵ
=================================================================*/
// ������ֵ
ASTnode *Set(ASTnode *node)
{
    if (!node) {return NULL;}
    if (node->nodeInfo->sonCnt != 2) {
        printf("err: Wrong sonCnt <%d> in Set, must be 2.\n", node->nodeInfo->sonCnt);
        return node;
    }

    ASTnode *LHS = node->sonHead->nxtNode;
    ASTnode *RHS = LHS->nxtNode;

    // ��ֵ�����ǵ�һ���������ڵ�
    if (LHS->nodeInfo->nodeType != NODETYPE_SYMBOL_VARNAME) {
        printf("err: Wrong type <%d> in Set[[1]], must be NODETYPE_SYMBOL_VARNAME,\n", LHS->nodeInfo->nodeType);
        return node;
    }

    // ������ɣ���RHS�����������һ��׼��ȡ�� node
    RHS->preNode->nxtNode = RHS->nxtNode;
    RHS->nxtNode->preNode = RHS->preNode;
    node->nodeInfo->sonCnt--;
    RHS->preNode = node->preNode;
    RHS->nxtNode = node->nxtNode;

    // ��ȡ������
    std::string varName = *((std::string*)(LHS->nodeVal));

    // �Ȼ�����ֵ
    RHS = Compute(RHS);

    // ����ͨ�����޸ı�����
    // string varName = *((std::string)(LHS->nodeVal))
    VariableTable::SetVarNode(varName, RHS);

    // ��Ҫ�����⹹ԭ�ڵ�
    Destroy(node);

    return mergeLR(RHS);
}

// ��ʱ��ֵ
ASTnode *SetDelayed(ASTnode *node)
{
    if (!node) {return NULL;}
    if (node->nodeInfo->sonCnt != 2) {
        printf("err: Wrong sonCnt <%d> in Set, must be 2.\n", node->nodeInfo->sonCnt);
        return node;
    }

    ASTnode *LHS = node->sonHead->nxtNode;
    ASTnode *RHS = LHS->nxtNode;

    // ��ֵ�����ǵ�һ���������ڵ�
    if (LHS->nodeInfo->nodeType != NODETYPE_SYMBOL_VARNAME) {
        printf("err: Wrong type <%d> in Set[[1]], must be NODETYPE_SYMBOL_VARNAME,\n", LHS->nodeInfo->nodeType);
        return node;
    }

    // ������ɣ���RHS�����������һ��׼��ȡ�� node
    RHS->preNode->nxtNode = RHS->nxtNode;
    RHS->nxtNode->preNode = RHS->preNode;
    node->nodeInfo->sonCnt--;
    RHS->preNode = node->preNode;
    RHS->nxtNode = node->nxtNode;

    // ��ȡ������
    std::string varName = *((std::string*)(LHS->nodeVal));

    // SetDelayed��������ֵ
    // RHS = Compute(RHS);

    // ����ͨ�����޸ı�����
    // string varName = *((std::string)(LHS->nodeVal))
    VariableTable::SetVarNode(varName, RHS);

    // ��Ҫ�����⹹ԭ�ڵ�
    Destroy(node);

    return mergeLR(RHS);
}


/*=================================================================
  �����ڵ�����ĺ��������û���ʹ�õĺ���һһ��Ӧ
  ����ֵΪ�����ɵĽڵ��void
  Ӧ�ý��ɵĽڵ�⹹�������½ڵ�����������ɽڵ�����������
  !!!ǧ��Ҫע��!!!
  Ҫ�Ѿͽڵ�� preNode, nxtNode ��Ϣ���Ƶ��½ڵ���!!!
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

    // Ҫ��û�в�����������ýڵ㣬����������0��ע��Ҫ�����ҽڵ���Ϣ����
    if (node->nodeInfo->sonCnt == 0) {
        ASTnode *ret = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", node->preNode, node->nxtNode);
        Integer tmp0 = Integer(0);
        SetNodeVal(ret, VALTYPE_INTEGER, &tmp0);
        Destroy(node);
        return mergeLR(ret);
    }

    // Ҫ��ֻ��һ�����������������ӹ�������������ӷ��ڵ�
    if (node->nodeInfo->sonCnt == 1) {
        ASTnode *ret = Duplicate(node->sonHead->nxtNode, node->preNode, node->nxtNode);
        Destroy(node);
        return mergeLR(ret);
    }

    // Ҫ���ж���������Ƚ���ɣ���ͬΪPlus�Ķ���չ��������
    for (ASTnode *p = node->sonHead->nxtNode; p!=node->sonTail; p=p->nxtNode) {
        if (p->nodeInfo->nodeType == node->nodeInfo->nodeType &&
            p->nodeInfo->headName == node->nodeInfo->headName
        ) {
            p = UpmountGrandSons(node, p);
        }
    }


    // �ϲ�ͬΪ��������
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
    

    // Ҫ����󻯼���ֻʣһ���ô�Ͱ��Ǹ�������������⹹ԭ�ڵ㲢����������Ķ��ӽڵ�
    // Ҫ��ʣ�����˵�������ˣ�����һ����λԪ 0 ���ء�
    // ע��Ҫ����ס pre/nxtNode ����Ϣ
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

    // Ҫ��û�в�����������ýڵ㣬����������0��ע��Ҫ�����ҽڵ���Ϣ����
    if (node->nodeInfo->sonCnt == 0) {
        ASTnode *ret = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", node->preNode, node->nxtNode);
        Integer tmp0 = Integer(1);
        SetNodeVal(ret, VALTYPE_INTEGER, &tmp0);
        Destroy(node);
        return mergeLR(ret);
    }

    // Ҫ��ֻ��һ�����������������ӹ�������������ӷ��ڵ�
    if (node->nodeInfo->sonCnt == 1) {
        ASTnode *ret = Duplicate(node->sonHead->nxtNode, node->preNode, node->nxtNode);
        Destroy(node);
        return mergeLR(ret);
    }

    // Ҫ���ж���������Ƚ���ɣ���ͬΪPlus�Ķ���չ��������
    for (ASTnode *p = node->sonHead->nxtNode; p!=node->sonTail; p=p->nxtNode) {
        if (p->nodeInfo->nodeType == node->nodeInfo->nodeType &&
            p->nodeInfo->headName == node->nodeInfo->headName
        ) {
            p = UpmountGrandSons(node, p);
        }
    }


    // �ϲ�ͬΪ��������
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
    

    // Ҫ����󻯼���ֻʣһ���ô�Ͱ��Ǹ�������������⹹ԭ�ڵ㲢����������Ķ��ӽڵ�
    // Ҫ��ʣ�����˵�������ˣ�����һ����λԪ 1 ���ء�
    // ע��Ҫ����ס pre/nxtNode ����Ϣ
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


ASTnode *Quotient(ASTnode *node)
{
    if (!node) {return NULL;}
    node = ComputeSons(node);

    // ��飬ֻ�������� Integer
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

    // ��飬ֻ�������� Integer
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

        // ���������Ϣ���滻����
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
                else {  //˵�����������ظ�
                    printf("err: same var %s in Apply.Function.\n", str.c_str());
                    fail = 1;
                }
            }
            else {
                fail=1;
                break;
            }
        }

        // ����
        if (!fail) {
            ASTnode *tmpNode = Duplicate(s1->sonHead->nxtNode->nxtNode, node->preNode, node->nxtNode);
            Destroy(node);
            tmpNode = Compute(tmpNode);
            node = tmpNode;
        }

        // ��ԭ������Ϣ
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
        if (s1->nodeInfo->nodeType == NODETYPE_SYMBOL_VARNAME) {    // ֱ���滻
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

    // ���
    if (node->nodeInfo->sonCnt != 3) {
        return mergeLR(Compute(node));
    }

    ASTnode *tmp;

    // �ȼ����ж�
    node->sonHead->nxtNode = Compute(node->sonHead->nxtNode);

    // �����ǿ��жϵĲ����ͣ���ʱ��Ϊ���ֻ�True��False�����򲻼���
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


/*=================================================================
  ���غ���
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
            // ����Ӧ��ֱ���ж� head �ǲ��� "symbol" ���е�
            // ���Ǹо���Ӧ��ֱ�����ַ������Ժ��ǻ�Ū�����ݽṹ�� head ӳ�䵽������
            // �������� VALTYPE �ж�һ�°�
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