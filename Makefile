CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iheaders -Ilogic -Ilogic/Roles -IGUI
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

SOURCES = main.cpp \
          logic/Game.cpp \
          logic/Player.cpp \
          logic/Roles/Governor.cpp \
          logic/Roles/Spy.cpp \
          logic/Roles/Judge.cpp \
          logic/Roles/General.cpp \
          logic/Roles/Baron.cpp \
          logic/Roles/Merchant.cpp \
          GUI/SFMLGui.cpp

TARGET = coup

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)