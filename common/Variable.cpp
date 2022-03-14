#include "../include/ASTnode.h"
#include "../include/Variable.h"
#include <string>
#include <map>

static std::map<std::string, ASTnode*> varMap;

    void VariableTable::SetVarNode(const std::string &varName_, const ASTnode *node)
    {

        // 处理 a=a 的情况，这时候不用做任何操作直接return
        if (
            node->nodeInfo->nodeType == NODETYPE_SYMBOL_VARNAME &&
            node->nodeVal &&
            *(std::string*)(node->nodeVal) == varName_
        ) {
            return;
        }

        std::map<std::string, ASTnode*>::iterator iter;
        iter = varMap.find(varName_);
        if (iter != varMap.end()) {
            Destroy(iter->second);
        }
        varMap[varName_]=Duplicate(node, NULL, NULL);
    }

    ASTnode *VariableTable::FindVarNode(const std::string &varName_)
    {
        std::map<std::string, ASTnode*>::iterator iter;
        iter = varMap.find(varName_);
        if (iter == varMap.end())
            return NULL;
        return iter->second;
    }

    void VariableTable::EraseVar(const std::string &varName_)
    {
        Destroy(FindVarNode(varName_));
        varMap.erase(varName_);
    }

    void VariableTable::EraseAllVars()
    {
        std::map<std::string, ASTnode*>::iterator iter;
        for (iter=varMap.begin(); iter!=varMap.end(); iter++) {
            Destroy(iter->second);
        }
        varMap.clear();
    }


