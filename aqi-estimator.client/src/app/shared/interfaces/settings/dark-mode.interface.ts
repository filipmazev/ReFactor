import { IDarkModeThemeSettings } from "./dark-mode-theme-settings.interface";

export interface IDarkMode {
    state: boolean;
    theming: IDarkModeThemeSettings[];
}