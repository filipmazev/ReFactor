import { Component } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { DbService } from './shared/services/db.service';
import { TranslateModule, TranslateService } from '@ngx-translate/core';
import { FooterComponent } from './components/footer/footer.component';
import { NavigationComponent } from './components/navigation/navigation.component';
import { SpinnerComponent } from './components/ui/spinner/spinner.component';

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
    FooterComponent,
    NavigationComponent,
    SpinnerComponent
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
    this.translate.setDefaultLang('en');
  }
}
