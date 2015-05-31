
CFLAGS= -g -c

DEPS=*.h
BIN_DIR=../../bin/
OBJDIR=../../obj/
CC=gcc
OBJ_FILES= $(OBJ_DIR)clienthandler.o $(OBJ)misc.o $(OBJ)rooms.o  $(OBJ)users.o $(OBJ_DIR)server.o $(OBJ_DIR)crypt.o
ifeq ($(OS),Windows_NT)
LIBS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_net -ljansson
INC_DIR=-I../../include -I/usr/include -I/usr/local/include
LIBPATHS= -L/usr/lib -L/usr/local/lib
else
UNAME_S:=$(shell uname -s)
ifeq ($(UNAME_S),Linux)
LIBS= -lSDL2 -lSDL2_net -ljansson
INC_DIR= -I../../include
endif
ifeq ($(UNAME_S),Darwin)
LIBS= -framework SDL2 -framework SDL2_net -ljansson
INC_DIR=-I../../include -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_net.framework/Headers -I/Library/Frameworks/SDL2_ttf.framework/Headers -I/Library/Frameworks/SDL2_image.framework/Headers
CFLAGS+= -D__apple__
endif
endif



all: $(BIN_DIR)server
debug: CFLAGS += -DDEBUG 
debug: $(BIN_DIR)server

$(BIN_DIR)server : $(OBJ_FILES)
	$(CC)  -o server $(OBJ_FILES) $(INC_DIR) $(LIBPATHS) $(LIBS)


$(OBJ_DIR)users.o : users.c
	$(CC) users.c  $(CFLAGS) $(INC_DIR) $(LIBPATHS)  $(LIBS)



$(OBJ_DIR)server.o : server.c
	$(CC) server.c  $(CFLAGS) $(INC_DIR) $(LIBPATHS) $(LIBS)


$(OBJ_DIR)clienthandler.o : clienthandler.c
	$(CC) clienthandler.c  $(CFLAGS) $(INC_DIR) $(LIBPATHS)  $(LIBS)

$(OBJ_DIR)misc.o : misc.c
	$(CC) misc.c  $(CFLAGS) $(INC_DIR) $(LIBPATHS)  $(LIBS)

$(OBJ_DIR)rooms.o : rooms.c
	$(CC) rooms.c  $(CFLAGS) $(INC_DIR)  $(LIBS)

$(OBJ_DIR)crypt.o : crypt.c
	$(CC) crypt.c  $(CFLAGS) $(INC_DIR)  $(LIBS)


clean:
	rm *o server
