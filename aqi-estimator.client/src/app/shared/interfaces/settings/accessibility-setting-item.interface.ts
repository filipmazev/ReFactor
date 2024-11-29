import { MatSelectType, SelectionChange } from "../../types/ui.types";

export interface IAccessibilitySettingItem {
    selfIdentifier: string;

    state?: boolean;

    translationKey?: string;
    matIcons?: string;

    ariaLabel?: string;

    type?: MatSelectType;

    changes?: (event: SelectionChange, value?: any) => any;

    filterStyle?: string;
}