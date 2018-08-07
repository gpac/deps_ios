#!/bin/sh

rm -rf ../gpac_public/extra_lib/include/ios/
mkdir ../gpac_public/extra_lib/include/ios/

mkdir -p ../gpac_public/extra_lib/include/ios/SDL2/
cp SDL_iOS/SDL2/include/*.h ../gpac_public/extra_lib/include/ios/SDL2/
cp SDL_iOS/glues/source/glues.h ../gpac_public/extra_lib/include/ios/
mkdir -p ../gpac_public/extra_lib/lib/iOS/
cp lib/iOS/*.a ../gpac_public/extra_lib/lib/iOS/

#replace FFMPEG headers in gpac public
cp -r ffmpeg_ios/FFmpeg-iOS/include/* ../gpac_public/extra_lib/include/ios/

#copy FFMPEG binaries
mkdir -p ./gpac_public/extra_lib/lib/iOS/
cp ffmpeg_ios/FFmpeg-iOS/lib/* ../gpac_public/extra_lib/lib/iOS/

#copy SSL libraries
cp -r OpenSSL-for-iPhone/include/* ../gpac_public/extra_lib/include/ios/
cp OpenSSL-for-iPhone/lib/*.a ../gpac_public/extra_lib/lib/iOS/


cp ../gpac_public/extra_lib/include/mad.h ../gpac_public/extra_lib/include/ios/
cp ../gpac_public/extra_lib/include/faad.h ../gpac_public/extra_lib/include/ios/
cp ../gpac_public/extra_lib/include/neaacdec.h ../gpac_public/extra_lib/include/ios/
