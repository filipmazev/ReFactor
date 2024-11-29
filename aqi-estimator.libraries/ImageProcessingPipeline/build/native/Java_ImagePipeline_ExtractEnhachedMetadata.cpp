#include "../../includes/pipeline/imagepipeline.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <jni.h>

extern "C" JNIEXPORT jdoubleArray JNICALL Java_ImagePipelineClass_ExtractEnhancedMetadata(JNIEnv* env, jobject obj, jbyteArray inputImageBytes) {
    jsize byteArrayLength = env->GetArrayLength(inputImageBytes);
    jbyte* byteArray = env->GetByteArrayElements(inputImageBytes, nullptr);

    std::vector<uchar> imageData(byteArray, byteArray + byteArrayLength);
    cv::Mat in_img = cv::imdecode(imageData, cv::IMREAD_COLOR);
    env->ReleaseByteArrayElements(inputImageBytes, byteArray, JNI_ABORT);

    if (in_img.empty()) {
        throw std::runtime_error("Error: Failed to decode image from byte array.");
    }

    int image_pixels_w_max = 896;
    int image_pixels_h_max = 896;

    int pixel_lower_bound = 30;
    int histogram_size = 64;
    float histogram_range[2] = {0, 256};
    int grayLevels = 256;

    ImagePipeline pipeline = ImagePipeline(image_pixels_w_max, image_pixels_h_max, pixel_lower_bound, histogram_size, histogram_range, grayLevels);
    std::vector<double> concatenated_features = pipeline.ExtractEnhancedMetadata(in_img);

    jdoubleArray resultArray = env->NewDoubleArray(concatenated_features.size());
    env->SetDoubleArrayRegion(resultArray, 0, concatenated_features.size(), concatenated_features.data());

    return resultArray;
}