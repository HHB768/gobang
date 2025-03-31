all: main.cc xq4gb logE
	g++ main.cc -o app -std=c++17 -g
xq4gb: xq4gb.cc
	g++ xq4gb.cc -o xq4gb -std=c++17
logE: log.cc
	g++ log.cc -o logE -std=c++17 -g
clean:
	$(RM) app xq4gb logE
logclean:
	rm -rf ./log ./archive ./inference
