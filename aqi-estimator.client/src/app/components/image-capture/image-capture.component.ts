import { Component } from '@angular/core';
import { Subject, Observable } from 'rxjs';
import { WebcamImage, WebcamModule } from 'ngx-webcam';

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
  permissionStatus: boolean = false;
  camData?: MediaStream;
  capturedImage?: string;
  trigger: Subject<void> = new Subject<void>();

  get $trigger(): Observable<void> {
    return this.trigger.asObservable();
  }

  checkPermission(): void {
    navigator.mediaDevices.getUserMedia({ video: {width: 1280, height: 720}, audio: false }).then((response: MediaStream) => {
      this.permissionStatus = true;
      this.camData = response;
    }).catch((error: any) => {
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
}
