#include "../include/ASTnode.h"
#include "../include/Integer.h"
#include "../include/Functions.h"
#include "../include/Variable.h"
#include "../include/Tools.h"
#include <cstdio>
#include <string>
#include <map>

ASTnode *Tools_SortSons(ASTnode *node)
{
    if (!node) return NULL;
    if (node->nodeInfo->sonCnt <= 1) return node;

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