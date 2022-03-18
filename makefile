FLAG 		:= -Wall -fsanitize=address -static-libasan -std=c++17

test_ASTnode:
	g++ $(FLAG) -o demo.exe ./tests/test_ASTnode.cpp ./common/Integer.cpp ./common/ASTnode.cpp ./common/Functions.cpp ./common/Variable.cpp

debug:
	g++ $(FLAG) -o demo.exe -g ./tests/test_ASTnode.cpp ./common/Integer.cpp ./common/ASTnode.cpp ./common/Functions.cpp ./common/Variable.cpp

static:
	g++ $(FLAG) -o demo.exe -static ./tests/test_ASTnode.cpp ./common/Integer.cpp ./common/ASTnode.cpp ./common/Functions.cpp ./common/Variable.cpp

win:
	g++ -Wall -o demo.exe -static ./tests/test_ASTnode.cpp ./common/Integer.cpp ./common/ASTnode.cpp ./common/Functions.cpp ./common/Variable.cpp