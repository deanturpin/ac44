CXX = g++-8

FLAGS = --std=c++2a --all-warnings --extra-warnings -Wno-address \
	-Werror -Wshadow -Wfloat-equal -Weffc++ -Wdelete-non-virtual-dtor -O1

tmp/%.o: %.cpp
	$(CXX) $(FLAGS) -o $@ $< 

all: tmp tmp/main.o run

tmp:
	mkdir $@

params = -q -f S16_LE -c 1 -r 44100

run:
	arecord $(params) -d 1 | tmp/main.o

clean:
	rm -rf tmp

generate:
	arecord $(params) -d 5 > tmp/long.raw
