# My Mathematica

# 1. ����

## 1.1 ���ż���

* һ�нԿɿ������š�

* �ɷ�����ɵı��ʽ����Ȼ��һ�����š�

* �����νṹ����ʾ���š���`FullForm` �� `TreeForm` ���鿴�ṹ��

    �� mathematica �п���ʹ�� FullForm �� TreeForm �������鿴���ʽ�ṹ
    ```
    In := FullForm[a+b/(c+1/d)]
    Out//FullForm = Plus[a, Times[b, Power[Plus[c, Power[d, -1]], -1]]]
    ```

* ����Ԫ�أ�Number(Integer, Rational, Real, Complex), String, Symbol, 

    ����� Symbol ָ���ʽ֮���

    �� mathematica �У�����ʹ�� Head ���鿴Ԫ�ػ���ʽ��ͷ
    ```
    In := {Head[1], Head[1.5], Head["123"], Head[a], Head[True], Head[a+b]}
    Out = {Integer, Real, String, Symbol, Symbol, Plus}
    ```

* �����ķ��š�

    ����Ҳ��һ�����ţ�����ͷ�����������������ֵ��ͷ����������ô��Ϊ�ɣ�����Ժ��������õ��뷨�ٸģ�

    ���Կ���������ֵ�� Null �ĺ������䷵��ֵ Null Ҳ�Ǹ����š�
    ```
    In := {# // Head, # // FullForm} &@Do[Print[i], {i, 1, 2}]
    1
    2
    Out = {Symbol, Null}

    In := {# // Head, # // FullForm} &@Do[Print[i], {i, 1, 2}] //FullForm
    1
    2
    Out//FullForm= List[Symbol, FullForm[Null]]

    ```


## 1.2 ����ʽ���

* �ú����������ﵽ��̵�Ŀ��

    �� mathematica �У��������в������Ǻ���������������ļӼ��˳������㡣
    �������� `FullForm` �� `TreeForm` ���鿴���ʽ�ĺ�����﷽ʽ��
    ```
    In := FullForm[a+b/(c+1/d)]
    Out//FullForm = Plus[a, Times[b, Power[Plus[c, Power[d, -1]], -1]]]
    ```

    �� mathematica �У��ƺ�����Ҳ��ͨ��ʹ�ú��������ڻ���Ԫ����ʵ�ֵġ�
    ��������Ӧ�����������е����ͣ������� NumberQ �����顣
    ```
    In := Head[Sqrt[-1]]
    Out = Complex

    In := FullForm[Sqrt[-1]]
    Out//FullForm = Complex[0, 1]

    In := FullForm[Sqrt[-3]]
    Out//FullForm = Times[Complex[0, 1], Power[3, Rational[1, 2]]]

    In := {NumberQ[Sqrt[-1]], NumberQ[Sqrt[-3]]}
    Out = {True, False}

    ```

* ��֧��ѭ��

    �� mathematica �У�����෽ʽ��ʵ�ַ�֧��ѭ�������Ҷ��Ǻ���ʽ�ġ�
    ```
    In := If[2>1, a, b]
    Out = a

    In := Do[Print[i, " is a prime number"], {i, {2, 3, 5, 7, 11}}]
    2 is a prime number
    3 is a prime number
    5 is a prime number
    7 is a prime number
    11 is a prime number
    
    In := Apply[Plus, Table[n, {n, 1, 100, 2}]]
    Out = 2500
    ```

* ��������

    �� mathematica �У��������������������ﵽ�Զ��庯����Ŀ�ġ�
    �����������ʹ�� `#`��`&` ��� `@`������ֱ���ú��� `Function`��
    ```
    In := #^2 & @ (x+1)
    Out = (1+x)^2

    In := #1+#2 & [a,b]
    Out = a+b

    In := Function[a, a^2][x+1]
    Out = (x+1)^2

    In := Function[{a,b}, a+b][x+y, z^2]
    Out = x+y+z^2
    ```

