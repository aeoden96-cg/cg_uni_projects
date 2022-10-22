#!/bin/bash
# rm -rf build/

find . -type d -name build -prune -exec rm -rf {} \;
find . -type d -name cmake-build-debug -prune -exec rm -rf {} \;



