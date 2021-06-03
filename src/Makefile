objects =  main.o boot.o cpu.o ppu.o apu.o bus.o mapper.o isa.o mode.o overloads.o display.o

flags = -g -std=c++17 -Wall -Werror -Wextra

libs = -lGLEW -lsfml-window -lsfml-system -lGL

project = NNES

$(project) : $(objects)
		g++ $(flags) $(objects) -o ../build/$(project) $(libs)
		rm $(objects)

main.o : main.cpp globals.h boot.h cpu.h ppu.h apu.h display.h
		g++ $(flags) -c main.cpp

boot.o : boot.h globals.h bus.h mapper.h
		g++ $(flags) -c boot.cpp

cpu.o : cpu.h globals.h isa.h bus.h
		g++ $(flags) -c cpu.cpp

ppu.o : ppu.h globals.h bus.h display.h
		g++ $(flags) -c ppu.cpp

apu.o : apu.h globals.h bus.h
		g++ $(flags) -c apu.cpp

isa.o : isa.h globals.h mode.h cpu.h
		g++ $(flags) -c isa.cpp

mode.o : mode.h globals.h cpu.h
		g++ $(flags) -c mode.cpp

bus.o : bus.h globals.h mapper.h
		g++ $(flags) -c bus.cpp

mapper.o : mapper.h globals.h
		g++ $(flags) -c mapper.cpp

display.o : display.h globals.h
		g++ $(flags) -c display.cpp

overloads.o : globals.h
		g++ $(flags) -c overloads.cpp

clean : 
		rm *.o
		rm ../build/$(project)