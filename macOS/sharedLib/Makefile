CFLAGS:=-g -O0 -L.
# in order to not have to set LD_LIBRARY_PATH
LFLAGS0:=-Wl,-rpath,.
LFLAGS1:=-Wl,-rpath,./blzblz

# remember, the lib name must begin by ... lib


all: test

test: test.c libinner1.dylib libinner2.dylib
	clang ${CFLAGS} ${LFLAGS0} -o $@ $< -linner1 -linner2

libinner1.dylib: inner1.c inner1.h libinner2.dylib
	clang ${CFLAGS} ${LFLAGS1} -dynamiclib -install_name @rpath/$@ -o $@ $< -linner2

libinner2.dylib: inner2.c inner2.h
	clang ${CFLAGS} -dynamiclib -install_name @rpath/$@ -o $@ $<

clean:
	rm -rf *~ *.dylib *.dSYM test
