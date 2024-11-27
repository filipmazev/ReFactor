import { Injectable, OnDestroy } from "@angular/core";
import { TranslateService } from "@ngx-translate/core";
import { BehaviorSubject, Observable, Subject, take, takeUntil } from "rxjs";

@Injectable({
    providedIn: 'root',
})
export class TranslationHelper implements OnDestroy {
  translations: Map<{ constructor: Function }, Map<string ,string>> = new Map();
  
  private languageSubject = new BehaviorSubject<string | undefined>(this.getLanguage());
  private unsubscribe$ = new Subject<void>();

  constructor(private translate: TranslateService ){ 
    this.CreateSubscriptions();
  }

  ngOnDestroy(): void {
    this.unsubscribe$.next();
    this.unsubscribe$.complete();
  }

  private async CreateSubscriptions() {
    this.translate.onLangChange.pipe(takeUntil(this.unsubscribe$)).subscribe(async (lang) => {
      await this.updateTranslations();
      this.languageSubject.next(lang.lang);
    });
  }

  private async updateTranslations(): Promise<void> {
    return new Promise(async (resolve) => {
      const promises: Promise<string[]>[] = [];
      for(let [key, value] of this.translations) {
        if(value){
          promises.push(this.getTranslations(Array.from(value.keys()), key));
        }
      }
      await Promise.all(promises);
      resolve();
    });
  }

  private getLanguage(): string | undefined {
    return this.translate ? this.translate.currentLang : undefined;
  }

  public language$ (): Observable<string | undefined> { 
    return this.languageSubject.asObservable(); 
  }

  public getSelfTranslations(self: { constructor: Function }): Map<string, string> | undefined {
    return this.translations.get(self);
  }

  public getTranslation(key: string, self: { constructor: Function }): Promise<string> {
    return new Promise((resolve) => {
      this.translate.get(key).pipe(take(1)).subscribe((res: string) => {
        if(this.translations.has(self)){
          this.translations.get(self)?.set(key, res);
        } else {
          this.translations.set(self, new Map([[key, res]]));
        }
        resolve(res);
      });
    });
  }

  public getTranslations(keys: string[], self: { constructor: Function }): Promise<string[]> {
    return new Promise((resolve) => {
      this.translate.get(keys).pipe(take(1)).subscribe((res: any) => {
        if(this.translations.has(self)){
          const selfInstance = this.translations.get(self);
          if(selfInstance){
            keys.forEach((key) => {
              selfInstance.set(key, res[key]);
            });
          }
        } else {
          const map = new Map<string, string>();
          keys.forEach((key) => {
            map.set(key, res[key]);
          });
          this.translations.set(self, map);
        }
        resolve(res);
      });
    });
  }

  public setDefaultLang(lang: string): void {
    this.translate.setDefaultLang(lang);
  }

  public changeLanguage(lang: string): void {
    this.translate.use(lang);
  }
}