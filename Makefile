CC = gcc

all:
	$(CC) -o ./Debug/xxtea_classic xxtea_classic.c

clean:
	rm -rf ./Debug/*