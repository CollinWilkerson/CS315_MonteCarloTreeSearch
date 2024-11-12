cmake_minimum_required(VERSION 3.1)

project(MCTS)

file(GLOB_RECURSE sources src/*.c src/*.h)

add_executable(MonteCarlo ${sources})
