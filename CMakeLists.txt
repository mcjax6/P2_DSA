cmake_minimum_required(VERSION 3.20)
project(P2_DSA)

set(CMAKE_CXX_STANDARD 17)

add_executable(P2_DSA main.cpp SplayTree.cpp SplayTree.h Competitor.h
        ListProcessor.cpp
        ListProcessor.h
        RedBlackTree.cpp
        RedBlackTree.h
)
