package com.codefu.refactor.api.aqiestimatorserver.DTO.clientResponses;

import com.codefu.refactor.api.aqiestimatorserver.enums.SensorType;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class SensorCalculatedAqiAverages {
    SensorType sensorType;
    double averageAQIValue;
}
