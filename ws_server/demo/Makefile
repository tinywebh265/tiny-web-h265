CC = gcc
AR = ar

CFLAGS := -o2 -Wall -I./../include
TARGET = streamapp

OBJS :=  main.o avfilestream.o

.PHONY : clean all

default: $(OBJS)                          
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) -L./../libs/gcc -lwsserver -lpthread -lm 
	
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)
	