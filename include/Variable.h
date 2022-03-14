#ifndef HEHEPIG_MMA_VARIABLE
#define HEHEPIG_MMA_VARIABLE

#include "ASTnode.h"
#include <string>
#include <map>

struct VariableTable {

    // 解构原先的语法树（要是有的话），然后按 node 复制一份挂上去
    static void SetVarNode(const std::string &varName_, const ASTnode *node);

    static ASTnode *FindVarNode(const std::string &varName_);

    // 删除一个变量
    static void EraseVar(const std::string &varName_);

    // 删除所有变量，包括常量。
    static void EraseAllVars();

    ~VariableTable() {EraseAllVars();}
};

#endif