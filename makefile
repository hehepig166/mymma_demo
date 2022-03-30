FLAG 		:= -Wall -fsanitize=address -static-libasan -std=c++17

COMMON_FILES		:= ./common/Integer.cpp ./common/ASTnode.cpp ./common/Functions.cpp ./common/Variable.cpp ./common/Tools.cpp

MAIN_FILE	:= ./tests/test_Driver.cpp
#MAIN_FILE	:= ./tests/test_ASTnode.cpp

test_Driver:
	g++ $(FLAG) -o demo.exe $(MAIN_FILE) $(COMMON_FILES)

test_ASTnode:
	g++ $(FLAG) -o demo.exe ./tests/test_ASTnode.cpp $(COMMON_FILES)

debug:
	g++ $(FLAG) -o demo.exe -g $(MAIN_FILE) $(COMMON_FILES)

static:
	g++ -Wall -o demo.exe -static $(MAIN_FILE) $(COMMON_FILES)
