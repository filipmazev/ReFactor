#include <cstddef>
#include "imagepipeline.h"

extern "C" {
    int process_image(const char* input_image_path, const char* output_folder, int pixel_lower_bound, int histogram_size, 
                      float histogram_range[], bool writeHistograms, bool writeOriginal, bool writeFogImage, bool writeFinalImage,
                      unsigned char** output_buffer, size_t* output_size);
}