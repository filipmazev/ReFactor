import { Language } from "../../enums/settings/languages.enum";
import { DeviceTheme } from "../../types/device.types";

export interface ISiteSettings {
    theme: DeviceTheme;
    language: Language; 
}