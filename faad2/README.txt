FAAD 2.11.4

git clone https://github.com/FreewareAdvancedAudio/faad2.git
cd faad2
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../../../faad2 -DFAAD_BUILD_CLI=OFF -DBUILD_SHARED_LIBS=OFF -G Ninja
ninja install
