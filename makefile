all: main.cpp
	g++ main.cpp -o app -std=c++17

clean:
	$(RM) app
