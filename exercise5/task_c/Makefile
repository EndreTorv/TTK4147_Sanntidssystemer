CC = clang # or "gcc"
# program executable name
TARGET = task_c
# compiler flags
CFLAGS = -g -Wall -std=gnu11
# linker flags
LDFLAGS = -g -lpthread -lcomedi -lm
# list of sources
SOURCES = $(shell find -name "*.c")
# default rule, to compile everything
all: $(TARGET)
# define object files
OBJECTS = $(SOURCES:.c=.o)
# link programs
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^
# compile
%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
# cleaning
clean:
	rm -f $(TARGET) $(OBJECTS)
