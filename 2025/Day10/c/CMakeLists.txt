cmake_minimum_required(VERSION 3.10)

# set the project name
project(solver)

set(C_STANDARD 90)

if (MSVC)
    add_compile_options(/O2)
else()
    add_compile_options(-o3)
endif()

# add the executable
add_executable(solver solver.c)