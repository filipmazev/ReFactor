package com.codefu.refactor.api.aqiestimatorserver.service;

import com.codefu.refactor.api.aqiestimatorserver.service.inter.IAqiPredictionService;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;

import java.util.Map;

@Service
public class AqiPredictionService implements IAqiPredictionService {


    private final RestTemplate restTemplate;

    public AqiPredictionService(RestTemplate restTemplate) {
        this.restTemplate = restTemplate;
    }

    @Override
    public Integer predict(double[] values) {
        // URL of your Flask app
        String flaskAppUrl = "http://127.0.0.1:5000/predict";

        // Set up headers for the request
        HttpHeaders headers = new HttpHeaders();
        headers.set("Content-Type", "application/json");

        // Wrap the double array into an HttpEntity
        HttpEntity<double[]> requestEntity = new HttpEntity<>(values, headers);

        // Send a POST request to the Flask API
        ResponseEntity<Map> response = restTemplate.exchange(
                flaskAppUrl,
                HttpMethod.POST,
                requestEntity,
                Map.class
        );

        // Extract the prediction from the response
        if (response.getBody() != null && response.getBody().containsKey("prediction")) {
            return ((Number) response.getBody().get("prediction")).intValue();
        } else {
            throw new RuntimeException("Invalid response from Flask API");
        }
    }
}
