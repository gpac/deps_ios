#!/bin/sh

cd ../../build/xcode_ios

if [ ! -z "$1" ] ; then
  echo "*** Cleaning for Simulator (i386) ***"
  xcodebuild -alltargets -parallelizeTargets -sdk iphonesimulator -configuration Release -project extra_libs.xcodeproj clean
  echo "*** Compile for Simulator (i386) ***"
  xcodebuild -alltargets -parallelizeTargets -sdk iphonesimulator -configuration Release -project extra_libs.xcodeproj
  if [ $? != 0 ] ; then
	exit 1
  fi
fi

echo "*** Cleaning for iOS (arm) ***"
xcodebuild -alltargets -parallelizeTargets -sdk iphoneos -configuration Release -project extra_libs.xcodeproj clean
echo "*** Compile for iOS (arm) ***"
xcodebuild -alltargets -parallelizeTargets -sdk iphoneos -configuration Release -project extra_libs.xcodeproj
if [ $? != 0 ] ; then
	exit 1
fi


echo "*** Create the universal library and copy the generated libs ***"
cd build/Release-iphoneos
mkdir -p ../../../../lib/iOS
for i in $(ls *.a); do
  echo item: $i
  if [ ! -z "$1" ] ; then
    lipo -create -output ../../../../lib/iOS/$i $i ../Release-iphonesimulator/$i
  else
    cp $i ../../../../lib/iOS/$i
  fi
done
if [ $? != 0 ] ; then
	exit 1
fi
cd ../..


