cl65 -o c64-studio.prg -O -t c64 c64-studio.c asm\sqrt8.s asm\sqrt16.s c64-tgimousedata.o
cl65 -o c64-browser.prg -O -t c64 c64-browser.c asm\http.s c64-tgimousedata.o -L asm/lib ip65_tcp.lib c64combo.lib

C:\Users\ant_t\Dropbox\C64\[Software]\WinVICE-3.0-x64\c1541 -format c64-studio,666 d64 c64-studio.d64 -attach c64-studio.d64 -write c64-studio.prg c64-studio.prg -write c64-browser.prg c64-browser.prg -write assets\c64-1351.mou c64-1351.mou -write assets\c64-reu.emd c64-reu.emd -write assets\c64-hi.tgi c64-hi.tgi -write assets\litt.tch litt.tch -write assets\box.stl box.stl -write assets\logo.stl logo.stl -write assets\monkey.stl monkey.stl

del c64-studio.o
del c64-studio.prg
del c64-browser.o
del c64-browser.prg
cd asm
del http.o
del sqrt8.o
del sqrt16.o

pause