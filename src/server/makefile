
CC=gcc
LIBS=-lpthread -ljson-c
INC_DIR=../../include
CFLAGS=-c -I$(INC_DIR)
DEPS=*.h
BIN_DIR=../../bin/
OBJDIR=../../obj/

$(BIN_DIR)server : $(OBJ_DIR)server.o $(OBJ_DIR)clienthandler.o
	$(CC) $(CLFAGS) $(LIBS) -o server server.o clienthandler.o 

$(OBJ_DIR)server.o : server.c $(INC_DIR)/$(DEPS)
	$(CC) $(CFLAGS) $(LIBS)  server.c


$(OBJ_DIR)clienthandler.o : clienthandler.c  $(INC_DIR)/$(DEPS)
	$(CC) $(CFLAGS) $(LIBS) clienthandler.c 

clean:
	rm *o server
