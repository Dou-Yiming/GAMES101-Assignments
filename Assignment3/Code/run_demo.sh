mkdir build
mkdir output
cd ./build
cmake ..
make clean
make -j4
./Rasterizer ../output/texture.png texture
./Rasterizer ../output/normal.png normal
./Rasterizer ../output/phong.png phong
./Rasterizer ../output/bump.png bump
./Rasterizer ../output/displacement.png displacement