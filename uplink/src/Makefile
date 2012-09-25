# Makefile for Uplink

DATA_DIR = ../../Installer/data
DATA_DOWNLOAD_DIR = ../../Installer/data_download
CONTRIB_DIR = ../../contrib
TCP4U_DIR = $(CONTRIB_DIR)/tcp4u.331
IRCLIB_DIR = $(CONTRIB_DIR)/irclib
UNRAR_DIR = $(CONTRIB_DIR)/unrar

INCLUDES = 	-I.					\
		-I/usr/X11R6/include/			\
		-I../../lib/eclipse/			\
		-I../../lib/tosser/			\
		-I../../lib/soundgarden/		\
		-I../../lib/vanbakel/			\
		-I../../lib/gucci/			\
		-I../../lib/bungle/			\
		-I../../lib/redshirt/			\
		-I../../lib/mmgr/			\
		-I$(TCP4U_DIR)/Include			\
		-I$(CONTRIB_DIR)/include		\
		-I$(IRCLIB_DIR)				\
		-I$(IRCLIB_DIR)/linux

LIBS_INCLUDES = \
	-L ../../lib/soundgarden \
	-L ../../lib/gucci \
	-L ../../lib/eclipse \
	-L ../../lib/vanbakel \
	-L ../../lib/redshirt \
	-L ../../lib/bungle \
	-L ../../lib/mmgr \
	-L $(TCP4U_DIR)/src \
	-L $(IRCLIB_DIR) \
	-L $(UNRAR_DIR) \
	-L $(CONTRIB_DIR)/install/lib

LIBS = \
	-lsoundgarden \
	-lgucci \
	-leclipse \
	-lvanbakel \
	-lredshirt \
	-lbungle \
	-lmmgr \
	-ltcp4ux \
	-lirclib \
	-lunrar \
	-lGL -lGLU \
	-lSDL -lSDL_mixer \
	-lftgl \
	-lfreetype \
	-ltiff \
	-ljpeg \
	-lz \
	-lpthread 
#	-lgltt \
#	-lttf \

CXX = g++
CC = gcc
LINK = $(CXX)

CXXFLAGS += -O2 -g
STRIP = strip 

NM = nm

CXXFLAGS += $(INCLUDES) -w -D_REENTRANT 

# To produce the source list, use:
# find . -name '*.cpp' | sed 's%^\./\(.*\)%\1 \\%'
# and remove the last backslash (\) by hand

