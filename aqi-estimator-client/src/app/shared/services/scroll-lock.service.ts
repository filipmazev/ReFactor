import { Injectable } from '@angular/core';
import { DeviceTypeService } from './device-type.service';
import { WindowDimensions } from '../interfaces/services/window-dimensions.interface';
import { WindowDimensionsService } from './window-dimension.service';
import { IScrollLockConfig } from '../interfaces/services/scroll-lock-config.interface';
import { SCROLL_LOCK_INSTANCE_IDENTIFIER } from '../constants/scroll-lock.constants';

@Injectable({
  providedIn: 'root'
})
export class ScrollLockService {
  private instanceId: string;

  private configurationInstances: Map<string, IScrollLockConfig> = new Map<string, IScrollLockConfig>();
  private windowDimensions: WindowDimensions = {} as WindowDimensions;

  constructor(
    private deviceTypeService: DeviceTypeService, 
    private windowDimensionSerivce: WindowDimensionsService) {
    this.instanceId = SCROLL_LOCK_INSTANCE_IDENTIFIER + Math.random().toString(36).substring(2, 9);
    this.windowDimensionSerivce.getWindowDimensions$().subscribe((dimensions) => {
      this.windowDimensions = dimensions;
    });
  }

  public disableScroll(config: IScrollLockConfig) {
    document.body.style.setProperty('overflow', 'hidden', 'important');
    
    const documentWidth = document.documentElement.clientWidth;
    const windowWidth = window.innerWidth;
    const scrollBarWidth = windowWidth - documentWidth;
    document.body.style.paddingRight = scrollBarWidth + 'px';
    document.body.style.setProperty('padding-right', scrollBarWidth + 'px', 'important');

    if (config.handle_touch_input !== false) { document.body.style.setProperty('touch-action', 'none', 'important'); }
        
    this.configurationInstances.set(this.instanceId, config);

    if(config.main_container !== undefined && config.main_container.parentElement !== null) {
      let currentNode = config.main_container.parentElement as HTMLElement | null;
      while (currentNode !== null) {
        currentNode.style.setProperty('overflow', 'hidden', 'important');
        
        if (config.handle_touch_input !== false) { 
          currentNode.addEventListener('touchmove', (event) => this.handleTouchMove(event), { passive: false });
          currentNode.style.setProperty('touch-action', 'none', 'important'); 
        }
        
        currentNode = currentNode.parentElement;
      }
    }

    setTimeout(() => { 
      if(config.handle_touch_input !== false){
        document.body.addEventListener('touchmove', (event) => this.handleTouchMove(event), { passive: false });
      }

      if (config.handle_extreme_overflow !== false) { 
        const options = { passive: false };

        window.addEventListener('wheel', this.preventDefault, options);
        window.addEventListener('mousewheel', this.preventDefault, options);
        window.addEventListener('scroll', this.preventDefault, options);
        window.addEventListener('DOMMouseScroll', this.preventDefault, options);
      }
    }, (config.animation_duration ?? 0) + 10);
  }
  
  public enableScroll(extreme_overflow?: boolean) {
    document.body.style.removeProperty('overflow'); 
    document.body.style.removeProperty('padding-right');
    
    let currentConfiguration = this.configurationInstances.get(this.instanceId);
    
    if(currentConfiguration && currentConfiguration.handle_touch_input !== false){ 
      document.body.removeEventListener('touchmove', this.handleTouchMove); 
      document.body.style.removeProperty('touch-action');
    }

    if(currentConfiguration !== undefined && currentConfiguration.main_container !== undefined && currentConfiguration.main_container.parentElement !== null){
      let currentNode = currentConfiguration.main_container.parentElement as HTMLElement | null;
      while (currentNode !== null) {
        currentNode.style.removeProperty('overflow');

        if (currentConfiguration.handle_touch_input !== false) { 
          currentNode.removeEventListener('touchmove', this.preventDefault);
          currentNode.style.removeProperty('touch-action'); 
        }
        
        currentNode = currentNode.parentElement;
      }
    }

    this.configurationInstances.delete(this.instanceId);

    if (extreme_overflow !== false) { 
      window.removeEventListener('wheel', this.preventDefault);
      window.removeEventListener('mousewheel', this.preventDefault);
      window.removeEventListener('scroll', this.preventDefault);
      window.removeEventListener('DOMMouseScroll', this.preventDefault);
    }
  }

  private handleTouchMove(event: Event) {
    const targetNode = event.target as Node;
    const currentConfiguration = this.configurationInstances.get(this.instanceId);
  
    if (!this.isAllowedToScroll(targetNode) && (currentConfiguration === null || currentConfiguration?.handle_touch_input !== false)) {
      if (currentConfiguration === null || currentConfiguration?.mobile_only_touch_prevention !== true ||
        (currentConfiguration?.mobile_only_touch_prevention === true && ((!this.deviceTypeService.getDeviceState().isMobile || !this.deviceTypeService.getDeviceState().isTablet)
          && (this.windowDimensions.width < this.windowDimensions.threshold_sm)))) {
        event.preventDefault();
        event.stopPropagation();
      }
    }
  }
  
  private isAllowedToScroll(targetNode: Node): boolean {
    const currentConfiguration = this.configurationInstances.get(this.instanceId);
  
    if (!currentConfiguration?.allow_touch_input_on || currentConfiguration.allow_touch_input_on.length === 0) { return true; }
  
    for (const element of currentConfiguration.allow_touch_input_on) {
      if (element.contains(targetNode)) {
        return true;
      }
    }
  
    return false;
  }
 
  private preventDefault(event: Event) {
    event.preventDefault();
    event.stopPropagation();
  }

  ngOnDestroy() {
    this.enableScroll();
  }
}
