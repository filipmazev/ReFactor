package com.codefu.refactor.api.aqiestimatorserver;

import com.codefu.refactor.api.aqiestimatorserver.imageProcessor.ImagePipelineClass;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.annotation.Bean;
import org.springframework.web.client.RestTemplate;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;

@SpringBootApplication
public class Application {

    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);

        // Path to the image file
        Path imagePath = Paths.get("C:/Users/filip.mazev/Downloads/BENGR_Good_2023-02-19-08.30-1-1.jpg"); // Replace with the actual image path

        try {
            // Read the image file into a byte array
            byte[] imageBytes = Files.readAllBytes(imagePath);

            // Pass the byte array to the ProcessImage method
            double[] test = ImagePipelineClass.ProcessImage(imageBytes);
            System.out.println("Processed metadata: " + Arrays.toString(test));
        } catch (Exception e) {
            System.err.println("Failed to process image: " + e.getMessage());
            e.printStackTrace();
        }
    }

    @Bean
    public RestTemplate restTemplate() {
        return new RestTemplate();
    }
}