* ����

    mathematica ��Ȼ֧�ֶԶ���Ĳ�����
    ��ʹ�õ��������������ԣ����ߴӸй�����˵����ֻ��һ�����ͣ����š�
    ��ֵ�����ָ�ֵ��ʽ��������ֵ `=` ����ʱ��ֵ `:=`��

* ��������

    �ڽ����Ŷ����ô�������������Camel������ `EvenQ`, `FactorInteger`, `E`, `Pi`, `Plus` �ȡ�
    Ϊ�˲�����������ͻ��ϣ���û��Զ�����Ų���С������������camel������ `a`, `b`, `c`, `myNumber`, `firstWordInTheSentence` �ȡ�

## 1.3 ����������
* ����һ�л�һ�δ��룬�������㲢������������
* �û��Զ��������Ϊȫ�ֱ�������ÿ�������й����˳�����������ʧ��


# 2. ������

* ˵��
    * Flat������ú���֧����������������
    * Orderless������ú������н����ԣ����⽻������˳�򣬽�����䡣
    * Only[x,y]������ú���ֻ֧���ض�������������������Ϊ֧�ֵ������б�
    * ����ʾ���������·��д���ʾ�������ǣ���ʵ�ֻ�ƻ�ʵ���˵ģ��ú����ĸ����÷���

* AtomQ
    * Only[1]
    * �����Ƿ��ǻ���Ԫ��

* FullForm

    * Only[1]
    * ��ӡ���ʽ������ʽ
    * �����ڸ���

* Head

    * Only[1]
    * ��ӡ���ʽ��ͷ

* Set

    * Only[2]
    * ������ֵ
    ```
    Set[a, expr]
    a = expr
    ```

* SetDelayed

    * Only[2]
    * ��ʱ��ֵ
    ```
    SetDelayed[a, expr]
    a := expr
    ```

* Plus

    * Flat
    * Orderless
    * �ӷ������������֮�͡�
    * ��Ч�����ֻ�����������ϲ���ʱ��
    ```
    Plus[]
        <=> 1
    Plus[a]
        <=> a
    Plus[a,b]
    Plus[a,b,c,d]
    a+b
    ```

* Times

    * Flat
    * Orderless
    * �˷������������֮��
    * ��Ч�����ֻ�����������ϲ���ʱ��
    ```
    Times[]
        <=> 1
    Times[a]
        <=> a
    Times[a, b]
    Times[a, b, c, d]
    a*b
    ```

* Power

    * Only[0, 1, 2]
    * �����㡣`Power[a, b]` ���� `a^b`
    * ʵ�������õ�ֻ�����ε����
    * ע�⣬��ʵ�� mathematica �� `Power` ����֧�ֶ�Σ�����������Կ��ٷ��ĵ��������ҵ�ʵ������ֻ֧�����������µĲ�����
    * ������������Ϊ
    ```
    Power[]
        <=> 1
    Power[a]
        <=> a
    Power[a,b]
    a^b
    ```

* Subtract

    * Only[2]
    * ������`Power[a, b]` ���� `a-b`
    * ��Ҫע����ǣ����յ��û������ `Subtract` �������ҵ�ʵ���лὫ��ת��Ϊ `Times` �� `Plus` ����ϡ������Ҳµģ�������׼������ʵ�֣����ڲ������У������� `Subtract` �������������� `mathematica` ���� `FullForm` �鿴��
    ```
    Subtract[a, b]
        <=> Plus[a, Times[-1, b]]
    a-b
    ```


* Divide

    * Only[2]
    * ������`Divide[a, b]` ���� `a/b`
    ```
    Divide[a,b]
    a/b
    ```

