
# FLAGS += -I. -I./ext/LambertW -DJE_VERSION=0.6.0dev -DJE_MANUFACTURER="Julien Eres"
FLAGS += -I. -I./ext/LambertW

SLUG = JE
VERSION = 0.6.0

SOURCES += $(wildcard src/*.cpp)
SOURCES += $(wildcard common/*.cpp)
SOURCES += $(wildcard components/*.cpp)
SOURCES += $(wildcard ext/LambertW/*.cc)
SOURCES += $(wildcard utils/*.cpp)
SOURCES += $(wildcard widgets/*.cpp)

DISTRIBUTABLES += $(wildcard LICENSE*) res

RACK_DIR ?= ../..
include $(RACK_DIR)/plugin.mk