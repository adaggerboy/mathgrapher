CXX := g++
CXXFLAGS := -I/usr/include/qt/QtWidgets -I/usr/include/qt -I/usr/include/qt/QtCore -I/usr/include/qt/QtGui -fPIC -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB

LXX := $(CXX)
LXXFLAGS := -lQt5Widgets -lQt5Gui -lQt5Core -lpthread

COMPONENTS := mathexpression mathgrapher qtdisplay renderer eventhandler controls

OBJDIR := obj
OBJS := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(COMPONENTS)))

all: build

obj/qtdisplay.o: src/moc_qtdisplay.cpp
	$(CXX) $(CXXFLAGS) -c src/moc_qtdisplay.cpp -o obj/qtdisplay.o

src/moc_qtdisplay.cpp: src/qtdisplay.cpp
	cat src/qtdisplay.cpp > src/moc_qtdisplay.cpp
	moc src/qtdisplay.cpp >> src/moc_qtdisplay.cpp

$(OBJDIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin:
	mkdir bin

$(OBJDIR):
	mkdir $(OBJDIR)

bin/mathgrapher: bin $(OBJDIR) $(OBJS)
	$(LXX) $(LXXFLAGS) $(OBJS) -o bin/mathgrapher

run: bin/mathgrapher
	bin/mathgrapher 0

test: bin/mathgrapher
	bin/mathgrapher 2*sin\(x/2\)

build: bin/mathgrapher

flush:
	rm -rf src/moc_qtdisplay.cpp
	rm -rf obj/*

clean: flush
	rm -rf bin/mathgrapher

rebuild: clean build run
