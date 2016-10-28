CFLAGS:=-g -O0 -L.
# in order to not have to set LD_LIBRARY_PATH
LFLAGS0:=-Wl,-rpath='$$ORIGIN/'.
LFLAGS1:=-Wl,-rpath=blabla

#-Wl,-rpath=/home/honorat/Documents/dev/sand-box/gcc_debug
# remember, the lib name must begin by ... lib


all: test

test: test.c libinner1.so libinner2.so
	gcc ${CFLAGS} ${LFLAGS0} -o $@ $< -linner1 -linner2

libinner1.so: inner1.c inner1.h libinner2.so
	gcc ${CFLAGS} ${LFLAGS1} -shared -fPIC -o $@ $< -linner2

libinner2.so: inner2.c inner2.h
	gcc ${CFLAGS} -shared -fPIC -o $@ $<

clean:
	rm -f *~ *.so test
