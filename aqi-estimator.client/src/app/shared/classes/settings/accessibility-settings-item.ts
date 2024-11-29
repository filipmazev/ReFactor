import { MatSelectType, SelectionChange } from "../../types/ui.types";
import { IAccessibilitySettingItem } from "../../interfaces/settings/accessibility-setting-item.interface";

export class AccessibilitySettingItem implements IAccessibilitySettingItem {
    selfIdentifier: string; 
    state: boolean;
    
    translationKey?: string;
    matIcons?: string;
    
    ariaLabel: string;

    type: MatSelectType;

    changes?: (event: SelectionChange, value: any) => any;

    filterStyle?: string;

    constructor(value?: IAccessibilitySettingItem) {
        this.selfIdentifier = value?.selfIdentifier ?? '';
        this.state = value?.state ?? false;

        this.translationKey = value?.translationKey;
        this.matIcons = value?.matIcons;

        this.ariaLabel = value?.ariaLabel ?? '';

        this.type = value?.type ?? 'toggle';

        this.changes = value?.changes;

        this.filterStyle = value?.filterStyle;
    }
}