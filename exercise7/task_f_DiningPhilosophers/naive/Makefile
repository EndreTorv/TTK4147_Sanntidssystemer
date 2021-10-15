CC = clang # or "gcc"
# program executable name
TARGET = dining_philosophers
# compiler flags
CFLAGS =  -g -Wall -std=gnu11 -fsanitize=thread
# linker flags
LDFLAGS = -g -lpthread -lcomedi -lm -fsanitize=thread
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
