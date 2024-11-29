import {SensorType} from "../../enums/api-responses/SensorType";

export interface SensorCalculatedAqiAverages {
  averageAQIValue: number;
  sensorType: SensorType;
  location: string;  // Assuming you want the sensor's location as well
}
