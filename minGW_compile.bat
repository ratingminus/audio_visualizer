g++.exe src/*.cpp src/fileinputgui/*.cpp -Iinclude -Llib -DOS2 -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -llibfftw3-3 -lcomdlg32 -lole32 -o bin64/musico.exe
cd bin64
./musico.exe
cd ..
