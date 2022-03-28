#include "../include/ASTnode.h"
#include "../include/Integer.h"
#include "../include/Functions.h"
#include "../include/Variable.h"
#include <iostream>
#include <string>
#include <cstring>
using namespace std;

static int done;

void fun()
{
    Integer *int1 = new Integer(15);
    Integer *int2 = new Integer(-6);
    ASTnode *n1 = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", NULL, NULL);
    SetNodeVal_move(n1, VALTYPE_INTEGER, int1);
    ASTnode *n2 = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", NULL, NULL);
    SetNodeVal_move(n2, VALTYPE_INTEGER, int2);
    ASTnode *n3 = CreateNode(NODETYPE_SYMBOL_FALSE, "Symbol", NULL, NULL);
    ASTnode *n4 = CreateNode(NODETYPE_SYMBOL_FUNCTION, "PLUS", NULL, NULL);
    ASTnode *root = CreateNode(NODETYPE_SYMBOL_FUNCTION, "AtomQ", NULL, NULL);
    AppendSon_move(n4, n1);
    AppendSon_move(n4, n2);
    AppendSon_move(n4, n3);
    AppendSon_move(root, n4);
    Print(root);
    puts("");

    Destroy(root);
}

ASTnode *tmpParse()
{
    ASTnode *ret=NULL;

    static char ch=getchar();
    string curstr;
    while (ch==' ' || ch=='\n')
        ch=getchar();
    
    if (ch == ';') {
        ch = getchar();
        return NULL;
    }
    if (ch == ']') {
        return NULL;
    }

    if (ch == '-' || isdigit(ch)) {   // Number
        int curint=0;
        int sig=1;
        while (ch=='-') {
            sig*= -1;
            ch = getchar();
        }
        while (ch>='0' && ch<='9') {
            curint = curint*10+ch-'0';
            ch = getchar();
        }
        curint*=sig;
        ret = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", NULL, NULL);
        SetNodeVal(ret, VALTYPE_INTEGER, &curint);
    }
    else if (isalpha(ch)) { //Symbol
        while (isalnum(ch)) {
            curstr+=ch;
            ch = getchar();
        }
        if (ch == '[') {
            ret = CreateNode(NODETYPE_SYMBOL_FUNCTION, curstr.c_str(), NULL, NULL);
            ch=getchar();// eat '['
            ASTnode *tmpnode=NULL;
            while (1) {
                tmpnode = tmpParse();
                AppendSon_move(ret, tmpnode);
                while (ch == ' ' || ch == '\n') ch = getchar();
                
                if (ch == ',') {
                    ch=getchar();
                    continue;
                }
                else if (ch == ']') {
                    ch=getchar();
                    break;
                }
                else {
                    ch=getchar();
                    puts("err");
                    break;
                }
            }
        }
        else {
            if (curstr == "True") {
                ret = CreateNode(NODETYPE_SYMBOL_TRUE, "Symbol", NULL, NULL);
            }
            else if (curstr == "False") {
                ret = CreateNode(NODETYPE_SYMBOL_FALSE, "Symbol", NULL, NULL);
            }
            else {
                ret = CreateNode(NODETYPE_SYMBOL_VARNAME, "Symbol", NULL, NULL);
                SetNodeVal(ret, VALTYPE_STRING, &curstr);
            }
            if (curstr == "Exit") {   
                done = 1;
            }
        }
    }
    else {
        ch=getchar();
    }

    return ret;
    
}

void test_Create_Destroy()
{
    puts("Start testing (Create, Destroy). Press Enter.");
    getchar();
    ASTnode *tmp;
    Integer tmpint =  Integer(1);
    for (int i=1; i<=1000000; i++) {
        tmp = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", NULL, NULL);
        SetNodeVal(tmp, VALTYPE_INTEGER, &tmpint);
        Destroy(tmp);
    }
    puts("Done (Create, Destroy).");
}

void test_Duplicate()
{
    puts("Start testing (Duplicate). Press Enter.");
    getchar();
    ASTnode *tmp;
    Integer tmpint =  Integer(1);

    ASTnode *A = CreateNode(NODETYPE_NUMBER_INTEGER, "Integer", NULL, NULL);
    SetNodeVal(A, VALTYPE_INTEGER, &tmpint);

    for (int i=1; i<=100000; i++) {
        tmp = Duplicate(A, NULL, NULL);
        Destroy(tmp);
    }
    puts("Done (Duplicate).");
}

void test_Repeate_10K()
{
    puts("Start testing (Repeate_10K). Press Enter.");
    getchar();
    printf("> ");
    ASTnode *tmp;
    Destroy(Compute(Duplicate(tmp = tmpParse(), NULL, NULL)));
    printf("> ");
    tmp = tmpParse();
    for (int i=1; i<=10000; i++) {
        Destroy(Compute(Duplicate(tmp, NULL, NULL)));
        if (i%1000==0)
            putchar('*');
    }
    puts("");
    puts("Done (Repeate_10K).");
}

int main()
{
    //test_Create_Destroy();
    //test_Duplicate();
    //test_Repeate_10K();

    ASTnode *root;

    done=0;

    while (!done) {
        printf("\n> ");
        root = tmpParse();
        ASTnode *dup = Duplicate(root, NULL, NULL);
        puts("");

/*
        printf("In//FullForm = ");
        Print(root);
        puts("");
*/

        root = Compute(root);
        printf("Out = ");
        Print(root);
        puts("\n");


        Destroy(root);
        Destroy(dup);
    }

    VariableTable::EraseAllVars();
    return 0;
}