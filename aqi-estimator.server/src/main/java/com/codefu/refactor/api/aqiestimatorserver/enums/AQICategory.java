package com.codefu.refactor.api.aqiestimatorserver.enums;

public enum AQICategory {
    GOOD,
    MODERATE,
    UNHEALTHY_FOR_SENSITIVE_GROUPS,
    UNHEALTHY,
    VERY_UNHEALTHY,
    HAZARDOUS;

    // Utility method to map AQI value to category
    public static AQICategory fromValue(int aqiValue) {
        if (aqiValue <= 50) {
            return GOOD;
        } else if (aqiValue <= 100) {
            return MODERATE;
        } else if (aqiValue <= 150) {
            return UNHEALTHY_FOR_SENSITIVE_GROUPS;
        } else if (aqiValue <= 200) {
            return UNHEALTHY;
        } else if (aqiValue <= 300) {
            return VERY_UNHEALTHY;
        } else {
            return HAZARDOUS;
        }
    }
}
