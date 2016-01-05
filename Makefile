CC = gcc
LDFLAGS =
CFLAGS = -Wall -Ofast -fstrict-aliasing -march=native
OBJECTS = roboarchvm.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

roboarchvm: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

debug:	CFLAGS= -Wall -O0 -g  -fstrict-aliasing -march=native
debug: 	$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o roboarchvm-$@ $(LDFLAGS)


clean:
	@echo cleaning oop
	@rm -f $(OBJECTS)
purge:
	@echo purging oop
	@rm -f $(OBJECTS)
	@rm -f roboarchvm
	@rm -f roboarchvm-debug
