flags=-Wall -O

all: tcp_sv tcp_cl	
tcp_sv: tcp_sv.o
	gcc $(flags) tcp_sv.o -o tcp_sv
tcp_cl: tcp_cl.o
	gcc $(flags) tcp_cl.o -o tcp_cl
tcp_sv.o: tcp_sv.c
	gcc $(flags) -c tcp_sv.c
tcp_cl.o: tcp_cl.c
	gcc $(flags) -c tcp_cl.c

clean:
	rm -rf *.o tcp_cl tcp_sv