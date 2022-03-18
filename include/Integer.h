#ifndef HEHEPIG_MMA_INTEGER
#define HEHEPIG_MMA_INTEGER

#include <cstdio>

struct Integer {
    int val;

    Integer():val(0){}
    Integer(int x):val(x){}

    void print()
    {
        printf("%d",val);
    }

    void operator+=(const Integer &x) {val+=x.val;}
    void operator*=(const Integer &x) {val*=x.val;}
    bool operator==(const Integer &x) {return val==x.val;}
    void operator/=(const Integer &x) {val/=x.val;}
    void operator%=(const Integer &x) {val%=x.val;}
};



#endif