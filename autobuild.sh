libtoolize && \
    aclocal && \
    automake --add-missing && autoconf

echo Now run: CFLAGS=\'-O2 -DDEBUG \' ./configure
