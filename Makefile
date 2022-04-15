# equivalent to #define in c code
VERSION = 0.12.0
CC = gcc
PREFIX = /usr/local
appimage: PREFIX = ./opgui.AppDir/usr

# Check if we are running on windows
UNAME := $(shell uname)
ifneq (, $(findstring _NT-, $(UNAME)))
	LDFLAGS = -mwindows
	HIDAPI_PKG = hidapi
else
	LDFLAGS += -lrt
	HIDAPI_PKG = hidapi-hidraw
endif

CFLAGS_GTK2 = `pkg-config --cflags gtk+-2.0`
LDFLAGS_GTK2 = `pkg-config --libs gtk+-2.0`

CFLAGS_GTK3 = `pkg-config --cflags gtk+-3.0`
LDFLAGS_GTK3 = `pkg-config --libs gtk+-3.0`

CFLAGS_HIDAPI = `pkg-config --cflags $(HIDAPI_PKG)`
LDFLAGS_HIDAPI = `pkg-config --libs $(HIDAPI_PKG)`

CFLAGS =  '-DVERSION="$(VERSION)"'
CFLAGS += -Os -s #size
#CFLAGS += -O3 -s #speed
#CFLAGS += -g #debug

CFLAGS += $(CFLAGS_GTK3) $(CFLAGS_HIDAPI)

OBJECTS_SHARED = deviceRW.o \
	fileIO.o \
	I2CSPI.o \
	progAVR.o \
	progEEPROM.o \
	progP12.o \
	progP16.o \
	progP18.o \
	progP24.o \
	strings.o

OBJECTS_OPGUI = opgui.o \
	coff.o \
	icd.o \
	icons.o

OBJECTS_HIDTEST = hid_test.o

OBJECTS_OP = op.o

LDFLAGS_OPGUI = $(LDFLAGS) $(LDFLAGS_GTK3)
LDFLAGS_OP = $(LDFLAGS)
LDFLAGS_HIDTEST = $(LDFLAGS) $(LDFLAGS_HIDAPI)

OBJECTS = $(OBJECTS_OP) $(OBJECTS_OPGUI) $(OBJECTS_SHARED)

targets = opgui op

# Targets
all: $(targets)

opgui: $(OBJECTS_OPGUI) $(OBJECTS_SHARED)
	$(CC) -o $@ $(OBJECTS_OPGUI) $(OBJECTS_SHARED) $(LDFLAGS_OPGUI)

op: $(OBJECTS_OP) $(OBJECTS_SHARED)
	$(CC) -o $@ $(OBJECTS_OP) $(OBJECTS_SHARED) $(LDFLAGS_OP)

hid_test: $(OBJECTS_HIDTEST)
	$(CC) -o $@ $(OBJECTS_HIDTEST) $(LDFLAGS_HIDTEST)

icons.c : write.png read.png sys.png
	echo "#include <gtk/gtk.h>" > icons.c
	gdk-pixbuf-csource --extern --build-list write_icon write.png read_icon read.png \
	system_icon sys.png go_icon go.png halt_icon halt.png step_icon step.png \
	stepover_icon stepover.png stop_icon stop.png >> icons.c

clean:
	rm -f $(targets) $(OBJECTS) icons.c
	
install: all
	#test -d $(prefix) || mkdir $(prefix)
	#test -d $(prefix)/bin || mkdir $(prefix)/bin
	@echo "Installing opgui"
	mkdir -p $(PREFIX)/bin
	install -m 0755 opgui $(PREFIX)/bin;
	
appimage: install
	@echo "Installing data in opgui.AppDir/ ..."
	@cp opgui.desktop ./opgui.AppDir
	@cp opgui_icon_linux.svg ./opgui.AppDir
	@echo "Downloading appimagetool.AppImage ..."
	@wget https://github.com/AppImage/AppImageKit/releases/download/13/appimagetool-x86_64.AppImage
	@chmod +x appimagetool-x86_64.AppImage
	@echo "Downloading appimagetool.AppImage ..."
	@wget https://github.com/AppImage/AppImageKit/releases/download/13/AppRun-x86_64
	@chmod +x AppRun-x86_64
	@mv AppRun-x86_64 ./opgui.AppDir/AppRun
	@echo "Deploy dependencies..."
	@ldd opgui | grep "/usr/lib*" > dependencies.txt
	@gcc deploy_dependencies_appimage.c -o deploy_dependencies_appimage
	@./deploy_dependencies_appimage
	@echo "Generating appimage..."
	@appimagetool ./opgui.AppDir
	@mv Opgui-x86_64.AppImage Opgui-$(VERSION)-x86_64.AppImage
	@echo "Cleaning..."
	@make clean
	@rm appimagetool-x86_64.AppImage
	@rm -r ./opgui.AppDir/
	@rm dependencies.txt
	@rm deploy_dependencies_appimage

package:
	@echo "Creating opgui_$(VERSION).tar.gz"
	@mkdir opgui-$(VERSION)
	@cp *.c *.h *.png gpl-2.0.txt Makefile readme opgui.desktop opgui_icon_linux.svg opgui-$(VERSION)
	@tar -czf opgui_$(VERSION).tar.gz opgui-$(VERSION)
	@rm -rf opgui-$(VERSION)

.PHONY: all clean install package