SOURCES= \
app/binreloc.cpp \
app/app.cpp \
app/dos2unix.cpp \
app/miscutils.cpp \
app/opengl.cpp \
app/opengl_interface.cpp \
app/probability.cpp \
app/serialise.cpp \
app/uplinkobject.cpp \
game/data/data.cpp \
game/game.cpp \
game/gameobituary.cpp \
game/scriptlibrary.cpp \
interface/interface.cpp \
interface/interfacescreen.cpp \
interface/localinterface/analyser_interface.cpp \
interface/localinterface/cheat_interface.cpp \
interface/localinterface/email_interface.cpp \
interface/localinterface/evtqueue_interface.cpp \
interface/localinterface/finance_interface.cpp \
interface/localinterface/gateway_interface.cpp \
interface/localinterface/hud_interface.cpp \
interface/localinterface/hw_interface.cpp \
interface/localinterface/irc_interface.cpp \
interface/localinterface/keyboardinterface.cpp \
interface/localinterface/lan_interface.cpp \
interface/localinterface/localinterface.cpp \
interface/localinterface/localinterfacescreen.cpp \
interface/localinterface/memory_interface.cpp \
interface/localinterface/mission_interface.cpp \
interface/localinterface/phonedialler.cpp \
interface/localinterface/sendmail_interface.cpp \
interface/localinterface/status_interface.cpp \
interface/localinterface/sw_interface.cpp \
interface/localinterface/worldmap/worldmap_interface.cpp \
interface/localinterface/worldmap/worldmap_layout.cpp \
interface/remoteinterface/academicscreen_interface.cpp \
interface/remoteinterface/accountscreen_interface.cpp \
interface/remoteinterface/bbsscreen_interface.cpp \
interface/remoteinterface/changegatewayscreen_interface.cpp \
interface/remoteinterface/codecardscreen_interface.cpp \
interface/remoteinterface/companyinfoscreen_interface.cpp \
interface/remoteinterface/consolescreen_interface.cpp \
interface/remoteinterface/contactscreen_interface.cpp \
interface/remoteinterface/criminalscreen_interface.cpp \
interface/remoteinterface/cypherscreen_interface.cpp \
interface/remoteinterface/dialogscreen_interface.cpp \
interface/remoteinterface/disconnectedscreen_interface.cpp \
interface/remoteinterface/faithscreen_interface.cpp \
interface/remoteinterface/fileserverscreen_interface.cpp \
interface/remoteinterface/highsecurityscreen_interface.cpp \
interface/remoteinterface/hwsalesscreen_interface.cpp \
interface/remoteinterface/linksscreen_interface.cpp \
interface/remoteinterface/loansscreen_interface.cpp \
interface/remoteinterface/logscreen_interface.cpp \
interface/remoteinterface/menuscreen_interface.cpp \
interface/remoteinterface/messagescreen_interface.cpp \
interface/remoteinterface/nearestgatewayscreen_interface.cpp \
interface/remoteinterface/newsscreen_interface.cpp \
interface/remoteinterface/nuclearwarscreen_interface.cpp \
interface/remoteinterface/passwordscreen_interface.cpp \
interface/remoteinterface/protovisionscreen_interface.cpp \
interface/remoteinterface/radiotransmitterscreen_interface.cpp \
interface/remoteinterface/rankingscreen_interface.cpp \
interface/remoteinterface/recordscreen_interface.cpp \
interface/remoteinterface/remoteinterface.cpp \
interface/remoteinterface/remoteinterfacescreen.cpp \
interface/remoteinterface/securityscreen_interface.cpp \
interface/remoteinterface/shareslistscreen_interface.cpp \
interface/remoteinterface/sharesviewscreen_interface.cpp \
interface/remoteinterface/socialsecurityscreen_interface.cpp \
interface/remoteinterface/swsalesscreen_interface.cpp \
interface/remoteinterface/useridscreen_interface.cpp \
interface/remoteinterface/voiceanalysisscreen_interface.cpp \
interface/remoteinterface/voicephonescreen_interface.cpp \
interface/scrollbox.cpp \
interface/taskmanager/decrypter.cpp \
interface/taskmanager/decypher.cpp \
interface/taskmanager/defrag.cpp \
interface/taskmanager/dictionaryhacker.cpp \
interface/taskmanager/faith.cpp \
interface/taskmanager/filecopier.cpp \
interface/taskmanager/filedeleter.cpp \
interface/taskmanager/firewalldisable.cpp \
interface/taskmanager/gatewaynuke.cpp \
interface/taskmanager/iplookup.cpp \
interface/taskmanager/ipprobe.cpp \
interface/taskmanager/lanforce.cpp \
interface/taskmanager/lanprobe.cpp \
interface/taskmanager/lanscan.cpp \
interface/taskmanager/lanspoof.cpp \
interface/taskmanager/logdeleter.cpp \
interface/taskmanager/logmodifier.cpp \
interface/taskmanager/logundeleter.cpp \
interface/taskmanager/motionsensor.cpp \
interface/taskmanager/passwordbreaker.cpp \
interface/taskmanager/proxydisable.cpp \
interface/taskmanager/revelation.cpp \
interface/taskmanager/revelationtracker.cpp \
interface/taskmanager/securitybypass.cpp \
interface/taskmanager/taskmanager.cpp \
interface/taskmanager/tracetracker.cpp \
interface/taskmanager/tutorial.cpp \
interface/taskmanager/uplinkagentlist.cpp \
interface/taskmanager/uplinktask.cpp \
interface/taskmanager/voiceanalyser.cpp \
mainmenu/closing_interface.cpp \
mainmenu/connectionlost_interface.cpp \
mainmenu/demogameover_interface.cpp \
mainmenu/disavowed_interface.cpp \
mainmenu/firsttimeloading_interface.cpp \
mainmenu/genericoptions_interface.cpp \
mainmenu/loading_interface.cpp \
mainmenu/login_interface.cpp \
mainmenu/mainmenu.cpp \
mainmenu/mainmenuscreen.cpp \
mainmenu/graphicoptions_interface.cpp \
mainmenu/networkoptions_interface.cpp \
mainmenu/obituary_interface.cpp \
mainmenu/options_interface.cpp \
mainmenu/revelationlost_interface.cpp \
mainmenu/revelationwon_interface.cpp \
mainmenu/theme_interface.cpp \
mainmenu/theteam_interface.cpp \
mainmenu/warezgameover_interface.cpp \
network/clientconnection.cpp \
network/interfaces/clientcommsinterface.cpp \
network/interfaces/clientstatusinterface.cpp \
network/interfaces/networkscreen.cpp \
network/network.cpp \
network/networkclient.cpp \
network/networkserver.cpp \
options/options.cpp \
uplink.cpp \
view/fps.cpp \
view/view.cpp \
world/agent.cpp \
world/company/company.cpp \
world/company/companyuplink.cpp \
world/company/mission.cpp \
world/company/news.cpp \
world/company/sale.cpp \
world/computer/bankaccount.cpp \
world/computer/bankcomputer.cpp \
world/computer/computer.cpp \
world/computer/computerscreen/bbsscreen.cpp \
world/computer/computerscreen/computerscreen.cpp \
world/computer/computerscreen/cypherscreen.cpp \
world/computer/computerscreen/dialogscreen.cpp \
world/computer/computerscreen/disconnectedscreen.cpp \
world/computer/computerscreen/genericscreen.cpp \
world/computer/computerscreen/highsecurityscreen.cpp \
world/computer/computerscreen/linksscreen.cpp \
world/computer/computerscreen/logscreen.cpp \
world/computer/computerscreen/menuscreen.cpp \
world/computer/computerscreen/messagescreen.cpp \
world/computer/computerscreen/passwordscreen.cpp \
world/computer/computerscreen/shareslistscreen.cpp \
world/computer/computerscreen/useridscreen.cpp \
world/computer/databank.cpp \
world/computer/gateway.cpp \
world/computer/gatewaydef.cpp \
world/computer/lancomputer.cpp \
world/computer/lanmonitor.cpp \
world/computer/logbank.cpp \
world/computer/recordbank.cpp \
world/computer/security.cpp \
world/computer/securitymonitor.cpp \
world/computer/securitysystem.cpp \
world/connection.cpp \
world/date.cpp \
world/generator/consequencegenerator.cpp \
world/generator/demoplotgenerator.cpp \
world/generator/langenerator.cpp \
world/generator/missiongenerator.cpp \
world/generator/namegenerator.cpp \
world/generator/newsgenerator.cpp \
world/generator/numbergenerator.cpp \
world/generator/plotgenerator.cpp \
world/generator/recordgenerator.cpp \
world/generator/worldgenerator.cpp \
world/message.cpp \
world/person.cpp \
world/player.cpp \
world/rating.cpp \
world/scheduler/arrestevent.cpp \
world/scheduler/attemptmissionevent.cpp \
world/scheduler/bankrobberyevent.cpp \
world/scheduler/changegatewayevent.cpp \
world/scheduler/eventscheduler.cpp \
world/scheduler/installhardwareevent.cpp \
world/scheduler/notificationevent.cpp \
world/scheduler/runplotsceneevent.cpp \
world/scheduler/seizegatewayevent.cpp \
world/scheduler/shotbyfedsevent.cpp \
world/scheduler/uplinkevent.cpp \
world/scheduler/warningevent.cpp \
world/vlocation.cpp \
world/world.cpp

