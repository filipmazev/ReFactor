import { Component } from '@angular/core';
import { Subject, Observable } from 'rxjs';
import { WebcamImage, WebcamModule } from 'ngx-webcam';
import { EMPTY_STRING } from '../../shared/constants/common.constants';
import { HttpClient } from '@angular/common/http';

@Component({
  selector: 'app-image-capture',
  standalone: true,
  imports: [
    WebcamModule
  ],
  templateUrl: './image-capture.component.html',
  styleUrl: './image-capture.component.scss'
})
export class ImageCaptureComponent {
  protected permissionStatus: boolean = false;
  protected camData?: MediaStream;
  protected capturedImage?: string;

  protected trigger: Subject<void> = new Subject<void>();

  private facingMode: string = 'environment';

  constructor(private httpClient: HttpClient) { }

  protected get $trigger(): Observable<void> {
    return this.trigger.asObservable();
  }

  protected get videoOptions(): MediaTrackConstraints {
    const result: MediaTrackConstraints = {};
    if (this.facingMode && this.facingMode !== EMPTY_STRING) {
      result.facingMode = { ideal: this.facingMode };
    }
    return result;
  }

  checkPermission(): void {
    navigator.mediaDevices.getUserMedia({ video: { width: 1280, height: 720 }, audio: false })
      .then((response: MediaStream) => {
        this.permissionStatus = true;
        this.camData = response;
      })
      .catch((error: any) => {
        this.permissionStatus = false;
        console.error(error);
      });
  }

  handleImage(event: WebcamImage): void {
    this.capturedImage = event.imageAsDataUrl;
  }

  captureImage(): void {
    this.trigger.next();
  }

  // Helper method to convert base64 string to Blob
  private convertBase64ToBlob(base64: string): Blob {
    const [header, data] = base64.split(',');
    const mimeMatch = header.match(/data:(.*?);base64/);  // Updated regular expression to correctly match mime type
    const mime = mimeMatch ? mimeMatch[1] : 'application/octet-stream';  // Default to 'application/octet-stream' if no mime type found

    const binary = atob(data); // Decode base64 data
    const length = binary.length;
    const buffer = new ArrayBuffer(length);
    const view = new Uint8Array(buffer);

    for (let i = 0; i < length; i++) {
      view[i] = binary.charCodeAt(i);
    }

    return new Blob([view], { type: mime });
  }

  // Method to send the captured image to the backend
  sendImage(): void {
    if (this.capturedImage) {
      const formData = new FormData();

      // Convert the base64 image to Blob
      const imageBlob = this.convertBase64ToBlob(this.capturedImage);

      // Append the Blob as a file to FormData (you can give it a name like 'captured-image.jpg')
      formData.append('image', imageBlob, 'captured-image.jpg');

      // Send the image via HTTP POST request
      this.httpClient.post("http://localhost:8080/api/aqi/process", formData).subscribe(
        (response) => {
          console.log('Image uploaded successfully:', response);
        },
        (error) => {
          console.error('Image upload failed:', error);
        }
      );
    } else {
      console.error('No image captured!');
    }
  }
}
