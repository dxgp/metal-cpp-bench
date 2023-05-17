xcrun -sdk macosx metal -c dot.metal -o MyLibrary.air
xcrun -sdk macosx metallib MyLibrary.air -o default.metallib
clang++ -std=c++17 -stdlib=libc++ -O2 -I../common/metal-cpp -I/opt/homebrew/opt/openblas/include -fno-objc-arc -framework Metal -framework Foundation -framework MetalKit -framework Accelerate -g -L/opt/homebrew/opt/openblas/lib main.cpp MetalDeMV.cpp -o benchmark.x
./benchmark.x
