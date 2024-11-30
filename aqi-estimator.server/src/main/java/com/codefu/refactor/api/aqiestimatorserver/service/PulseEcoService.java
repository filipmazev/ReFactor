package com.codefu.refactor.api.aqiestimatorserver.service;

import com.codefu.refactor.api.aqiestimatorserver.AQICalculator.AQICalculator;
import com.codefu.refactor.api.aqiestimatorserver.DTO.SensorData;
import com.codefu.refactor.api.aqiestimatorserver.DTO.clientResponses.SensorCalculatedAqiAverages;
import com.codefu.refactor.api.aqiestimatorserver.service.inter.IPulseEcoService;
import com.codefu.refactor.api.aqiestimatorserver.webConfig.ApplicationProperties;
import lombok.RequiredArgsConstructor;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.Collectors;

@Service
@RequiredArgsConstructor
public class PulseEcoService implements IPulseEcoService {

    private final ApplicationProperties properties;

    private final double LOCATION_RADIUS = 5.0;
    // In-memory cache to store sensor data and timestamps
    private static final Map<String, CachedSensorData> sensorDataCacheByCityName = new ConcurrentHashMap<>();
    private static final long CACHE_EXPIRATION_TIME_MS = 15 * 60 * 1000; // 15 minutes in milliseconds

    @Override
    public Optional<List<SensorData>> getLatestSensorDataForCity(String cityName) {
        try{
            ApplicationProperties.PulseEcoProperties pulseEco = properties.getPulseEco();
            String username = pulseEco.getUsername();
            String password = pulseEco.getPassword();
            String plainCredentials = String.format("%s:%s", username, password);
            String base64Credentials = new String(Base64.getEncoder().encode(plainCredentials.getBytes()));

            // Create HTTP Header and RestTemplate object
            HttpHeaders headers = new HttpHeaders();
            headers.add("Authorization", "Basic " + base64Credentials);
            RestTemplate restTemplate = new RestTemplate();
            HttpEntity<String> requestEntity = new HttpEntity<>(headers);

            // Execute the request
            SensorData[] output = restTemplate.exchange(
                    "https://" + cityName + ".pulse.eco/rest/current",
                    HttpMethod.GET,
                    requestEntity,
                    SensorData[].class
            ).getBody();

            if (output == null) {
                return Optional.empty();
            }

            // Return the result as a list
            return Optional.of(Arrays.asList(output));
        }catch (Exception e){
            System.out.println("Error getting the sensors data from the pulse-eco api");
        }
        return Optional.empty();
    }

    @Override
    public Double calculateAqiFromSensorData(List<SensorData> sensorsData) {
        return 0.0;
    }

    private Map<String, Double> calculateSensorAverages(List<SensorData> sensors, double userLat, double userLon) {
        // Define the set of allowed sensor types
        Set<String> allowedTypes = Set.of("pm10", "pm25", "o3", "no2");

        // Group sensors by type after filtering by distance and allowed types
        Map<String, List<SensorData>> sensorGroups = sensors.stream()
                .filter(sensor -> isWithinRadius(userLat, userLon, parsePosition(sensor.getPosition()), LOCATION_RADIUS))
                .filter(sensor -> allowedTypes.contains(sensor.getType().toLowerCase())) // Filter by allowed types
                .collect(Collectors.groupingBy(SensorData::getType));

        Map<String, Double> sensorAverages = new HashMap<>();
        for (String type : sensorGroups.keySet()) {
            // Sort sensors by proximity and take the closest 3
            List<SensorData> sensorList = sensorGroups.get(type).stream()
                    .sorted(Comparator.comparingDouble(sensor ->
                            calculateDistance(userLat, userLon, parsePosition(sensor.getPosition()))))
                    .limit(3)
                    .toList();

            // Calculate the average of the sensor values
            double average = sensorList.stream()
                    .mapToDouble(sensor -> Double.parseDouble(sensor.getValue()))
                    .average()
                    .orElse(0.0);

            sensorAverages.put(type, average);
        }

        return sensorAverages;
    }

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

    private double[] parsePosition(String position) {
        String[] parts = position.split(",");
        if (parts.length != 2) {
            throw new IllegalArgumentException("Invalid position format: " + position);
        }
        return new double[]{Double.parseDouble(parts[0]), Double.parseDouble(parts[1])};
    }

    @Override
    public List<SensorCalculatedAqiAverages> calculateSensorsAQI(String cityName, double userLat, double userLon) {
        // Check if the cached data is available and not expired
        CachedSensorData cachedData = sensorDataCacheByCityName.get(cityName);
        if (cachedData != null && !isCacheExpired(cachedData.timestamp)) {
            // If data is in cache and valid, use it
            Map<String, Double> sensorAveragesByTypeMap = calculateSensorAverages(cachedData.sensorData, userLat, userLon);
            return AQICalculator.calculateAQI(sensorAveragesByTypeMap);
        } else {
            // If data is not cached or expired, make a new request
            Optional<List<SensorData>> sensorDataList = getLatestSensorDataForCity(cityName);
            if (sensorDataList.isEmpty()) {
                return null;
            }

            // Cache the new sensor data with the current timestamp
            sensorDataCacheByCityName.put(cityName, new CachedSensorData(sensorDataList.get(), System.currentTimeMillis()));

            // Proceed with AQI calculation
            Map<String, Double> sensorAveragesByTypeMap = calculateSensorAverages(sensorDataList.get(), userLat, userLon);
            return AQICalculator.calculateAQI(sensorAveragesByTypeMap);
        }
    }


    // Helper method to check if the cache is expired (older than 15 minutes)
    private boolean isCacheExpired(long timestamp) {
        return (System.currentTimeMillis() - timestamp) > CACHE_EXPIRATION_TIME_MS;
    }

    // Cached sensor data class that holds the data and the timestamp
    private record CachedSensorData(List<SensorData> sensorData, long timestamp){

    }

}
