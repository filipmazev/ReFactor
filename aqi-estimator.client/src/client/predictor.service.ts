import { HttpClient } from "@angular/common/http";
import { Injectable } from "@angular/core";
import { take } from "rxjs";
import { environment } from "../environments/environment";

@Injectable({
    providedIn: 'root'
  })
export class PredictorService {
    private readonly BASE_PATH: string = environment.apiPath;

    constructor(private httpClient: HttpClient) {

    }

    public predict(data: Blob): Promise<any> {
        return new Promise<any>((resolve, reject) => {
            try {
                this.httpClient.post(this.BASE_PATH + "/aqi/process", data).pipe(take(1))
                .subscribe((response: any) => {
                    resolve(response);
                });
            } catch (error) {
                reject(error);
            }
        });
    }
}