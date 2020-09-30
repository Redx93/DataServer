CC = gcc
LD = gcc


SRC_DIR = src
INC_DIR = include
OBJ_DIR = objects
LIB_DIR = lib

PROG = db_server
LIB = request

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
RM = /bin/rm
CFLAGS = -I$(INC_DIR) -I$(LIB_DIR) -g
LIBFLAGS = -L$(LIB_DIR) -l$(LIB) 

#all: $(PROG)

$(PROG): $(OBJS)
	$(LD) -o $@ $^ $(CFLAGS) $(LIBFLAGS)

#$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
#	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) $(PROG) $(OBJS)