## Compile

```bash
g++ -DDEBUG -std=c++17 -fpic -I. -I/System/Library/Frameworks/JavaVM.framework/Headers \
    main.cpp Pipeline/DCP/hazeremoval.cpp Pipeline/DCP/guidedfilter.cpp Pipeline/GLCM/glcm.cpp Pipeline/imagepipeline.cpp program.cpp \
    -o ../ImageProcessingPipeline `pkg-config --libs --cflags opencv4`
```

##