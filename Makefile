LIB=libedgpio.a
OBJ=ds1307.o mcp23017.o i2c.o
INC=i2c.h edgpio.h
AR=ar
ARFLAGS=rvs
GCC=gcc
GCCFLAGS=

$(LIB): $(OBJ)
	$(AR) $(ARFLAGS) $(LIB) $(OBJ)

%.o: %.c $(INC)
	$(GCC) -c -o $@ $< $(GCCFLAGS)

clean:
	rm -f $(LIB)
	rm -f $(OBJ)
