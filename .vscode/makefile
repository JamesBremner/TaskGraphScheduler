#source file search paths
VPATH = ./src ../raven-set ../Pathfinder/src

	# compiler include search paths
INCS = -I./include \
	-I../windex/include \
	-I../raven-set \
	-I../Pathfinder/src \
	-I../../code_ext/boost/boost1_78 

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs -lComctl32 \


static_link_flags = -static-libstdc++ -static-libgcc -static

# folders
ODIR = ./.vscode/obj

OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_starter = \
	$(ODIR)/TaskGraphScheduler.o \
	$(ODIR)/cProcessor.o \
	$(ODIR)/cResultsRecord.o \
	$(ODIR)/tgs.o \
	$(ODIR)/cTask.o \
	$(ODIR)/cGraph.o \
	$(ODIR)/GraphTheory.o \
	$(ODIR)/cRunWatch.o \


### COMPILE

$(ODIR)/%.o: %.cpp tgs.h
	g++  $(CCflags) \
	-c -o $@ $< $(INCS)

starter: $(OBJ_starter)
	g++ $(OPT) \
	-o bin/starter.exe $^  \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

