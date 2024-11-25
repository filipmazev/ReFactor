#include "imagepipeline.h"

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

    ImagePipeline pipeline = ImagePipeline(pixel_lower_bound, histogram_size, histogram_range);

	for (const auto& INPUT_IMAGE_PATH : imagePaths) {
		bool result = pipeline.ProcessImage(INPUT_IMAGE_PATH, true, true, true);
		if(!result) {
			std::cerr << "Error processing image: " << INPUT_IMAGE_PATH << std::endl;
		} else {
			std::cout << "Image processed successfully: " << INPUT_IMAGE_PATH << std::endl;
		}
	}
	
	return 0;
}