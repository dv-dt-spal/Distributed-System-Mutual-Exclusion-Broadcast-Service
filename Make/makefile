#------------------------------------------------------------------------------
#  File        :   makefile for AOS Summer 2015 Project 2
#  Author      :   Swaroop Pal Harsh Desai
#  Description :   Builds the AOS Project 
#------------------------------------------------------------------------------

#Defines  
COMPILER            := g++
CFLAGS           	:= -Wall -std=c++0x -pthread
DELETE           	:= rm -f
MOVE                := mv -f
COPY                := cp -f
NODEBINARY          := AOSProject

#Folder Paths
CURRENTDIR      	:= . \

INCLUDEPATH         :=  -I ../inc \

BINARYPATH          :=  ../bin\
 
SOURCEFILES    		:= ../src/CParser.cpp \
                       ../src/CThread.cpp \
                       ../src/CMutex.cpp \
                       ../src/CSemaphore.cpp \
                       ../src/CServer.cpp \
                       ../src/CClient.cpp \
                       ../src/CNodeMessageQ.cpp \
                       ../src/CNode.cpp \
                       ../src/CSystemAPI.cpp \
                       ../src/CAppServer.cpp \
                       ../src/CAppMessageQ.cpp \
                       ../src/CAppModule.cpp \
                       ../src/CTokenRequestQ.cpp \
                       ../src/Main.cpp \


#Build the EXE
.PHONY : default all clean EXE

default: all

all : EXE

EXE : 
	$(COMPILER) $(CFLAGS) $(SOURCEFILES) -o $(NODEBINARY) $(INCLUDEPATH) -w
	@echo
	@echo "Node Binary Built"  
	$(MOVE) $(NODEBINARY) $(BINARYPATH)    
	@echo "Node Binary Stored in bin"
	@echo

clean :
	@echo
	@echo "Delete the Binary File"
	$(DELETE) ../bin/AOSProject
	@echo

#------------------------------ End of File ------------------------------------
