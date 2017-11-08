CC = g++
CXXFLAGS = -O2 -Wall

SRCDIR = .
#LDFLAGS = -lglut -lGLU -lGL -lm
LDFLAGS = -lglfw3 -lGLEW -lXinerama -lXcursor -lrt `pkg-config --static --libs x11 xrandr xi xxf86vm gl`

SOURCES = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))

#OBJS = $(SOURCES:%.c=%.o)
OBJS = main.o glsl.o Sprite2D.o

TARGET = hoge

$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o $@


all: clean $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)


