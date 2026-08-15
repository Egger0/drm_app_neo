#pragma once

void ui_battery_init();
void ui_battery_destroy();
// 切主题后重设颜色(电池在 lv_layer_top, report_style_change 不覆盖它)
void ui_battery_apply_theme(void);
