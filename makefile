CXX = g++-8

FLAGS = --std=c++2a --all-warnings --extra-warnings -Wno-address \
	-Werror -Wshadow -Wfloat-equal -Weffc++ -Wdelete-non-virtual-dtor -O1

tmp/%.o: %.cpp
	$(CXX) $(FLAGS) -o $@ $<

all: tmp tmp/main.o tmp/example.wav noise

tmp:
	mkdir $@

params = -q -f S16_LE -c 1 -r 44100

noise:
	arecord $(params) | tmp/main.o

clean:
	rm -rf tmp

tmp/example.wav:
	arecord $(params) -d 2 > $@
