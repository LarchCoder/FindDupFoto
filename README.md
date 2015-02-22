# FindDupFoto
find Duplicated in  folder hierarchy

Dependencies:
- apr-1.4.5
	./configure CFLAGS=-g3 -O0
	make
	
- sqlite-autoconf-3070900
	./configure
	make
	
- apr-util-1.4.1
	./configure CFLAGS=-g3 -O0  --with-apr=/libsExtern/apr-1.4.5 --with-sqlite3=/libsExtern/sqlite-autoconf-3070900 --disable-util-dso
	make
