import { IAccessibilitySettingItem } from "./accessibility-setting-item.interface";
import { ISettingItem } from "./setting-item.interface";

export interface IAccessibilitySettings extends ISettingItem {
    items: IAccessibilitySettingItem[];
    
    state: boolean;
}