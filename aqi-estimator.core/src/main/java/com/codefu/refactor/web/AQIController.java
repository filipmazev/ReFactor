package com.codefu.refactor.web;

import com.codefu.refactor.imgProcessingInterpreter.ImagePipelineClass;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.multipart.MultipartFile;

import java.io.IOException;

@RestController
public class AQIController {

  @PostMapping("/predict")
  public ResponseEntity<Void> predictAQI(@RequestParam("image") MultipartFile file) throws IOException {
    if (file.isEmpty()) {
      System.out.println("File is empty");
    }
    byte[] imageBytes = file.getBytes();
    double[] processedImage = ImagePipelineClass.ProcessImage(imageBytes);
    return ResponseEntity.ok().build();
  }
}