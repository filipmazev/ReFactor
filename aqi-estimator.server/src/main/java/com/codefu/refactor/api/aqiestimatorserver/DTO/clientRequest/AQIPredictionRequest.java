package com.codefu.refactor.api.aqiestimatorserver.DTO.clientRequest;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import lombok.NonNull;

import java.util.Base64;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class AQIPredictionRequest {
    private String indata;
    private Double lat;
    private Double lon;

    public byte[] getIndataAsBytes() {
        return Base64.getDecoder().decode(indata);
    }
}
