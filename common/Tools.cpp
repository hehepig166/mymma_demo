#include "../include/ASTnode.h"
#include "../include/Integer.h"
#include "../include/Functions.h"
#include "../include/Variable.h"
#include "../include/Tools.h"
#include <cstdio>
#include <string>
#include <map>

//const ASTnode NODE_TRUE = 


ASTnode *Tools_SortSons(ASTnode *node)
{
    if (!node) return NULL;
    if (node->nodeInfo->sonCnt <= 2) return node;

    std::vector<ASTnode*> vec;
    for (ASTnode *p=node->sonHead->nxtNode, *nxt; p!=node->sonTail; p=nxt) {
        nxt=p->nxtNode;
        vec.emplace_back(UnmountSon(node, p, NULL, NULL));
    }

    sort(vec.begin(), vec.end(), [](ASTnode *x, ASTnode *y){return *x < *y;});

    for (ASTnode*p: vec) {
        AppendSon_move(node, p);
    }

    return node;
}


ASTnode *Tools_SortSons(ASTnode *node, const ASTnode *cmpNode)
{
    if (!node) return NULL;
    if (node->nodeInfo->sonCnt <= 2) return node;

    std::vector<ASTnode*> vec;
    for (ASTnode *p=node->sonHead->nxtNode, *nxt; p!=node->sonTail; p=nxt) {
        nxt=p->nxtNode;
        vec.emplace_back(UnmountSon(node, p, NULL, NULL));
    }

    sort(vec.begin(), vec.end(), [cmpNode](ASTnode *x, ASTnode *y){
        ASTnode *tmp = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Apply", NULL, NULL);
        AppendSon_move(tmp, Duplicate(cmpNode, NULL, NULL));
        ASTnode *tmplist = CreateNode(NODETYPE_SYMBOL_FUNCTION, "List", NULL, NULL);
        AppendSon_move(tmplist, Duplicate(x, NULL, NULL));
        AppendSon_move(tmplist, Duplicate(y, NULL, NULL));
        AppendSon_move(tmp, tmplist);
        tmp = Compute(tmp);
        bool ret = (tmp->nodeInfo->nodeType == NODETYPE_SYMBOL_TRUE);
        Destroy(tmp);
        return ret;
    });

    for (ASTnode*p: vec) {
        AppendSon_move(node, p);
    }

    return node;
}