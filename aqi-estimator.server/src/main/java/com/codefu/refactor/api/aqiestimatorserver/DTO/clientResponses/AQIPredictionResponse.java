package com.codefu.refactor.api.aqiestimatorserver.DTO.clientResponses;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class AQIPredictionResponse {
    Integer modelPredictionValue;
    SensorCalculatedAqiAverages sensorCalculationHighestAqi;
}