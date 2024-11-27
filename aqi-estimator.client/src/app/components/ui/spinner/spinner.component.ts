import { Component } from '@angular/core';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { SpinnerService } from '../../../shared/services/spinner.service';

@Component({
  selector: 'app-spinner',
  standalone: true,
  imports: [
    MatProgressSpinnerModule
  ],
  templateUrl: './spinner.component.html',
  styleUrl: './spinner.component.scss'
})
export class SpinnerComponent {
  private _show_spinner: boolean = false;
  private set show_spinner(value: boolean) { this._show_spinner = value; }
  public get show_spinner(): boolean { return this._show_spinner; }

  constructor(private spinnerService: SpinnerService) { 
    this.CreateSubscriptions();
  }

  private CreateSubscriptions() {
    this.spinnerService.getShowSpinnerState$().subscribe((show_spinner) => {
      this.show_spinner = show_spinner;
    });
  }
}
