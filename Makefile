# equivalent to #define in c code
VERSION = 0.12.0
CC = gcc
PREFIX = /usr/local
appimage: PREFIX = ./opgui.AppDir/usr

VERBOSE ?= 0
ifeq ($(VERBOSE),1)
	CFLAGS += -DVERBOSE
endif

DEBUG ?= 0
ifeq ($(DEBUG),1)
	CFLAGS += -DDEBUG
else
	CC += -Os -s
endif

# Check if we are running on windows
UNAME := $(shell uname)
ifneq (, $(findstring _NT-, $(UNAME)))
	HIDAPI_PKG = hidapi
else
	LIBS += -lrt
	HIDAPI_PKG = hidapi-hidraw
endif

ICONS = opgui.svg read.png sys.png write.png

CFLAGS_GTK2 = `pkg-config --cflags gtk+-2.0`
LIBS_GTK2 = `pkg-config --libs gtk+-2.0`

CFLAGS_GTK3 = `pkg-config --cflags gtk+-3.0`
LIBS_GTK3 = `pkg-config --libs gtk+-3.0`

CFLAGS_HIDAPI = `pkg-config --cflags $(HIDAPI_PKG)`
LIBS_HIDAPI = `pkg-config --libs $(HIDAPI_PKG)`

CFLAGS +=  '-DVERSION="$(VERSION)"'
CFLAGS += -DGTK_DISABLE_SINGLE_INCLUDES -DGSEAL_ENABLE -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED
CFLAGS += $(CFLAGS_GTK3) $(CFLAGS_HIDAPI)

OBJECTS_SHARED = common.o \
	deviceRW.o \
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
	icons.o \
	style.o

OBJECTS_HIDTEST = hid_test.o

OBJECTS_OP = op.o

LIBS_OPGUI = $(LIBS) $(LIBS_GTK3)
LIBS_OP = $(LIBS)
LIBS_HIDTEST = $(LIBS) $(LIBS_HIDAPI)

OBJECTS = $(OBJECTS_OP) $(OBJECTS_OPGUI) $(OBJECTS_SHARED)

targets = opgui op

# Targets
all: $(targets)

opgui: $(OBJECTS_OPGUI) $(OBJECTS_SHARED)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS_OPGUI) $(OBJECTS_SHARED) $(LIBS_OPGUI)

op: $(OBJECTS_OP) $(OBJECTS_SHARED)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS_OP) $(OBJECTS_SHARED) $(LIBS_OP)

hid_test: $(OBJECTS_HIDTEST)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS_HIDTEST) $(LIBS_HIDTEST)

opgui.o: opgui.c icons.h style.h
	$(CC) $(CFLAGS) -c -o $@ $<

icons.c: $(ICONS) icons.xml
	glib-compile-resources icons.xml --generate-source

icons.h: $(ICONS) icons.xml
	glib-compile-resources icons.xml --generate-header

style.c: style.css style.xml
	glib-compile-resources style.xml --generate-source

style.h: style.css style.xml
	glib-compile-resources style.xml --generate-header

clean:
	rm -f $(targets) $(OBJECTS) icons.c icons.h
	
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
