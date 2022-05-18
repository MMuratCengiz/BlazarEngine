@REM @ECHO OFF

SET bullet_path="%~dp0../../external/bullet3"
SET project_path="%~dp0../../"
SET build_path=%project_path%build/bullet3/win/

cmake -DINSTALL_LIBS=ON -G "CodeBlocks - MinGW Makefiles" -DBUILD_UNIT_TESTS=OFF -DBUILD_OPENGL3_DEMOS=OFF -DBUILD_BULLET2_DEMOS=OFF -S %bullet_path% -B %build_path% -DCMAKE_INSTALL_PREFIX:path=%build_path%
cmake --build %build_path%
cmake --install %build_path%