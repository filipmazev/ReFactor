export interface IScrollLockConfig {
    allow_touch_input_on?: Element[];
    main_container?: HTMLElement;
    handle_extreme_overflow?: boolean;
    animation_duration?: number;
    handle_touch_input?: boolean;
    mobile_only_touch_prevention?: boolean;
}