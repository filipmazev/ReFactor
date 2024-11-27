import { Injectable, OnDestroy } from '@angular/core';
import { BehaviorSubject, Observable } from 'rxjs';
import { DeviceTheme } from '../types/device.types';

@Injectable({
  providedIn: 'root'
})
export class DeviceThemeService implements OnDestroy {

  private deviceThemeSubject = new BehaviorSubject<DeviceTheme>(this.getDeviceTheme());
  private mediaQueryList: MediaQueryList;
  private mediaQueryListener?: (event: MediaQueryListEvent) => void;

  constructor() { 
    this.mediaQueryList = window.matchMedia('(prefers-color-scheme: dark)');
    this.CreateSubscription();
  }

  private CreateSubscription() {
    this.mediaQueryListener = (event: MediaQueryListEvent) => {
      this.deviceThemeSubject.next(event.matches ? 'dark' : 'light');
    };

    this.mediaQueryList.addEventListener('change', this.mediaQueryListener);
  }

  public getDeviceTheme$(): Observable<DeviceTheme> { 
    return this.deviceThemeSubject.asObservable(); 
  }

  public getDeviceTheme(): DeviceTheme { 
    return this.deviceThemeSubject !== undefined ? this.deviceThemeSubject.value : (window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light');
  }

  ngOnDestroy() {
    if(this.mediaQueryListener) {
        this.mediaQueryList.removeEventListener('change', this.mediaQueryListener);
    }
  }
}
