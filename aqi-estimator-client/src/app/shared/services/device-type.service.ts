import { Injectable } from '@angular/core';
import { DesktopOS } from '../enums/services/desktop-os.enum';
import { MobileOS } from '../enums/services/mobile-os.enum';
import { DeviceState } from '../interfaces/services/device-state.interface';
import { DeviceOrientationType, DeviceOS } from '../types/device.types';

@Injectable({
  providedIn: 'root'
})
export class DeviceTypeService {
  private userAgent: string = navigator.userAgent || navigator.vendor || (window as any).opera || undefined;
  private isDesktopDevice: boolean = !this.isMobileDevice() && !this.isTabletDevice();
  
  private isMobileDevice(): boolean {
    const regexs = [/(Android)(.+)(Mobile)/i, /BlackBerry/i, /iPhone|iPod/i, /Opera Mini/i, /IEMobile/i];
    return regexs.some(b => this.userAgent.match(b) !== null);
  }
  
  private isTabletDevice(): boolean {
    const regex = /(ipad|tablet|(android(?!.*mobile))|(windows(?!.*phone)(.*touch))|kindle|playbook|silk|(puffin(?!.*(IP|AP|WP))))/;
    return regex.test(this.userAgent.toLowerCase());
  }

  private getMobileOS(): MobileOS | undefined {
    if (this.isMobileDevice()) {
      if (/windows phone/i.test(this.userAgent)) return MobileOS.WindowsPhone;
      else if (/android/i.test(this.userAgent)) return MobileOS.Android;
      else if (/iPad|iPhone|iPod/.test(this.userAgent) && !(window as any).MSStream) return MobileOS.iOS;

      return MobileOS.Unknown;
    } else return undefined;
  }

  private getDesktopOS(): DesktopOS | undefined {
    if (this.isDesktopDevice) {
      if (this.userAgent.indexOf('Win') !== -1) return DesktopOS.Windows;
      else if (this.userAgent.indexOf('Mac') !== -1) return DesktopOS.MacOS;
      else if (this.userAgent.indexOf('X11') !== -1) return DesktopOS.Unix;
      else if (this.userAgent.indexOf('Linux') !== -1) return DesktopOS.Linux;

      return DesktopOS.Unknown;
    } else return undefined;
  }

  private getDeviceOS(): DeviceOS | undefined {
    return this.getMobileOS() ?? this.getDesktopOS();
  }

  private supportedScreenOrientation =
    (screen?.orientation || {}).type ?? (screen as any).mozOrientation ?? (screen as any).msOrientation;

  private safariScreenOrientation: DeviceOrientationType =
    !screen?.orientation && matchMedia('(orientation: portrait)').matches ? 'portrait-primary' : 'landscape-primary';

  private initialScreenOrientation: DeviceOrientationType = this.supportedScreenOrientation ?? this.safariScreenOrientation ?? 'portrait-primary';
  private screenOrientation: DeviceOrientationType = this.initialScreenOrientation;

  constructor() {
    if (screen.orientation) {
      screen.orientation.addEventListener(
        'change',
        (ev: Event) => (this.screenOrientation = (ev.target ?? ({} as any)).type as OrientationType)
      );
    }
  }

  isLandscapeOrientation(): boolean {
    return ['landscape-primary', 'landscape-secondary'].includes(this.screenOrientation);
  }

  isPortraitOrientation(): boolean {
    return ['portrait-primary', 'portrait-secondary'].includes(this.screenOrientation);
  }

  getDeviceState(): DeviceState {
    const isDesktop = this.isDesktopDevice;
    const isMobile = this.isMobileDevice();
    const isTablet = this.isTabletDevice();
    const mobileOS: MobileOS | undefined = this.getMobileOS();
    const isAndroidDevice = this.getDeviceOS() === MobileOS.Android;
    const isAppleDevice = this.getDeviceOS() === MobileOS.iOS || this.getDeviceOS() === DesktopOS.MacOS;
    const isUnknownMobileDevice = this.getDeviceOS() === MobileOS.Unknown;
    const desktopOS: DesktopOS | undefined = this.getDesktopOS();
    const isWindowsDesktop = this.getDeviceOS() === DesktopOS.Windows;
    const isLinuxOrUnixDesktop = this.getDeviceOS() === DesktopOS.Linux || this.getDeviceOS() === DesktopOS.Unix;

    return {
      isDesktop,
      desktopOS,
      isWindowsDesktop,
      isLinuxOrUnixDesktop,
      isMobile,
      mobileOS,
      isAndroidDevice,
      isAppleDevice,
      isUnknownMobileDevice,
      isTablet,
      isLandscapeOrientation: () => this.isLandscapeOrientation(),
      isPortraitOrientation: () => this.isPortraitOrientation()
    };
  }
}
