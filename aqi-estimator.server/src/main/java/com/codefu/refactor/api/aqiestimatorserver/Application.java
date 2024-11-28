package com.codefu.refactor.api.aqiestimatorserver;

import com.codefu.refactor.api.aqiestimatorserver.libraryLoader.NativeLibraryLoader;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.annotation.Bean;
import org.springframework.web.client.RestTemplate;

@SpringBootApplication
public class Application {

    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);

        // Load the library
        NativeLibraryLoader.loadLibrary("/native/libImagePipeline.so", "libImagePipeline.so");

        // Use the native library as usual
        System.out.println("Native library loaded successfully.");
    }

    @Bean
    public RestTemplate restTemplate() {
        return new RestTemplate();
    }


}
