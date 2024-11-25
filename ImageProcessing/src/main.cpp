#include "imagepipeline.h"

const std::string OUTPUT_FOLDER_DESTINATION = 
#if DEBUG
	"/Users/filipmazev/Documents/Code/ReFactor/ImageProcessing/output/";
#else 
	"";
#endif

int main(int argc, char **args) 
{
	if (argc < 2) {
		std::cerr << "Usage: " << args[0] << " <images_folder>" << std::endl;
		return -1;
	}

	std::string INPUT_FOLDER_PATH = args[1];
	std::vector<std::string> imagePaths = fs_get_image_paths_from_folder(INPUT_FOLDER_PATH);

    int pixel_lower_bound = 30;
    int histogram_size = 64;
    float histogram_range[2] = {0, 256};

    ImagePipeline pipeline = ImagePipeline(OUTPUT_FOLDER_DESTINATION, pixel_lower_bound, histogram_size, histogram_range);

	for (const auto& INPUT_IMAGE_PATH : imagePaths) {
		std::vector<double> result = pipeline.ExtractEnhancedMetadata(INPUT_IMAGE_PATH);
		if(result.empty()) {
			std::cerr << "Error processing image: " << INPUT_IMAGE_PATH << std::endl;
		} else {
			std::cout << "Image processed successfully: " << INPUT_IMAGE_PATH << std::endl;
			for(const auto& feature : result) {
				std::cout << feature << " ";
			}
			std::cout << std::endl;
		}
	}
	
	return 0;
}