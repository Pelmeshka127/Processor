all: create_res_file

create_res_file: main.o text_functions.o
	g++ main.o text_functions.o -g -o onegin

main.o: main.cpp
	g++ main.cpp -c -g -o main.o 

text_functions.o: text_functions.cpp 
	g++ text_functions.cpp -c -g -o text_functions.o

.PHONY: clean

clean:
	rm *.o onegin
