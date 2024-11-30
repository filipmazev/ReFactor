import { HttpClient, HttpErrorResponse } from "@angular/common/http";
import { Injectable } from "@angular/core";
import { take, timeout } from "rxjs";
import { environment } from "../environments/environment";
import { AQIPredictionResponse } from "../app/shared/classes/models/incoming/AQIPredictionResponse";
import { AQIPredictionRequest } from "../app/shared/classes/models/outgoing/AQIPredictionRequest";
import { DEFAULT_REQUEST_TIMEOUT_IN_MS } from "../app/shared/constants/common.constants";

@Injectable({
    providedIn: 'root'
  })
export class PredictorService {
    private readonly BASE_PATH: string = environment.apiPath;

    constructor(private httpClient: HttpClient) {

    }

    public predict(data: AQIPredictionRequest): Promise<AQIPredictionResponse> {
        return new Promise<AQIPredictionResponse>((resolve, reject) => {
            this.httpClient.post<AQIPredictionResponse>(this.BASE_PATH + "/process", data)
                .pipe(take(1), timeout(DEFAULT_REQUEST_TIMEOUT_IN_MS))
                .subscribe({
                    next: (response: AQIPredictionResponse) => resolve(response),
                    error: (error: HttpErrorResponse) => {
                        if (error.name === 'HttpErrorResponse') {
                            reject(new Error('Request timed out.'));
                        } else {
                            reject(error);
                        }
                    }
                });
        });
    }
}