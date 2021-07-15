objects =  nnes.o boot.o cart.o console.o cpu.o ppu.o apu.o mem.o $(mapper_objects) isa.o mode.o scheduler.o savestate.o video.o audio.o input.o controller.o util.o imgui.o imgui_draw.o imgui_tables.o imgui_widgets.o imgui_impl_sdl.o imgui_impl_opengl3.o

mapper_objects = mapper000.o mapper001.o mapper002.o mapper003.o mapper007.o mapper009.o

compiler = g++

flags = -g -fsanitize=address -std=c++17 -Wall -Werror -Wextra
flags += -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align
flags += -Wunused -Woverloaded-virtual -Wconversion -Wsign-conversion
flags += -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches
flags += -Wlogical-op -Wnull-dereference -Wlogical-op -Wnull-dereference
flags += -Wuseless-cast -Wdouble-promotion -Wformat=2
flags += -I/usr/include/SDL2 -I/usr/include/freetype2

libs = -lGLEW -lGL -lGLU -lSDL2 -lfreetype

target = NNES

$(target) : $(objects)
		$(compiler) $(flags) $(objects) -o ../build/$(target) $(libs)
		rm $(objects)

nnes.o : nnes.cpp globals.h boot.h console.h audio.h video.h input.h savestate.h
		$(compiler) $(flags) -c nnes.cpp

boot.o : boot.h globals.h mappers/mapper.h
		$(compiler) $(flags) -c boot.cpp

console.o : console.h globals.h scheduler.h cpu.h ppu.h apu.h mem.h peripheral.h cart.h
		$(compiler) $(flags) -c console.cpp

processor.o : processor.h globals.h savestate.h
		$(compiler) $(flags) -c processor.cpp

cpu.o : cpu.h globals.h processor.h isa.h mem.h savestate.h
		$(compiler) $(flags) -c cpu.cpp

ppu.o : ppu.h globals.h processor.h mem.h video.h savestate.h util.h
		$(compiler) $(flags) -c ppu.cpp

apu.o : apu.h globals.h processor.h audio.h mem.h
		$(compiler) $(flags) -c apu.cpp

isa.o : isa.h globals.h mode.h cpu.h
		$(compiler) $(flags) -c isa.cpp

mode.o : mode.h globals.h cpu.h
		$(compiler) $(flags) -c mode.cpp

mem.o : mem.h globals.h console.h savestate.h
		$(compiler) $(flags) -c mem.cpp

cart.o : cart.h globals.h peripheral.h boot.h mappers/mapper.h
		$(compiler) $(flags) -c cart.cpp

mapper000.o : mappers/mapper.h globals.h savestate.h
		$(compiler) $(flags) -c mappers/mapper000.cpp

mapper001.o : mappers/mapper.h globals.h savestate.h
		$(compiler) $(flags) -c mappers/mapper001.cpp

mapper002.o : mappers/mapper.h globals.h savestate.h
		$(compiler) $(flags) -c mappers/mapper002.cpp

mapper003.o : mappers/mapper.h globals.h savestate.h
		$(compiler) $(flags) -c mappers/mapper003.cpp

#TODO figure out how to incorporate console memory
#mapper004.o : mappers/mapper.h globals.h savestate.h mem.h
#		$(compiler) $(flags) -c mappers/mapper004.cpp

mapper007.o : mappers/mapper.h globals.h savestate.h
		$(compiler) $(flags) -c mappers/mapper007.cpp

mapper009.o : mappers/mapper.h globals.h savestate.h
		$(compiler) $(flags) -c mappers/mapper009.cpp

video.o : video.h globals.h util.h savestate.h libs/imgui/imgui.h libs/imgui/imgui_impl_sdl.h libs/imgui/imgui_impl_opengl3.h
		$(compiler) $(flags) -c video.cpp

audio.o : audio.h globals.h
		$(compiler) $(flags) -c audio.cpp

input.o : input.h globals.h console.h audio.h video.h peripheral.h
		$(compiler) $(flags) -c input.cpp

controller.o : peripheral.h
		$(compiler) $(flags) -c controller.cpp

scheduler.o : scheduler.h globals.h processor.h
		$(compiler) $(flags) -c scheduler.cpp

savestate.o : savestate.h globals.h mappers/mapper.h
		$(compiler) $(flags) -c savestate.cpp

util.o : util.h globals.h mem.h
		$(compiler) $(flags) -c util.cpp

# external libs
imgui.o : libs/imgui/imgui.h libs/imgui/imconfig.h libs/imgui/imgui_internal.h
		$(compiler) $(flags) -c libs/imgui/imgui.cpp

imgui_draw.o : libs/imgui/imgui.h libs/imgui/imgui_internal.h libs/imgui/imstb_rectpack.h libs/imgui/imstb_truetype.h
		$(compiler) $(flags) -c libs/imgui/imgui_draw.cpp

imgui_tables.o : libs/imgui/imgui.h libs/imgui/imgui_internal.h
		$(compiler) $(flags) -c libs/imgui/imgui_tables.cpp

imgui_widgets.o : libs/imgui/imgui.h libs/imgui/imgui_internal.h libs/imgui/imstb_textedit.h
		$(compiler) $(flags) -c libs/imgui/imgui_widgets.cpp

imgui_impl_sdl.o : libs/imgui/imgui_impl_sdl.h libs/imgui/imgui.h
		$(compiler) $(flags) -c libs/imgui/imgui_impl_sdl.cpp

imgui_impl_opengl3.o : libs/imgui/imgui_impl_opengl3.h libs/imgui/imgui.h
		$(compiler) $(flags) -c libs/imgui/imgui_impl_opengl3.cpp

clean : 
		rm *.o
		rm ../build/$(target)