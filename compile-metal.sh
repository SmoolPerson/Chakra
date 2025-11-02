echo "This program will fail to work if the XCode app is not installed."
xcrun -sdk macosx metal -c src/metal/Shaders.metal -o build/Shaders.air
xcrun -sdk macosx metal build/Shaders.air -o build/Shaders.metallib
