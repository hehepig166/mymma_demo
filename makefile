FLAG 		:= -Wall -fsanitize=address -static-libasan -std=c++17

COMMON_FILES		:= ./common/Integer.cpp ./common/ASTnode.cpp ./common/Functions.cpp ./common/Variable.cpp ./common/Tools.cpp

test_ASTnode:
	g++ $(FLAG) -o demo.exe ./tests/test_ASTnode.cpp $(COMMON_FILES)

debug:
	g++ $(FLAG) -o demo.exe -g ./tests/test_ASTnode.cpp $(COMMON_FILES)

static:
	g++ -Wall -o demo.exe -static ./tests/test_ASTnode.cpp $(COMMON_FILES)

win:
	g++ -Wall -o demo.exe -static ./tests/test_ASTnode.cpp ./common/Integer.cpp ./common/ASTnode.cpp ./common/Functions.cpp ./common/Variable.cpp ./common/Tools.cpp