
# SRC=a.cpp
# OUTPUT=run

# CFLAG = -lstdc++

# $(OUTPUT): $(SRC)
# 	gcc $^ -o $@ $(CFLAG)

# clean:
# 	rm $(OUTPUT)

.PHONY : rebuild clean
 
 
 
INC_DIR =
 
LIB_DIR =
 
 
 
CC = gcc
 
CFLAGS = -g -Wall -std=c++11
 
LDFLAGS = -lstdc++
 
 
 
SRC = b.cpp socketutils.cpp UdpSocket.cpp
 
TARGET = run_me
 
 
 
$(TARGET) : $(SRC)

	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
 
	#$(CC) $(CFLAGS) -I$(INC_DIR) -L$(LIB_DIR) $(LDFLAGS)  $^ -o $@
 
 
 
rebuild :
 
	make clean
 
	make
 
 
 
clean :
 
	rm -f $(TARGET)

	rm -rf $(TARGET).dSYM