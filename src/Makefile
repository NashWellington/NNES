objects =  main.o boot.o cpu.o ppu.o apu.o bus.o $(mapper_objects) isa.o mode.o savestate.o display.o audio.o input.o util.o imgui.o imgui_draw.o imgui_tables.o imgui_widgets.o imgui_impl_sdl.o imgui_impl_opengl3.o

mapper_objects = mapper000.o mapper001.o

compiler = g++

flags = -g -std=c++17 -Wall -Werror -Wextra
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

main.o : main.cpp globals.h boot.h cpu.h ppu.h apu.h display.h input.h savestate.h
		$(compiler) $(flags) -c main.cpp

boot.o : boot.h globals.h bus.h mappers/mapper.h
		$(compiler) $(flags) -c boot.cpp

cpu.o : cpu.h globals.h isa.h bus.h savestate.h
		$(compiler) $(flags) -c cpu.cpp

ppu.o : ppu.h globals.h bus.h display.h savestate.h
		$(compiler) $(flags) -c ppu.cpp

apu.o : apu.h globals.h bus.h
		$(compiler) $(flags) -c apu.cpp

isa.o : isa.h globals.h mode.h cpu.h
		$(compiler) $(flags) -c isa.cpp

mode.o : mode.h globals.h cpu.h
		$(compiler) $(flags) -c mode.cpp

bus.o : bus.h globals.h apu.h mappers/mapper.h savestate.h
		$(compiler) $(flags) -c bus.cpp

mapper000.o : mappers/mapper.h globals.h savestate.h
		$(compiler) $(flags) -c mappers/mapper000.cpp

mapper001.o : mappers/mapper.h globals.h savestate.h
		$(compiler) $(flags) -c mappers/mapper001.cpp

display.o : display.h globals.h util.h savestate.h libs/imgui/imgui.h libs/imgui/imgui_impl_sdl.h libs/imgui/imgui_impl_opengl3.h
		$(compiler) $(flags) -c display.cpp

audio.o : audio.h globals.h
		$(compiler) $(flags) -c audio.cpp

input.o : input.h globals.h display.h bus.h
		$(compiler) $(flags) -c input.cpp

savestate.o : savestate.h globals.h mappers/mapper.h
		$(compiler) $(flags) -c savestate.cpp

util.o : util.h globals.h
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