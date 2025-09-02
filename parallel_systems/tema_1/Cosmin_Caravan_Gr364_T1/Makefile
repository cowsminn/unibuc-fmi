CC = mpicc
CFLAGS = -Wall -g
OBJS = main.o io.o compute.o
TARGET = avg_calc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c io.h compute.h
	$(CC) $(CFLAGS) -c $<

io.o: io.c io.h
	$(CC) $(CFLAGS) -c $<

compute.o: compute.c compute.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET) *~

run: $(TARGET)
	mpirun -np 4 ./$(TARGET)

.PHONY: all clean run
