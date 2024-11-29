import { Language } from "../../enums/settings/languages.enum";
import { ILanguageSettings } from "../../interfaces/settings/language-settings.interface";

export class LanguageSettings implements ILanguageSettings {
    selected: Language;
    available: Language[];

    state: boolean;

    constructor(value?: ILanguageSettings) {
        this.selected = value?.selected ?? Language.Macedonian;
        this.available = value?.available ?? Object.values(Language);

        this.state = value?.state ?? false;
    }
}