RACK_DIR ?= ../..

FLAGS += -I. -I./ext/LambertW

SOURCES = $(wildcard src/*.cpp)
SOURCES += $(wildcard common/*.cpp)
SOURCES += $(wildcard components/*.cpp)
SOURCES += $(wildcard ext/LambertW/*.cc)
SOURCES += $(wildcard utils/*.cpp)
SOURCES += $(wildcard widgets/*.cpp)

DISTRIBUTABLES += $(wildcard LICENSE*) res

include $(RACK_DIR)/plugin.mk