DEMO_CPPFLAGS=-DDEMOGAME=1
#FULL_CPPFLAGS=-DFULLGAME=1 -DCODECARD_ENABLED=1
FULL_CPPFLAGS=-DFULLGAME=1
##COMPLETE_CPPFLAGS=-DFULLGAME=1 
#COMPLETE_CPPFLAGS=-DFULLGAME=1 -DONLINEDOWNLOADRELEASE=1
##PATCH_CPPFLAGS=-DFULLGAME=1 -DCODECARD_ENABLED=1
#PATCH_CPPFLAGS=-DFULLGAME=1

OBJDIR_BASE=.linux-objs
DEMO_OBJDIR=$(OBJDIR_BASE)/demo
FULL_OBJDIR=$(OBJDIR_BASE)/full
#COMPLETE_OBJDIR=$(OBJDIR_BASE)/complete
#PATCH_OBJDIR=$(OBJDIR_BASE)/patch

ALL_OBJECTS=$(SOURCES:%.cpp=%.o)
DEMO_OBJECTS=$(ALL_OBJECTS:%.o=$(DEMO_OBJDIR)/%.o)
FULL_OBJECTS=$(ALL_OBJECTS:%.o=$(FULL_OBJDIR)/%.o)
#COMPLETE_OBJECTS=$(ALL_OBJECTS:%.o=$(COMPLETE_OBJDIR)/%.o)
#PATCH_OBJECTS=$(ALL_OBJECTS:%.o=$(PATCH_OBJDIR)/%.o)

all: uplink.full

uplink.full version.full: $(FULL_OBJECTS)
	@echo -n "Linking... "
	@$(LINK) $(LIBS_INCLUDES) $+ $(LIBS) -o $@ 
	@$(CXX) $(FULL_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) version.cpp -o version.full
	@echo done.

