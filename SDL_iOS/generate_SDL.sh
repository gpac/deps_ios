#!/bin/sh


#SDL2 and GLUES generation
if [ ! -z "$1" ] ; then 
  echo "*** Compile for Simulator (i386) ***"
  xcodebuild -target libSDL -sdk iphonesimulator -configuration Release -project SDL2/Xcode-iOS/SDL/SDL.xcodeproj
  xcodebuild -target glues -sdk iphonesimulator -configuration Release -project glues/ios/glues/glues.xcodeproj
fi

echo "*** Compile for iOS (arm) ***"
xcodebuild -target libSDL -sdk iphoneos -configuration Release -project SDL2/Xcode-iOS/SDL/SDL.xcodeproj
xcodebuild -target glues -sdk iphoneos -configuration Release -project glues/ios/glues/glues.xcodeproj

echo "*** Create the universal library and copy the generated lib ***"
if [ ! -z "$1" ] ; then
  lipo -create -output ../lib/iOS/libSDL2.a SDL2/Xcode-iOS/SDL/build/Release-iphoneos/libSDL2.a SDL2/Xcode-iOS/SDL/build/Release-iphonesimulator/libSDL2.a
  lipo -create -output ../lib/iOS/libglues.a glues/ios/glues/build/Release-iphoneos/libglues.a glues/ios/glues/build/Release-iphonesimulator/libglues.a
else
  cp SDL2/Xcode-iOS/SDL/build/Release-iphoneos/libSDL2.a ../lib/iOS/libSDL2.a
  cp glues/ios/glues/build/Release-iphoneos/libglues.a ../lib/iOS/libglues.a
fi


echo "*** SDL generation for iOS completed! ***"
echo 
