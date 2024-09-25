g++ -Wall -std=c++17 -fexceptions -Wno-invalid-offsetof -O3 -Iinclude -c bialo-plus.cpp -o obj/Release/bialo-plus.o
g++ -Wall -std=c++17 -fexceptions -Wno-invalid-offsetof -O3 -Iinclude -c handler.cpp -o obj/Release/handler.o
g++ -Wall -std=c++17 -fexceptions -Wno-invalid-offsetof -O3 -Iinclude -c main.cpp -o obj/Release/main.o
g++  -o bin/Release/fcgi-site obj/Release/bialo-plus.o obj/Release/handler.o obj/Release/main.o  -s  -lfcgi

