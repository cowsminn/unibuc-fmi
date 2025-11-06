# Compile each component and link
all: main

main: main.o read_data.o numerator.o denominator.o
	mpicc -o main main.o read_data.o numerator.o denominator.o

main.o: main.c
	mpicc -c main.c

read_data.o: read_data.c
	mpicc -c read_data.c

numerator.o: numerator.c
	mpicc -c numerator.c

denominator.o: denominator.c
	mpicc -c denominator.c

clean:
	rm -f *.o main result.txt
