#include "../include/ASTnode.h"
#include "../include/Integer.h"
#include "../include/Functions.h"
#include "../include/Variable.h"
#include <iostream>
#include <string>
#include <cstring>
#include <map>

extern VariableTable globalVariableTable;

static Integer ZERO_INTEGER = Integer(0);
static Integer ONE_INTEGER = Integer(1);
static Integer NEG_ONE_INTEGER = Integer(-1);


#define TOKEN_INTEGER       (1)
#define TOKEN_SYMBOL        (2)
#define TOKEN_OPERATOR      (3)
#define TOKEN_ENTER         (4)


ASTnode *LogError(const char *str)
{
    printf("%s\n",str);
    return NULL;
}



Integer val_integer;
std::string val_string;
int CurTok;

char nxtch;
void SkipSpace() {while (isspace(nxtch) && nxtch!='\n') nxtch=getchar();}
int GetNextToken()
{
    SkipSpace();

    // enter
    if (nxtch == '\n' || nxtch == ';') {
        nxtch = ' ';
        return CurTok = TOKEN_ENTER;
    }

    // number
    if (nxtch>='0' && nxtch<='9') {
        val_integer = Integer(0);
        while (nxtch>='0' && nxtch<='9') {
            val_integer *= Integer(10);
            val_integer += Integer(nxtch-'0');
            nxtch = getchar();
        }
        return CurTok = TOKEN_INTEGER;
    }

    // symbol
    if (isalpha(nxtch) || nxtch<0 || nxtch>128) {
        val_string = "";
        while (isalnum(nxtch) || nxtch<0 || nxtch>128) {
            val_string += nxtch;
            nxtch = getchar();
        }
        return CurTok = TOKEN_SYMBOL;
    }

    // operator
    val_string = "";
    val_string += nxtch;
    char curch = nxtch;
    nxtch = getchar();
    switch(curch) {
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
        case '^':
        case '*':
        case '/':
        case '+':
        case '-': {
            break;
        }
        case '<':
        case '>': {
            break;
        }
        case '=': {
            if (nxtch == '=') {
                val_string += nxtch;
                nxtch = getchar();
                break;
            }
            break;
        }
        case ':': {
            if (nxtch == '=') {
                val_string += nxtch;
                nxtch = getchar();
                break;
            }
            break;
        }
    }
    return CurTok = TOKEN_OPERATOR;
}

void SkipEnter() {while (CurTok == TOKEN_ENTER) GetNextToken();}





std::map<std::string, int> operatorPrec;
std::map<std::string, std::string> operator2Head;

ASTnode *ParseNumberExpr();
ASTnode *ParseSymbolExpr();
ASTnode *ParseOperatorExpr();
ASTnode *ParsePrimaryExpr();
ASTnode *ParseRHS(ASTnode *LHS, int prec);
ASTnode *ParseExpr();


ASTnode *ParseNumberExpr()
{
    // 1
    ASTnode *ret = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", NULL, NULL);
    SetNodeVal(ret, VALTYPE_INTEGER, &val_integer);
    GetNextToken();
    return ret;
}

ASTnode *ParseSymbolExpr()
{
    // a
    ASTnode *ret = CreateNode(NODETYPE_SYMBOL_VARNAME, "Symbol", NULL, NULL);
    SetNodeVal(ret, VALTYPE_STRING, &val_string);
    GetNextToken();
    return ret;
}

