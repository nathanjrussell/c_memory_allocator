CC = gcc
CFLAGS = -I. -I./matrix_operations -I./memory_allocator

DEPS = matrix_operations/matrix_operations.h memory_allocator/memory_allocator.h
OBJ = matrix_test.o matrix_operations/matrix_operations.o memory_allocator/memory_allocator.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

matrix_test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean test_mymalloc

clean:
	rm -f $(OBJ) matrix_test

test_malloc: clean matrix_test
	./matrix_test

test_mymalloc: CFLAGS += -DUSE_MY_MALLOC
test_mymalloc: clean matrix_test
	$(CC) -c -o matrix_operations/matrix_operations.o matrix_operations/matrix_operations.c $(CFLAGS)
	./matrix_test