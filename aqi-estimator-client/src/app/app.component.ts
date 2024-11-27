import { Component } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { DbService } from './shared/services/db.service';
import { TranslateModule, TranslateService } from '@ngx-translate/core';

export function prefersReducedMotion(): boolean {
  const mediaQueryList = window.matchMedia('(prefers-reduced-motion)');

  return mediaQueryList.matches;
}

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [
    RouterOutlet,
    TranslateModule,
],
  templateUrl: './app.component.html',
  styleUrl: './app.component.scss'
})
export class AppComponent {
  title = 'aqi-estimator.client';

  constructor(
    public dbService: DbService,
    private translate: TranslateService,
  ) {
    this.translate.setDefaultLang('mk');
  }
}
