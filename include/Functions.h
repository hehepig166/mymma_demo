#ifndef HEHEPIG_MMA_FUNCTION
#define HEHEPIG_MMA_FUNCTION

#include "ASTnode.h"

void Print(const ASTnode *node);

ASTnode *Compute(ASTnode *node);

ASTnode *Set(ASTnode *node);

ASTnode *SetDelayed(ASTnode *node);

ASTnode *FullForm(ASTnode *node);



ASTnode *AtomQ(ASTnode *node);

ASTnode *Plus(ASTnode *node);

ASTnode *Times(ASTnode *node);

ASTnode *Quotient(ASTnode *node);

ASTnode *Mod(ASTnode *Mod);


ASTnode *List(ASTnode *node);

ASTnode *Function(ASTnode *node);

ASTnode *Apply(ASTnode *node);

ASTnode *If(ASTnode *node);

#endif