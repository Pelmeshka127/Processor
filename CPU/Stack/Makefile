all: stacktask

FLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++\
 -Wc++14-compat -Wmissing-declarations\
  -Wcast-align -Wcast-qual -Wchar-subscripts\
   -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal\
    -Wformat-nonliteral -Wformat-security -Wformat=2\
	 -Winline -Wlong-long -Wnon-virtual-dtor -Wopenmp\
	 -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow\
	   -Wsign-conversion -Wsign-promo -Wstrict-overflow=2\
		 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef\
		  -Wunreachable-code -Wunused -Wvariadic-macros\
		   -Wno-literal-range -Wno-missing-field-initializers -Wno-narrowing\
		    -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new\
			 -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer\
			  -Wlarger-than=8192 -pie -fPIE\
			   -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,\
			   -fsanitize=float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,\
			   -fsanitize=object-size,return,returns-nonnull-attribute,shift,\
			   -fsanitize=address,signed-integer-overflow,undefined,unreachable,vla-bound,vptr,

stacktask: main.o stack.o debug.o log.o
	g++ ./obj/main.o ./obj/stack.o ./obj/debug.o ./obj/log.o $(FLAGS) -o stack

main.o: ./src/main.cpp
	g++ -c ./src/main.cpp $(FLAGS) -o ./obj/main.o

stack.o: ./src/stack.cpp
	g++ -c ./src/stack.cpp $(FLAGS) -o ./obj/stack.o

debug.o: ./src/debug.cpp
	g++ -c ./src/debug.cpp $(FLAGS) -o ./obj/debug.o

log.o: ./src/log.cpp
	g++ -c ./src/log.cpp $(FLAGS)	-o ./obj/log.o

.PHONY: clean

clean:
	rm *.o stackj