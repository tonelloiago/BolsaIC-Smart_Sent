cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SimGridTemplateProject) # TODO: give a real name to your project here

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(SimGrid 3.22 REQUIRED) # This template requires SimGrid v3.22


include_directories(
	${SimGrid_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/tarefa_3
        ${PROJECT_SOURCE_DIR}/includes
)

file(GLOB all_SRCS
        "${PROJECT_SOURCE_DIR}/tarefa_3/*.cpp"       
        )

add_executable(main ${all_SRCS})
target_link_libraries(main ${SimGrid_LIBRARY})






