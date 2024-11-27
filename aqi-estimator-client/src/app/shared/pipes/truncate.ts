import { Pipe, PipeTransform } from '@angular/core';
import { EMPTY_STRING, TRUNCATE_SUFFIX } from '../constants/common.constants';

@Pipe({
  name: 'truncate',
  standalone: true,
})
export class TruncatePipe implements PipeTransform {
  transform(value: string, maxLength: number): string {
    if (!value) return EMPTY_STRING;
    return value.length > maxLength ? value.substring(0, maxLength) + TRUNCATE_SUFFIX : value;
  }
}