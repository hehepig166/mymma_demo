#include "../include/ASTnode.h"
#include "../include/Integer.h"
#include <cstdio>


ASTnode *CreateNode(int nodeType_, std::string headName_, const ASTnode *preNode_, const ASTnode *nxtNode_)
{
    ASTnode *ret = new ASTnode;
    ret->nodeInfo = new ASTnodeInfo;
    ret->nodeVal = NULL;
    ret->sonHead = new ASTnode;
    ret->sonTail = new ASTnode;
    ret->sonHead->preNode = NULL;   // !!��Ҫ 20220318
    ret->sonHead->nxtNode = ret->sonTail;
    ret->sonTail->preNode = NULL;   // !!��Ҫ 20220318
    ret->sonTail->preNode = ret->sonHead;
    ret->preNode = (ASTnode*)preNode_;
    ret->nxtNode = (ASTnode*)nxtNode_;

    ret->nodeInfo->nodeType = nodeType_;
    ret->nodeInfo->valType = VALTYPE_NULL;
    ret->nodeInfo->sonCnt = 0;
    ret->nodeInfo->headName = headName_;
    ret->nodeInfo->headID = 0;    //����

    //printf("Create Node %d %s %d %d\n",ret, headName_.c_str(), ret->preNode, ret->nxtNode);
    return ret;
}

void DestroyNodeVal(ASTnode *node)
{
    //puts("DestroyNodeVal");
    if (!node) {return;}
    int vt = node->nodeInfo->valType;
    node->nodeInfo->valType = VALTYPE_NULL;
    if (!node->nodeVal) {
        return;
    }

    switch(vt) {
        case VALTYPE_NULL:
            break;
        case VALTYPE_INTEGER:
            delete (Integer*)(node->nodeVal);
            break;
        case VALTYPE_CHARSTAR:
            delete[] (char*)(node->nodeVal);
            break;
        case VALTYPE_STRING:
            delete (std::string*)(node->nodeVal);
            break;
        default:
            printf("Err: Unknown valtype <%s> in DestroyNodeVal\n", node->nodeInfo->headName.c_str());
            printf("This will lead to memory leaking.\n");
            break;
    }
}


void SetNodeVal_move(ASTnode *node, int valType_, void *val_)
{
    if (!node) {return;}
    //puts("SetNodeVal_move");
    DestroyNodeVal(node);
    node->nodeInfo->valType = valType_;
    node->nodeVal = val_;
}


void SetNodeVal(ASTnode *node, int valType_, const void *val_)
{
    if (!node) {return;}
    //puts("SetNodeVal");
    DestroyNodeVal(node);
    node->nodeInfo->valType = valType_;

    switch(valType_) {
        case VALTYPE_NULL:
            break;
        case VALTYPE_INTEGER:
            node->nodeVal = new Integer;
            *((Integer*)node->nodeVal) = *((Integer*)val_);
            break;
        case VALTYPE_CHARSTAR:
            node->nodeVal = new (char*);
            *((char*)node->nodeVal) = *((char*)val_);
            break;
        case VALTYPE_STRING:
            node->nodeVal = new std::string;
            *((std::string*)node->nodeVal) = *((std::string*)val_);
            break;
        case VALTYPE_ASTNODE: {
            ASTnode *tmp = Duplicate((ASTnode *)val_, NULL, NULL);
            node->nodeVal = tmp;
            break;
        }
        default:
            printf("Err: Unknown valtype <%d> in SetNodeVal\n", valType_);
            break;
    }
}


void AppendSon_move(ASTnode *node, ASTnode *son_)
{
    if (!node) {return;}
    if (!son_) {return;}

    node->nodeInfo->sonCnt++;

    if (son_->nxtNode) {
        son_->nxtNode->preNode = son_->preNode;
    }
    if (son_->preNode) {
        son_->preNode->nxtNode = son_->nxtNode;
    }

    son_->preNode = node->sonTail->preNode;
    son_->nxtNode = node->sonTail;

    node->sonTail->preNode->nxtNode = son_;
    node->sonTail->preNode = son_;
}

ASTnode *InsertSon_move(ASTnode *node, ASTnode *son_, ASTnode *preSon_)
{
    if (!node) {return NULL;}
    if (!son_) {return NULL;}
    if (!preSon_) {return son_;}
    if (!preSon_->nxtNode) {printf("err in InsertSon_move, no preSon_->nxtNode.\n"); return son_;}

    node->nodeInfo->sonCnt++;

    if (son_->nxtNode) {
        son_->nxtNode->preNode = son_->preNode;
    }
    if (son_->preNode) {
        son_->preNode->nxtNode = son_->nxtNode;
    }

    son_->preNode = preSon_;
    son_->nxtNode = preSon_->nxtNode;

    preSon_->nxtNode->preNode = son_;
    preSon_->nxtNode = son_;

    return son_;
}


ASTnode *UnmountSon(ASTnode *node, ASTnode *son_, ASTnode *preNode_, ASTnode *nxtNode_)
{
    if (!node || !son_) {return NULL;}
    node->nodeInfo->sonCnt--;
    son_->preNode->nxtNode = son_->nxtNode;
    son_->nxtNode->preNode = son_->preNode;
    son_->preNode = preNode_;
    son_->nxtNode = nxtNode_;
    return son_;
}

ASTnode *DeleteSon(ASTnode *node, ASTnode *son_)
{
    ASTnode *ret = son_->preNode;
    UnmountSon(node, son_, NULL, NULL);
    Destroy(son_);
    return ret;
}


ASTnode* GetSon(ASTnode *node, int index)
{
    if (!node) {return NULL;}
    ASTnode *ret=node->sonHead->nxtNode;
    for (int i=1; i<index && ret!=node->sonTail; i++)
        ret = ret->nxtNode;
    return ret;
}




ASTnode *Duplicate(const ASTnode *node, const ASTnode *preNode_, const ASTnode *nxtNode_)
{
    if (!node) {return NULL;}

    ASTnode *ret = CreateNode(node->nodeInfo->nodeType, node->nodeInfo->headName, preNode_, nxtNode_);
    SetNodeVal(ret, node->nodeInfo->valType, node->nodeVal);
    ret->preNode = (ASTnode*)preNode_;
    ret->nxtNode = (ASTnode*)nxtNode_;

    for (const ASTnode *ps=node->sonHead->nxtNode; ps!=node->sonTail; ps=ps->nxtNode) {
        AppendSon_move(ret, Duplicate(ps, NULL, NULL));
    }

    return ret;
}


void Destroy(ASTnode *node)
{
    if (!node) {return;}
    //printf("Destroy node %d\n", node);
    DestroyNodeVal(node);
    delete node->nodeInfo;

    ASTnode *curSon = node->sonHead->nxtNode;
    ASTnode *nxtSon=NULL;
    while (curSon != node->sonTail) {
        nxtSon = curSon->nxtNode;
        Destroy(curSon);
        curSon = nxtSon;
    }

    delete node->sonHead;
    delete node->sonTail;
    // ��Ҫ�����������˰��Լ��ͷŵ� -20220303
    delete node;
}