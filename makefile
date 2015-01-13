Gpp = g++
DEBUG = -O0 -g -DDEBUG_PRINTS
#DEBUG = -O3

# SFML library
SFMLDIR = ../External/SFML-2.2
SFMLLIB = ${SFMLDIR}/lib
SFMLINC = ${SFMLDIR}/include

# 2d Engine
EngineDIR = ../2dEngine
EngineOBJ = ${EngineDIR}/*.o
EngineINC = ${EngineDIR}

CFLAGS = -I${SFMLINC}
CFLAGS += -I${EngineINC}
CFLAGS += ${DEBUG} -std=c++11
LDFLAGS = -L${SFMLLIB} -lsfml-system -lsfml-window -lsfml-graphics -lpthread

SourceDir = Source

srcs = $(wildcard Source/*.cpp)
srcs += $(wildcard Source/MainMenu/*.cpp)
srcs += $(wildcard Source/Props/*.cpp)

objs = $(srcs:.cpp=.o)
deps = $(srcs:.cpp=.d)

main: $(objs)
	$(Gpp) ${CFLAGS} $^ -o $@ ${EngineOBJ} ${LDFLAGS} ${LDFLAGS}

%.o: %.cpp
	$(Gpp) ${CFLAGS} -MMD -MP -c $< -o $@ ${LDFLAGS}

.PHONY: clean

# $(RM) is rm -f by default
clean:
	$(RM) $(objs) $(deps) main

-include $(deps)
