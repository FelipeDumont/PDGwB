PROJECT = PDGwB
CFLAGS = -Wall -g -O3
OBJECTS = Main.o

all: $(PROJECT)

.cpp.o:
	g++ -c $(CFLAGS) $<

$(PROJECT): $(OBJECTS)
	g++ $(CFLAGS) -o $(PROJECT) $(OBJECTS)
	
Main.o: Main.cpp
	g++ $(CFLAGS) -c Main.cpp
	
clean:
	rm $(OBJECTS) $(PROJECT)
