CFLAGS=-g -Wall -Wextra -I.

LIBS=-ldl 

get_objs = $(addsuffix .o,$(basename $(wildcard $(1))))

SOURCES=$(wildcard *.cpp) 

OBJECTS=$(patsubst %.cpp,%.o,${SOURCES})

LIB_SRC=$(filter-out main.cpp,${SOURCES})

LIB_OBJ=$(filter-out main.o,${OBJECTS})

DEPS=$(patsubst %.cpp, %.d,${SOURCES})


all: ${OBJECTS}
	g++ $(CFLAGS) ${OBJECTS} -o main 

clean:
	rm -f *.o 	
	rm -f *.d
	rm -f main	
	rm -f core.*

%.d: %.cpp
	g++ -MM -MD $<

%.o: %.cpp		
	g++ $(CFLAGS) -c $< -o $@


-include $(DEPS)


