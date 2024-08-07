xcrun -sdk macosx metal -c matrixMultiply.metal -o MyLibrary.air
xcrun -sdk macosx metallib MyLibrary.air -o default.metallib
clang++ -std=c++17 -stdlib=libc++ -I../common/metal-cpp -I/opt/homebrew/opt/openblas/include -fno-objc-arc -framework Metal -framework Foundation -framework MetalKit -framework Accelerate -g  main.cpp MetalMatmul.cpp -o benchmark.x
./benchmark.x
python ../common/plot.py data.csv matmul

