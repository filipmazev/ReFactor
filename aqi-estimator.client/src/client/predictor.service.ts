import { HttpClient } from "@angular/common/http";
import { Injectable } from "@angular/core";
import { take } from "rxjs";

@Injectable({
    providedIn: 'root'
  })
export class PredictorService {
    
    constructor(private httpClient: HttpClient) {

    }

    public predict(data: Blob): Promise<any> {
        return new Promise<any>((resolve, reject) => {
            try {
                this.httpClient.post("http://localhost:8080/api/aqi/process", data).pipe(take(1))
                .subscribe((response: any) => {
                    resolve(response);
                });
            } catch (error) {
                reject(error);
            }
        });
    }
}