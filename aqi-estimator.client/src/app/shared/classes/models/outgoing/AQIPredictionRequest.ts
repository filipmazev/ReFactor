export class AQIPredictionRequest {
    indata: string;
    lat: number | undefined | null;
    lon: number | undefined | null;

    constructor(_indata: string, _lat: number | undefined | null, _lon: number | undefined | null) {
        this.indata = _indata;
        this.lat = _lat;
        this.lon = _lon;
    }
}