import { Component } from '@angular/core';
import { MatMenuModule } from '@angular/material/menu';
import { MatIconModule } from '@angular/material/icon';
import { FormsModule } from '@angular/forms';
import { TranslateModule } from '@ngx-translate/core';
import * as classNames from '../../shared/constants/class-names.constants';

@Component({
  selector: 'app-navigation',
  standalone: true,
  imports: [
    MatMenuModule,
    MatIconModule,
    FormsModule,
    TranslateModule,
],
  templateUrl: './navigation.component.html',
  styleUrl: './navigation.component.scss'
})
export class NavigationComponent {
  protected readonly classNames = classNames;

  constructor() { }
}
