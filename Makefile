all: bcrypt.o base64.o const.o
	clang -o bcrypt bcrypt.o base64.o const.o
	make clean

clean:
	rm *.o

bcrypt.o:
	clang -c bcrypt.c

blowfish.o:
	clang -c blowfish.c

base64.o:
	clang -c base64.c

const.o:
	clang -c const.c

