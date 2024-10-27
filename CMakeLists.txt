cmake_minimum_required(VERSION 3.1)

project(MCTS)

file(GLOB_RECURSE sources src/*.c)

add_executable(MonteCarlo ${sources})
