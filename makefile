Gpp = g++
DEBUG = -O0 -g -DDEBUG_PRINTS -Wall -Werror
#DEBUG = -O2

# SFML library
SFMLDIR = ../External/SFML-2.3.1
SFMLLIB = ${SFMLDIR}/lib
SFMLINC = ${SFMLDIR}/include

# 2d Engine
EngineDIR = NativeBlocks/2dEngine
EngineINC = ${EngineDIR}

CFLAGS = -I${SFMLINC}
CFLAGS += -INativeBlocks/Source
CFLAGS += -I${EngineINC}
CFLAGS += ${DEBUG} -std=c++11
LDFLAGS = -L${SFMLLIB} -lsfml-system -lsfml-window -lsfml-graphics -lsfml-audio -lpthread -Wl,-Bstatic -lphysfs -lz -Wl,-Bdynamic -Wl,-rpath,Lib/

SourceDir = Source

srcs = $(wildcard NativeBlocks/Source/*.cpp)
srcs += $(wildcard NativeBlocks/2dEngine/*.cpp)
srcs += $(wildcard NativeBlocks/Source/MainMenu/*.cpp)
srcs += $(wildcard NativeBlocks/Source/Common/*.cpp)
srcs += $(wildcard NativeBlocks/Source/Common/Props/*.cpp)
srcs += $(wildcard NativeBlocks/Source/Common/Widgets/*.cpp)
srcs += $(wildcard NativeBlocks/Source/ChallengeLevel/*.cpp)
srcs += $(wildcard NativeBlocks/Source/HighScores/*.cpp)
srcs += $(wildcard NativeBlocks/Source/Credits/*.cpp)

objs = $(srcs:.cpp=.o)
deps = $(srcs:.cpp=.d)

main: $(objs)
	$(Gpp) ${CFLAGS} $^ -o $@ ${EngineOBJ} ${LDFLAGS}

%.o: %.cpp
	$(Gpp) ${CFLAGS} -MMD -MP -c $< -o $@

.PHONY: clean

# $(RM) is rm -f by default
clean:
	$(RM) $(objs) $(deps) main

#///////////////////////////////
# Export
#//////////////////////////////
.PHONY: export

export:
	# zip media folder
	(cd NativeBlocks/ && zip Media.pak -r Media.zip Media/)
	# copy files to export folder
	mkdir -p Export/ && cp main Export/NativeBlocks
	mkdir -p Export/Media && mv NativeBlocks/Media.pak Export/Media
	mkdir -p Export/Lib && cp -r ${SFMLLIB}/. Export/Lib
	# Create the config folder
	mkdir -p Export/Config

-include $(deps)
