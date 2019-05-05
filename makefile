CXX = g++-8

FLAGS = --std=c++2a --all-warnings --extra-warnings -Wno-address \
	-Werror -Wshadow -Wfloat-equal -Weffc++ -Wdelete-non-virtual-dtor -O1

all: tmp tmp/ac44 noise

tmp/%.o: %.cpp
	$(CXX) $(FLAGS) -c -o $@ $<


tmp/ac44: tmp/main.o tmp/fourier.o tmp/ascii.o tmp/timestamp.o
	$(CXX) $(FLAGS) -o $@ $^

tmp:
	mkdir $@

params = -q -f S16_LE -c 2 -r 44100

noise: tmp/ac44
	arecord $(params) -d 2 | $<

clean:
	rm -rf tmp

tmp/example.wav: tmp
	arecord $(params) -d 2 > $@
