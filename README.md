# kiyosVideoAnalysisTool
Welcome to Kiyo's video analysis tool!

Hopefully a useful tool to analyse media files and urls. Should work with Dash/HLS and local mp4/ogv/webm files.

Currently, Kiyo's video analysis tool can print metadata, test playback and analyse the video bitrate.

It can be quite powerful when run from qtcreator's debugger; set breakpoints or print statements to analyse ffmpeg library structs at runtime. The player isn't the best, but is small and easy to learn, looking at the player engine code and working out what's going on shouldn't take too long.

GPL v3 licence

Test streams:

http://rdmedia.bbc.co.uk/dash/ondemand/bbb/2/client_manifest-high_profile-level_40-common_init.mpd

https://dash.akamaized.net/dash264/TestCases/2a/qualcomm/1/MultiResMPEG2.mpd

Build info:

Kiyo's video analysis tool has been built on Windows and GNU/Linux.

Windows:

To build on windows, other than qt5 almost all the dependencies come included with the source code.
Boost and qcustomplot are included in the thirdParty folder, you shouldn't have to do anything here...
In the thirdParty directory there is a zip folder of the ffmpeg windows dlls. This can be copied and pasted into your qt5 installation. 
For example, copy the dlls here (defualt qt installation path: C:\Qt\5.12.6\mingw73_64\bin) then open the pro file in qtcreator.
Thats it!

GNU/Linux:

Like windows you only really have to build and install ffmpeg as the other dependencies come with the source code (use the package manager to install qt5).
Once ffmpeg is built and installed the pkg_config should do the rest. ffmpeg version 4.2.1 or greater was tested so that version is probably required.
Building with CMake is also supported, CMake version greater than 3.1 is required.

ffmpeg 4.2.1 build instructions with ubuntu (tested on 18.04) are as follows:

sudo apt install libavc1394-dev nasm libchromaprint-dev libbluray-dev libbs2b-dev libcaca-dev libdc1394-22-dev libflite1 frei0r-plugins-dev libshine-dev libsnappy-dev libgsm1-dev libgnutls28-dev libmysofa-dev libopenjp2-7-dev libopenmpt-dev librsvg2-dev ladspa-sdk librubberband-dev libsoxr-dev libssh-dev speex libspeex-dev libiec61883-dev libtheora-dev libtwolame-dev libwavpack-dev libwebp-dev libxvidcore-dev libzmq3-dev libass-dev libdrm-dev libzvbi-dev libopenal-dev libomxil-bellagio-dev libgme-dev libmp3lame-dev libopus-dev libpulse-dev libvpx-dev libx264-dev libx265-dev libnuma-dev libfdk-aac-dev libgl1-mesa-dev libsdl2-dev libopencore-amrnb-dev libbamf3-dev

./configure --enable-gpl --enable-version3 --enable-fontconfig --enable-gnutls --enable-iconv --enable-libass --enable-libbluray --enable-libfreetype --enable-libmp3lame --enable-libopencore-amrnb  --enable-libopenjpeg --enable-libopus --enable-libshine --enable-libsnappy --enable-libsoxr --enable-libtheora --enable-libtwolame --enable-libvpx --enable-libwavpack --enable-libwebp --enable-libx264 --enable-libx265 --enable-libxml2 --enable-lzma --enable-zlib --enable-gmp --enable-libvorbis  --enable-libspeex --enable-libxvid --enable-avisynth --enable-libopenmpt

make 

sudo make install