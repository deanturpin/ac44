CXX = g++-8

FLAGS = --std=c++2a --all-warnings --extra-warnings -Wno-address \
	-Werror -Wshadow -Wfloat-equal -Weffc++ -Wdelete-non-virtual-dtor -O1

tmp/%.o: %.cpp
	$(CXX) $(FLAGS) -o $@ $< 

all: tmp tmp/main.o run

tmp:
	mkdir $@

run:
	@tmp/main.o

clean:
	rm -rf tmp
