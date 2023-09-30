SRCS := fps_counter.cpp main_window.cpp piano_roll.cpp window.cpp project_view.cpp
OBJS := $(patsubst %.cpp, build/%.o, $(SRCS))


target: $(OBJS)
	g++ $(OBJS) gui.cpp -o gui -lSDL2 -lSDL2_ttf -lSDL2_gfx -g3

build/%.o: %.cpp
	g++ $*.cpp -c -lSDL2 -lSDL2_ttf -lSDL2_gfx -g3
	mv $*.o build/$*.o

clear:
	rm *.o build/*.o