ASTnode *ParseOperatorExpr()
{
    // +.. | -..
    if (val_string == "+") {
        GetNextToken(); // eath "+"
        SkipEnter();
        ASTnode *ret = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Plus", NULL, NULL);
        ASTnode *nxt = ParsePrimaryExpr();
        AppendSon_move(ret, nxt);
        return ret;
    }
    else if (val_string == "-") {
        GetNextToken(); // eat "-"
        SkipEnter();
        ASTnode *ret = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Times", NULL, NULL);
        ASTnode *tmp = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", NULL, NULL);
        SetNodeVal(tmp, VALTYPE_INTEGER, &NEG_ONE_INTEGER);
        AppendSon_move(ret, tmp);
        SkipEnter();
        ASTnode *nxt = ParsePrimaryExpr();
        AppendSon_move(ret, nxt);
        return ret;
    }

    // (..)
    if (val_string == "(") {
        GetNextToken(); // eat "("
        SkipEnter();
        ASTnode *ret = ParseExpr();
        SkipEnter();
        if (!ret || CurTok != TOKEN_OPERATOR || val_string != ")") {
            Destroy(ret);
            return LogError("Err when parsing (...).");
        }
        GetNextToken(); // eat ")"
        return ret;
    }

    // {..}
    if (val_string == "{") {
        GetNextToken(); // eat "{"
        SkipEnter();
        ASTnode *ret = CreateNode(NODETYPE_SYMBOL_FUNCTION, "List", NULL, NULL);
        while (1) {
            if (CurTok==TOKEN_OPERATOR && val_string=="}") {
                GetNextToken(); // eat "}"
                break;
            }
            
            SkipEnter();
            ASTnode *tmp = ParseExpr();
            SkipEnter();
            AppendSon_move(ret, tmp);
            if (CurTok==TOKEN_OPERATOR && val_string==",") {
                GetNextToken(); // eat ","
                SkipEnter();
                continue;
            }
            else if (!(CurTok==TOKEN_OPERATOR && val_string=="}")) {
                Destroy(ret);
                LogError("Err when parsing {...}.");
                return NULL;
            }
        }
        return ret;
    }

    LogError("Unknown head operator in ParseOperatorExpr.");
    GetNextToken();
    return NULL;
}

ASTnode *ParsePrimaryExpr()
{
    switch(CurTok) {
        case TOKEN_SYMBOL:
            return ParseSymbolExpr();
        case TOKEN_INTEGER:
            return ParseNumberExpr();
        case TOKEN_OPERATOR:
            return ParseOperatorExpr();
        case TOKEN_ENTER:
            GetNextToken();
            return NULL;
        default:
            GetNextToken();
            return LogError("unknown token in ParsePrimaryExpr.");
    }

    return NULL;
}

std::string GetOp()
{
    if (CurTok == TOKEN_INTEGER || CurTok == TOKEN_SYMBOL || (CurTok==TOKEN_OPERATOR && (val_string=="{" || val_string=="("))) {
        return "*";
    }
    if (CurTok == TOKEN_OPERATOR) {
        return val_string;
    }
    return "";
}
int GetPrec(const std::string &str)
{
    if (str == "") return -1;
    auto p = operatorPrec.find(str);
    if (p!=operatorPrec.end())
        return p->second;
    return -1;
}

