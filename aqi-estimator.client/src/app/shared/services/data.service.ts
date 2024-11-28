import { Injectable } from "@angular/core";
import { IDataService } from "../interfaces/services/data-service.interface";

@Injectable({
    providedIn: 'root'
  })
export class DataService implements IDataService {

    constructor() { }

    public convertBase64ToBlob(base64: string): Blob {
        const [header, data] = base64.split(',');
        const mimeMatch = header.match(/data:(.*?);base64/);  
        const mime = mimeMatch ? mimeMatch[1] : 'application/octet-stream';  
        
        const binary = atob(data); 
        const length = binary.length;
        const buffer = new ArrayBuffer(length);
        const view = new Uint8Array(buffer);
        
        for (let i = 0; i < length; i++) {
            view[i] = binary.charCodeAt(i);
        }
        
        return new Blob([view], { type: mime });
    }
}