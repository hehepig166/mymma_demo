#ifndef HEHEPIG_MMA_VARIABLE
#define HEHEPIG_MMA_VARIABLE

#include "ASTnode.h"
#include <string>
#include <map>

struct VariableTable {

    std::map<std::string, ASTnode*> varMap;

    // �⹹ԭ�ȵ��﷨����Ҫ���еĻ�����Ȼ�� node ����һ�ݹ���ȥ
    void SetVarNode(const std::string &varName_, const ASTnode *node);

    // ���ص�ַ��ע��Ҫ�õĻ��õ��� Duplicate���ƣ�����ֱ��ȥ�޸ģ�
    ASTnode *FindVarNode(const std::string &varName_);

    // ɾ��һ������
    void EraseVar(const std::string &varName_);

    // ɾ�����б���������������
    void EraseAllVars();

    // �����Map�����ͷŽڵ�
    void Drop() {varMap.clear();}

    ~VariableTable() {EraseAllVars();}
};

#endif