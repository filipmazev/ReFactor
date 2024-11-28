import { Component, OnInit } from '@angular/core';
import { TranslateModule } from '@ngx-translate/core';
import { WebcamImage, WebcamModule } from 'ngx-webcam';
import { Subject, Observable, takeUntil } from 'rxjs';
import { PredictorService } from '../../../client/predictor.service';
import { EMPTY_STRING } from '../../shared/constants/common.constants';
import { DataService } from '../../shared/services/data.service';
import { NgClass, NgStyle } from '@angular/common';
import { WindowDimensionsService } from '../../shared/services/window-dimension.service';
import { WindowDimensions } from '../../shared/interfaces/services/window-dimensions.interface';
import * as camConsts from '../../shared/constants/camera.constants';
import * as uiConsts from '../../shared/constants/ui.constants';
import * as animConsts from '../../shared/constants/animation.constants';

@Component({
    selector: 'app-landing',
    standalone: true,
    imports: [
        TranslateModule,
        NgClass,
        NgStyle,
        WebcamModule
    ],
    templateUrl: './landing.component.html',
    styleUrls: ['./landing.component.scss']
})
export class LandingComponent implements OnInit { 
    //#region Variables
    protected windowDimensions: WindowDimensions = {} as WindowDimensions;

    protected readonly animation = animConsts;
    //#endregion

    //#region Subscription Variables
    public readonly unsubscribe$: Subject<void> = new Subject();
    //#endregion

    //#region Camera Variables
    protected capturing_status: boolean = false;
    protected live_camera_feed?: MediaStream;
    protected capturedImage?: string;

    protected capturing_trigger: Subject<void> = new Subject<void>();

    protected capture_display_tutorial_message: boolean = true;

    private camera_facing_mode: string = camConsts.DEFAULT_FACING_MODE;

    private readonly requested_capture_sizes = {
        'small': {
          height: camConsts.REQUESTED_CAPTURE_HEIGHT_IN_PX_SM,
          width: camConsts.REQUESTED_CAPTURE_WIDTH_IN_PX_SM
        },
        'medium': {
          height: camConsts.REQUESTED_CAPTURE_HEIGHT_IN_PX_MD,
          width: camConsts.REQUESTED_CAPTURE_WIDTH_IN_PX_MD
        },
        'large': {
          height: camConsts.REQUESTED_CAPTURE_HEIGHT_IN_PX_LG,
          width: camConsts.REQUESTED_CAPTURE_WIDTH_IN_PX_LG
        },
        'x-large': {
          height: camConsts.REQUESTED_CAPTURE_HEIGHT_IN_PX_XL,
          width: camConsts.REQUESTED_CAPTURE_WIDTH_IN_PX_XL
        },
        '2x-large': {
          height: camConsts.REQUESTED_CAPTURE_HEIGHT_IN_PX_2XL,
          width: camConsts.REQUESTED_CAPTURE_WIDTH_IN_PX_2XL
        }
    };

    protected current_requested_capture_size: { height: number, width: number } = this.requested_capture_sizes['2x-large'];

    protected get $capturing_trigger(): Observable<void> {
        return this.capturing_trigger.asObservable();
    }

    protected get videoOptions(): MediaTrackConstraints {
        const result: MediaTrackConstraints = {};
        if (this.camera_facing_mode && this.camera_facing_mode !== EMPTY_STRING) {
            result.facingMode = { ideal: this.camera_facing_mode };
        }
        return result;
    }
    //#endregion

    //#region Camera Button UI Variables
    private capture_button_hold_timer?: ReturnType<typeof setInterval>;
   
    protected readonly capture_button_max_size: number = uiConsts.CAPTURE_BUTTON_SIZE;
    private capture_button_size: number = this.capture_button_max_size;

    protected readonly capture_button_initial_size: string = `${this.capture_button_max_size}rem`;
    protected capture_button_center_circle_size: string = this.capture_button_initial_size; 

    private readonly capture_button_center_circle_min_size: number = uiConsts.CAPTURE_BUTTON_MIN_SIZE; 
    private readonly capture_button_center_circle_shrink_speed: number = uiConsts.CAPTURE_BUTTON_SHRINK_SPEED;
    //#endregion

    constructor(
        private windowDimensionService: WindowDimensionsService,
        private dataService: DataService,
        private predictorService: PredictorService) {
    }

    public ngOnInit(): void {
        this.CreateSubscriptions();
    }

    private CreateSubscriptions() {
        this.windowDimensionService.getWindowDimensions$().pipe(takeUntil(this.unsubscribe$)).subscribe((dimensions: WindowDimensions) => {
            this.windowDimensions = dimensions;

            const screenSize = 
                this.windowDimensions.width <= this.windowDimensions.threshold_sm ? 'small'
                : this.windowDimensions.width <= this.windowDimensions.threshold_md ? 'medium'
                : this.windowDimensions.width <= this.windowDimensions.threshold_lg ? 'large'
                : this.windowDimensions.width <= this.windowDimensions.threshold_xl ? 'x-large'
                : '2x-large';

            this.current_requested_capture_size = this.requested_capture_sizes[screenSize];
        });
    }

    //#region Image Capture Methods
    protected capture_requestCapture(): void {
        this.capturing_status = true;

        navigator.mediaDevices.getUserMedia({ video: { width: this.current_requested_capture_size.width, height: this.current_requested_capture_size.height }, audio: false })
            .then((response: MediaStream) => {
            this.live_camera_feed = response;
            this.capture_display_tutorial_message = true;
            setTimeout(() => {
                this.capture_display_tutorial_message = false;
            }, animConsts.CAPTURE_TUTORIA_DURATION)
        })
        .catch((error: any) => {
            this.capturing_status = false;
            console.error(error);
        });
    }

    protected capture_handleImage(event: WebcamImage): void {
        this.capturedImage = event.imageAsDataUrl;
    }

    protected capture_takeImage(): void {
        this.capturing_trigger.next();
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
    //#endregion

    //#region UI Methods
    protected ui_startCaptureButtonShrink(): void {
        this.capture_button_hold_timer = setInterval(() => {
            this.capture_button_size = Math.max(this.capture_button_size - this.capture_button_center_circle_shrink_speed, this.capture_button_center_circle_min_size);
            this.capture_button_center_circle_size = `${this.capture_button_size}rem`;
        }, 50);
    }

    protected ui_stopCaptureButtonShrink(): void {
        if (this.capture_button_hold_timer) {
            clearInterval(this.capture_button_hold_timer);
            this.capture_button_hold_timer = undefined;
        }
        this.ui_resetCaptureButtonSize();
    }

    private ui_resetCaptureButtonSize(): void {
        this.capture_button_size = this.capture_button_max_size;
        this.capture_button_center_circle_size = `${this.capture_button_size}rem`;
    }
    //#endregion
}