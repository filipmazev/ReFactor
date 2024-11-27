import { Injectable } from "@angular/core";
import { BehaviorSubject, Observable, Subject, takeUntil } from "rxjs";
import { WindowDimensions } from "../interfaces/services/window-dimensions.interface";
import { WindowDimensionsService } from "./window-dimension.service";
import { TruncateLengths } from "../interfaces/services/truncate-lengths.interface";
import { TruncateLength } from "../interfaces/services/truncate-length.interface";

@Injectable({
    providedIn: 'root',
})
export class TruncateService {
    protected readonly DefaultLengths: TruncateLengths = {
      threshold_xs: { short: 110, medium: 160, long: 180 },
      threshold_sm: { short: 130, medium: 180, long: 210 },
      threshold_md: { short: 150, medium: 200, long: 290 }, 
      threshold_lg: { short: 210, medium: 260, long: 350 }, 
      threshold_xl: { short: 230, medium: 280, long: 370 }, 
      threshold_2xl: { short: 250, medium: 300, long: 400 }, 
      threshold_3xl: { short: 270, medium: 320, long: 420 }, 
      threshold_4xl: { short: 290, medium: 340, long: 440 }, 
    };

    private windowDimensions: WindowDimensions = {} as WindowDimensions;
  
    private readonly unsubscribe$: Subject<void> = new Subject<void>();
  
    private truncateLengthMap: { threshold: number, length: TruncateLength }[] = [];

    private _autoTruncateLength: TruncateLength = this.getAutoTruncateLength();
    private set autoTruncateLength(value: TruncateLength) { this._autoTruncateLength = value; }
    public get autoTruncateLength(): TruncateLength { return this._autoTruncateLength; }

    getAutoTruncateLength$(): Observable<TruncateLength> {
        return this.autoTruncateLengthSubject.asObservable();
    }

    private autoTruncateLengthSubject = new BehaviorSubject<TruncateLength>(
        this.autoTruncateLength
    );
  
    constructor(private windowDimension: WindowDimensionsService) {
      this.CreateSubscriptions();

      this.truncateLengthMap = [
        { threshold: this.windowDimensions.threshold_xs, length: this.DefaultLengths.threshold_xs },
        { threshold: this.windowDimensions.threshold_sm, length: this.DefaultLengths.threshold_sm },
        { threshold: this.windowDimensions.threshold_md, length: this.DefaultLengths.threshold_md },
        { threshold: this.windowDimensions.threshold_lg, length: this.DefaultLengths.threshold_lg },
        { threshold: this.windowDimensions.threshold_xl, length: this.DefaultLengths.threshold_xl },
        { threshold: this.windowDimensions.threshold_2xl, length: this.DefaultLengths.threshold_2xl },
        { threshold: this.windowDimensions.threshold_2xl, length: this.DefaultLengths.threshold_3xl },
        { threshold: Infinity, length: this.DefaultLengths.threshold_4xl }, 
      ];
    }
  
    ngOnDestroy(): void {
      this.unsubscribe$.next();
      this.unsubscribe$.complete();
    }
  
    private CreateSubscriptions(): void {
      this.windowDimension.getWindowDimensions$().pipe(takeUntil(this.unsubscribe$)).subscribe((windowDimensions: WindowDimensions) => {
        this.windowDimensions = windowDimensions;
        this.autoTruncateLength = this.getAutoTruncateLength();
        this.autoTruncateLengthSubject.next(this.autoTruncateLength);
      });
    }

    public getAutoTruncateLength(): TruncateLength {
        return this.truncateLengthMap.find(item => this.windowDimensions.width <= item.threshold)?.length || this.DefaultLengths.threshold_md;
    }
}