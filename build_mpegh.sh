#!/bin/bash

set -x

pushd .

if [ -d "mpeghdec" ];
then

cd mpeghdec

sed -i'.bak' -e 's/git@github.com:/https:\/\/github.com\//'  CMakeLists.txt



mpeghdec="${PWD}"
IOS_SDK_VERSION=""
IOS_MIN_SDK_VERSION="8.0"
DEVELOPER=`xcode-select -print-path`

buildIOS() {

	ARCH=$1

    if [[ "${ARCH}" == "i386" || "${ARCH}" == "x86_64" ]]; then
        PLATFORM="iPhoneSimulator"
		SDK_TO_USE="iphonesimulator"
    else
        PLATFORM="iPhoneOS"
		SDK_TO_USE="iphoneos"
    fi

	mkdir -p build_$ARCH

	cat << EOF > build_$ARCH/ios_clang_$ARCH.cmake
	set(CMAKE_SYSTEM_NAME "iOS")
	set(IOS TRUE)

	# https://gitlab.kitware.com/cmake/cmake/-/issues/19655
	set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

	set(SDK_TO_USE $SDK_TO_USE)
	set(CMAKE_OSX_ARCHITECTURES $ARCH CACHE STRING "OSX Architectures")
	set(CMAKE_OSX_DEPLOYMENT_TARGET 9.0 CACHE STRING "OSX Deployment target")

	if(NOT "\${CMAKE_GENERATOR}" MATCHES "Xcode")
	message(FATAL_ERROR "Only Xcode as generator for iOS builds supported")
	endif()

	execute_process(COMMAND xcodebuild -version -sdk \${SDK_TO_USE} Path
	OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
	ERROR_QUIET
	OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	if(NOT DEFINED CMAKE_OSX_SYSROOT)
	message(FATAL_ERROR "Please make sure Xcode is installed")
	endif()
EOF


    if [[ "${ARCH}" == "armv7"  ]]; then
        echo 'set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -marm" CACHE STRING "c++ flags")' >> build_$ARCH/ios_clang_$ARCH.cmake
		echo 'set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -marm" CACHE STRING "c flags")' >> build_$ARCH/ios_clang_$ARCH.cmake

		sed -i'.bak' -e 's/__arm__/__thumb__/'  src/libMpeghDec/src/stereo.cpp src/libFDK/src/scale.cpp


    fi


	echo -e "**Building mpeghdec for ${PLATFORM} ${IOS_SDK_VERSION} ${ARCH}"

	cmake -S . -B build_$ARCH -G Xcode -Dmpeghdec_BUILD_BINARIES=OFF -DCMAKE_TOOLCHAIN_FILE=build_$ARCH/ios_clang_$ARCH.cmake
	cmake --build build_$ARCH --config Release

	libtool -static -o build_$ARCH/lib/libmpegh.a build_$ARCH/lib/Release/*.a

}

buildIOS "armv7"
buildIOS "arm64"
buildIOS "i386"
buildIOS "x86_64"
# #buildIOS "arm64e" "nobitcode"

lipo build_*/lib/*.a  -create -output ../lib/iOS/libmpegh.a

# lipo \
# 	"${NGHTTP2}/iOS/armv7/lib/libnghttp2.a" \
# 	"${NGHTTP2}/iOS/arm64/lib/libnghttp2.a" \
# 	"${NGHTTP2}/iOS/i386/lib/libnghttp2.a" \
# 	"${NGHTTP2}/iOS/x86_64/lib/libnghttp2.a" \
# 	-create -output "${NGHTTP2}/iOS/libnghttp2.a"

fi

popd

# cp -av ${NGHTTP2}/iOS/libnghttp2.a lib/iOS/

set +x