import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { MatDialogModule } from '@angular/material/dialog';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import 'hammerjs'; 
import { LandingComponent } from './pages/landing/landing.component';

export const routes: Routes = [
    {
        path: '',
        component: LandingComponent,
    },
];

export function prefersReducedMotion(): boolean {
    const mediaQueryList = window.matchMedia('(prefers-reduced-motion)');
    return mediaQueryList.matches;
}  

@NgModule({
    imports: [
    RouterModule.forChild(routes),
    FormsModule,
    ReactiveFormsModule,
    MatDialogModule,
    BrowserAnimationsModule.withConfig({
        disableAnimations: prefersReducedMotion()
    }),
],
declarations: [],
    exports: [RouterModule]
})
export class RegisterRouter { }


  