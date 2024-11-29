import { IAccessibilitySettings } from "./accessibility-settings.interface";
import { ILanguageSettings } from "./language-settings.interface";
import { IThemeSettings } from "./theme-settings.interface";

export interface ISiteSettings {
    theme: IThemeSettings;
    accessibility: IAccessibilitySettings;
    language: ILanguageSettings; 
}