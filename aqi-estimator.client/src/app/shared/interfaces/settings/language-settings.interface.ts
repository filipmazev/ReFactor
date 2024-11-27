import { Language } from "../../enums/settings/languages.enum";
import { ISettingItem } from "./setting-item.interface";

export interface ILanguageSettings extends ISettingItem {
    selected: Language;
    available: Language[];

    state: boolean;
}