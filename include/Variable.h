#ifndef HEHEPIG_MMA_VARIABLE
#define HEHEPIG_MMA_VARIABLE

#include "ASTnode.h"
#include <string>
#include <map>

struct VariableTable {

    std::map<std::string, ASTnode*> varMap;

    // 解构原先的语法树（要是有的话），然后按 node 复制一份挂上去
    void SetVarNode(const std::string &varName_, const ASTnode *node);

    // 返回地址，注意要用的话得调用 Duplicate复制，不能直接去修改！
    ASTnode *FindVarNode(const std::string &varName_);

    // 删除一个变量
    void EraseVar(const std::string &varName_);

    // 删除所有变量，包括常量。
    void EraseAllVars();

    // 仅清空Map，不释放节点
    void Drop() {varMap.clear();}

    ~VariableTable() {EraseAllVars();}
};

#endif