CC = gcc9
echos : echos.c
	$(CC) $@.c -o $@
echoc : echoc.c
	$(CC) $@.c -o $@
clean :
	rm echos echoc
all : echos echoc
