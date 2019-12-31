#!/bin/bash
myDir=$PWD
valgrind --tool=cachegrind --branch-sim=yes --cache-sim=yes ./../build/bin/kiyosVideoAnalysisTool
cg_annotate cachegrind.out.4576 /home/coco/Music/build/bin/kiyosVideoAnalysisTool #must be abolute path, no ./../. in path either this is a bit of a bug.
