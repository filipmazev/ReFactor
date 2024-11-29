import {SensorCalculatedAqiAverages} from "./SensorCalculatedAqiAverages";
import {AQICategory} from "../../enums/api-responses/AQICategory";

export interface AQIPredictionResponse {
  modelPredictionValue: number;
  sensorCalculationHighestAqi: SensorCalculatedAqiAverages;
  aqiCategory: AQICategory;
}
