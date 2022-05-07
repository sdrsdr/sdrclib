cmake_minimum_required(VERSION 2.6)
project(sdrclib)
#add_definitions(-Wall -std=c99)
add_definitions(-Wall)
add_library(sdrclib STATIC daemonize.c bindaddr.c conds.c dllists.c easyparse.c str_replace.c str_split.c str_split_m.c str_split_mb.c str_split.c superio.c usock.c strtokex.c)
TARGET_LINK_LIBRARIES(sdrclib pthread )

