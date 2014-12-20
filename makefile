########## DEFINITIONS ##########
TARGET		:= libcore.a

SUBDIRS		:= $(shell find . -type d -path './[!.]*')
INCLUDES	:= -I.
LIBRARIES	:=
DEFINES		:= -D_FILE_OFFSET_BITS=64

########## OPTIONS ##########
CXX			:= clang++
CXXFLAGS	:= -std=c++11 -stdlib=libc++ -O2 -Wall

include ../makefile_tgt.mk
