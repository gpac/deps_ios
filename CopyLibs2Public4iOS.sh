#!/bin/sh

#rm -rf ../gpac_public/extra_lib/include/ios/
mkdir -p ../gpac_public/extra_lib/include/ios/

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


cp -av libmad/msvc++/mad.h ../gpac_public/extra_lib/include/ios/
cp -av faad2/include/faad.h faad2/include/neaacdec.h ../gpac_public/extra_lib/include/ios/
mkdir -p  ../gpac_public/extra_lib/include/freetype
cp -av freetype/include/* ../gpac_public/extra_lib/include/freetype/
mkdir -p ../gpac_public/extra_lib/include/png
cp -av libpng/png*.h ../gpac_public/extra_lib/include/png
mkdir -p ../gpac_public/extra_lib/include/jpeg
cp -av libjpg/{jconfig,jmorecfg,jpeglib}.h ../gpac_public/extra_lib/include/jpeg

cp -a include/nghttp2 ../gpac_public/extra_lib/include/ios/

cp -av libcaption/caption ../gpac_public/extra_lib/include/ios/

cp -av mpeghdec/include/mpeghdecoder.h ../gpac_public/extra_lib/include/ios/