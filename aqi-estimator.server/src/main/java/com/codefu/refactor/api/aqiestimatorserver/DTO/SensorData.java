package com.codefu.refactor.api.aqiestimatorserver.DTO;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class SensorData {
    private String sensorId;
    private String stamp;
    private String type;
    private String position;
    private String value;
}
