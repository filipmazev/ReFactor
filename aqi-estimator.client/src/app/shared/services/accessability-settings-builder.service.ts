import { Injectable, OnDestroy } from '@angular/core';
import { IAccessibilitySettingItem } from '../interfaces/settings/accessibility-setting-item.interface';
import { MatIcons } from '../enums/ui/mat-icons.enum';
import { SelectionChange } from '../types/ui.types';
import { MatOptionSelectionChange } from '@angular/material/core';
import { MatSlideToggleChange } from '@angular/material/slide-toggle';
import { MatCheckboxChange } from '@angular/material/checkbox';
import { MatRadioChange } from '@angular/material/radio';
import { DbStores } from '../enums/db/db-stores.enum';
import { Subject, takeUntil } from 'rxjs';
import { DbService } from './db.service';
import { LARGE_TEXT_MULTIPLIER } from '../constants/accessibility-settings.constants';
import { REM_MULTIPLIER } from '../constants/unit.constants';

@Injectable({
  providedIn: `root`
})
export class AccessabilitySettingsBuilderService implements OnDestroy {
  private subscriptions: Subject<void>[] = [];

  private initialBuild: boolean = true;

  private observer!: MutationObserver;
  private largeTextState: boolean = false;

  private htmlElement: HTMLElement = document.querySelector('html') as HTMLElement;

  constructor(private dbService: DbService,
  ) { 
    this.adjustFontSizes();
    this.observeDOMChanges();
  }

  //#region Build Methods
  public async build(): Promise<IAccessibilitySettingItem[]> {
    return new Promise(async (resolve, reject) => {
      try 
      {
        this.subscriptions.push(new Subject<void>());

        this.dbService.getDbCreated$().pipe(takeUntil(this.subscriptions[this.subscriptions.length - 1])).subscribe(async (result: boolean) => {
          if(result === true) {
            resolve(await this.startBuilding());
          }
        });
      } 
      catch (error) {
        reject(error);
      }
    });
  }

  private async startBuilding(): Promise<IAccessibilitySettingItem[]> {
    return new Promise(async (resolve, reject) => {
      try 
      {
        const settings: IAccessibilitySettingItem[] = [];

        // Defining the settings
        const highContrast: IAccessibilitySettingItem = { selfIdentifier: `high_contrast`, translationKey: 'accessibility_settings.high_contrast', ariaLabel: 'high_contrast', matIcons: MatIcons.Contrast, changes: this.filterSettingClicked.bind(this), filterStyle: "contrast(130%)" };
        const monochrome: IAccessibilitySettingItem = { selfIdentifier: `monochrome`, translationKey: 'accessibility_settings.monochrome', ariaLabel: 'monochrome', matIcons: MatIcons.Monochrome, changes: this.filterSettingClicked.bind(this), filterStyle: "grayscale(100%)" };
        const invertColors: IAccessibilitySettingItem = { selfIdentifier: `invert_colors`, translationKey: 'accessibility_settings.invert_colors', ariaLabel: 'invert_colors', matIcons: MatIcons.InvertColors, changes: this.filterSettingClicked.bind(this), filterStyle: "invert(100%)" };
        const largeText: IAccessibilitySettingItem = { selfIdentifier: `large_text`, translationKey: 'accessibility_settings.large_text', ariaLabel: 'large_text', matIcons: MatIcons.LargeText, changes: this.largeTextClicked.bind(this) };
            
        settings.push(highContrast, monochrome, invertColors, largeText);
    
        // Check for selfIdentifier duplicates
        this.selfIdentifierCheck(settings);
    
        // Get the states from IndexedDb
        await this.synchronizeStates(settings);

        this.initialBuild = false;
        resolve(settings);
      } 
      catch (error) {
        reject(error);
      }
    });
  }
  //#endregion

  //#region Private Methods
  private settingClicked(event: SelectionChange, setting: IAccessibilitySettingItem): Promise<boolean> {
    return new Promise(async (resolve, reject) => {
      try 
      {
        let state: boolean = setting.state ?? false;
  
        if(event instanceof MatOptionSelectionChange) { state = event.source.selected; }
        else if (event instanceof MatSlideToggleChange) { state = event.checked; }
        else if (event instanceof MatCheckboxChange) { state = event.checked; }
        else if (event instanceof MatRadioChange) { state = event.source.checked; }
        else if (event instanceof MouseEvent) { state = !state; }
        else if (event instanceof Event) { state = !state; }

        await this.saveSetting(setting);
        resolve(state);
      }
      catch(error) 
      { 
        reject(error); 
      }
    });
  }

