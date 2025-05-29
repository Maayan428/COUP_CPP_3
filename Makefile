//maayan428@gmail.com

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iheaders -Ilogic -Ilogic/Roles -IGUI -I/usr/include/doctest
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

LOGIC_SRC = logic/Game.cpp logic/Player.cpp
ROLES_SRC = logic/Roles/Governor.cpp logic/Roles/Spy.cpp logic/Roles/Judge.cpp logic/Roles/General.cpp logic/Roles/Baron.cpp logic/Roles/Merchant.cpp
GUI_SRC = GUI/SFMLGui.cpp 

MAIN_SRC = main.cpp $(LOGIC_SRC) $(ROLES_SRC)
GUI_APP = run_gui.cpp $(LOGIC_SRC) $(ROLES_SRC) $(GUI_SRC)

TEST_SRC = Tests/test.cpp $(LOGIC_SRC) $(ROLES_SRC)  

.PHONY: all clean Main gui test valgrind

all: Main

Main: main
main: $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) -o main $(MAIN_SRC)
	./main
gui: $(GUI_APP)
	$(CXX) $(CXXFLAGS) -o gui $(GUI_APP) $(LDFLAGS)
	./gui

test: $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -o test $(TEST_SRC)
	./test

valgrind: main
	valgrind --leak-check=full ./main

clean:
	rm -f main gui test 