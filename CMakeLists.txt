cmake_minimum_required(VERSION 3.1)

project(MCTS)

file(GLOB_RECURSE sources src/*.c src/*.h)

# Exclude agentRandom.c and agentRandom.h if present
list(FILTER sources EXCLUDE REGEX ".*agentRandom.c$")
list(FILTER sources EXCLUDE REGEX ".*agentRandom.h$")

# Add agentC.c and agentC.h explicitly if needed
set(sources ${sources} src/agentC.c src/agentC.h)

add_executable(MonteCarlo ${sources})
