# Compiler:
CC = g++-4.9

# Optimization:
CFLAGS = -O3

# Check OS
UNAME := $(shell uname)

# Libs and Frameworks:
ifeq ($(UNAME), Linux)
	FRAMEWORKS = -lGLEW -lglfw3 -lGL -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl -std=c++11
else
	FRAMEWORKS = -lglew -lglfw3 -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -framework Opengl -framework Cocoa -framework IOKit -framework CoreVideo -std=c++11	
endif



# Location for libs:
LIBFOLD = -L"/usr/local/lib"
# Location for includes:
INCFOLD = -I"/usr/local/include" -I"/usr/local/include/bullet" 

# Files:
FILES = $(wildcard src/*.cpp) $(wildcard src/tools/*.cpp) $(wildcard src/utils/*.cpp)

# Binary folder:
BINFOLD = bin/

# Binary name:
BINNAME = voronoifracture


all: compile
.PHONY: all

compile: $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o $(BINFOLD)$(BINNAME) $(LIBFOLD) $(INCFOLD) $(FRAMEWORKS)
.PHONY: compile

run:
	./$(BINFOLD)$(BINNAME)
.PHONY: run

clean:
	rm -f $(BINFOLD)*
.PHONY: clean
