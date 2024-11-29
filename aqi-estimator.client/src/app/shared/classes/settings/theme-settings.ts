import { DARK_MODE_CLASS_NAME_PREFIX } from "../../constants/class-names.constants";
import { DarkModeTheme } from "../../enums/settings/dark-mode-theme.enum";
import { IDarkMode } from "../../interfaces/settings/dark-mode.interface";
import { IThemeSettings } from "../../interfaces/settings/theme-settings.interface";
import { DeviceTheme } from "../../types/device.types";

export class ThemeSettings implements IThemeSettings {
    darkMode: IDarkMode;
    
    autoDarkMode: boolean;
    deviceTheme: DeviceTheme;

    state: boolean;
    
    constructor(value?: IThemeSettings,) {
        this.darkMode = value?.darkMode ?? { 
            state: false, 
            theming: Object.values(DarkModeTheme).map((theme, index) => ({
                className: DARK_MODE_CLASS_NAME_PREFIX + theme.toLowerCase(),
                theme,
                selected: index === 0,
                translationKey: 'ui.dark_theme_' + theme.toLocaleLowerCase(),
            }))
        };

        this.autoDarkMode = value?.autoDarkMode ?? false;
        this.deviceTheme = value?.deviceTheme ?? 'light';

        this.state = value?.state ?? false;
    }
}