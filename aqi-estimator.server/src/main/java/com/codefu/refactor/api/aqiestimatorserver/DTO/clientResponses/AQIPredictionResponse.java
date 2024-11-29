package com.codefu.refactor.api.aqiestimatorserver.DTO.clientResponses;

import com.codefu.refactor.api.aqiestimatorserver.enums.AQICategory;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class AQIPredictionResponse {
    Integer modelPredictionValue;
    SensorCalculatedAqiAverages sensorCalculationHighestAqi;
    AQICategory aqiCategory;
}
