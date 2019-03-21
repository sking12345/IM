#makefile 
#


cc = xcrun -sdk iphoneos clang
CPP = xcrun -sdk iphoneos clang
AR = ar
CFLAGS += -arch armv7 -mios-version-min=6.0  -march=armv7-a
LDFLAGS += -arch armv7 -mios-version-min=6.0  -march=armv7-a  -Wl,  -Bsymbolic-functions -read_only_relocs suppress




object = thread_pool.o socket.o log.o

server:server.o $(object)
	cc -o server server.o $(object) -levent
	rm  server.o $(object)

client:  client.o  $(object)
	cc -o client client.o  $(object)
	rm  client.o  $(object)

client.o: client.c
	cc -c client.c

server.o: server.c
	cc -c server.c

thread_pool.o: thread_pool.c thread_pool.h
	cc -c thread_pool.c

socket.o: socket.c socket.h
	cc -c socket.c
log.o: log.c log.h
	cc -c log.c

clear_all:
	rm  client server  client.o server.o thread_pool.o socket.o log.o
clear_server:
	rm  server server.o thread_pool.o socket.o log.o
clear_client:
	rm  client client.o thread_pool.o socket.o log.o






