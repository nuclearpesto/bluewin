	
CFLAGS=-c -I$(INC_DIR)
DEPS=*.h
BIN_DIR=../../bin/
OBJDIR=../../obj/
CC=gcc

ifeq ($(OS),Windows_NT)
LIBS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_net -ljansson 
INC_DIR=-I../../include -I/usr/include -I/usr/local/include
LIBPATHS= -L/usr/lib -L/usr/local/lib
else
UNAME_S:=$(shell uname -s)
ifeq ($(UNAME_S),Linux)
LIBS= -lSDL2 -lSDL2_net -ljansson		
INC_DIR=../../include
endif
ifeq ($(UNAME_s),Darwin)
LIBS= -lSDL2 -lSDL2_net -ljansson
INC_DIR=../../include
endif
endif

all: $(BIN_DIR)server
$(BIN_DIR)server : $(OBJ_DIR)server.o $(OBJ_DIR)clienthandler.o $(OBJ)misc.o $(OBJ)rooms.o  
	$(CC)  -o server server.o clienthandler.o misc.o rooms.o $(INC_DIR) $(LIBPATHS) $(LIBS)

$(OBJ_DIR)server.o : server.c 
	$(CC) server.c  $(CFLAGS) $(INC_DIR)  $(LIBS) 


$(OBJ_DIR)clienthandler.o : clienthandler.c  
	$(CC) $(CFLAGS) $(INC_DIR)  $(LIBS)  clienthandler.c 

$(OBJ_DIR)misc.o : misc.c
	$(CC) $(CFLAGS) $(INC_DIR)  $(LIBS)    misc.c 


$(OBJ_DIR)rooms.o : rooms.c  
	$(CC) rooms.c  $(CFLAGS) $(INC_DIR)  $(LIBS) 

clean:
	rm *o server
