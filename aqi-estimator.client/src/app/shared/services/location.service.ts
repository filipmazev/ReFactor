import { Injectable } from "@angular/core";

@Injectable({
    providedIn: 'root'
})
export class LocationService {

    constructor() {

    }

    public get_userLocation(): Promise<GeolocationPosition | null> {
        return new Promise((resolve) => {
            if (!navigator.geolocation) {
                resolve(null);  
            } else {
                navigator.geolocation.getCurrentPosition(
                    (position) => resolve(position), 
                    () => resolve(null)  
                );
            }
        });
    }
}