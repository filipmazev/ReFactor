package com.codefu.refactor.api.aqiestimatorserver.DTO.clientRequest;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import lombok.NonNull;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class AQIPredictionRequest {
    byte @NonNull [] indata;

    Double lat;
    Double lon;
}
