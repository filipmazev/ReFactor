import { DeviceTheme } from "../../types/device.types";
import { IDarkMode } from "./dark-mode.interface";
import { ISettingItem } from "./setting-item.interface";

export interface IThemeSettings extends ISettingItem {
    darkMode: IDarkMode;

    autoDarkMode: boolean;
    deviceTheme: DeviceTheme;

    state: boolean;
}