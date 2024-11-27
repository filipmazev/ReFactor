import { Injectable } from "@angular/core";
import { BehaviorSubject, Observable } from "rxjs";

@Injectable({
    providedIn: 'root'
})
export class SpinnerService {
    private _show_spinner: boolean = false;
    private set show_spinner(value: boolean) { 
        this._show_spinner = value; 
        this.showSpinnerStateSubject.next(this._show_spinner);
    }
    public get show_spinner(): boolean { return this._show_spinner; }

    constructor() { }  
    
    private showSpinnerStateSubject = new BehaviorSubject<boolean>( this.getShowSpinnerState() );

    public getShowSpinnerState$(): Observable<boolean> {
        return this.showSpinnerStateSubject.asObservable();
    }

    public getShowSpinnerState(): boolean {
        return this.showSpinnerStateSubject !== undefined ? this.showSpinnerStateSubject.value : false;
    }

    public showSpinner() {
        this.show_spinner = true;
    }

    public hideSpinner() {
        this.show_spinner = false;
    }
}