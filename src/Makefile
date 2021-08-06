objects =  nnes.o boot.o cart.o config.o console.o cpu.o ppu.o apu.o mem.o 
objects += $(mapper_objects) 
objects += isa.o scheduler.o savestate.o video.o audio.o input.o 
objects += controller.o util.o 

mapper_objects = mapper.o mapper000.o mapper001.o mapper002.o mapper003.o mapper004.o mapper007.o mapper009.o

compiler = g++

optimizations = -march=native -O3 -flto
debug = -g #-Og
profiler = -pg
leak_check = -fsanitize=address

flags = -std=c++17
flags += $(optimizations)
#flags += $(debug)
#flags += $(leak_check)
#flags += $(profiler)
flags += -Wall -Werror -Wextra
flags += -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align
flags += -Wunused -Woverloaded-virtual -Wconversion -Wsign-conversion
flags += -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches
flags += -Wlogical-op -Wnull-dereference -Wlogical-op -Wnull-dereference
flags += -Wuseless-cast -Wdouble-promotion -Wformat=2
flags += -I/usr/include/SDL2 -I/usr/include/freetype2

libs = -lGLEW -lGL -lGLU -lSDL2 -lfreetype -lpthread -lfmt

target = NNES

$(target) : $(objects)
		$(compiler) $(flags) $(objects) -o ../build/$(target) $(libs)
		rm $(objects)

nnes.o : nnes.cpp globals.hpp boot.hpp console.hpp audio.hpp video.hpp input.hpp savestate.hpp
		$(compiler) $(flags) -c nnes.cpp

boot.o : boot.hpp globals.hpp mappers/mapper.hpp
		$(compiler) $(flags) -c boot.cpp

config.o : config.hpp globals.hpp
		$(compiler) $(flags) -c config.cpp

console.o : console.hpp globals.hpp scheduler.hpp cpu.hpp ppu.hpp apu.hpp mem.hpp peripheral.hpp cart.hpp config.hpp
		$(compiler) $(flags) -c console.cpp

processor.o : processor.hpp globals.hpp savestate.hpp
		$(compiler) $(flags) -c processor.cpp

cpu.o : cpu.hpp globals.hpp processor.hpp mem.hpp savestate.hpp
		$(compiler) $(flags) -c cpu.cpp

ppu.o : ppu.hpp globals.hpp processor.hpp mem.hpp video.hpp savestate.hpp util.hpp
		$(compiler) $(flags) -c ppu.cpp

apu.o : apu.hpp globals.hpp processor.hpp audio.hpp mem.hpp
		$(compiler) $(flags) -c apu.cpp

isa.o : cpu.hpp globals.hpp
		$(compiler) $(flags) -c isa.cpp

mem.o : mem.hpp globals.hpp console.hpp savestate.hpp
		$(compiler) $(flags) -c mem.cpp

cart.o : cart.hpp globals.hpp peripheral.hpp boot.hpp mappers/mapper.hpp
		$(compiler) $(flags) -c cart.cpp

mapper.o : mappers/mapper.hpp globals.hpp savestate.hpp console.hpp
		$(compiler) $(flags) -c mappers/mapper.cpp

mapper000.o : mappers/mapper.hpp globals.hpp savestate.hpp console.hpp
		$(compiler) $(flags) -c mappers/mapper000.cpp

mapper001.o : mappers/mapper.hpp globals.hpp savestate.hpp console.hpp
		$(compiler) $(flags) -c mappers/mapper001.cpp

mapper002.o : mappers/mapper.hpp globals.hpp savestate.hpp console.hpp
		$(compiler) $(flags) -c mappers/mapper002.cpp

mapper003.o : mappers/mapper.hpp globals.hpp savestate.hpp console.hpp
		$(compiler) $(flags) -c mappers/mapper003.cpp

mapper004.o : mappers/mapper.hpp globals.hpp savestate.hpp console.hpp
		$(compiler) $(flags) -c mappers/mapper004.cpp

mapper007.o : mappers/mapper.hpp globals.hpp savestate.hpp console.hpp
		$(compiler) $(flags) -c mappers/mapper007.cpp

mapper009.o : mappers/mapper.hpp globals.hpp savestate.hpp console.hpp
		$(compiler) $(flags) -c mappers/mapper009.cpp

video.o : video.hpp globals.hpp util.hpp savestate.hpp
		$(compiler) $(flags) -c video.cpp

audio.o : audio.hpp globals.hpp
		$(compiler) $(flags) -c audio.cpp

input.o : input.hpp globals.hpp console.hpp audio.hpp video.hpp peripheral.hpp
		$(compiler) $(flags) -c input.cpp

controller.o : peripheral.hpp
		$(compiler) $(flags) -c controller.cpp

scheduler.o : scheduler.hpp globals.hpp processor.hpp
		$(compiler) $(flags) -c scheduler.cpp

savestate.o : savestate.hpp globals.hpp mappers/mapper.hpp
		$(compiler) $(flags) -c savestate.cpp

util.o : util.hpp globals.hpp mem.hpp
		$(compiler) $(flags) -c util.cpp

clean : 
		rm *.o
		rm ../build/$(target)