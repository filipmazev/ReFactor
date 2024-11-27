import { MatCheckboxChange } from "@angular/material/checkbox";
import { MatOptionSelectionChange } from "@angular/material/core";
import { MatRadioChange } from "@angular/material/radio";
import { MatSlideToggleChange } from "@angular/material/slide-toggle";

export type MatSelectType = 'toggle' | 'radio' | 'checkbox';
export type SelectionChange = MatOptionSelectionChange | MatSlideToggleChange | MatCheckboxChange | MatRadioChange | MouseEvent | Event | undefined;

export type ButtonType = 'confirm' | 'warn' | 'default' | 'confirm_stylized';