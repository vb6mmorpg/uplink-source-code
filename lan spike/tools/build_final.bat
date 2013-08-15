
cd..
mkdir build
del /q/s build

copy *.dat build
copy *.dll build

copy uplink.exe build
copy changes.txt build
copy readme.txt build

tools\upx --best build\uplink.exe

@pause


