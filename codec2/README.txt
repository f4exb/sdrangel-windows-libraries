Codec2 1.2.0 ported to MSVC

https://github.com/srcejon/codec2
cmake -S . -B build-msvc -G Ninja -DUNITTEST=OFF -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=C:\Users\jon\source\repos\sdrangel-windows-libraries\codec2
cmake --install build-msvc --config Release

Note - dll currently in lib dir to match old layout, until SDRangel updated to point to bin.