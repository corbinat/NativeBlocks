DEBUG = -O0 -g -DDEBUG_PRINTS

# SFML library
SFMLDIR = ../External/SFML-2.0-rc
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


all: Beans

Beans: main.o cPlayer.o cHumanPlayer.o cAiPlayer.o cAiPersonality.o cBean.o cBeanInfo.o cStaging.o cStagingObserver.o cFloor.o cWall.o cRoof.o cCountdownStart.o cFinalMenu.o cBeanLevel.o cTitle.o cGrass.o cCloud.o cTextBox.o cButton.o cSelectionBox.o cMainMenu.o cMainMenuLevel.o
	g++ ${CFLAGS} -o main main.o cPlayer.o cHumanPlayer.o cAiPlayer.o cAiPersonality.o cBean.o cBeanInfo.o cStaging.o cStagingObserver.o cFloor.o cWall.o cRoof.o cCountdownStart.o cFinalMenu.o cBeanLevel.o cTitle.o cGrass.o cCloud.o cTextBox.o cButton.o cSelectionBox.o cMainMenu.o cMainMenuLevel.o ${EngineOBJ} ${LDFLAGS}

main.o: ${SourceDir}/main.cpp
	g++ ${CFLAGS} -c ${SourceDir}/main.cpp ${LDFLAGS}

cPlayer.o: ${SourceDir}/cPlayer.cpp ${SourceDir}/cPlayer.h cStaging.o
	g++ ${CFLAGS} -c ${SourceDir}/cPlayer.cpp ${LDFLAGS}

cHumanPlayer.o: ${SourceDir}/cHumanPlayer.cpp ${SourceDir}/cHumanPlayer.h cPlayer.o
	g++ ${CFLAGS} -c ${SourceDir}/cHumanPlayer.cpp ${LDFLAGS}

cAiPlayer.o: ${SourceDir}/cAiPlayer.cpp ${SourceDir}/cAiPlayer.h cPlayer.o cAiPersonality.o
	g++ ${CFLAGS} -c ${SourceDir}/cAiPlayer.cpp ${LDFLAGS}

cAiPersonality.o: ${SourceDir}/cAiPersonality.cpp ${SourceDir}/cAiPersonality.h cPlayer.o
	g++ ${CFLAGS} -c ${SourceDir}/cAiPersonality.cpp ${LDFLAGS}

cBean.o: ${SourceDir}/cBean.cpp ${SourceDir}/cBean.h
	g++ ${CFLAGS} -c ${SourceDir}/cBean.cpp ${LDFLAGS}

cBeanInfo.o: ${SourceDir}/cBeanInfo.cpp ${SourceDir}/cBeanInfo.h
	g++ ${CFLAGS} -c ${SourceDir}/cBeanInfo.cpp ${LDFLAGS}

cStaging.o: ${SourceDir}/cStaging.cpp ${SourceDir}/cStaging.h
	g++ ${CFLAGS} -c ${SourceDir}/cStaging.cpp ${LDFLAGS}

cStagingObserver.o: ${SourceDir}/cStagingObserver.cpp ${SourceDir}/cStagingObserver.h cStaging.o
	g++ ${CFLAGS} -c ${SourceDir}/cStagingObserver.cpp ${LDFLAGS}

cFloor.o: ${SourceDir}/cFloor.cpp ${SourceDir}/cFloor.h
	g++ ${CFLAGS} -c ${SourceDir}/cFloor.cpp ${LDFLAGS}

cWall.o: ${SourceDir}/cWall.cpp ${SourceDir}/cWall.h
	g++ ${CFLAGS} -c ${SourceDir}/cWall.cpp ${LDFLAGS}

cRoof.o: ${SourceDir}/cRoof.cpp ${SourceDir}/cRoof.h
	g++ ${CFLAGS} -c ${SourceDir}/cRoof.cpp ${LDFLAGS}

cCountdownStart.o: ${SourceDir}/cCountdownStart.cpp ${SourceDir}/cCountdownStart.h
	g++ ${CFLAGS} -c ${SourceDir}/cCountdownStart.cpp ${LDFLAGS}

cFinalMenu.o: ${SourceDir}/cFinalMenu.cpp ${SourceDir}/cFinalMenu.h cButton.o
	g++ ${CFLAGS} -c ${SourceDir}/cFinalMenu.cpp ${LDFLAGS}

cBeanLevel.o: ${SourceDir}/cBeanLevel.cpp ${SourceDir}/cBeanLevel.h
	g++ ${CFLAGS} -c ${SourceDir}/cBeanLevel.cpp ${LDFLAGS}

cTitle.o: ${SourceDir}/MainMenu/cTitle.cpp ${SourceDir}/MainMenu/cTitle.h
	g++ ${CFLAGS} -c ${SourceDir}/MainMenu/cTitle.cpp ${LDFLAGS}

cGrass.o: ${SourceDir}/Props/cGrass.cpp ${SourceDir}/Props/cGrass.h
	g++ ${CFLAGS} -c ${SourceDir}/Props/cGrass.cpp ${LDFLAGS}

cCloud.o: ${SourceDir}/Props/cCloud.cpp ${SourceDir}/Props/cCloud.h
	g++ ${CFLAGS} -c ${SourceDir}/Props/cCloud.cpp ${LDFLAGS}

cTextBox.o: ${SourceDir}/MainMenu/cTextBox.cpp ${SourceDir}/MainMenu/cTextBox.h
	g++ ${CFLAGS} -c ${SourceDir}/MainMenu/cTextBox.cpp ${LDFLAGS}

cButton.o: ${SourceDir}/MainMenu/cButton.cpp ${SourceDir}/MainMenu/cButton.h
	g++ ${CFLAGS} -c ${SourceDir}/MainMenu/cButton.cpp ${LDFLAGS}

cSelectionBox.o: ${SourceDir}/MainMenu/cSelectionBox.cpp ${SourceDir}/MainMenu/cSelectionBox.h cButton.o cTextBox.o
	g++ ${CFLAGS} -c ${SourceDir}/MainMenu/cSelectionBox.cpp ${LDFLAGS}

cMainMenu.o: ${SourceDir}/MainMenu/cMainMenu.cpp ${SourceDir}/MainMenu/cMainMenu.h cSelectionBox.o
	g++ ${CFLAGS} -c ${SourceDir}/MainMenu/cMainMenu.cpp ${LDFLAGS}

cMainMenuLevel.o: ${SourceDir}/cMainMenuLevel.cpp ${SourceDir}/cMainMenuLevel.h
	g++ ${CFLAGS} -c ${SourceDir}/cMainMenuLevel.cpp ${LDFLAGS}

clean:
	rm -f *.o *.a main

