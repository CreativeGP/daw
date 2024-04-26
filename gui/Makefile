SRCS := fps_counter.cpp main_window.cpp piano_roll.cpp window.cpp project_view.cpp collision_detector.cpp generator.cpp
OBJS := $(patsubst %.cpp, build/%.o, $(SRCS))


target: $(OBJS)
	g++ $(OBJS) gui.cpp ../libremidi/build/liblibremidi.a -o gui -lSDL2 -lSDL2_ttf -lSDL2_gfx  -g3 -std=c++2a

build/%.o: %.cpp
	g++ $*.cpp -c -lSDL2 -lSDL2_ttf -lSDL2_gfx  ../libremidi/build/liblibremidi.a -g3 -std=c++2a
	mv $*.o build/$*.o

clear:
	rm *.o build/*.o