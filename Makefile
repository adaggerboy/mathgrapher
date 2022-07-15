CXX := g++
CXXFLAGS := -DQT_WIDGETS_LIB -I/usr/include/qt/QtWidgets -I/usr/include/qt -I/usr/include/qt/QtCore -DQT_GUI_LIB -I/usr/include/qt/QtGui -DQT_CORE_LIB -lQt5Widgets -lQt5Gui -lQt5Core -fPIC -lpthread

all: bin/mathgrapher

bin/mathgrapher:
	$(CXX) $(CXXFLAGS) -o bin/mathgrapher src/*.cpp

run: bin/mathgrapher
	bin/mathgrapher

clean:
	rm -f bin/mathgrapher

full: clean all run
