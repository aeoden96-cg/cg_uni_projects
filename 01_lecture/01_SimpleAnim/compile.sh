#!/bin/bash
mkdir -p build
g++ -o build/SimpleAnim src/SimpleAnim.cpp src/util.cpp -lGLEW -lGL -lGLU -lglut -lpthread
cp src/SimpleFragmentShader.fragmentshader build/
cp src/SimpleVertexShader.vertexshader build/
