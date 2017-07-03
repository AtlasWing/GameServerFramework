CC=g++ 
FLAGS=-W -O3 -Dlinux

INCLUDES=-I../../include

LIBS= -L../../libforsuse64 -litc_client -lz -lpthread -ldl -lm -lrt

TARGET=cdemo

OBJS=$(patsubst %.cpp,obj/%.o,$(wildcard *.cpp))

obj/%.o: %.cpp
	$(CC) $(INCLUDES) $(FLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(FLAGS) -o $(TARGET) $(OBJS) $(LIBS)
	cp $(TARGET) ../bin

clean:
	-rm obj/*.o