#uplink.complete version.complete: $(COMPLETE_OBJECTS)
#	@echo -n "Linking... "
#	@$(LINK) $(LIBS_INCLUDES) $+ $(LIBS) -o $@ 
#	@$(CXX) $(COMPLETE_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) version.cpp -o version.complete
#	@echo done.

uplink.demo version.demo: $(DEMO_OBJECTS)
	@echo -n "Linking... "
	@$(LINK) $(LIBS_INCLUDES) $+ $(LIBS) -o $@ 
	@$(CXX) $(DEMO_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) version.cpp -o version.demo
	@echo done.

#uplink.patch version.patch: $(PATCH_OBJECTS)
#	@echo -n "Linking... "
#	@$(LINK) $(LIBS_INCLUDES) $+ $(LIBS) -o $@ 
#	@$(CXX) $(PATCH_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) version.cpp -o version.patch
#	@echo done.

dist-demo: DEST=demo

dist-demo: TARGET=demo

dist-demo: COPY_DAT=cp \
	$(DATA_DIR)/data.dat \
	$(DATA_DIR)/fonts.dat \
	$(DATA_DIR)/graphics.dat \
	$(DATA_DIR)/loading.dat \
	$(DATA_DIR)/music.dat \
	$(DATA_DIR)/patch3.dat \
	$(DATA_DIR)/patch2.dat \
	$(DATA_DIR)/patch.dat \
	$(DATA_DIR)/sounds.dat \
	dist/$(DEST)/uplink/lib
	
dist-demo: COPY_EXTRA=

dist-demo: uplink.demo

dist-full: DEST=full

dist-full: TARGET=full

