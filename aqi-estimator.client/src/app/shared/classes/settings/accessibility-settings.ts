import { IAccessibilitySettingItem } from "../../interfaces/settings/accessibility-setting-item.interface";
import { IAccessibilitySettings } from "../../interfaces/settings/accessibility-settings.interface";
import { AccessibilitySettingItem } from "./accessibility-settings-item";

export class AccessabilitySettings implements IAccessibilitySettings {
    items: IAccessibilitySettingItem[];
    
    state: boolean;

    constructor(value?: IAccessibilitySettings) {
        this.items = value?.items?.map((setting: IAccessibilitySettingItem) => new AccessibilitySettingItem(setting)) || [];
        
        this.state = value?.state ?? false;
    }
}