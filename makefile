all: main.cc xq4gb
	g++ main.cc -o app -std=c++17 -g
xq4gb: xq4gb.cc
	g++ xq4gb.cc -o xq4gb -std=c++17
clean:
	$(RM) app
