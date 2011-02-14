cmake_minimum_required(VERSION 2.6)
project(sdrclib)
#add_definitions(-Wall -std=c99)
add_definitions(-Wall )
add_library(sdrclib STATIC bindaddr.c conds.c dllists.c easyparse.c str_replace.c str_split.c superio.c usock.c)
TARGET_LINK_LIBRARIES(sdrclib pthread )

