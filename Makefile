CC = gcc
CFLAGS = -g -I/usr/local/lib
CPPFLAGS = -O2 -Wall -pedantic -Wextra

OBJ_DIR=build/obj
LIB_DIR=build/lib
BIN_DIR=build/bin

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRCS))

TAGLIB = /usr/local/lib/
#libtag_c.a
TAGINCLUDE = /usr/local/include/taglib/

LIB = ../JLib/$(LIB_DIR)
LIB_INCLUDE = ../JLib/src

all: main

clean:
	rm -rf build main lib.db

# directories
$(OBJ_DIR) $(LIB_DIR) $(BIN_DIR):
	mkdir -p $@

# compile obj files
$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -Isrc -I$(LIB_INCLUDE) -I$(TAGINCLUDE) $< -o $@


# CREATE TEST FILES ####################################################
$(BIN_DIR)/main: $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -Isrc -I$(LIB_INCLUDE) $^ \
	-L$(LIB) \
	-L$(TAGLIB) \
	-L/usr/local/lib \
	-lJHELPER \
	-lJHASHMAP \
	-lJVEC \
	-ltag_c \
	-ltag \
	-lsqlite3 \
	-lm \
	-lstdc++ \
	-o $@


main: $(BIN_DIR)/main | $(BIN_DIR)
	ln -sf  $< $a