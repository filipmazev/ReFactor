package com.codefu.refactor.api.aqiestimatorserver.libraryLoader;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.nio.file.Files;

public class NativeLibraryLoader {

    public static void loadLibrary(String libraryPath, String libraryName) {
        try {
            // Create a temporary directory
            File tempDir = Files.createTempDirectory("native-libs").toFile();
            tempDir.deleteOnExit();

            // Extract the library from resources
            try (InputStream inputStream = NativeLibraryLoader.class.getResourceAsStream(libraryPath);
                 FileOutputStream outputStream = new FileOutputStream(new File(tempDir, libraryName))) {

                if (inputStream == null) {
                    throw new IllegalArgumentException("Library not found in resources: " + libraryPath);
                }

                byte[] buffer = new byte[1024];
                int bytesRead;
                while ((bytesRead = inputStream.read(buffer)) != -1) {
                    outputStream.write(buffer, 0, bytesRead);
                }
            }

            // Load the extracted library
            System.load(new File(tempDir, libraryName).getAbsolutePath());
        } catch (Exception e) {
            throw new RuntimeException("Failed to load native library", e);
        }
    }
}
