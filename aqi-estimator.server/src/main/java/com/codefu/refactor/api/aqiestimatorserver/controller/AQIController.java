package com.codefu.refactor.api.aqiestimatorserver.controller;
import com.codefu.refactor.api.aqiestimatorserver.DTO.SensorData;
import com.codefu.refactor.api.aqiestimatorserver.DTO.clientRequest.AQIPredictionRequest;
import com.codefu.refactor.api.aqiestimatorserver.DTO.clientResponses.AQIPredictionResponse;
import com.codefu.refactor.api.aqiestimatorserver.DTO.clientResponses.SensorCalculatedAqiAverages;
import com.codefu.refactor.api.aqiestimatorserver.enums.AQICategory;
import com.codefu.refactor.api.aqiestimatorserver.imageProcessor.ImagePipelineClass;
import com.codefu.refactor.api.aqiestimatorserver.service.inter.IAqiPredictionService;
import com.codefu.refactor.api.aqiestimatorserver.service.inter.IPulseEcoService;
import org.springframework.http.*;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.client.RestTemplate;

import java.time.LocalTime;
import java.util.*;

@RestController
@RequestMapping("/api")
public class AQIController {

    private final IPulseEcoService pulseEcoService;
    private final IAqiPredictionService aqiPredictionService;

    public AQIController(IPulseEcoService pulseEcoService, IAqiPredictionService aqiPredictionService) {
        this.pulseEcoService = pulseEcoService;
        this.aqiPredictionService = aqiPredictionService;
    }

    @GetMapping("/test-pulse-eco")
    public List<SensorData> pulseEco() {
        String plainCreds = "refactorTeam" + ":" + "refactorAdmin";
        String base64Credentials = new String(Base64.getEncoder().encode(plainCreds.getBytes()));

        HttpHeaders headers = new HttpHeaders();
        headers.add("Authorization", "Basic " + base64Credentials);
        RestTemplate restTemplate = new RestTemplate();
        var requestEntity = new HttpEntity(headers);

        SensorData[] output = restTemplate.exchange("https://skopje.pulse.eco/rest/current",
                HttpMethod.GET, requestEntity, SensorData[].class).getBody();
        assert output != null;
        return Arrays.asList(output);
    }

    @GetMapping("/sensors-aqi-calculation-by-type")
    public ResponseEntity<List<SensorCalculatedAqiAverages>> getSensorsAverages(){
        double userLat = 41.98390993402405;
        double userLon = 21.451233146520536;

        List<SensorCalculatedAqiAverages> result = this.pulseEcoService.calculateSensorsAQI("skopje", userLat, userLon);
        if(!result.isEmpty()){
            return ResponseEntity.ok(result);
        }
        return ResponseEntity.notFound().build();
    }

    @PostMapping("/process")
    public ResponseEntity<AQIPredictionResponse> process(@RequestBody AQIPredictionRequest request){
        Double userLat = request.getLat();
        Double userLon = request.getLon();

        // Prepare the response
        AQIPredictionResponse result = new AQIPredictionResponse();

        if(userLat != null && userLon != null)
        {
            List<SensorCalculatedAqiAverages> aqiCalculatedAverages = this.pulseEcoService.calculateSensorsAQI("skopje", userLat, userLon);

            // If no averages are found, return 404
            if (aqiCalculatedAverages == null || aqiCalculatedAverages.isEmpty()) {
                return ResponseEntity.notFound().build();
            }

            // Find the highest value in the averages list
            SensorCalculatedAqiAverages highestAverage = aqiCalculatedAverages.stream()
                    .max(Comparator.comparingDouble(SensorCalculatedAqiAverages::getAverageAQIValue)) // Assuming getAqiValue() gives the AQI value
                    .orElseThrow(() -> new RuntimeException("Failed to find the highest AQI value"));

            result.setSensorCalculationHighestAqi(highestAverage);
        }

        double[] imageFeatures;
        try {
            imageFeatures = ImagePipelineClass.ProcessImage(request.getIndata());
        } catch (Exception e) {
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body(null);
        }

        // Predict AQI using the model
        Integer modelPrediction = this.aqiPredictionService.predict(imageFeatures);
        result.setModelPredictionValue(modelPrediction);

        // Map AQI value to category using enum
        AQICategory aqiCategory = AQICategory.fromValue(modelPrediction);
        result.setAqiCategory(aqiCategory);

        // Return the response
        return ResponseEntity.ok(result);
    }
}
