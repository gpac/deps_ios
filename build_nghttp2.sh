#!/bin/bash

set -x

pushd .

if [ -d "nghttp2" ];
then

cd nghttp2

autoreconf -i
automake
autoconf

NGHTTP2="${PWD}"
IOS_SDK_VERSION=""
IOS_MIN_SDK_VERSION="8.0"
DEVELOPER=`xcode-select -print-path`

buildIOS() {

	ARCH=$1
	BITCODE=$2

    if [[ "${ARCH}" == "i386" || "${ARCH}" == "x86_64" ]]; then
        PLATFORM="iPhoneSimulator"
    else
        PLATFORM="iPhoneOS"
    fi

    if [[ "${BITCODE}" == "nobitcode" ]]; then
            CC_BITCODE_FLAG=""
    else
            CC_BITCODE_FLAG="-fembed-bitcode"
    fi

	export PLATFORM=$PLATFORM
	export CROSS_TOP="${DEVELOPER}/Platforms/${PLATFORM}.platform/Developer"
	export CROSS_SDK="${PLATFORM}${IOS_SDK_VERSION}.sdk"
	export BUILD_TOOLS="${DEVELOPER}"
	export CC="${BUILD_TOOLS}/usr/bin/gcc"
	export CFLAGS="-arch ${ARCH} -pipe -Os -gdwarf-2 -isysroot ${CROSS_TOP}/SDKs/${CROSS_SDK} -miphoneos-version-min=${IOS_MIN_SDK_VERSION} ${CC_BITCODE_FLAG}"
	export LDFLAGS="-arch ${ARCH} -isysroot ${CROSS_TOP}/SDKs/${CROSS_SDK}"

	echo -e "**Building nghttp2 for ${PLATFORM} ${IOS_SDK_VERSION} ${archbold}${ARCH}${dim} (iOS ${IOS_MIN_SDK_VERSION})"
	if [[ "${ARCH}" == "arm64" || "${ARCH}" == "arm64e"  ]]; then
		./configure --disable-shared --disable-app --disable-threads --enable-lib-only  --prefix="${NGHTTP2}/iOS/${ARCH}" --host="arm-apple-darwin"
	else
		./configure --disable-shared --disable-app --disable-threads --enable-lib-only --prefix="${NGHTTP2}/iOS/${ARCH}" --host="${ARCH}-apple-darwin"
	fi

	make -j3
	make install
	make clean

}

buildIOS "armv7" "nobitcode"
buildIOS "arm64" "nobitcode"
buildIOS "i386" "nobitcode"
buildIOS "x86_64" "nobitcode"
# #buildIOS "arm64e" "nobitcode"


lipo \
	"${NGHTTP2}/iOS/armv7/lib/libnghttp2.a" \
	"${NGHTTP2}/iOS/arm64/lib/libnghttp2.a" \
	"${NGHTTP2}/iOS/i386/lib/libnghttp2.a" \
	"${NGHTTP2}/iOS/x86_64/lib/libnghttp2.a" \
	-create -output "${NGHTTP2}/iOS/libnghttp2.a"

fi

popd

cp -av ${NGHTTP2}/iOS/libnghttp2.a lib/iOS/

set +x