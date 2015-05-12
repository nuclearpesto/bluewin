
CFLAGS=-c -g -std=c++11 $(INC_DIR)

DEPS=*.h
BIN_DIR=../../bin/
OBJDIR=../../obj/
CC=g++
LOCALINCDIR=../../include/
OBJ_FILES= $(OBJ)clientcrypt.o $(OBJ)main.o $(OBJ)client.o
ifeq ($(OS),Windows_NT)
LIBS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_net -lSDL2_image -lSDL2_ttf -ljansson
INC_DIR=-I../../include -I/usr/include -I/usr/local/include
LIBPATHS= -L/usr/lib -L/usr/local/lib
else
UNAME_S:=$(shell uname -s)
ifeq ($(UNAME_S),Linux)
LIBS= -lSDL2 -lSDL2_net -ljansson
INC_DIR=-I../../include
endif
ifeq ($(UNAME_s),Darwin)
LIBS= -lSDL2 -lSDL2_net -ljansson
INC_DIR=-I../../include
endif
endif


all: client
client: $(OBJ_FILES)
	$(CC) -o client $(OBJ_FILES) $(INC_DIR) $(LIBPATHS) $(LIBS)


clientcrypt.o: clientcrypt.c
	$(CC) clientcrypt.c $(CFLAGS) $(INC_DIR) $(LIBPATHS) $(LIBS)

main.o: main.cpp
	$(CC) main.cpp  $(CFLAGS) $(INC_DIR) $(LIBPATHS) $(LIBS)

client.o: client.cpp
	$(CC) client.cpp  $(CFLAGS) $(INC_DIR) $(LIBPATHS) $(LIBS)
