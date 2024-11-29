## Compile

```bash
g++ -std=c++17 -fpic -I. -I/System/Library/Frameworks/JavaVM.framework/Headers \
    ./src/main.cpp src/dcp/hazeremoval.cpp ./src/dcp/guidedfilter.cpp ./src/pipeline/imagepipeline.cpp ./src/pipeline/program.cpp \
    -o ImageProcessingPipeline `pkg-config --libs --cflags opencv4`
```

## Compile With Debug

```bash
g++ -DDEBUG -std=c++17 -fpic -I. -I/System/Library/Frameworks/JavaVM.framework/Headers \
    ./src/main.cpp src/dcp/hazeremoval.cpp ./src/dcp/guidedfilter.cpp ./src/pipeline/imagepipeline.cpp ./src/pipeline/program.cpp \
    -o ImageProcessingPipeline `pkg-config --libs --cflags opencv4`
```

## Compile for Function | Process Image Into Byte Array
### This function will convert your image into a byte array and save that result to a given folder. It requires 2 arguments:
#### 1. full image path
#### 2. full output path (where you want to save the result)

```bash
g++ -DIMAGE_TO_BYTE_ARRAY -std=c++17 -fpic -I. -I/System/Library/Frameworks/JavaVM.framework/Headers \
    ./src/main.cpp src/dcp/hazeremoval.cpp ./src/dcp/guidedfilter.cpp ./src/pipeline/imagepipeline.cpp ./src/pipeline/program.cpp \
    -o ImageProcessingPipeline `pkg-config --libs --cflags opencv4`
```

```use
./ImageProcessingPipeline <full_path_to_image> <full_path_to_output_destination>
```

## Compile for Function | Enhanced Metadata from Byte Array
### This function will extract enhanced metadata from a given byte array. The byte array is taken from a provided .txt file, the function required 1 argument:
#### 1. byte array text file full path

```bash
g++ -DBYTE_ARRAY_TO_ENHANCED_METADATA -std=c++17 -fpic -I. -I/System/Library/Frameworks/JavaVM.framework/Headers \
    ./src/main.cpp src/dcp/hazeremoval.cpp ./src/dcp/guidedfilter.cpp ./src/pipeline/imagepipeline.cpp ./src/pipeline/program.cpp \
    -o ImageProcessingPipeline `pkg-config --libs --cflags opencv4`
```

```use
./ImageProcessingPipeline <full_path_to_byte_array_text_file>
```

## Compile for Function | CSV Dataset to Enhanced Metadata CSV dataset
### This function will extract enhanced metadata for all images in a dataset. Please follow the exact structure of the CSV you provide from Mock_Dataset.csv or IND_and_Nep_AQI_Dataset.csv (Found in the data folder). The function requires 3 arguments:
#### 1. full path to input csv
#### 2. full path to folder containing images of dataset
#### 3. full path of ouput csv (where you want to save the result)

```bash
g++ -DCSV_TO_ENHANCED_METADATA -std=c++17 -fpic -I. -I/System/Library/Frameworks/JavaVM.framework/Headers \
    ./src/main.cpp src/dcp/hazeremoval.cpp ./src/dcp/guidedfilter.cpp ./src/pipeline/imagepipeline.cpp ./src/pipeline/program.cpp \
    -o ImageProcessingPipeline `pkg-config --libs --cflags opencv4`
```

```use
./ImageProcessingPipeline <input_csv_full_path> <images_folder_full_path> <output_folder_full_path>
```

## Compile .so Library
```bash
g++ -std=c++17 -shared -fpic \
-I<path_to_java_headers_jni> \
-I<path_to_java_headers_darwin> \
-I<path_to_open_cv_headers> \
./build/native/Java_ImagePipeline_ExtractEnhachedMetadata.cpp ./src/dcp/hazeremoval.cpp ./src/dcp/guidedfilter.cpp ./src/pipeline/imagepipeline.cpp \
-o ./build/java/unix/libImagePipeline.so \
`pkg-config --cflags --libs opencv4`
```

### MacOS
#### replace <username> with your Username on your machine

#### path_to_java_headers_jni = /Users/<username>/Library/Java/JavaVirtualMachines/openjdk-23.0.1/Contents/Home/include 
#### path_to_java_headers_darwin = /Users/<username>/Library/Java/JavaVirtualMachines/openjdk-23.0.1/Contents/Home/include/darwin
#### path_to_open_cv_headers = /opt/homebrew/Cellar/opencv/4.10.0_12/include/opencv4/