import { Component } from '@angular/core';
import { TranslateModule } from '@ngx-translate/core';
import { ImageCaptureComponent } from '../../components/image-capture/image-capture.component';

@Component({
    selector: 'app-landing',
    standalone: true,
    imports: [
        TranslateModule,
        ImageCaptureComponent
    ],
    templateUrl: './landing.component.html',
    styleUrl: './landing.component.scss'
})
export class LandingComponent {

}
