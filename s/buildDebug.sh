#!/bin/bash
myDir=$PWD
mkdir -p ./../build && cd ./../build

cmake $myDir -DCMAKE_BUILD_TYPE=debug
# or cmake -DCMAKE_INSTALL_PREFIX:PATH=/home
make

# apt-file can find package from file ... run ~$ apt-file update and then ~$ apt-file search kwallet.h

#to uninstall software (note will not remove directories)
#~$ xargs sudo rm < install_manifest.txt
