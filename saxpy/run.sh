xcrun -sdk macosx metal -c saxpy.metal -o MyLibrary.air
xcrun -sdk macosx metallib MyLibrary.air -o default.metallib
clang++ -std=c++17 -stdlib=libc++ -O2 -I./metal-cpp -I/opt/homebrew/opt/openblas/include -fno-objc-arc -framework Metal -framework Foundation -framework MetalKit -framework Accelerate -g -L/opt/homebrew/opt/openblas/lib main.cpp MetalSaxpy.cpp -o benchmark.x
./benchmark.x
python ../common/plot.py data.csv saxpy