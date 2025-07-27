This directory is for pre-compiled libraries (.a, .so).
You can link against libraries in this directory by adding the following to CMakeLists.txt:

target_link_libraries(YourTarget PRIVATE your_lib_name)

