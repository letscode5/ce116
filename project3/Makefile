project3: project3.o objdb.o
	gcc -Wall -g project3.o objdb.o -o project3
	
project3.o: project3.c objdb.h
	gcc -Wall -g -c project3.c
	
objdb.o: objdb.c objdb.h
	gcc -Wall -g -c objdb.c
.PHONY: clean
clean:
	rm *.o
