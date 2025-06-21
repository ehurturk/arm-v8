.PHONY: clean lib

CC       = gcc
CFLAGS   = -Wall -Wextra -g  -pedantic    # warnings, debug, *optimisation*
CPPFLAGS = -I.. -I../.. -I../../utils -MMD -MP # search path + dependency flags 

SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
DEP := $(OBJ:.o=.d)

AR      = ar
ARFLAGS = -rcs
LIB     = libexecute.a

lib: $(LIB)

$(LIB): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^	

# adds more dependencies
-include $(DEP)

# %.o: %.c
# 	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(DEP) $(LIB)
