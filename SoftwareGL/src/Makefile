CC = g++
CFLAGS = -std=c++11 -c -Wall
LDFLAGS =
BIN = ../bin
DIRS = ../bin
SRCS = main.cpp tga.cpp model.cpp
SRCDIR = src
INCDIR = -I ../include
OBJS = $(SRCS:.cpp=.o)
EXEC = softgl.bin
MKDIR_P = mkdir -p
RM = rm

all: @echo=off directories $(SRCS) $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(addprefix $(BIN)/, $(OBJS)) $(INCDIR) -o ./$(BIN)/$@ 

.cpp.o: $(BIN)
	$(CC) $(CFLAGS) $(INCDIR) $< -o ./$(BIN)/$@ 

directories:
	$(MKDIR_P) $(DIRS)
    

clean:
	@echo=off $(RM) ./$(BIN)/*.bin ./$(BIN)/*.o;
	

# all: 
# 	g++ --std=c++11 main.cpp tga.cpp -I ./ -o softgl
