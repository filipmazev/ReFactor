package com.codefu.refactor.web;

import com.codefu.refactor.Calculator.AQICalculator;
import com.codefu.refactor.imgProcessingInterpreter.ImagePipelineClass;
import com.codefu.refactor.model.Responses.SensorData;
import org.springframework.http.*;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.client.RestTemplate;
import java.nio.charset.StandardCharsets;
import java.util.*;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.multipart.MultipartFile;

import java.util.stream.Collectors;


import java.io.IOException;

@RestController
@RequestMapping("/api/aqi")
public class AQIController {

  private final RestTemplate restTemplate;
  private final double LOCATION_RADIUS = 5.0;

  public AQIController(RestTemplate restTemplate) {
    this.restTemplate = restTemplate;
  }

  @GetMapping("/test")
  public ResponseEntity<String> test(){
    return ResponseEntity.ok("Hello World");
  }

    @PostMapping("/predict")
    public ResponseEntity<Void> predictAQI(@RequestParam("image") MultipartFile file) throws IOException {
        if (file.isEmpty()) {
            System.out.println("File is empty");
        }
        byte[] imageBytes = file.getBytes();
        double[] processedImage = ImagePipelineClass.ProcessImage(imageBytes);
        return ResponseEntity.ok().build();
    }

  @PostMapping("/process")
  public ResponseEntity<?> processAQI(
          @RequestParam("photo") MultipartFile file,
          @RequestParam(value = "lat", required = false) Double lat,
          @RequestParam(value = "lon", required = false) Double lon
  ) {
    try {
      Map<String, Double> calculatedAQI = null;

      // Step 1: If location is provided, fetch sensor data
      if (lat != null && lon != null) {
        SensorData[] sensors = fetchSensorsWithAuth();
        if (sensors != null) {
          // Filter sensors within a 5 km radius and calculate averages
          Map<String, Double> sensorAverages = calculateSensorAverages(sensors, lat, lon);
          calculatedAQI = calculateAQI(sensorAverages);
        }
      }

      byte[] imageBytes = file.getBytes();

      // Step 2: Call Flask app for AQI prediction
      String flaskApiUrl = "http://127.0.0.1:5000/predict";
      double[] imageFeatures = ImagePipelineClass.ProcessImage(imageBytes); // Custom logic to extract features
      HttpHeaders headers = new HttpHeaders();
      headers.setContentType(MediaType.APPLICATION_JSON);
      HttpEntity<double[]> requestEntity = new HttpEntity<>(imageFeatures, headers);

      ResponseEntity<Double> flaskResponse = restTemplate.exchange(
              flaskApiUrl,
              HttpMethod.POST,
              requestEntity,
              Double.class
      );

      Double predictedAQI = flaskResponse.getBody();

      // Step 3: Combine results
      Map<String, Object> response = new HashMap<>();
      if (predictedAQI != null) {
        response.put("predictedAQI", predictedAQI);
      }
      if (calculatedAQI != null) {
        response.put("calculatedSensorAQI", calculatedAQI);
      }

      return ResponseEntity.ok(response);

    } catch (Exception e) {
      return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body("Error processing AQI: " + e.getMessage());
    }
  }

  private SensorData[] fetchSensorsWithAuth() {
    // Use java.util.Base64 to encode credentials
    String plainCreds = "userName" + ":" + "userPassword";
    byte[] plainCredsBytes = plainCreds.getBytes(StandardCharsets.UTF_8);
    String base64Creds = Base64.getEncoder().encodeToString(plainCredsBytes);

    HttpHeaders headers = new HttpHeaders();
    headers.add("Authorization", "Basic " + base64Creds);

    HttpEntity<String> requestEntity = new HttpEntity<>(headers);

    String sensorApiUrl = "https://skopje.pulse.eco/rest/current";

    ResponseEntity<SensorData[]> responseEntity = restTemplate.exchange(
            sensorApiUrl,
            HttpMethod.GET,
            requestEntity,
            SensorData[].class
    );

    return responseEntity.getBody();
  }

  private Map<String, Double> calculateSensorAverages(SensorData[] sensors, double userLat, double userLon) {
    // Group sensors by type after filtering by distance
    Map<String, List<SensorData>> sensorGroups = Arrays.stream(sensors)
            .filter(sensor -> isWithinRadius(userLat, userLon, parsePosition(sensor.getPosition()), LOCATION_RADIUS))
            .collect(Collectors.groupingBy(SensorData::getType));

    Map<String, Double> sensorAverages = new HashMap<>();
    for (String type : sensorGroups.keySet()) {
      // Sort sensors by proximity and take the closest 3
      List<SensorData> sensorList = sensorGroups.get(type).stream()
              .sorted(Comparator.comparingDouble(sensor ->
                      calculateDistance(userLat, userLon, parsePosition(sensor.getPosition()))))
              .limit(3)
              .collect(Collectors.toList());

      // Calculate the average of the sensor values
      double average = sensorList.stream()
              .mapToDouble(sensor -> Double.parseDouble(sensor.getValue()))
              .average()
              .orElse(0.0);

      sensorAverages.put(type, average);
    }

    return sensorAverages;
  }

  // Update the isWithinRadius method to use the parsed position
  private boolean isWithinRadius(double userLat, double userLon, double[] sensorPosition, double radiusKm) {
    double distance = calculateDistance(userLat, userLon, sensorPosition);
    return distance <= radiusKm;
  }

  // Calculate the distance between two lat/lon points
  private double calculateDistance(double lat1, double lon1, double[] sensorPosition) {
    double lat2 = sensorPosition[0];
    double lon2 = sensorPosition[1];
    double earthRadiusKm = 6371;

    double dLat = Math.toRadians(lat2 - lat1);
    double dLon = Math.toRadians(lon2 - lon1);
    double a = Math.sin(dLat / 2) * Math.sin(dLat / 2)
            + Math.cos(Math.toRadians(lat1)) * Math.cos(Math.toRadians(lat2))
            * Math.sin(dLon / 2) * Math.sin(dLon / 2);
    double c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
    return earthRadiusKm * c;
  }

  private Map<String, Double> calculateAQI(Map<String, Double> sensorAverages) {
    return AQICalculator.calculateAQI(sensorAverages);
  }

  public static class AQIPrediction {
    private double predictedAqi;

    // Getters and setters
  }

  private double[] parsePosition(String position) {
    String[] parts = position.split(",");
    if (parts.length != 2) {
      throw new IllegalArgumentException("Invalid position format: " + position);
    }
    return new double[]{Double.parseDouble(parts[0]), Double.parseDouble(parts[1])};
  }
}