ASTnode *ParseRHS(ASTnode *LHS, int prec)
{
    if (!LHS) {
        return LogError("Err: NULL LHS when in ParseRHS");
    }
    // opr ...
    // [Expr, Expr, ...]
    // 1
    // a
    // {Expr, Expr, ...}
    // (Expr)

    // ENTER
    while (1) {
        // ENTER
        if (CurTok == TOKEN_ENTER) {
            //GetNextToken(); // eat Enter
            return LHS;
        }

        std::string curOp = GetOp();
        int curPrec = GetPrec(curOp);

        if (curPrec < prec) {
            return LHS;
        }
        
        // [Expr, Expr, ...]
        // opr ...
        // 1
        // a
        // {Expr, Expr, ...}
        // (Expr)
        if (CurTok == TOKEN_OPERATOR && (val_string!="{" && val_string!="(")) {
            GetNextToken(); // eat op
            SkipEnter();
        }       

        if (curOp == "[") {
            ASTnode *ret;
            if (LHS->nodeInfo->nodeType == NODETYPE_SYMBOL_VARNAME) {
                // 若是一层调用，则把直接建一个 SymbolFunction 节点，Head 为LHS的串
                ret = CreateNode(NODETYPE_SYMBOL_FUNCTION, *((std::string*)LHS->nodeVal), NULL, NULL);
                Destroy(LHS);
                LHS = ret;
            }
            else {
                // 若是多层调用，则自动调用 Apply
                // 其实不应该这样的，但一开始写的时候，把 nodeInfo->Head 定死为 string 了……
                // 要补救的话可以把 Head 改成 ASTnode*，就可以通用了，不过现在还有好多作业，先不改了，将就一下吧。
                ret = CreateNode(NODETYPE_SYMBOL_FUNCTION, "Apply", NULL, NULL);
                AppendSon_move(ret, LHS);
                LHS = ret;
                ret = CreateNode(NODETYPE_SYMBOL_FUNCTION, "List", NULL, NULL);
                AppendSon_move(LHS, ret);
            }
            while (1) {
                if (CurTok==TOKEN_OPERATOR && val_string=="]") {
                    GetNextToken(); // eat "]"
                    break;
                }
                else {
                    SkipEnter();
                    ASTnode *tmp = ParseExpr();
                    SkipEnter();
                    AppendSon_move(ret, tmp);
                    if (CurTok==TOKEN_OPERATOR && val_string==",") {
                        GetNextToken(); // eat ","
                        SkipEnter();
                        continue;
                    }
                    else if (!(CurTok==TOKEN_OPERATOR && val_string=="]")){
                        Destroy(ret);
                        LogError("Err when parsing [...].");
                        return NULL;
                    }
                }
            }

        }
        else {
            ASTnode *RHS = ParsePrimaryExpr();
            if (!RHS) {
                return LHS;
            }

            while (1) {
                // 处理类似 LHS + RHS * xxx 的情况，RHS要继续
                // 左结合，判断 prec < nxtPrec
                // 右结合，判断 prec <= nxtPrec （目前只有 "^" 与 赋值 是右结合）
                std::string nxtOp = GetOp();
                int nxtPrec = GetPrec(nxtOp);
                if (curPrec < nxtPrec ||
                    (curPrec==nxtPrec &&
                        (curOp=="^" || curOp=="=" || curOp==":=")
                    )
                ) {
                    RHS = ParseRHS(RHS, nxtPrec);
                    if (!RHS) {
                        return LogError("Err: no RHS when parsing op.");
                    }
                }
                else {
                    break;
                }
            }

            ASTnode *tmp = CreateNode(NODETYPE_SYMBOL_FUNCTION, operator2Head[curOp], NULL, NULL);
            AppendSon_move(tmp, LHS);
            AppendSon_move(tmp, RHS);
            LHS = tmp;
        }
    }
    return LHS;
}

ASTnode *ParseExpr()
{
    ASTnode *LHS = ParsePrimaryExpr();
    if (!LHS)
        return NULL;
    
    return ParseRHS(LHS, 0);
}



void Init()
{
    operatorPrec["["]=100;
    operatorPrec["^"]=50;
    operatorPrec["*"]=operatorPrec["/"]=40;
    operatorPrec["+"]=operatorPrec["-"]=30;
    operatorPrec[">"]=operatorPrec["<"]=operatorPrec["=="]=20;
    operatorPrec["="]=operatorPrec[":="]=10;

    operator2Head["^"]="Power";
    operator2Head["*"]="Times";
    operator2Head["/"]="Divide";
    operator2Head["+"]="Plus";
    operator2Head["-"]="Subtract";
    operator2Head[">"]="Greater";
    operator2Head["<"]="Less";
    operator2Head["=="]="Equal";
    operator2Head["="]="Set";
    operator2Head[":="]="SetDelayed";

    nxtch = ' ';
    CurTok = TOKEN_ENTER;
}

void test1();

int main()
{
    Init();
    ASTnode *doneNode = CreateNode(NODETYPE_SYMBOL_VARNAME, "Symbol", NULL, NULL);
    std::string doneStr = "Exit";
    SetNodeVal(doneNode, VALTYPE_STRING, &doneStr);
    while (1) {
        printf("\n> ");
        SkipEnter();
        ASTnode *root = ParseExpr();
        if (root) {
            root = Compute(root);
            printf("\nOut = ");
            Print(root);
            printf("\n\n");
            if (*root == *doneNode) {
                Destroy(root);
                break;
            }
            Destroy(root);
        }
    }
    Destroy(doneNode);
    globalVariableTable.EraseAllVars();
    return 0;
}


void test1()
{
    while (1) {
        int ttt=GetNextToken();
        printf("[%d]\t",ttt);
        if (ttt == TOKEN_INTEGER) {
            printf("%d\n",val_integer.val);
        }
        else if (ttt == TOKEN_ENTER) {
            printf("$\n");
        }
        else {
            printf("%s\n",val_string.c_str());
        }
    }
}