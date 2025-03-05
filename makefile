all: main.cpp
	g++ main.cpp -o app -std=c++17 -g

clean:
	$(RM) app
