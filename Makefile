SRCDIR = src
OBJDIR = bin
SOURCES = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(addsuffix .o,$(basename $(SOURCES))))
BINARY = see
DEPFILE = dependencies.d
COMMON_LFLAGS = -lglfw -lGLEW -pg -lBulletDynamics -lBulletCollision -lLinearMath
COMMON_CFLAGS = -D _USE_MATH_DEFINES=1 -I/usr/include/bullet -pg -Werror
LINUX_LFLAGS = `pkg-config --libs lua5.1` -Llibs/glfw-2.7.2/lib/x11 -lopenal -Llibs/glfw-2.7.2/lib/x11
LINUX_CFLAGS = `pkg-config --cflags lua5.1`

.PHONY: default osx linux
default:
	@echo "make [osx | linux]"

osx:
	make -s depend ARCH_DEFINES=-D_OSX
	make $(BINARY) "LFLAGS = -framework OpenAL -framework OpenGL -framework Carbon -framework Foundation -framework AppKit -llua $(COMMON_LFLAGS)" "CFLAGS = $(COMMON_CFLAGS) -g -D_OSX -I/usr/local/Cellar/bullet/2.79/include/bullet" 

linux:
	make -s depend
	make $(BINARY) "LFLAGS = -lGL -lGLU -pg $(COMMON_LFLAGS) $(LINUX_LFLAGS)" "CFLAGS = $(COMMON_CFLAGS) $(LINUX_CFLAGS) -g -pg -DGL_GLEXT_PROTOTYPES" 

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
	$(CXX) -MM -Ilibs/glfw-2.7.2/include $(COMMON_CFLAGS) $(ARCH_DEFINES) $(SOURCES) | \
	sed 's,^\([^/]*\.o\) *:,$(OBJDIR)/\1: ,' > $(DEPFILE)

-include $(DEPFILE)

