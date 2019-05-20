# 842 C simulation stage

This repository keeps the c simulation stage for https://github.com/stiXits/842_hls

# Build & Run
To run the c simulation, create a new vivado hls project and add:
* addresscache.cpp
* addresscache.h
* hw_compress.cpp
* io.cpp
* io.h
* ringbuffer.cpp
* ringbuffer.h
* settings.h
to the sources section and all remaining files to the test bench.

create a new solution, build & run it via the "Run c simulation" button
