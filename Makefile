CXXFLAGS := -std=c++20 -Wall -Wextra -Wconversion -Werror

run: app
	./app

app: *.cpp
	g++ $(CXXFLAGS) *.cpp -o app

clean:
	rm -f app
