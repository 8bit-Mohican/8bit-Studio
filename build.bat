cl65 -o C64-Studio.prg -O -t c64 C64-Studio.c asm\http.s asm\sqrt8.s asm\sqrt16.s c64-tgimousedata.o -L libs ip65_tcp.lib c64combo.lib c64.lib

C:\Users\ant_t\Dropbox\C64\[Software]\WinVICE-3.0-x64\c1541 -format c64-studio,666 d64 C64-Studio.d64 -attach C64-Studio.d64 -write C64-Studio.prg C64-Studio.prg -write assets\c64-1351.mou c64-1351.mou -write assets\c64-reu.emd c64-reu.emd -write assets\c64-hi.tgi c64-hi.tgi -write assets\litt.tch litt.tch -write assets\box.stl box.stl -write assets\logo.stl logo.stl -write assets\monkey.stl monkey.stl

del C64-Studio.o
del C64-Studio.prg
cd asm
del sqrt8.o
del sqrt16.o

pause