#!/bin/bash

valgrind --tool=callgrind ./../build/bin/kiyosVideoAnalysisTool

###use kCachegrind to view output file.