  private async saveSetting(setting: IAccessibilitySettingItem): Promise<void> {
    return new Promise(async (resolve, reject) => {
      try 
      {
        await this.dbService.saveData(DbStores.ACCESSIBILITY_SETTINGS, setting.selfIdentifier, setting.state);
        resolve();
      } 
      catch (error) {
        reject(error);
      }
    });
  }

  private async synchronizeStates(settings: IAccessibilitySettingItem[]): Promise<void> {
    for(let setting of settings) {
      const state = await this.dbService.fetchData(DbStores.ACCESSIBILITY_SETTINGS, setting.selfIdentifier);
      setting.state = state !== null && state !== undefined ? state : setting.state;
      if(this.initialBuild && setting.state === true && setting.changes) {
        await setting.changes(undefined, setting);
      }
    }
  }

  private selfIdentifierCheck(settings: IAccessibilitySettingItem[]): void {
    const settingsClone = JSON.parse(JSON.stringify(settings));
    for(let i = 0; i < settingsClone.length; i++) {
      const setting = settings[i];
      if (settings.find((s, index) => {
        return s.selfIdentifier === setting.selfIdentifier && index !== i;
      })) {
        settingsClone.splice(settingsClone.indexOf(setting), 1);
        setting.selfIdentifier = setting.selfIdentifier + i;
      }
    }
  }
  //#endregion

  //#region Filter Settings
  private async filterSettingClicked(event: SelectionChange, setting: IAccessibilitySettingItem): Promise<void> {
    return new Promise(async (resolve, reject) => {
      try 
      {
        const state = await this.settingClicked(event, setting);
        if(setting.filterStyle) {
          if(state) {
            this.htmlElement.style.filter += `${setting.filterStyle}`;
          } else {
            this.htmlElement.style.filter = this.htmlElement.style.filter.replace(setting.filterStyle, '');
          }
        }

        resolve();
      }
      catch(error) 
      { 
        reject(error); 
      }
    });
  }
  //#endregion

  //#region Large Text
  private async largeTextClicked(event: SelectionChange, setting: IAccessibilitySettingItem): Promise<void> {
    return new Promise(async (resolve, reject) => {
      try 
      {
        const state = await this.settingClicked(event, setting);
        this.largeTextState = state;
        this.adjustFontSizes();

        resolve();
      }
      catch(error) 
      { 
        reject(error); 
      }
    });
  }

  private adjustFontSizes(): void {
    const elements = document.body.querySelectorAll(`*`);
    elements.forEach((el: Element) => {
      const element = el as HTMLElement;
      if (element.tagName !== `SCRIPT`) {
        const text = element.innerText;
        const style = getComputedStyle(element);

        if (text && text.length > 0 && style.fontSize && !isNaN(parseInt(style.fontSize)) && el.parentElement?.tagName !== `MAT-ICON`) {
          const fontSize = parseInt(style.fontSize) / REM_MULTIPLIER;
          const increasedFontSize = fontSize * LARGE_TEXT_MULTIPLIER;
          if(element.getAttribute(`data-font-resized`) !== `true` || !this.largeTextState) {
            element.style.fontSize = this.largeTextState ? `${increasedFontSize}rem` : ``;
          }
          element.setAttribute(`data-font-resized`, this.largeTextState ? `true` : `false`);
        }
      }
    });
  }

  private observeDOMChanges(): void {
    this.observer = new MutationObserver(() => {
      this.adjustFontSizes();
    });

    const config = { childList: true, subtree: true };
    this.observer.observe(document.body, config);
  }
  //#endregion

  //#region Lifecycle Hooks
  ngOnDestroy(): void {
    this.subscriptions.forEach(subscription => {
      subscription.next();
      subscription.complete();
    });

    if (this.observer) {
      this.observer.disconnect();
    }
  }
  //#endregion
}
