import { DesktopOS } from "../enums/services/desktop-os.enum";
import { MobileOS } from "../enums/services/mobile-os.enum";

export type DeviceTheme = "light" | "dark";
export type DeviceOS = DesktopOS | MobileOS;
export type DeviceOrientationType = 'portrait-primary' | 'landscape-primary' | 'portrait-secondary' | 'landscape-secondary';
