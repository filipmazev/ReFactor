import {SensorType} from "../../enums/api-responses/SensorType";

export interface SensorCalculatedAqiAverages {
  averageAQIValue: number;
  sensorType: SensorType;
}
