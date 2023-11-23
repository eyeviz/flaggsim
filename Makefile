# Makefile
SHELL =         /bin/sh
CXX =		c++
MAKE =		make
####################
ARCH = $(shell uname -m)
ifeq ($(ARCH),arm64)
     PREFIX = /opt/homebrew
else
     PREFIX = /usr/local
endif     
##########
# -Ofast
CFLAGS =	-Ofast -Wno-deprecated -Wno-\#pragma-messages \
      		-Wno-format -Wno-parentheses \
		-Wno-reorder-ctor -Wno-char-subscripts \
		-stdlib=libc++ -std=c++14
##########
#MACDEFS =	-D__MAC__ -DIL_STD
MACDEFS =	-D__MAC__
##########
MACINC =	-I$(PREFIX)/include \
		-I./include \
		`pkg-config --cflags flann` \
		`pkg-config --cflags opencv4` \
		`pkg-config --cflags gmp` \
	    				     
##########
MACLIBS =	-L$(PREFIX)/lib \
		`pkg-config --libs flann` \
		`pkg-config --libs opencv4` \
		`pkg-config --libs gmp` \
       		-lfltk_gl -lfltk \
		-framework OpenGL -framework GLUT \
		-lgmp \
		-lm -lpthread

##########
SRC =           \
		Set.cpp \
		Expansion.cpp \
		Linkage.cpp \
		Dijkstra.cpp \
		Triangulate.cpp \
		Votes.cpp \
		Outline.cpp \
		\
		Concave.cpp \
		\
		CSVIO.cpp \
		\
		Spatial.cpp \
		Contour.cpp \
		Drawing.cpp \
		Workspace.cpp \
		Adjuster.cpp \
		\
		GLMacro.cpp \
    		GLDrawing.cpp \
		GLLayout.cpp \
		\
		FLSliderInput.cpp \
		FLControl.cpp \
		\
		main.cpp \
		\
		GCoptimization.cpp \
		LinkedBlockList.cpp \
		graphcut.cpp \
			 
HDR =	        \
    		Common.h \
    		CGAL.h \
		Config.h \
		\
		Network.h \
		Directed.h \
		Set.h \
		Expansion.h \
		Linkage.h \
		Dijkstra.h \
		Triangulate.h \
		Votes.h \
		Outline.h \
		\
		Concave.h \
		\
		CSVIO.h \
		\
		Spatial.h \
		Contour.h \
		Drawing.h \
		Workspace.h \
		Adjuster.h \
		\
		GLBase.h \
    		GLDrawing.h \
		GLLayout.h \
		\
		FLSliderInput.h \
		FLControl.h \
		\
    		GCoptimization.h \
		LinkedBlockList.h \
		graphcut.h \
		block.h \
		energy.h \
		graph.h	\

OBJ =		$(SRC:.cpp=.o)
BIN =		flaggsim
#
.cpp.o	:	$(SRC) $(HDR)
	$(CXX) $(DEFS) $(CFLAGS) $(INC) -c $<

mac	:
	make $(OBJ) "DEFS = $(MACDEFS)" "INC = $(MACINC)"
	$(CXX) -o $(BIN) $(MACDEFS) $(MACLINK) $(OBJ) $(MACLIBS)

depend :
	-if [ -f Make-depend ] ; \
       	   then rm -f      Make-depend ; \
	fi
	$(CXX) -MM $(MACDEFS) $(CFLAGS) $(MACINC) $(SRC) > Make-depend 

clean:
	rm -f *.o *~* core $(BIN)

#
include Make-depend
#
