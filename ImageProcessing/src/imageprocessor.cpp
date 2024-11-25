#include "imageprocessor.h"

extern "C" {
    // <summary>
    // input_image_path | The path of the input image.
    // output_folder | The output folder where the processed images will be saved.
    // pixel_lower_bound | Pixel lower bound is the minimum pixel value that will be considered in the image processing.
    // histogram_size | Histogram size is the number of bins in the histogram.
    // histogram_range | Histogram range is the range of the histogram.
    // writeHistograms | If true, histograms will be written to the output folder.
    // writeOriginal | If true, the original image will be written to the output folder.
    // writeFogImage | If true, the fog image will be written to the output folder.
    // writeFinalImage | If true, the final image will be written to the output folder.
    // output_buffer | The output buffer where the processed image will be saved.
    // output_size | The size of the output buffer.
    // </summary>
    int process_image(const char* input_image_path, const char* output_folder, int pixel_lower_bound, int histogram_size, 
                      float histogram_range[], bool writeHistograms, bool writeOriginal, bool writeFogImage, bool writeFinalImage,
                      unsigned char** output_buffer, size_t* output_size) 
    {
        try {
            ImagePipeline pipeline(output_folder != nullptr ? output_folder : "", pixel_lower_bound, histogram_size, histogram_range);
            std::vector<unsigned char> processed_image = pipeline.ProcessImage(input_image_path, writeHistograms, writeOriginal, writeFogImage, writeFinalImage);

            if (processed_image.empty()) {
                std::cerr << "Error processing image: " << input_image_path << std::endl;
                return -1; 
            }

            *output_size = processed_image.size();
            *output_buffer = static_cast<unsigned char*>(malloc(*output_size));
            if (*output_buffer == nullptr) {
                std::cerr << "Error allocating memory for output buffer." << std::endl;
                return -2; 
            }

            memcpy(*output_buffer, processed_image.data(), *output_size);
            return 0;
        } catch (...) {
            std::cerr << "Exception occurred while processing image." << std::endl;
            return -3; 
        }
    }
}