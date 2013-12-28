CC = gcc
CFLAGS =
LDFLAGS = 
LDLIBS = -lncurses -largtable2
DEBUGFLAGS=-ggdb

all: clean SlotHub

SlotHub:
	$(CC) $(LD_FLAGS) $(LDLIBS) $(CFLAGS) $(DEBUGFLAGS) CarreraSlotCar.c ttyTools.c -o SlotHub

clean:
	rm -rf *o SlotHub
