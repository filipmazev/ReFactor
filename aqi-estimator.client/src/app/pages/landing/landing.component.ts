import { Component, OnInit } from '@angular/core';
import { TranslateModule } from '@ngx-translate/core';
import { WebcamImage, WebcamModule } from 'ngx-webcam';
import { Subject, Observable, takeUntil } from 'rxjs';
import { PredictorService } from '../../../client/predictor.service';
import { AQI_PREDICTION_HIGH_DISCREPENCY_LIMIT, EMPTY_STRING } from '../../shared/constants/common.constants';
import { DataService } from '../../shared/services/data.service';
import { NgClass, NgStyle } from '@angular/common';
import { WindowDimensionsService } from '../../shared/services/window-dimension.service';
import { WindowDimensions } from '../../shared/interfaces/services/window-dimensions.interface';
import { BODY_BG_BLACK, FOOTER_BG_BLACK } from '../../shared/constants/class-names.constants';
import { AQIPredictionResponse } from '../../shared/classes/models/incoming/AQIPredictionResponse';
import { AQIPredictionRequest } from '../../shared/classes/models/outgoing/AQIPredictionRequest';
import { SpinnerService } from '../../shared/services/spinner.service';
import { AQICategory } from '../../shared/enums/api-responses/AQICategory';
import { AQIResultDiscrepencyCategory } from '../../shared/enums/ui/discrepency-category.enum';
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
        WebcamModule,
    ],
    templateUrl: './landing.component.html',
    styleUrls: ['./landing.component.scss']
})
export class LandingComponent implements OnInit { 
    //#region Variables
    protected windowDimensions: WindowDimensions = {} as WindowDimensions;

    protected readonly animation = animConsts;

    protected isLoading: boolean = false;

    protected request_failed: boolean = false;

    protected readonly AQICategory = AQICategory;
    protected readonly AQIResultDiscrepencyCategory = AQIResultDiscrepencyCategory; 

    protected currentDiscrepencyCategory: AQIResultDiscrepencyCategory = AQIResultDiscrepencyCategory.None;
    //#endregion

    //#region Subscription Variables
    public readonly unsubscribe$: Subject<void> = new Subject();
    //#endregion

    //#region Camera Variables
    protected capturing_status: boolean = false;
    protected live_camera_feed?: MediaStream;
    protected capturedImage?: string;

    protected initial_capture: boolean = true;

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

    //#region UI Variables
    private capture_button_hold_timer?: ReturnType<typeof setInterval>;
   
    protected readonly capture_button_max_size: number = uiConsts.CAPTURE_BUTTON_SIZE;
    private capture_button_size: number = this.capture_button_max_size;

    protected readonly capture_button_initial_size: string = `${this.capture_button_max_size}rem`;
    protected capture_button_center_circle_size: string = this.capture_button_initial_size; 

    private readonly capture_button_center_circle_min_size: number = uiConsts.CAPTURE_BUTTON_MIN_SIZE; 
    private readonly capture_button_center_circle_shrink_speed: number = uiConsts.CAPTURE_BUTTON_SHRINK_SPEED;
    //#endregion

    //#region Data Variables
    private _aqi_prediction_result?: AQIPredictionResponse = undefined;
    public get aqi_prediction_result(): AQIPredictionResponse | undefined {
        return this._aqi_prediction_result;
    }
    private set aqi_prediction_result(value: AQIPredictionResponse | undefined) {
        this._aqi_prediction_result = value;
    }
    //#endregion

    constructor(
        private windowDimensionService: WindowDimensionsService,
        private dataService: DataService,
        private spinnerService: SpinnerService,
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

        this.spinnerService.getShowSpinnerState$().pipe(takeUntil(this.unsubscribe$)).subscribe((show_spinner) => {
            this.isLoading = show_spinner;
        });
    }

    //#region Image Capture Methods
    protected capture_requestCapture(): void {
        this.resetVariables();

        navigator.mediaDevices.getUserMedia({ video: { width: this.current_requested_capture_size.width, height: this.current_requested_capture_size.height }, audio: false })
            .then((response: MediaStream) => {
            this.live_camera_feed = response;
            this.capture_display_tutorial_message = true;
            this.ui_toggleBlackedOutMode(true);
            setTimeout(() => {
                this.capture_display_tutorial_message = false;
            }, animConsts.CAPTURE_TUTORIAL_DURATION)
        })
        .catch((error: any) => {
            this.capturing_status = false;
            console.error(error);
        });
    }

    protected capture_handleImage(event: WebcamImage): void {
        this.capturedImage = event.imageAsDataUrl;
        if(this.initial_capture) {
            this.initial_capture = false;
        }
    }

    protected capture_takeImage(): void {
        this.capturing_trigger.next();
    }

    protected async capture_sendImage() {
        if (this.capturedImage) {
            this.request_failed = false;

            const formData = new FormData();

            const imageBlob = this.dataService.convertBase64ToBlob(this.capturedImage);

            formData.append('image', imageBlob, 'captured-image.jpg');

            var request: AQIPredictionRequest = new AQIPredictionRequest(imageBlob, undefined, undefined);

            this.spinnerService.showSpinner();

            await this.predictorService.predict(request).then((result: AQIPredictionResponse) => {
                this.aqi_prediction_result = result;
                this.determineDiscrepencyCategory(result);
            }).catch((error: any) => {
                console.error(error);
                this.request_failed = true;
            }).finally(() => {
                this.spinnerService.hideSpinner();
            });
        }
    }

    protected capture_cancelCapture(softCancel?: boolean): void {
        this.live_camera_feed?.getTracks().forEach((track: MediaStreamTrack) => {
            track.stop();
        });
        this.live_camera_feed = undefined;
        if(!softCancel) {
            this.capturedImage = undefined;
            this.capturing_status = false;
            this.ui_toggleBlackedOutMode(false);
        }
    }

    protected capture_resetCapture(): void {
        this.capture_cancelCapture(true);
        this.capturedImage = undefined;
        this.capture_requestCapture();
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

    private ui_toggleBlackedOutMode(state?: boolean): void {
        const footer = document.querySelector('footer');

        document.body.classList.toggle(BODY_BG_BLACK, state);

        if (footer) {
            footer.classList.toggle(FOOTER_BG_BLACK, state);
        }
    }
    //#endregion

    //#region Helper Methods 
    private resetVariables(): void {
        this.capturing_status = true;
        this.request_failed = false;
        this.aqi_prediction_result = undefined;
        this.currentDiscrepencyCategory = AQIResultDiscrepencyCategory.None;
    }   

    private determineDiscrepencyCategory(result: AQIPredictionResponse): void {
        if(result.sensorCalculationHighestAqi) {
            const sensorAqi = result.sensorCalculationHighestAqi.averageAQIValue;
            const modelAqi = result.modelPredictionValue;

            const delta = Math.abs(sensorAqi - modelAqi);

            if(sensorAqi === modelAqi) {
                this.currentDiscrepencyCategory = AQIResultDiscrepencyCategory.None;
            } else if(delta > AQI_PREDICTION_HIGH_DISCREPENCY_LIMIT) {
                this.currentDiscrepencyCategory = AQIResultDiscrepencyCategory.High;
            } else {
                this.currentDiscrepencyCategory = AQIResultDiscrepencyCategory.Low;
            }
        } else {
            this.currentDiscrepencyCategory = AQIResultDiscrepencyCategory.Unknown;
        }
    }
    //#endregion
}