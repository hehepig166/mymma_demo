#ifndef HEHEPIG_MMA_VARIABLE
#define HEHEPIG_MMA_VARIABLE

#include "ASTnode.h"
#include <string>
#include <map>

struct VariableTable {

    // �⹹ԭ�ȵ��﷨����Ҫ���еĻ�����Ȼ�� node ����һ�ݹ���ȥ
    static void SetVarNode(const std::string &varName_, const ASTnode *node);

    // ���ص�ַ��ע��Ҫ�õĻ��õ��� Duplicate���ƣ�����ֱ��ȥ�޸ģ�
    static ASTnode *FindVarNode(const std::string &varName_);

    // ɾ��һ������
    static void EraseVar(const std::string &varName_);

    // ɾ�����б���������������
    static void EraseAllVars();

    ~VariableTable() {EraseAllVars();}
};

#endif