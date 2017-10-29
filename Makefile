bpm: audio.o bpm.o
	cc -o bpm bpm.o audio.o -lxmp -lasound

bpm.o : bpm.c sound.h
	cc -c bpm.c

autio.o : audio.c sound.h
	cc -c audio.c

clean :
	rm bpm bpm.o audio.o
