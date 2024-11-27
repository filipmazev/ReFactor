import { Component } from '@angular/core';
import { TranslateModule } from '@ngx-translate/core';
import { ButtonComponent } from '../../components/ui/button/button.component';

@Component({
    selector: 'app-landing',
    standalone: true,
    imports: [
        TranslateModule,
        ButtonComponent,
    ],
    templateUrl: './landing.component.html',
    styleUrl: './landing.component.scss'
})
export class LandingComponent {

}
