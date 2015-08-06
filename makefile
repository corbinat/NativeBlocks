Gpp = g++
#DEBUG = -O0 -g -DDEBUG_PRINTS -Wall -Werror
DEBUG = -O2

# SFML library
SFMLDIR = ../External/SFML-2.3.1
SFMLLIB = ${SFMLDIR}/lib
SFMLINC = ${SFMLDIR}/include

# 2d Engine
EngineDIR = ../2dEngine
EngineOBJ = ${EngineDIR}/*.o
EngineINC = ${EngineDIR}

CFLAGS = -I${SFMLINC}
CFLAGS += -ISource
CFLAGS += -I${EngineINC}
CFLAGS += ${DEBUG} -std=c++11
LDFLAGS = -L${SFMLLIB} -lsfml-system -lsfml-window -lsfml-graphics -lsfml-audio -lpthread -Wl,-Bstatic -lphysfs -lz -Wl,-Bdynamic -Wl,-rpath,Lib/

SourceDir = Source

srcs = $(wildcard Source/*.cpp)
srcs += $(wildcard Source/MainMenu/*.cpp)
srcs += $(wildcard Source/Common/*.cpp)
srcs += $(wildcard Source/Common/Props/*.cpp)
srcs += $(wildcard Source/Common/Widgets/*.cpp)
srcs += $(wildcard Source/ChallengeLevel/*.cpp)
srcs += $(wildcard Source/HighScores/*.cpp)
srcs += $(wildcard Source/Credits/*.cpp)

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
	zip Media.pak -r Media.zip Media/
	# copy files to export folder
	mkdir -p Export/Data/ && cp main Export/Data/NativeBlocks
	mkdir -p Export/Data/Media && mv Media.pak Export/Data/Media
	mkdir -p Export/Data/Lib && cp -r ${SFMLLIB}/. Export/Data/Lib
	#Create launcher script
	echo "cd Data" > NativeBlocks.sh
	echo "export LD_LIBRARY_PATH=Lib/" >> NativeBlocks.sh
	echo "./NativeBlocks" >> NativeBlocks.sh
	chmod +x NativeBlocks.sh

	mv NativeBlocks.sh Export/

-include $(deps)
