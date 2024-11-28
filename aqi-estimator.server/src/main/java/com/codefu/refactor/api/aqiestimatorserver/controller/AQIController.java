package com.codefu.refactor.api.aqiestimatorserver.controller;
import com.codefu.refactor.api.aqiestimatorserver.DTO.SensorData;
import com.codefu.refactor.api.aqiestimatorserver.DTO.clientResponses.AQIPredictionResponse;
import com.codefu.refactor.api.aqiestimatorserver.DTO.clientResponses.SensorCalculatedAqiAverages;
import com.codefu.refactor.api.aqiestimatorserver.imageProcessor.ImagePipelineClass;
import com.codefu.refactor.api.aqiestimatorserver.service.inter.IAqiPredictionService;
import com.codefu.refactor.api.aqiestimatorserver.service.inter.IPulseEcoService;
import org.springframework.http.*;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.client.RestTemplate;
import java.util.Arrays;
import java.util.Base64;
import java.util.Comparator;
import java.util.List;

@RestController
@RequestMapping("/api")
public class AQIController {

    private final RestTemplate restTemplate;
    private final IPulseEcoService pulseEcoService;
    private final IAqiPredictionService aqiPredictionService;

    public AQIController(RestTemplate restTemplate, IPulseEcoService pulseEcoService, IAqiPredictionService aqiPredictionService) {
        this.restTemplate = restTemplate;
        this.pulseEcoService = pulseEcoService;
        this.aqiPredictionService = aqiPredictionService;
    }

    @GetMapping("/test-pulse-eco")
    public List<SensorData> pulseEco() {
        String plainCreds = "refactorTeam" + ":" + "refactorAdmin";
        String base64Credentials = new String(Base64.getEncoder().encode(plainCreds.getBytes()));
        //Create HTTP Header and RestTemplate object
        HttpHeaders headers = new HttpHeaders();
        headers.add("Authorization", "Basic " + base64Credentials);
        RestTemplate restTemplate = new RestTemplate();
        HttpEntity requestEntity = new HttpEntity(headers);
        //Execute the request
        SensorData[] output = restTemplate.exchange("https://skopje.pulse.eco/rest/current",
                HttpMethod.GET, requestEntity, SensorData[].class).getBody();
        //Printout the result
        return Arrays.asList(output);
//        sensorList.stream().forEach(e -> System.out.println(e.getType()));
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
    public ResponseEntity<AQIPredictionResponse> process(@RequestBody byte[] imageBytes){
        double userLat = 41.98390993402405;
        double userLon = 21.451233146520536;

        // Get AQI averages from the service
        List<SensorCalculatedAqiAverages> aqiCalculatedAverages = this.pulseEcoService.calculateSensorsAQI("skopje", userLat, userLon);

        // If no averages are found, return 404
        if (aqiCalculatedAverages == null || aqiCalculatedAverages.isEmpty()) {
            return ResponseEntity.notFound().build();
        }

        // Find the highest value in the averages list
        SensorCalculatedAqiAverages highestAverage = aqiCalculatedAverages.stream()
                .max(Comparator.comparingDouble(SensorCalculatedAqiAverages::getAverageAQIValue)) // Assuming getAqiValue() gives the AQI value
                .orElseThrow(() -> new RuntimeException("Failed to find the highest AQI value"));

        // Prepare the response
        AQIPredictionResponse result = new AQIPredictionResponse();
        result.setSensorCalculationHighestAqi(highestAverage);

        double[] imageFeatures;
        try {
            imageFeatures = ImagePipelineClass.ProcessImage(imageBytes);
        } catch (Exception e) {
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR).body(null);
        }

        // Define the feature values for the image prediction model
//        double[] imageFeatures = {8, 0.8111680349223023, 202.86, 227.56, 232.28, 130.1882174744898, 1605.2222272435238,
//                0.9629555132385564, 152.05795599489795, 1260.744408959802, -0.16986878450064857, 174.41908482142858,
//                2147.1343168920357, -1.2729143943131467, 0.857589615508211, 0.7486225360834763, 0.8729379677012992,
//                59.901785714285715, 4261.925940688775, 1.4202556304727196, 105.26889349489795, 1939.6319373598317,
//                1.3711425640454187, 167.80436862244898, 3524.475758976365, -0.8383848244095626};

        // Predict AQI using the model
        Integer modelPrediction = this.aqiPredictionService.predict(imageFeatures);
        result.setModelPredictionValue(modelPrediction);

        // Return the response
        return ResponseEntity.ok(result);
    }
}
