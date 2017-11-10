
SOURCES = $(wildcard src/*.cpp)
SOURCES += $(wildcard common/*.cpp)
SOURCES += $(wildcard components/*.cpp)
SOURCES += $(wildcard utils/*.cpp)
SOURCES += $(wildcard widgets/*.cpp)

FLAGS += -I. -DVERSION=0.0.1 -DMANUFACTURE_SLUG="JulienEres" -DMANUFACTURE_NAME="Julien Eres"

include ../../plugin.mk

dist: all
	mkdir -p dist/JE
	cp LICENSE* dist/JE/
	cp plugin.* dist/JE/
	cp -R res dist/JE/
