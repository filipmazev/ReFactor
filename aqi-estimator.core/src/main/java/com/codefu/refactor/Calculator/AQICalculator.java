package com.codefu.refactor.Calculator;

import java.util.Map;
import java.util.stream.Collectors;

public class AQICalculator {
    // Breakpoints for PM2.5
    private static final double[] PM25_CONCENTRATION_BREAKPOINTS = {0.0, 12.0, 35.4, 55.4, 150.4, 250.4, 350.4, 500.4};
    private static final int[] PM25_AQI_BREAKPOINTS = {0, 50, 100, 150, 200, 300, 400, 500};

    // Breakpoints for PM10
    private static final double[] PM10_CONCENTRATION_BREAKPOINTS = {0.0, 54.0, 154.0, 254.0, 354.0, 424.0, 504.0, 604.0};
    private static final int[] PM10_AQI_BREAKPOINTS = {0, 50, 100, 150, 200, 300, 400, 500};

    // Breakpoints for O3
    private static final double[] O3_CONCENTRATION_BREAKPOINTS = {0.000, 0.054, 0.070, 0.085, 0.105, 0.200};
    private static final int[] O3_AQI_BREAKPOINTS = {0, 50, 100, 150, 200, 300};

    // Breakpoints for NO2
    private static final double[] NO2_CONCENTRATION_BREAKPOINTS = {0.0, 0.053, 0.100, 0.360, 0.649, 1.249, 1.649, 2.049};
    private static final int[] NO2_AQI_BREAKPOINTS = {0, 50, 100, 150, 200, 300, 400, 500};

    public static int calculatePollutantAQI(double concentration, double[] concentrationBreakpoints, int[] aqiBreakpoints) {
        for (int i = 0; i < concentrationBreakpoints.length - 1; i++) {
            if (concentration >= concentrationBreakpoints[i] && concentration <= concentrationBreakpoints[i + 1]) {
                double clow = concentrationBreakpoints[i];
                double chigh = concentrationBreakpoints[i + 1];
                int ilow = aqiBreakpoints[i];
                int ihigh = aqiBreakpoints[i + 1];
                return (int) Math.round(((ihigh - ilow) / (chigh - clow)) * (concentration - clow) + ilow);
            }
        }
        return -1; // Return -1 if concentration is outside defined range
    }

    public static Map<String, Double> calculateAQI(Map<String, Double> sensorAverages) {
        return sensorAverages.entrySet().stream()
                .collect(Collectors.toMap(
                        Map.Entry::getKey,
                        entry -> {
                            String type = entry.getKey();
                            double concentration = entry.getValue();
                            switch (type.toLowerCase()) {
                                case "pm25":
                                    return (double) calculatePollutantAQI(concentration, PM25_CONCENTRATION_BREAKPOINTS, PM25_AQI_BREAKPOINTS);
                                case "pm10":
                                    return (double) calculatePollutantAQI(concentration, PM10_CONCENTRATION_BREAKPOINTS, PM10_AQI_BREAKPOINTS);
                                case "o3":
                                    return (double) calculatePollutantAQI(concentration, O3_CONCENTRATION_BREAKPOINTS, O3_AQI_BREAKPOINTS);
                                case "no2":
                                    return (double) calculatePollutantAQI(concentration, NO2_CONCENTRATION_BREAKPOINTS, NO2_AQI_BREAKPOINTS);
                                default:
                                    return -1.0; // Unknown pollutant type
                            }
                        }
                ));
    }
}