* Rational

    * Only[2]
    * ����ֻ���� Integer ���͡�
    * ����������һ������Ϊ���ӣ��ڶ�������Ϊ��ĸ��
    * �� mathematica �У����� `Rational[1,1.5]*1.5` ��`Rational[a,b]*b`��`Rational[1,Sqrt[3]]*Sqrt[3]` �ᷢ��������ԭ�������ֻ���������� `Rational[4,5]*5` ��������ȫ������ʱ�Ż�������㡣�Ҳ�ǰ���ļ�������Ϊ����������������ƥ�䣬���԰�����δ֪����������
    * ����Ҫ�Է��Ž��г������㣬������ Divide��������`Times`��`Power`��ϣ���Ҳ�ǳ����������ɵ���ʽ�������� mathematica �г��� `Divide @@ Rational[a, b]*b`���� Rational ��ͷ���� Divide���ͻ���ֻ����Ĵ� `a` �ˡ�
    * Ϊ�˲������û����뾡����Ҫ�øú�������ʹ��`Divide`��
    ```
    Rational[2,3]
    ```

* Complex

    * Only[2]
    * ����ֻ���� Integer �� Real ����

* Function
    * �����Զ��庯��
    * `Function[List[xxx], expr]`������ú����Ĳ�����Ϊ `List`�����ʽΪ `expr`��
    * Ҫ��������ھ�Ϊ���ɵ����ı��������� Function ֻ�����������ӡ��������㣬�����á�
    * mathematica ����ƺ�������̫���ˣ�Ū����������Ū����С��İɡ�
    * ����������ȫ�ֱ����г�ͻ����ȫ�ֱ����� expr �в��ɼ���
    ```
    Function[List[x,y], Plus[x,Times[y, y, y]]]
    ```

* Apply
    * ���ڸı�ͷ��
    * `Apply[fun, expr]`�������� fun ���� List �﷨����ͷ��
    * `Apply[PureFunction, List]`�������� List ��Ϊ PureFunction �Ĳ��������������ֵ��
    * mathematica �����Ҳ�кöණ����Ҳ��Ū����С��ġ�
    * 20220331: ��ʶ����Ӧ�ò�̫�ԡ��������ĵ��������rules��������ReplaceAll������ʵ��
    ```
    Apply[Plus, f[a, b]]
        => Plus[a, b]
    
    Apply[Function[List[x,y], Plus[x, Times[y,y,y]]], Plus[a,b]]
        => Plus[a, Times[b,b,b]]
    ```

* If
    * �ж�
    * `If[expr0, expr1, expr2]`�������ж� expr0 ��ֵ����ΪTrue���򷵻� expr1 ��ֵ����ΪFalse���򷵻� expr2 ��ֵ������������ԭ����
    * Only[3]
    * ��ʼ�����ˣ�ֻ������������С��� mathematica �ˣ��ܶ������﷨�Ȳ�֧���ˡ�
    * mathematica �������ֲ��ܵ�����ֵ��ֻ�� True ���� False ����Ч�ġ�
    ```
    If[True, a, b]
        => a
    If[False, a, b]
        => b
    If[Equal[1, 1], a, b]
        => a
    If[Equal[1, 0], a, b]
        => b
    ```

* Quotient
    * ��
    * `Quotient[m, n]`������ the integer quotient of m and n.
    ```
    Quotient[5,2]
    ``` 

* Mod
    * ȡģ
    * `Mod[m, n]`
    ```
    Mod[8, 3]
    ```

* Equal
    * �ж��Ƿ����
    * `Equal[expr1, expr2]`
    ```
    Equal[1, 1]
        => True

    Equal[1, 0]
        => False

    Equal[a, b]
        => Equal[a, b]
    
    Equal[a, 1]
        => Equal[a, 1]
    
    Set[a, 1]; Equal[a, 1]
        => True

    Equal[Plus[a, a], Times[2, a]]
        => True

    Equal[1, 1, 1, 1]
        => True

    Equal[1, 1, 0, 1]
        => False

    Equal[1, 1, 1, a]
        => Equal[1, 1, 1, a]
    
    Set[a, 1]; Equal[1, 1, 1, a]
        => True
    ```

