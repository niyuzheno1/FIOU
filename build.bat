call cmake .
call cmake --build . --config Debug
call cmake --build  . --target post_compilation_validation_windows
