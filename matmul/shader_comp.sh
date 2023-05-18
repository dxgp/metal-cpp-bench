xcrun -sdk macosx metal -c matrixMultiply.metal -o MyLibrary.air
xcrun -sdk macosx metallib MyLibrary.air -o default.metallib
