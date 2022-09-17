@REM create a build dir if not exists
if not exist build mkdir build
cd build
call cmake ..
call cmake --build . --config Debug
cd ..