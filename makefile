all: main.cpp
	g++ main.cc -o app -std=c++17 -g

clean:
	$(RM) app
