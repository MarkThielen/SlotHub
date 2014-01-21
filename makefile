CC = g++
CFLAGS = -std=c++11 -Wall -I./

PB=protoc

PB_DIRS=./pb
PB_FLAGS=--cpp_out=$(PB_DIRS)

LDFLAGS = 
LDLIBS = -lncurses -largtable2 -lprotobuf -lzmq
DEBUGFLAGS=-ggdb

CC_SOURCE_FILES = SlotHub.c ttyTools.c ControlUnit.cpp TextDisplay.cpp CarStatus.cpp TrackStatus.cpp CarreraResponse.cpp event/Session.cpp 
PB_SOURCE_FILES = pb/TrackStatusMessage.pb.cc pb/CarStatusMessage.pb.cc pb/SessionMessage.pb.cc


all: clean ProtoBuf SlotHub

SlotHub: ProtoBuf
	$(CC) $(LD_FLAGS) $(LDLIBS) $(CFLAGS) $(DEBUGFLAGS) $(CC_SOURCE_FILES) $(PB_SOURCE_FILES)
	
ProtoBuf:
	$(PB) -I$(PB_DIRS) $(PB_FLAGS) $(PB_DIRS)/CarStatusMessage.proto
	$(PB) -I$(PB_DIRS) $(PB_FLAGS) $(PB_DIRS)/TrackStatusMessage.proto	
	$(PB) -I$(PB_DIRS) $(PB_FLAGS) $(PB_DIRS)/SessionMessage.proto

CarStatus:
	$(CC) $(LD_FLAGS) $(LDLIBS) $(CFLAGS) $(DEBUGFLAGS) CarStatus.cpp

CarreraResponse:
	$(CC) $(LD_FLAGS) $(LDLIBS) $(CFLAGS) $(DEBUGFLAGS) CarreraResponse.cpp


TrackStatus:
	$(CC) $(LD_FLAGS) $(LDLIBS) $(CFLAGS) $(DEBUGFLAGS) TrackStatus.cpp


ControlUnit:
	$(CC) $(LD_FLAGS) $(LDLIBS) $(CFLAGS) $(DEBUGFLAGS) ControlUnit.cpp

clean:
	rm -rf *o SlotHub
	rm -f $(PB_DIRS)/*.pb.cc $(PB_DIRS)/*.pb.h
