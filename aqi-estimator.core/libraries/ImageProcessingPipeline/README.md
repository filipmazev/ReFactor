## Compile

```bash
g++ -std=c++17 -fpic -I. -I/System/Library/Frameworks/JavaVM.framework/Headers \
    main.cpp Pipeline/DCP/hazeremoval.cpp Pipeline/DCP/guidedfilter.cpp Pipeline/imagepipeline.cpp program.cpp \
    -o ../ImageProcessingPipeline `pkg-config --libs --cflags opencv4`
```

## Compile With Debug

```bash
g++ -DDEBUG -std=c++17 -fpic -I. -I/System/Library/Frameworks/JavaVM.framework/Headers \
    main.cpp Pipeline/DCP/hazeremoval.cpp Pipeline/DCP/guidedfilter.cpp Pipeline/imagepipeline.cpp program.cpp \
    -o ../ImageProcessingPipeline `pkg-config --libs --cflags opencv4`
```

## Compile .so Library
```bash
g++ -std=c++17 -shared -fpic \
-I<path_to_java_headers_jni> \
-I<path_to_java_headers_darwin> \
-I<path_to_open_cv_headers> \
Java/Java_ImagePipeline_ExtractEnhachedMetadata.cpp Pipeline/DCP/hazeremoval.cpp Pipeline/DCP/guidedfilter.cpp Pipeline/imagepipeline.cpp \
-o Java/libImagePipeline.so \
`pkg-config --cflags --libs opencv4`

g++ -std=c++17 -shared -fpic -I<path_to_java_headers> -I<path_to_open_cv_headers> -o \ 
libImagePipeline.so Java/Java_ImagePipeline_ExtractEnhachedMetadata.cpp `pkg-config --cflags --libs opencv4`
```

```MacOS
replace <username> with your Username on your machine

path_to_java_headers_jni = /Users/<username>/Library/Java/JavaVirtualMachines/openjdk-23.0.1/Contents/Home/include 
path_to_java_headers_darwin = /Users/<username>/Library/Java/JavaVirtualMachines/openjdk-23.0.1/Contents/Home/include/darwin
path_to_open_cv_headers = /opt/homebrew/Cellar/opencv/4.10.0_12/include/opencv4/
```