CXXFLAGS := -std=c++20 -pedantic -Wpedantic \
            -Wall -Wextra -Wconversion -Wsign-conversion \
            -Wshadow -Wnon-virtual-dtor -Wold-style-cast \
            -Wnull-dereference -Wdouble-promotion -Woverloaded-virtual \
            -Werror

run: app
	./app

app: *.cpp
	g++ $(CXXFLAGS) *.cpp -o app

clean:
	rm -f app transactions.txt accounts.txt