* Greater/Less
    * ����/С��
    * `Greater[expr1, expr2, ...]`
    * `Less[expr1, expr2, ...]`
    * ���� True��False������ԭ���������ɱȣ�
    * �����������Ϊ True
    ```
    Less[1,2]
    Greater[5,4,3,2,1]
    Greater[5,4,3,5,1]
    ```

* Flatten
    * չ��
    * `Flatten[List]`����List��ȫչ��
    * ˵������ֻ�� mathematica Flatten ��һ����
    ```
    Flatten[List[List[1], b, List[2,3,4], 6]]
        => List[1, b, 2, 3, 4, 6]
    
    Flatten[List[List[]]]
        => List[]
    ```

* Sort
    * ����
    * `Sort[List]`������List�е�����
    * `Sort[List, func]`��sorts using the ordering function p. 
    * ע�⣬��ʵ��һ����List���κ�ͷ�����ԡ�
    ```
    Sort[List[1, 5, 2, 4]]
    Sort[List[a, b, fun[d, e], fun[a, b], x, 1, 2]]
    Sort[List[fun[2, 4], Times[a, d, b, 7], fun[1, 2], 7, a, 1, 2, b, a]]

    Sort[List[1, 5, 2, 4], Greater]
        => List[5, 4, 2, 1]
    
    Sort[a[1, 5, 3]]
        => a[1, 3, 5]
    
    Sort[{4, 3, 1, 7, 9}, # > 3 &]
        => {4, 7, 9, 1, 3}
    ```

* Rule
    * ����һ���滻���򣬵�������ʱ�ƺ�û�����壬��Ҫ�������������ʹ�á�
    * `Rule[LHS, RHS]`������ LHS �滻Ϊ RHS
    * ���߶���������һ��
    * `LHS -> RHS`
    ```
    Rule[a, b]
        => Rule[a, b]
    a=10; b=15
    Rule[a, b]
        => Rule[10, 15]
    Rule[a+b, c]
        => Rula[15, c]
    ```

* RuleDelayed
    * ����Rule�������߲������ȡ�
    * `LHS :> RHS`
    ```
    Rule[a, b]
        => Rule[a, b]
    a=10; b=15
    Rule[a, b]
        => Rule[a, b]
    Rule[a+b, c]
        => Rula[a+b, c]
    ```

* Replace
    * applies a rule or list of rules in an attempt to transform the entire expression expr. 
    * `Replace[expr, rules]`
    * ֻ�ж��������ʽ
    ```
    Replace[a+b, a+b->c]
        => c
    Replace[a+b+c, a+b->d]
        => a+b+c
    ```

* ReplaceAll
    * applies a rule or list of rules in an attempt to transform each subpart of an expression expr. 
    * `ReplaceAll[expr, rules]`
    * `expr /. rules`
    * ע�����ֻ���滻һ�Σ��Ȳ��ң��ٰ�Ҫ�滻���滻�������г�ͻ����ǰ�������
    ```
    ReplaceAll[a+b+c, a+c->d]
        => b+d

    ReplaceAll[a + b + c, {a + c -> d, a -> x}]
        => b+d
    
    {a, b, c} /. List -> f
        => f[a,b,c]
    
    ReplaceAll[a + b + c, {a :> x, a :> c -> d}]
        => b+c+x
    
    ReplaceAll[a + b + c, {a -> x, a + c -> d}]
        => b+d
    ```

* ReplaceRepeated
    * repeatedly performs replacements until expr no longer changes. 
    * `ReplaceRepeated[expr, rules]`
    * `expr //. rules`
    * �൱�ڶ�ε��� ReplaceAll ֱ�����䡣ע����ܻ���ѭ��

* Exit[]
    * �˳�


# 3. ʵ��ϸ��

* 20220301
    * ���в�����������ʽ�϶� ASTnode* ���У�Ҳ���ǲ������﷨���Ľڵ�
    * ͳһ�ɺ��� ASTnode *Compute(ASTnode *) �������﷨���ļ��㡣
        * �ڸú����У����ǵݹ���㵱ǰ�ڵ���ӽڵ����������ǣ��ٸ��ݵ�ǰ�ڵ�� NodeInfo ������Ӧ�ĺ������м��㡣
        * �������֮��ԭ���﷨���ᱻ�滻Ϊ����﷨�����ɽڵ���ͬ��ռ�ᱻ�ͷţ������������ù����µ��﷨���ϣ���
