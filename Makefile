all: Pegasus

ComponentManager.o: ComponentManager.cpp ComponentManager.h Component.h
EntityManager.o: EntityManager.cpp EntityManager.h

.cpp.o:
	g++ -std=c++11 -c -O3 -Wall $<

Pegasus: ComponentManager.o EntityManager.o
	g++ -O3 -o $@ $^ 

clean:
	rm -f Pegasus *.o *.a