import { Component } from '@angular/core';
import { Subject, Observable, take } from 'rxjs';
import { WebcamImage, WebcamModule } from 'ngx-webcam';
import { EMPTY_STRING } from '../../shared/constants/common.constants';
import { HttpClient } from '@angular/common/http';
import { PredictorService } from '../../../client/predictor.service';
import { DataService } from '../../shared/services/data.service';

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

  constructor(
    private dataService: DataService,
    private predictorService: PredictorService) { 

  }

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

  async sendImage() {
    if (this.capturedImage) {
      const formData = new FormData();

      const imageBlob = this.dataService.convertBase64ToBlob(this.capturedImage);

      formData.append('image', imageBlob, 'captured-image.jpg');

      await this.predictorService.predict(imageBlob).then((result) => {
        console.log(result);
      });
    }
  }
}
