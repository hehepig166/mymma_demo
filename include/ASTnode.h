#ifndef HEHEPIG_MMA_ASTNODE
#define HEHEPIG_MMA_ASTNODE

#include <string>

/*
��4bit
    NUMBER: ���֣��������ͺ�ʵ��
    SYMBOL: ���ţ�����������������
    STRING: �ַ���
����λ������ϸ�֣���������ֻ��������Ԫ�أ�
*/
#define NODETYPEBITS                (0x0f)
#define NODETYPE_NUMBER             (0x01)
#define NODETYPE_SYMBOL             (0x02)
#define NODETYPE_STRING             (0x04)
#define NODETYPE_NUMBER_INTEGER     (0x11)
#define NODETYPE_NUMBER_RATIONAL    (0x21)
#define NODETYPE_NUMBER_REAL        (0x31)
#define NodeTYPE_NUMBER_COMPLEX     (0x41)
#define NODETYPE_SYMBOL_VARNAME     (0x12)
#define NODETYPE_SYMBOL_FUNCTION    (0x22)
#define NODETYPE_SYMBOL_TRUE        (0x32)
#define NODETYPE_SYMBOL_FALSE       (0x42)

#define VALTYPE_NULL        (0)
#define VALTYPE_INTEGER     (1)
#define VALTYPE_REAL        (2)
#define VALTYPE_CHARSTAR    (3)
#define VALTYPE_STRING      (4)
#define VALTYPE_ASTNODE     (5)




struct ASTnodeInfo {
    int nodeType;

    int valType;

    int sonCnt;
    
    std::string headName;
    int headID;
};

struct ASTnode {

    ASTnodeInfo *nodeInfo;

    // nodeVal ������Դ��κ����͵Ķ���
    // �洢���ͷ��ǵ���DestrpyNodeval��SetNodeVal���� valType ʵ�ֵ�
    // �ڲ������ new, delete�����Զ����ù�������
    void *nodeVal;

    ASTnode *sonHead;
    ASTnode *sonTail;

    ASTnode *preNode;
    ASTnode *nxtNode;

    //~ASTnode();
};



// ���øú����Ժ���Ӧ������SetNodeVal_move
ASTnode *CreateNode(int nodeType_, std::string headName_, const ASTnode *preNode_, const ASTnode *nxtNode_);

// ���� valType ���� delete ���пռ��ͷ�
void DestroyNodeVal(ASTnode *node);

// ���� DestroyNodeVal �ͷŵ�ԭ���Ŀռ䣬��������ռ䣬ֱ�ӽ�nodeVal_ָ�봫�� node��
void SetNodeVal_move(ASTnode *node, int valType_, void *val_);

// ���� DestroyNodeVal �ͷŵ�ԭ���Ŀռ䣬����������ռ䡣
// ע���¿ռ�����ݻ�ֱ���õ��ںŸ��ơ�
void SetNodeVal(ASTnode *node, int valType_, const void *val_);

// ֻ�ǰ�ָ�����ȥ�����һ��޸� *son_ �� preNode, nxtNode
// һ����˵ son_ Ӧ���Ѿ�������Ǹ������ĸ��ڵ��ˣ�
// ���� AddSon_move ���ǻ����� son_ �� preNode��nxtNode ����һ�� 
void AppendSon_move(ASTnode *node, ASTnode *son_);

// node �� preSon_ ����Ӷ��� son_
// ���ؼ���֮��ĵ�ַ��Ҳ���� son_��
// ��ָ��
// ��ȷ�� preSon_ �� node �Ķ���
ASTnode *InsertSon_move(ASTnode *node, ASTnode *son_, ASTnode *preSon_);

// ��� node ��һ������ son_�������⹹������ son_ 
// ��ȷ�� son_ �� node �Ķ���
// ���ã�
ASTnode *UnmountSon(ASTnode *node, ASTnode *son_, ASTnode *preNode_, ASTnode *nxtNode_);

// �⹹��ɾ�� node ��һ������ son_
// ע��ά���� son_ �� pre/nxt
// ��ȷ�� son_ �� node �Ķ���
// ���� son_->preNode
ASTnode *DeleteSon(ASTnode *node, ASTnode *son_);

// �±�� 1 ��ʼ
// ��û����һ��򷵻� NULL
ASTnode *GetSon(ASTnode *node, int index);

// �ݹ鸴��
ASTnode *Duplicate(const ASTnode *node, const ASTnode *preNode_, const ASTnode *nxtNode_);

// �ݹ�⹹
void Destroy(ASTnode *node);

#endif