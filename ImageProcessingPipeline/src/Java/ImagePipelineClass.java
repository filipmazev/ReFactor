import java.nio.file.Files;
import java.nio.file.Paths;

public class ImagePipelineClass {
    static {
        System.loadLibrary("libImagePipeline.so"); 
    }

    public native double[] ExtractEnhancedMetadata(byte[] inputImageBytes);

    public static double[] ProcessImage(byte[] imageBytes) {
        try {
            ImagePipelineClass processor = new ImagePipelineClass();
            double[] metadata = processor.ExtractEnhancedMetadata(imageBytes);

            return metadata;
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}