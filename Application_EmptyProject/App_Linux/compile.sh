#!/bin/sh

make -f ../../common/Linux/Makefile SRCS='$(wildcard ../MainClass*.cpp)' $*
