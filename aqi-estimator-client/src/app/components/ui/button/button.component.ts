import { NgClass } from '@angular/common';
import { Component, EventEmitter, Input, Output } from '@angular/core';
import { TranslateModule } from '@ngx-translate/core';
import { ButtonType } from '../../../shared/types/ui.types';

@Component({
  selector: 'custom-button',
  standalone: true,
  imports: [
    NgClass,
    TranslateModule
  ],
  templateUrl: './button.component.html',
  styleUrl: './button.component.scss'
})
export class ButtonComponent {
  @Input() translationKey: string = '';
  @Input() classes: string = '';
  @Input() styles: string = '';
  @Input() style: ButtonType = 'default';
  @Input() id: string = '';
  @Input() disabled: boolean = false;

  @Output() click: EventEmitter<void> = new EventEmitter<void>();

  constructor() {
  }

  protected onClick() {
    if (!this.disabled) {
      this.click.emit();
    }
  }
}
