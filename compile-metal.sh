xcrun -sdk macosx metal -c src/metal/Shaders.metal -o build/Shaders.air
xcrun -sdk macosx metal build/Shaders.air -o build/Shaders.metallib