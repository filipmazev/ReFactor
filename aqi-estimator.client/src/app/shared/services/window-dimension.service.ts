import { Injectable } from '@angular/core';
import { BehaviorSubject, Observable } from 'rxjs';
import { WindowDimensions } from '../interfaces/services/window-dimensions.interface';

@Injectable({
  providedIn: 'root',
})
export class WindowDimensionsService {
  private windowDimensionsSubject = new BehaviorSubject<WindowDimensions>(
    this.getWindowDimensions()
  );

  constructor() {
    this.handleResize();
    window.addEventListener('resize', this.handleResize.bind(this));
  }

  getWindowDimensions(): WindowDimensions {
    return {
      width: window.innerWidth,
      height: window.innerHeight,
      threshold_xs: 450,
      threshold_sm: 640,
      threshold_md: 768,
      threshold_lg: 1024,
      threshold_xl: 1280,
      threshold_2xl: 1536,
      threshold_3xl: 1920,
      threshold_4xl: 2560,
    };
  }

  private handleResize() {
    this.windowDimensionsSubject.next(this.getWindowDimensions());
  }

  getWindowDimensions$(): Observable<WindowDimensions> {
    return this.windowDimensionsSubject.asObservable();
  }
}
