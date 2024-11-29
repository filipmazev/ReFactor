import { IAccessibilitySettings } from "../../interfaces/settings/accessibility-settings.interface";
import { ILanguageSettings } from "../../interfaces/settings/language-settings.interface";
import { ISiteSettings } from "../../interfaces/settings/site-settings.interface";
import { IThemeSettings } from "../../interfaces/settings/theme-settings.interface";
import { AccessabilitySettings } from "./accessibility-settings";
import { LanguageSettings } from "./language-settings";
import { ThemeSettings } from "./theme-settings";

export class SiteSettings implements ISiteSettings {
    theme: IThemeSettings;
    accessibility: IAccessibilitySettings;
    language: ILanguageSettings; 

    constructor(value?: ISiteSettings) {
        this.theme = new ThemeSettings(value?.theme);
        this.accessibility = new AccessabilitySettings(value?.accessibility);
        this.language = new LanguageSettings(value?.language);
    }
}