* 20220302
    * ���� Set �� SetDelayed �Ĺ���
        * ִ�� `Set[varName, expr]` ʱ���ȵ��� Compute �� expr ���������� SetDelayed �򲻻������������޷�����ı������������ٽ������� Duplicate �����������档
        * ���Ժ�Ҫ�õ� varName ������ʱ���� Duplicate ����һ�ݳ����ٶԸ��Ƴ������Ƿ��ֳ�������Ϊ�п����������һЩ��������ֵ�ˣ���
* 20220305
    * ���� Apply��
        ����֮ǰ����� VariableTable �࣬��ѯ���޸�ȫ�ֱ�����ܷ��㡣
        ���ǣ�Apply�ͷֳ��˼�����
        * ���������Ϣ
        * �޸ı�����
        * ���㲢����𰸣����ڷ��ż�������ԣ���ʱֻ��ҪComputeһ��Function��expr���ɣ�
        * �ָ�������Ϣ

* 20220328
    * ���� Power��
        ���ֲο��� mathematica ���ĵ�
        * `(a b)^c` is automatically converted to `a^c b^c` only if c is an integer. 
        * `(a^b)^c` is automatically converted to `a^(b c)` only if c is an integer. 
        * `Power[x,y,z,...]]` is taken to be `Power[x,Power[y,z,...]]]`.
        * `X^0` = 1
        * `X^1` = X
        * Integer ^ Integer ����ֱ����

