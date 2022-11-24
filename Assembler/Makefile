all: asmtask

FLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++\
 -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations\
  -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported\
   -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal\
    -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2\
	 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual\
	  -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow\
	   -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2\
	    -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types\
		 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef\
		  -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros\
		   -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing\
		    -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new\
			 -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer\
			  -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE\
			   -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,\
			   -fsanitize=float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,\
			   -fsanitize=object-size,return,returns-nonnull-attribute,shift,\
			   -fsanitize=address,signed-integer-overflow,undefined,unreachable,vla-bound,vptr,

asmtask: main.o assembler.o
	g++ ./asm_obj/main.o ./asm_obj/assembler.o $(FLAGS) -o assembly

main.o: ./asm_src/main.cpp
	g++ -c asm_src/main.cpp $(FLAGS) -o asm_obj/main.o

assembler.o: ./asm_src/assembler.cpp
	g++ -c asm_src/assembler.cpp $(FLAGS) -o asm_obj/assembler.o

.PHONY: clean

clean:
	rm *.o assembly