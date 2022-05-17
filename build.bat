cmake -DBLAZAR_INSTALL_LIBS=ON -DBLAZAR_INSTALL_LOCATION=C:\Dev\BlazarEngine\cmake-build-debug\BlazarInstall -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - MinGW Makefiles" -S C:\Dev\BlazarEngine -B .\BlazarEngine\
cmake --build .\BlazarEngine
cmake --install .\BlazarEngine