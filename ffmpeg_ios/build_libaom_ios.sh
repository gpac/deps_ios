#!/bin/sh

set -x

mkdir aom-head
cd aom-head

if [ ! -d aom ]; then
   git clone https://aomedia.googlesource.com/aom
fi

mkdir arm64-ios armv7-ios armv7s-ios x86-ios x86_64-ios

IOS_TARGET="9.0"
ARM_FLAGS=" -mios-version-min=$IOS_TARGET "
X86_FLAGS=" -mios-simulator-version-min=$IOS_TARGET "

cd arm64-ios  ; CFLAGS="$ARM_FLAGS" cmake ../aom -DCMAKE_TOOLCHAIN_FILE=../aom/build/cmake/toolchains/arm64-ios.cmake && make aom ; cd ..
cd armv7-ios  ; CFLAGS="$ARM_FLAGS" cmake ../aom -DCMAKE_TOOLCHAIN_FILE=../aom/build/cmake/toolchains/armv7-ios.cmake && make aom ; cd ..
cd armv7s-ios ; CFLAGS="$ARM_FLAGS" cmake ../aom -DCMAKE_TOOLCHAIN_FILE=../aom/build/cmake/toolchains/armv7s-ios.cmake && make aom ; cd ..
cd x86-ios    ; CFLAGS="$X86_FLAGS" cmake ../aom -DCMAKE_TOOLCHAIN_FILE=../aom/build/cmake/toolchains/x86-ios-simulator.cmake && make aom ; cd ..
cd x86_64-ios ; CFLAGS="$X86_FLAGS" cmake ../aom -DCMAKE_TOOLCHAIN_FILE=../aom/build/cmake/toolchains/x86_64-ios-simulator.cmake && make aom ; cd ..

mkdir -p aom/lib
lipo -create ./arm64-ios/libaom.a ./armv7-ios/libaom.a ./armv7s-ios/libaom.a ./x86-ios/libaom.a ./x86_64-ios/libaom.a -output aom/lib/libaom.a

p=$(pwd)/aom

cat <<EOF > ../aom.pc
# libaom pkg-config.
prefix=$p
exec_prefix=$p/bin
libdir=$p/lib
includedir=$p

Name: aom
Description: AV1 codec library
Version: 1.0.0
Requires:
Conflicts:
Libs: -L$p/lib -laom -lm -lpthread
Libs.private: -lm -lpthread
Cflags: -I$p
EOF


set +x
