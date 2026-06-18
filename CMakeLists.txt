cmake_minimum_required(VERSION 3.30)
project(computimg)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#OpenMP (лаб 1, 2, 3, 5)

set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
set(OpenMP_CXX_LIB_NAMES "omp")
set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/libomp/lib/libomp.dylib")
include_directories("/opt/homebrew/opt/libomp/include")
find_package(OpenMP COMPONENTS CXX REQUIRED)

#MPI (лаб 4)
find_package(MPI REQUIRED)

# лаб4
add_executable(lab4_task1 lab4/task1.cpp)
target_link_libraries(lab4_task1 PRIVATE MPI::MPI_CXX)


# лаб5
add_executable(lab5_task1 lab5/task1.cpp)
target_link_libraries(lab5_task1 PRIVATE OpenMP::OpenMP_CXX)


# лаб3
add_executable(lab3_task1 lab3/task1.cpp)
target_link_libraries(lab3_task1 PRIVATE OpenMP::OpenMP_CXX)

add_executable(lab3_task2 lab3/task2.cpp)
target_link_libraries(lab3_task2 PRIVATE OpenMP::OpenMP_CXX)

# лаб2
add_executable(lab2_task1 lab2/task1.cpp)
target_link_libraries(lab2_task1 PRIVATE OpenMP::OpenMP_CXX)

add_executable(lab2_task2 lab2/task2.cpp)
target_link_libraries(lab2_task2 PRIVATE OpenMP::OpenMP_CXX)

add_executable(lab2_task3 lab2/task3.cpp)
target_link_libraries(lab2_task3 PRIVATE OpenMP::OpenMP_CXX)


# лаб1
add_executable(lab1_task1 lab1/task1.cpp)
target_link_libraries(lab1_task1 PRIVATE OpenMP::OpenMP_CXX)

add_executable(lab1_task2 lab1/task2.cpp)
target_link_libraries(lab1_task2 PRIVATE OpenMP::OpenMP_CXX)