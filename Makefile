# $Id: Makefile,v 1.4 2002/06/26 14:20:16 dwchin Exp $

#DMTHOME=/export/home/dwchin/DmtHome
#DMTHOME=/export/home/dmt/new
#DMTHOME=/export/home/dmt/pro
#DMTHOME=/home/dwchin/Code/gds
#DMTHOME=/export/home/dmt/dmt/$(DMTVERSION)
ROOTSYS=/opt/CERN/root_3.02-07
ROOTLIB=$(ROOTSYS)/lib
LIGOTOOLSLIB=/opt/ligotools/lib
LIB=$(DMTHOME)/$(DMTVERSION)/lib
INC=$(DMTHOME)/$(DMTVERSION)/include
RPATH=/usr/lib:/usr/openwin/lib:/usr/local/lib:/usr/local/sparc-sun-solaris2.7/lib:$(ROOTLIB):$(LIGOTOOLSLIB):$(LIB)

LIBS    = -L. -L$(LIB) -ldmtsigp -ldmtview -lsand -ldatio
#LIBS_LOCKLOSS    = -L$(LIB) -ldmtsigp -ldmtview -lsand -ldatio

LIBS_LOCKLOSS    = -Wl,-rpath,$(RPATH) -Wl,-rpath-link,$(RPATH) -L$(LIB) -L$(ROOTLIB) -L$(LIGOTOOLSLIB) \
        -lsdmtenv -lmonitor \
        -losc -lframeio -llmsg -ldmtsigp \
        -lgdstrig -lgdscntr -lhtml -lxsil -lparsl -lsgdsbase \
        -lframecpp -lgeneral -lz

LIBS_STATIC_OSC    = -Wl,-rpath,$(RPATH) -Wl,-rpath-link,$(RPATH) -L$(LIB) -L$(ROOTLIB) -L$(LIGOTOOLSLIB) \
        -lsdmtenv -lmonitor \
        -lframeio -llmsg -ldmtsigp \
        -lgdstrig -lgdscntr -lhtml -lxsil -lparsl -lsgdsbase \
        -lframecpp -lgeneral -lz

CXX     = g++
#CXXOPTS = -Wall -g -DDEBUG -I. -I$(INC)
#CXXOPTS = -Wall -O3 -DDEBUG -I. -I$(INC)
#CXXOPTS = -O3 -I. -I$(INC)
CXXOPTS = -Wall -I. -I$(INC) -fPIC -DDEBUG -D_POSIX_C_SOURCE=199506L -D_XOPEN_SOURCE=500 -g
DOCXX = /usr/local/bin/doc++

.cc.o:
	$(CXX) $(CXXOPTS) -c -o $@ $?

OBJS =  osc/boolean/InfixStrBuffer.o \
        osc/boolean/error.o \
        osc/boolean/Parser.o\
        osc/boolean/Scanner.o \
        osc/boolean/Token.o \
        osc/ParamInfo.o \
        osc/TypeInfo.o \
        osc/OperStateCond.o \
        osc/ValueAbove.o \
        osc/ValueBelow.o \
        osc/ValueRange.o \
        osc/MeanAbove.o \
        osc/MeanBelow.o \
        osc/MeanRange.o \
        osc/MeanRise.o \
        osc/MeanFall.o \
        osc/RmsAbove.o \
        osc/RmsBelow.o \
        osc/RmsRange.o \
        osc/BitAnd.o \
        osc/BitNand.o \
        osc/BitOr.o \
        osc/BitNor.o \
        osc/AbsPowerAbove.o \
        osc/AbsPowerBelow.o \
        osc/AbsPowerRange.o \
        osc/FractPowerAbove.o \
        osc/FractPowerBelow.o \
        osc/FractPowerRange.o \
        osc/AbsPowerRise.o \
        osc/AbsPowerFall.o \
        osc/AbsPowerGain.o \
        osc/FractPowerRise.o \
        osc/FractPowerFall.o \
        osc/Boolean.o \
        osc/TransitUp.o \
        osc/TransitDown.o \
        OperStateCondList.o 



libosc.a: $(OBJS)
	ar ruv libosc.a $(OBJS)
#	ar ruv $(DMTHOME)/lib/libdmt.a $(OBJS)
#	cp -f OperStateCondList.hh $(INC)
#	mkdir $(INC)/osc ; /bin/cp -f osc/*.hh $(INC)/osc
#	mkdir $(INC)/osc/boolean ; /bin/cp -f osc/boolean/*.hh $(INC)/osc/boolean

libosc.so: $(OBJS)
	ld -shared -o $@ $(OBJS)

osctest_static: osctest.o $(OBJS)
	$(CXX) $(CXXOPTS) -o $@ osctest.o $(OBJS) $(LIBS_STATIC_OSC)

osctest_dl: osctest.o libosc.so
	$(CXX) $(CXXOPTS) -o $@ osctest.o $(LIBS_LOCKLOSS)

LockLoss.o: LockLoss.cc TSWindow.hh
	$(CXX) $(CXXOPTS) -c LockLoss.cc

LockLoss: LockLoss.o $(OBJS)
	$(CXX) $(CXXOPTS) -o $@ LockLoss.o $(OBJS) $(LIBS_LOCKLOSS)


doc:    html latex

html:   OperStateCondList.hh
	$(DOCXX) -p -d oscdoc OperStateCondList.hh

latex:  OperStateCondList.hh
	doc++ -t -p -o oscdoc.tex OperStateCondList.hh
	latex oscdoc.tex
	latex oscdoc.tex
	dvips -o oscdoc.ps oscdoc.dvi

clean:
	/bin/rm -f *.aux *.dvi *.ps *.tex *.html *.o osc/*.o osc/boolean/*.o \
            libosc.a osctest osctest_dl osctest_static
	/bin/rm -rf oscdoc
	/bin/rm -rf Tests/*.o Tests/Datadir/H0/*.F

realclean: clean
	/bin/rm -f *.out  *.log *.lis output *~ osc/*~ osc/boolean/*~


