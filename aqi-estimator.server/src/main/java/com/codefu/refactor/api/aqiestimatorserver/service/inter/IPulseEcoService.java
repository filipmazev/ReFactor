package com.codefu.refactor.api.aqiestimatorserver.service.inter;

import com.codefu.refactor.api.aqiestimatorserver.DTO.SensorData;
import com.codefu.refactor.api.aqiestimatorserver.DTO.clientResponses.SensorCalculatedAqiAverages;

import java.util.List;
import java.util.Optional;

public interface IPulseEcoService {
    Optional<List<SensorData>> getLatestSensorDataForCity(String cityName);
    Double calculateAqiFromSensorData(List<SensorData> sensorsData);
    List<SensorCalculatedAqiAverages> calculateSensorsAQI(String cityName, double userLat, double userLon);
}
