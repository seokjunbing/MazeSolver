# Makefile for: Maze Challenge
# Dartmouth College CS 50 W15
# Group C-Sick:
#        Jun Bing
#        Van Nguyen
#        Sahil Seekond
#        Matthew West
TARGET = AMStartup
EXECUTABLE = AMStartup

CC	= gcc
# compiling flags #here
CFLAGS	= -std=c11 -Wall -pedantic -I. -D_XOPEN_SOURCE

LINKER	= gcc -o
# linking flags here
LFLAGS	= -Wall -I. -lm -lcurl -pthread

# Directories for .o, .c/.h, and executable
OBJDIR	= obj
SRCDIR	= src
BINDIR	= .
TESTDIR	= test

SOURCES	:= $(wildcard $(SRCDIR)/*.c)
INCLUDES:= $(wildcard $(SRCDIR)/*.h)
OBJECTS	:= $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TESTS 	:= $(wildcard $(TESTDIR)/*.c)

rm	:= rm -f

.PHONY: all
all:
	@echo "Building maze solver..."
	make $(EXECUTABLE)
	make test
	./unit_tests

AMStartup: $(OBJECTS)
	$(LINKER) $@ $(LFLAGS) $(OBJECTS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: test
test: $(SOURCES) $(INCLUDES) $(TESTS)
	$(CC) $(CFLAGS) $(LFLAGS) -D TESTING -g -ggdb -o unit_tests $(TESTS) $(SOURCES)

.PHONY: debug
debug: $(SRCS)
	$(CC) $(CFLAGS) $(LFLAGS) -g -ggdb3 -o $(TARGET) $(SOURCES)

.PHONY: profile
profile: $(SRCS)
	$(CC) $(CFLAGS) $(LFLAGS) -pg -o $(EXECUTABLE) $(SOURCES)

.PHONY: clean
clean:
	$(rm) $(OBJECTS)
	$(rm) $(wildcard *.log)

.PHONY: remove
remove:
	$(rm) $(TARGET) unit_tests

