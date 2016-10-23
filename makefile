analy_sound: wavfile.o analy_sound.o
	gcc -std=c11 -o analy_sound wavfile.o analy_sound.o

analy_sound.o: analy_sound.c
	gcc -std=c11 -c analy_sound.c

wavefile.o: wavfile.h wavfile.c
	gcc -std=c11 -c wavfile.c

clean:
	rm analy_sound *.o
