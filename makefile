proc : main.o tiny.o doit.o parse_uri.o serve_static.o serve_dynamic.o clienterror.o
	cc -o proc tiny.o main.o doit.o parse_uri.o serve_static.o serve_dynamic.o clienterror.o -pthread
	
tiny.o : tiny.c tiny.h
	cc -c tiny.c
main.o : main.c tiny.h doit.c 
	cc -c main.c 
doit.o : doit.c tiny.h parse_uri.c serve_static.c serve_dynamic.c clienterror.c
	cc -c doit.c
parse_uri.o : parse_uri.c tiny.h
	cc -c parse_uri.c
serve_static.o : serve_static.c tiny.h
	cc -c serve_static.c
serve_dynamic.o : serve_dynamic.c tiny.h
	cc -c serve_dynamic.c
clienterror.o : clienterror.c tiny.h
	cc -c clienterror.c
clean :
	rm proc main.o tiny.o parse_uri.o doit.o serve_static.o serve_dynamic.o clienterror.o

