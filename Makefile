CXX := g++
CXXFLAGS := -I/usr/include/qt/QtWidgets -I/usr/include/qt -I/usr/include/qt/QtCore -I/usr/include/qt/QtGui -fPIC -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB

LXX := $(CXX)
LXXFLAGS := -lQt5Widgets -lQt5Gui -lQt5Core -lpthread

COMPONENTS := mathexpression mathgrapher qtdisplay renderer

OBJDIR := obj
OBJS := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(COMPONENTS)))

all: build

$(OBJDIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin:
	mkdir bin

$(OBJDIR):
	mkdir $(OBJDIR)

bin/mathgrapher: bin $(OBJDIR) $(OBJS)
	$(LXX) $(LXXFLAGS) $(OBJS) -o bin/mathgrapher

run: bin/mathgrapher
	bin/mathgrapher

build: bin/mathgrapher

flush:
	rm -rf obj/*

clean: flush
	rm -rf bin/mathgrapher

rebuild: clean build run
