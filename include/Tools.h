#ifndef HEHEPIG_MMA_TOOLS
#define HEHEPIG_MMA_TOOLS

#include "../include/ASTnode.h"
#include "../include/Integer.h"
#include "../include/Functions.h"
#include "../include/Variable.h"
#include "../include/Tools.h"
#include <cstdio>
#include <string>
#include <map>
#include <algorithm>
#include <vector>


ASTnode *Tools_SortSons(ASTnode *node);

ASTnode *Tools_SortSons(ASTnode *node, const ASTnode *cmpNode);

ASTnode *Tools_Replace(ASTnode *node, const VariableTable &rules);




#endif