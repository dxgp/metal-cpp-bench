xcrun -sdk macosx metal -c dot.metal -o MyLibrary.air
xcrun -sdk macosx metallib MyLibrary.air -o default.metallib
clang++ -std=c++17 -stdlib=libc++ -O2 -I./metal-cpp -fno-objc-arc -framework Metal -framework Foundation -framework MetalKit -framework Accelerate -g main.cpp MetalDot.cpp -o benchmark.x