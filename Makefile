SRCDIR = src
OBJDIR = bin
SOURCES = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(addsuffix .o,$(basename $(SOURCES))))
BINARY = see
DEPFILE = dependencies.d
COMMON_LFLAGS = -lglfw -Llibs/glfw-2.7.2/lib/x11 -lGLEW -lpnglite -lz -pg -lBulletDynamics -lBulletCollision -lLinearMath -lopenal `pkg-config --libs lua5.1`
COMMON_CFLAGS = -D _USE_MATH_DEFINES=1 -I/usr/include/bullet `pkg-config --cflags lua5.1` -pg -Werror

.PHONY: default osx linux
default:
	@echo "make [osx | linux]"

osx:
	make -s depend ARCH_DEFINES=-D_OSX
	make $(BINARY) "LFLAGS = $(shell sdl-config --libs) -framework OpenGL -L/opt/local/lib $(COMMON_LFLAGS)" "CFLAGS = $(COMMON_CFLAGS) -g -std=c++98 -D_OSX $(shell sdl-config --cflags) -Ilibs/Box2D/Box2D" 

linux:
	make -s depend
	make $(BINARY) "LFLAGS = $(shell sdl-config --libs) -lGL -lGLU -pg $(COMMON_LFLAGS)" "CFLAGS = $(COMMON_CFLAGS) $(shell sdl-config --cflags) -g -pg -DGL_GLEXT_PROTOTYPES" 

docs:
	doxygen doxconf

$(BINARY): $(OBJDIR) $(OBJECTS)
	$(CXX) -o $(BINARY) $(OBJECTS) $(LFLAGS) 

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -std=c++0x -I src -I libs/glfw-2.7.2/include -o $@ -c $<

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I src -I libs/glfw-2.7.2/include -o $@ -c $<

clean:
	rm -rf $(OBJDIR)
	rm -f $(BINARY)
	rm -f $(DEPFILE)

.PHONY: depend
depend:
	$(CXX) -MM -Ilibs/glfw-2.7.2/include $(COMMON_CFLAGS) $(shell sdl-config --cflags) $(ARCH_DEFINES) $(SOURCES) | \
	sed 's,^\([^/]*\.o\) *:,$(OBJDIR)/\1: ,' > $(DEPFILE)

-include $(DEPFILE)

