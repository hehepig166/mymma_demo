test_ASTnode:
	g++ -Wall ./tests/test_ASTnode.cpp ./common/Integer.cpp ./common/ASTnode.cpp ./common/Functions.cpp ./common/Variable.cpp -o test_ASTnode.exe -std=c++17 -g