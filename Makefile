

all: clean bin/mathgrapher

bin/mathgrapher:
	g++ -o bin/mathgrapher src/*.cpp

run: bin/mathgrapher
	bin/mathgrapher

clean:
	rm bin/mathgrapher
