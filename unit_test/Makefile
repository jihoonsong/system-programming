CC = gcc
CFLAGS = -g -Wall
TARGET = unit_test.out

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
DEPS := $(SRCS:.c=.d)

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

sinclude $(DEPS)

%.d: %.c
	@set -e; \
	$(CC) $(CFLAGS) -MM $< \
	| sed 's,\($*\)\.o[ :]*,\1.o $@: ,g' > $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS)
	rm -f $(TARGET)
