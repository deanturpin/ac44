CXX = g++-8

FLAGS = --std=c++2a --all-warnings --extra-warnings -Wno-address \
	-Werror -Wshadow -Wfloat-equal -Weffc++ -Wdelete-non-virtual-dtor -O1

tmp/%.o: %.cpp
	$(CXX) $(FLAGS) -o $@ $< 

all: tmp tmp/main.o run

tmp:
	mkdir $@

	# Get some WAVs to process
	# curl https://github.com/deanturpin/dft/blob/master/wav/didgeridoo_big_tony_toot.wav?raw=true -o $@/audio1.wav
	# curl https://github.com/deanturpin/dft/blob/master/wav/singing_bowl3.wav?raw=true -o $@/audio2.wav

run:
	@tmp/main.o

clean:
	rm -rf tmp
