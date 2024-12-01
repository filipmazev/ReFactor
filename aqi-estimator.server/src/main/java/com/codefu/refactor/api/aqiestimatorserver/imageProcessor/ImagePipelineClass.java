package com.codefu.refactor.api.aqiestimatorserver.imageProcessor;

public class ImagePipelineClass {
    static {
        System.loadLibrary("ImageProcessingPipeline");
    }

    public static native double[] ExtractEnhancedMetadata(byte[] imageBytes);

    public static double[] ProcessImage(byte[] imageBytes) {
        try {
            ImagePipelineClass processor = new ImagePipelineClass();
            double[] metadata = processor.ExtractEnhancedMetadata(imageBytes);

            return metadata;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return new double[0];
    }
}