dist-full: COPY_DAT=cp $(DATA_DIR)/*.dat dist/$(DEST)/uplink/lib

dist-full: COPY_EXTRA=

dist-full: uplink.full

dist-complete: DEST=complete

dist-complete: TARGET=full
#dist-complete: TARGET=complete

dist-complete: COPY_DAT=cp \
	$(DATA_DIR)/data.dat \
	$(DATA_DIR)/fonts.dat \
	$(DATA_DIR)/graphics.dat \
	$(DATA_DIR)/loading.dat \
	$(DATA_DIR)/music.dat \
	$(DATA_DIR)/patch3.dat \
	$(DATA_DIR)/patch2.dat \
	$(DATA_DIR)/patch.dat \
	$(DATA_DIR)/sounds.dat \
	$(DATA_DOWNLOAD_DIR)/world.dat \
	dist/$(DEST)/uplink/lib

dist-complete: COPY_EXTRA=cp \
	$(DATA_DOWNLOAD_DIR)/codecard.txt \
	dist/$(DEST)/uplink/

dist-complete: uplink.full
#dist-complete: uplink.complete

dist-patch: DEST=patch

dist-patch: TARGET=full
#dist-patch: TARGET=patch

dist-patch: COPY_DAT=cp \
	$(DATA_DIR)/patch3.dat \
	$(DATA_DIR)/patch2.dat \
	$(DATA_DIR)/patch.dat \
	$(DATA_DIR)/sounds.dat \
	dist/$(DEST)/uplink/lib

dist-patch: COPY_EXTRA=

dist-patch: uplink.full
#dist-patch: uplink.patch

dist-%: 
	rm -rf dist/$(DEST)
	mkdir -p dist/$(DEST)/uplink/lib
	mkdir -p symbols
	nm uplink.$(TARGET) > symbols/uplink-$(DEST)-$(shell ./version.$(TARGET))-$(shell date +%Y-%m-%d$).txt
	cp uplink.$(TARGET) dist/$(DEST)/uplink/lib/uplink.bin.x86
	strip dist/$(DEST)/uplink/lib/uplink.bin.x86
	#Removed, use FTGL instead of GLTT (also no more needed, static link)
	#cp $(CONTRIB_DIR)/install/lib/libttf.so.2 dist/$(DEST)/uplink/lib
	#Removed, use FTGL instead of GLTT
	#cp $(CONTRIB_DIR)/install/lib/libgltt.so.0 dist/$(DEST)/uplink/lib
	cp $(CONTRIB_DIR)/install/lib/libfreetype.so.6 dist/$(DEST)/uplink/lib
	cp $(CONTRIB_DIR)/install/lib/libftgl.so.0 dist/$(DEST)/uplink/lib
	cp $(CONTRIB_DIR)/install/lib/libSDL-1.2.so.0 dist/$(DEST)/uplink/lib
	cp $(CONTRIB_DIR)/install/lib/libSDL_mixer-1.2.so.0 dist/$(DEST)/uplink/lib
	#cp $(CONTRIB_DIR)/install/lib/libmikmod.so.3 dist/$(DEST)/uplink/lib
	#Removed cause problems on x86_64?
	#cp /lib/libgcc_s.so.1  dist/$(DEST)/uplink/lib
	$(COPY_DAT)
	$(COPY_EXTRA)
	cp $(DATA_DIR)/changes.txt dist/$(DEST)/uplink
	cp $(DATA_DIR)/mods.txt dist/$(DEST)/uplink
	cp $(DATA_DIR)/readme.txt dist/$(DEST)/uplink
	cp $(DATA_DIR)/docs/*.txt dist/$(DEST)/uplink
	cp $(DATA_DIR)/docs/*.patch dist/$(DEST)/uplink
	chmod -x dist/$(DEST)/uplink/lib/*.so.*
	find dist/$(DEST)/uplink -name '.svn' -print0 | xargs -0 rm -rf
	cp $(DATA_DIR)/uplink.png dist/$(DEST)/uplink
	cp $(DATA_DIR)/license.txt dist/$(DEST)/uplink
	cp -T ../../loki/run-uplink.sh dist/$(DEST)/uplink/uplink
	tar -C dist/$(DEST) -czf uplink-$(DEST)-$(shell ./version.$(TARGET)).tar.gz uplink
	#Installer part
	rm -f dist/$(DEST)/uplink/uplink
	cp -r ../../loki/common/* dist/$(DEST)/uplink
	find dist/$(DEST)/uplink -name '.svn' -print0 | xargs -0 rm -rf
	sed -e "s/SHORT_VERSION/$(shell ./version.$(TARGET))/" \
        ../../loki/setup.$(DEST).xml > dist/$(DEST)/uplink/setup.data/setup.xml
	mkdir -p dist/$(DEST)/uplink/bin/Linux/x86
	cp ../../loki/run-uplink.sh dist/$(DEST)/uplink/bin/Linux/x86/uplink
	#sh -c "cd dist/$(DEST)/uplink/setup.data/bin/Linux ; ln -s x86 amd64"
	sh -c "cd dist/$(DEST)/uplink/setup.data/bin/Linux ; cp -p -r x86 amd64"
	$(CONTRIB_DIR)/loki_setup/makeself/makeself.sh \
       dist/$(DEST)/uplink uplink-$(DEST)-$(shell ./version.$(TARGET)).sh "Uplink $(DEST) $(shell ./version.$(TARGET))" ./setup.sh

clean:
	rm -rf $(FULL_OBJDIR) $(DEMO_OBJDIR) uplink.demo uplink.full version.demo version.full
#	rm -rf $(FULL_OBJDIR) $(DEMO_OBJDIR) $(COMPLETE_OBJDIR) $(PATCH_OBJDIR) uplink.demo uplink.full uplink.complete uplink.patch version.demo version.full version.complete version.patch

.linux-objs/demo/%.o: %.cpp
	@echo Compiling $<
	@mkdir -p $(dir $@)
	@$(CXX) $(DEMO_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.linux-objs/demo/%.o: %.c
	@echo Compiling $<
	@mkdir -p $(dir $@)
	@$(CC) $(DEMO_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.linux-objs/full/%.o: %.cpp
	@echo Compiling $<
	@mkdir -p $(dir $@)
	@$(CXX) $(FULL_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.linux-objs/full/%.o: %.c
	@echo Compiling $<
	@mkdir -p $(dir $@)
	@$(CC) $(FULL_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

#.linux-objs/complete/%.o: %.cpp
#	@echo Compiling $<
#	@mkdir -p $(dir $@)
#	@$(CXX) $(COMPLETE_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
#
#.linux-objs/complete/%.o: %.c
#	@echo Compiling $<
#	@mkdir -p $(dir $@)
#	@$(CC) $(COMPLETE_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

#.linux-objs/patch/%.o: %.cpp
#	@echo Compiling $<
#	@mkdir -p $(dir $@)
#	@$(CXX) $(PATCH_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
#
#.linux-objs/patch/%.o: %.c
#	@echo Compiling $<
#	@mkdir -p $(dir $@)
#	@$(CC) $(PATCH_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

#%.dep: TARGET=$(patsubst %.dep,%,$@)
#
#%.dep:
#	@fastdep -O $(OBJDIR_BASE)/$(TARGET) $(INCLUDES) $(SOURCES) > $@
#
#-include full.dep
#-include demo.dep
#-include complete.dep
##-include patch.dep
