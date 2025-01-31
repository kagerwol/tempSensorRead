#
# (c) 2016 kwo W.Kager all rights reserverd
#
# Makefile for UmplaufPumpe Program
#

SRCSFX=.cpp
OBJSFX=.o
INCSFX=.h
EXESFX=

TGTDIR=../
SRCDIR=../src/
INCDIR=../inc/
MAKDIR=../make/
OBJDIR=../obj/

TARGET=tempSensorRead64
ADD0=KwoDbAccess
ADD1=KwoPidFileHdl
ADD2=SensorValues
ADD3=TempSensorRead
ADD4=TempSensorReadMain
ADD5=TempSensorReadDefs
 
INCDEP=\
$(INCDIR)$(ADD0)$(INCSFX) $(SEP)\
$(INCDIR)$(ADD1)$(INCSFX) $(SEP)\
$(INCDIR)$(ADD2)$(INCSFX) $(SEP)\
$(INCDIR)$(ADD3)$(INCSFX) $(SEP)\
$(INCDIR)$(ADD5)$(INCSFX)
 
SRCDEP=\
$(SRCDIR)$(ADD0)$(SRCSFX) $(SEP)\
$(SRCDIR)$(ADD1)$(SRCSFX) $(SEP)\
$(SRCDIR)$(ADD2)$(SRCSFX) $(SEP)\
$(SRCDIR)$(ADD3)$(SRCSFX) $(SEP)\
$(SRCDIR)$(ADD4)$(SRCSFX)

OBJDEP=\
$(OBJDIR)$(ADD0)$(OBJSFX) $(SEP)\
$(OBJDIR)$(ADD1)$(OBJSFX) $(SEP)\
$(OBJDIR)$(ADD2)$(OBJSFX) $(SEP)\
$(OBJDIR)$(ADD3)$(OBJSFX) $(SEP)\
$(OBJDIR)$(ADD4)$(OBJSFX)


COMP=g++ -g3 -std=c++11  -c -o 
LINK=g++ -g3 -std=c++11 -L/usr/local/lib -o
GCC=g++ -o
LIBS=-lstdc++ -lmysqlcppconn-static -lmariadbclient 
INCS=-I$(INCDIR) -I/usr/local/include
SEP=

$(TGTDIR)$(TARGET)$(EXESFX) : $(OBJDEP) 
	$(LINK) $(TGTDIR)$(TARGET)$(EXESFX) $(OBJDEP) $(LIBS)

#$(OBJDIR)$(TARGET)$(OBJSFX) : $(SRCDIR)$(TARGET)$(SRCSFX) $(SEP) $(INCDEP)
#	$(COMP) $(OBJDIR)$(TARGET)$(OBJSFX) $(SRCDIR)$(TARGET)$(SRCSFX) $(INCS)
	
$(OBJDIR)$(ADD0)$(OBJSFX) : $(SRCDIR)$(ADD0)$(SRCSFX) $(SEP) $(INCDEP)
	$(COMP) $(OBJDIR)$(ADD0)$(OBJSFX) $(SRCDIR)$(ADD0)$(SRCSFX) $(INCS)
	
$(OBJDIR)$(ADD1)$(OBJSFX) : $(SRCDIR)$(ADD1)$(SRCSFX) $(SEP) $(INCDEP)
	$(COMP) $(OBJDIR)$(ADD1)$(OBJSFX) $(SRCDIR)$(ADD1)$(SRCSFX) $(INCS)
	
$(OBJDIR)$(ADD2)$(OBJSFX) : $(SRCDIR)$(ADD2)$(SRCSFX) $(SEP) $(INCDEP)
	$(COMP) $(OBJDIR)$(ADD2)$(OBJSFX) $(SRCDIR)$(ADD2)$(SRCSFX) $(INCS)
	
$(OBJDIR)$(ADD3)$(OBJSFX) : $(SRCDIR)$(ADD3)$(SRCSFX) $(SEP) $(INCDEP)
	$(COMP) $(OBJDIR)$(ADD3)$(OBJSFX) $(SRCDIR)$(ADD3)$(SRCSFX) $(INCS)
	
$(OBJDIR)$(ADD4)$(OBJSFX) : $(SRCDIR)$(ADD4)$(SRCSFX) $(SEP) $(INCDEP)
	$(COMP) $(OBJDIR)$(ADD4)$(OBJSFX) $(SRCDIR)$(ADD4)$(SRCSFX) $(INCS)
		
	
clean :
	rm $(TGTDIR)$(TARGET)$(EXESFX) $(SEP) $(OBJDEP)
