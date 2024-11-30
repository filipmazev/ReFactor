export class AQIPredictionRequest {
    indata: Blob;
    lan: number | undefined | null;
    lon: number | undefined | null;

    constructor(_indata: Blob, _lan: number | undefined | null, _lon: number | undefined | null) {
        this.indata = _indata;
        this.lan = _lan;
        this.lon = _lon;
    }
}