* 20220329
    * ���� Times �� Power��
        Ϊ�˴��� `Times[a, Power[a, -1]]` ��������� Times ���Ĭ�϶����������� Power[#,1]���ٺϲ�ͬ����

* 20220331
    * ���� Rules��
        ����׼����һ�����ݽṹ������һ�� Rule ��һ�� List�������� Map ������


# 999. ��־
|���|����|�漰�ļ�|˵��|
|--|--|--|--|
||20220224|README.md|��ʼ����д�ĵ�|
||20220228|ASTnode.h ASTnode.cpp <br> Functions.h Functions.cpp <br> Integer.h Integer.cpp|ASTnode �ṹ���Լ�һЩ��������<br> AtomQ <br> FullForm <br> ��С������ͣ����ڲ��ԣ�|
||20220301|ASTnode.h ASTnode.cpp<br>Functions.h Functions.cpp|�޸��� CreateASTnode��ʹ�����ʱ����ָ�� preNode �� nxtNode <br> Compute|
||20220302|ASTnode.h ASTnode.cpp<br>Functions.h Functions.cpp<br>Variable.h Variable.cpp|ASTnode* Duplicate(xxx)<br>Set<br>VariableTable<br>���б������ˣ���������Set���б�����ֵ�ˣ����Ҹ�����һ�²���driver��ʹ�����ʶ�𼸺�����֧�ֱ����Ĵ��������ʽ��|
||20220303|test_ASTnode.cpp<br>ASTnode.h ASTnode.cpp <br> Functions.h Functions.cpp|Plus �Ľ���� <br>SetDelayed<br> **!!��Ҫ!!����Destroy���ڴ�й©bug���ĺ��ˣ����ڲ����м��˼������ڲ����ڴ��С���ߺ���**|
||20220304|ASTnode.h ASTnode.cpp<br>Functions.h Functions.cpp<br>Variable.h Variable.cpp|����ASTnode�Ķ��壬��ɴ�ͷβ�ڵ��˫�������������ֲ������Ը����<br>����ʹ�ñ���������ʱ������ڴ�й¶���޸��ˣ�����Compute�����ж�Var�ļ�������Destroy(node)�ˣ�<br>Plus �������ĺϲ�<br>Times�Ľ���ԡ��������ĺϲ�|
||20220305|Functions.h Functions.cpp|Function, Apply��ʵ�����Զ��庯��|
||20220315|ASTnode.h ASTnode.cpp <br> Functions.h Functions.cpp|����Compute����������ҽڵ������ڵ�û�޸ĺã����ܵ���ָ������ڴ棬���ˣ�Ŀǰû���������� <br> If ��� <br> Ϊ ASTnode �� Unmount ��������˱���ָ���� preNode_ �� nxtNode_ ������Ҳ��Ϊ��ʱ�����ѱ��ڴ� <br> ��������Apply���������ʱmap����Ӧ�ø���һ�ݶ����ǰ�ȫ�ֺ�����ĵ�ַ���ţ���Ϊ�����ڵݹ��ʱ��ȫ�ֺ�����ĵ�ַ�����ǡ��ͷ��� <br> Ϊ����쳲��������еĺ������ڼ�ª�ʷ��������������ʶ������|
||20220316|Functions.h Functions.cpp|������һ��Compute��ʹ�Զ��庯����ֱ�� f[] �������ã�����Ҫ���ֶ� Apply �� <br> ������������������ݹ��õ�ջ�ռ䣬���µݹ��������̫�࣬����mma����ݹ����Ҳ����������1024�����ջ���Ҫ�ں���Ż����¹���|
||20220318|Functions.cpp Functions.h <br> Integer.h|Quotient, Mod|
||20220319|Functions.cpp Functions.h <br> ASTnode.cpp|Flatten<br>�ڲ��� `Flatten[List[List[]]]`ʱ������CreateASTnode�еķ��գ�û��sonHead->preNode, sonTail->nxtNode ��ʼ�������ܻᵼ�·Ƿ���ַ���ʣ����ҵ����޸���һ���ڴ�bug<br>Help, Exit<br>��g++���߷������ڴ�й©����������������ʱд��test_ASTnode.cpp���û��VariableTable::EraseAllVars()����Ҳȷʵ��������ʱ��Ӧ�ð�VariableTableŪ�ɿ��Է�������Ķ����Զ�����<br>Equal����If���ˣ����ֲ��ܿ�������ֵ�ˣ�ֻ����True��False<br>�����Ѿ����Ա�̽��������ֽ���|
||20220325|ASTnode.h ASTnode.cpp<br>Functions.h Functions.cpp<br>Integer.h<br>Tools.h Tools.cpp|Sort<br>Plus, Times ��������Զ�����|
||20220327|Functions.h Functions.cpp<br>Tools.h Tools.cpp|Sort ������Զ�����������<br>Less/Greater<br>������һ��Apply��ʹ�õ��ú���ʱ�ṩ�Ĳ����������Դ��ں�������Ĳ��������������ڣ�������ǰ�������ν�ȡ��Ҫ�ģ�����Ĳ���|
||20220328|Functions.h Functions.cpp<br>Integer.h|Power����Ӧ�ĸ���Times<br>Divide|
||20220329|Functions.h Functions.cpp|Times���ܽ���ͬ��ϲ�ΪPower����ͬ������Power�ܺϲ�|
||20220330|Functions.h Functions.cpp<br>test_Driver.cpp<br>Varable.cpp|Subtract<br>���׵��﷨����<br>����һ��VariableTable::SetVarNode��Сbug��������û����node��NULL�����Σ�������|
||20220331|Variable.h Variable.cpp<br>Functions.cpp|���˸�VariableTable����������ʵ��������Ϊȫ���ṩһ�� GlobalVariableTable������ʵ������Ϊ���Ժ�� Replace �Ȳ�����׼��|

# һЩ mathematica ����
```
In := 2 + {FullForm[2 + 1], b}

In := a=b; b=a

In := a=b+1; b=a-1

In := a=b+1; b=a+1

In := x=1; fx=fun[x, y]; Print[fx]; x=2; Print[fx]; y=3; Print[fx]; y=4; Print[fx];

In := Set[a, 1]; Set[b, 1]; Set[c, 2]; Set[c, Plus[Set[a, b], Set[b, c]]]

In := Plus[1, 2, Times[a, Plus[Plus[], x], b], Plus[Plus[1, 2], 3]]

In := Plus[Plus[]]

In := Times[1, 2, Times[a, 5, f[Times[x, t, 2, z, 3], 6], 7, c], end1]

In := Times[1, 1]

In := Times[AtomQ[1], 0]

In := Set[a, 1]; Apply[Function[List[a, b], Times[a, b]], List[5, 6]]

In := Set[f, Function[List[x, y], Plus[Times[x,x,x], Times[y, y]]]]; Apply[f, List[2, 3]]

In :=   SetDelayed[ preSum,
            Function[ List[n],
                If[ Equal[n,0],0,
                    Plus[n, preSum[Plus[n, -1]]],
                ]
            ]
        ]

In :=   SetDelayed[ fib,
            Function[ List[n],
                If[ Equal[n, 1], 1,
                    If[ Equal[n, 2], 2,
                        Plus[
                            fib[Plus[n, -2]],
                            fib[Plus[n, -1]]
                        ]
                    ]
                ]
            ]
        ]
        SetDelayed[ showFib,
            Function[ List[num],
                If[ Equal[num,0], end,
                    List[showFib[Plus[num, -1]], fib[num]],
                ]
            ]
        ]
        showFib[25]
        Flatten[showFib[25]]

In :=   SetDelayed[power, Function[
            List[x, k],
            If[Equal[k, 0], 1, Times[x, power[x, Plus[k, -1]]]]
        ]]

In :=   SetDelayed[ksm, Function[
            List[x, k],
            If[Equal[k, 0], 1,
                If[Equal[Mod[k, 2], 0],
                    ksm[Times[x,x], Quotient[k, 2]],
                    Times[x, ksm[Times[x,x], Quotient[k, 2]]]
                ]
            ]
        ]]
        ksm[Plus[a, 10], 5];
        ksm[Times[a, 10], 5];

In :=   Flatten[List[a, b, List[c], d]]

In :=   Flatten[List[List[]]]

In :=   Flatten[List[List[], a, List[b, c, List[d, e], f, fun[List[g]]], Plus[a, b, 5, 6]]]

In :=   Set[digits, Function[List[n],
            If[Equal[n, 0], List[],
                Flatten[List[ digits[Quotient[n, 10]], Mod[n, 10] ]]
            ]
        ]]
        digits[666233]
        Set[fun, Function[List[x],
            Apply[Plus, digits[x]]
        ]]
        fun[666233]

In :=   Set[digits, Function[List[n, k],
            If[Equal[n, 0], List[],
                Flatten[List[ digits[Quotient[n, k], k], Mod[n, k] ]]
            ]
        ]]
        digits[10, 2]

In :=   Set[a, 1]; Equal[1, 1, 1, a]

In :=   ������
        SetDelayed[power, Function[
            List[x, k],
            If[Equal[k, 0], 1,
                If[Equal[Mod[k, 2], 0],
                    power[Times[x,x], Quotient[k, 2]],
                    Times[x, power[Times[x,x], Quotient[k, 2]]]
                ]
            ]
        ]]

        ��ȡ����
        SetDelayed[getm, Function[ List[n, k],
            If[ Equal[Mod[n, k], 0],
                Plus[1, getm[Quotient[n, k], k]],
                0
            ]
        ]]

        �ֽ⸨��
        SetDelayed[fac, Function[ List[n, k],
            If[ Equal[n, 1], List[],
                If[ Equal[n, k], g[k, 1],
                    If[ Equal[Set[nk, getm[n, k]], 0],
                        fac[Quotient[n, power[k, nk]], Plus[k, 1]],
                        Flatten[List[g[k, nk], fac[Quotient[n, power[k, nk]], Plus[k, 1]]]]
                    ]
                ]
            ]
        ]]

        �ֽ�
        SetDelayed[factors, Function[List[n], fac[n, 2]]]

In :=   Plus[a, b, Plus[f, h, a], Times[6, b], 5, 3, d, a, f1[x], f2[y], f1[z], f1[x], f1[1]]

In :=   SetDelayed[f, Function[List[x], Greater[x, 3]]]
        Sort[List[1, 6, 3, 4, 2, 8], f]

In :=   SetDelayed[g, Function[List[x,y], Greater[x, 3]]]
        Sort[List[1, 6, 3, 4, 2, 8], f]

In :=   SetDelayed[f, Function[List[x], Greater[x,3]]]
        f[5]
        f[5,2]
        f[2,5]

����λ������
In :=   SetDelayed[digits, Function[List[n],
            If[Equal[n, 0], List[],
                Flatten[List[ digits[Quotient[n, 10]], Mod[n, 10] ]]
            ]
        ]]
        SetDelayed[digitSum, Function[List[n], Apply[Plus, digits[n]]]]
        SetDelayed[cmp, Function[List[n1,n2],Less[digitSum[n1],digitSum[n2]]]]
        Sort[List[1,2,3,10,100,5,6,7,8,321,4,124,142,4124,643,234,43,764536], cmp]

In :=   Times[b, Power[Times[c, d], -1]] // FullForm
        Divide[b, Times[c, d]] // FullForm

In :=   Times[a, Power[Times[b, Power[Times[c, d], -1]], -1]] // FullForm
        Divide[a, Divide[b, Times[c, d]]] // FullForm

In :=   Power[Times[Power[a,b,c],d,e],Power[f,g]]
        Power[Times[Power[a,b,c],d,e],5]
        Power[Power[a, Power[b, c]], 5]

In :=   Power[Times[1,-5], 5]

In :=   Times[Power[Times[a, b], -2], Power[Times[b, c], -1], b, c, Times[b, b]]

In :=   Divide[Times[2,5,6], Power[2, -1]]
        Divide[Times[a,b,c], Power[d, -2]]

In :=   Times[a, Times[1,7,b,5], Power[a,5], b]
        Times[a, a, Power[a, Plus[f, 6]], Power[Times[a, b], -7], Divide[6, 4]]
        Times[Power[7, 5], Divide[Power[a, 2], 7], Divide[2, 7]]
        Divide[Power[a, 7], Power[Times[a, b, 6], Times[Plus[g[5], 2]]]]
        Divide[Power[a, 7], Power[Times[a, b, 6], Plus[2, 9]]]
        Times[Power[Times[a, b], -2], Power[Times[b, c, d], -1], b, c, Times[b, b]]

20220330
In :=   a/b*c
        a^b^c^d
        a+b*c^d^(e-f)/g+18-(6+f[16])
        5-3
        a^b*c
        a/b*c/d/e
        a/b/c/d
        (a/b)/(c/d)
        a+(b+(c+(d-e+f)))
        (a+b)/((c+d)/(e/f))
        a-b^c/z
        a^b-c/d^e/f
        a=1
        {}
        {1,2,a,fun[{1,2}]}
        {1,10817, 1926, 114, 514, fun[{1,2}], 3}
        쳲��������� = fib := Function[{n},
            If[n<3, 1, fib[n-1]+fib[n-2]]
        ]

        getm := Function[ {n, k},
            If[Mod[n, k] == 0,
                1+getm[Quotient[n, k], k],
            0]
        ]

        fac := Function[ {n, k},
            If[ n<2, {},
                If[ n==k, g[k, 1],
                    If[ (tmp=getm[n, k]) == 0,
                        fac[Quotient[n, k^tmp], k+1],
                        Flatten[{g[k, tmp], fac[Quotient[n, k^tmp], k+1]}]
                    ]
                ]
            ]
        ]

        factors = ��ʽ�ֽ� = Function[{n}, fac[n, 2]]

        powerN = Function[{n}, Function[{x}, x^n]]
        powerN[3][5]
        Function[{n}, Function[{x}, x^n]][2]

```