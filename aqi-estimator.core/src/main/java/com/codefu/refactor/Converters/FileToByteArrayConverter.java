package com.codefu.refactor.Converters;

import com.codefu.refactor.imgProcessingInterpreter.ImagePipelineClass;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.ByteArrayOutputStream;
import java.util.Arrays;

public class FileToByteArrayConverter {

    public static byte[] fileToByteArray(String filePath) throws IOException {
        File file = new File(filePath);
        FileInputStream fileInputStream = new FileInputStream(file);
        ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();

        byte[] buffer = new byte[1024];
        int bytesRead;

        // Read file and write to byte array output stream
        while ((bytesRead = fileInputStream.read(buffer)) != -1) {
            byteArrayOutputStream.write(buffer, 0, bytesRead);
        }

        fileInputStream.close();

        // Return byte array
        return byteArrayOutputStream.toByteArray();
    }

    public static void main(String[] args) {
        try {
            String filePath = "C:\\Users\\Marko\\Downloads\\BENGR_Good_2023-02-19-08.30-1-2.jpg"; // Change this to your file path
            byte[] byteArray = fileToByteArray(filePath);


            double[] results = ImagePipelineClass.ProcessImage(byteArray);

            System.out.println("Results: " + Arrays.toString(Arrays.stream(results).toArray()));
            // Print the byte array size
            System.out.println("Byte array size: " + byteArray.length);

            // Optionally, print the first few bytes
            System.out.println("First few bytes: " + byteArray[0] + ", " + byteArray[1] + ", " + byteArray[2]);


        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
