import {AQICategory} from "../../../enums/api-responses/AQICategory";
import { SensorCalculatedAqiAverages } from "../../../interfaces/models/SensorCalculatedAqiAverages.interface";

export class AQIPredictionResponse {
  modelPredictionValue: number;
  sensorCalculationHighestAqi: SensorCalculatedAqiAverages | undefined | null;
  aqiCategory: AQICategory;

  constructor(_modelPredictionValue: number, _sensorCalculationHighestAqi: SensorCalculatedAqiAverages | undefined | null, _aqiCategory: AQICategory) {
    this.modelPredictionValue = _modelPredictionValue;
    this.sensorCalculationHighestAqi = _sensorCalculationHighestAqi;
    this.aqiCategory = _aqiCategory;
  }
}
