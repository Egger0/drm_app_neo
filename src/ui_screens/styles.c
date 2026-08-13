#include "styles.h"
#include "ui/font_registry.h"
#include "ui/ui_theme.h"
#include "ui_metrics.h"

// 字号以 360 基准书写 (与 EEZ 烘焙字号对齐)，font_get 内部套 S()。
#define PX_LABEL_LARGE 24
#define PX_LABEL_SMALL 14
#define PX_FA_LABEL    60

// ---- label_large: 标题 ----
static lv_style_t *style_label_large(void)
{
    static lv_style_t *s;
    if (!s) {
        s = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(s);
        lv_style_set_text_font(s, font_get(FONT_TITLE, PX_LABEL_LARGE));
    }
    return s;
}
void add_style_label_large(lv_obj_t *obj)
{
    lv_obj_add_style(obj, style_label_large(), LV_PART_MAIN | LV_STATE_DEFAULT);
}

// ---- label_small: 正文 ----
static lv_style_t *style_label_small(void)
{
    static lv_style_t *s;
    if (!s) {
        s = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(s);
        lv_style_set_text_font(s, font_get(FONT_BODY, PX_LABEL_SMALL));
    }
    return s;
}
void add_style_label_small(lv_obj_t *obj)
{
    lv_obj_add_style(obj, style_label_small(), LV_PART_MAIN | LV_STATE_DEFAULT);
}

void set_style_label_size(lv_obj_t *obj, bool large)
{
    lv_obj_remove_style(obj, style_label_large(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, style_label_small(), LV_PART_MAIN | LV_STATE_DEFAULT);
    if (large)
        add_style_label_large(obj);
    else
        add_style_label_small(obj);
}

// ---- fa_label: 图标 ----
static lv_style_t *style_fa_label(void)
{
    static lv_style_t *s;
    if (!s) {
        s = (lv_style_t *)lv_malloc(sizeof(lv_style_t));
        lv_style_init(s);
        lv_style_set_text_font(s, font_get(FONT_ICON, PX_FA_LABEL));
    }
    return s;
}
void add_style_fa_label(lv_obj_t *obj)
{
    lv_obj_add_style(obj, style_fa_label(), LV_PART_MAIN | LV_STATE_DEFAULT);
}

// ======================= 带色 style (随主题翻转) =======================
// 非色属性 (pad/radius/margin/opa/font) 首次 ensure 时设一次；颜色统一在
// styles_apply_palette() 里按当前调色板 (重)设，切主题走重着色刷新，不重建屏。
static bool s_inited;

static lv_style_t s_main_btn_def,   s_main_btn_foc;
static lv_style_t s_main_small_def, s_main_small_foc;
static lv_style_t s_op_btn_def,     s_op_btn_foc;
static lv_style_t s_op_entry;       // 仅 margin，无色
static lv_style_t s_flag_sd, s_flag_run, s_flag_fg, s_flag_notrun, s_flag_res;
static lv_style_t s_fill_primary, s_fill_warning, s_fill_danger, s_fill_success, s_fill_neutral;
static lv_style_t s_spinner_arc, s_log_text;
static lv_style_t s_focus;      // 键盘焦点外框 (比主题默认更粗、随 S() 缩放)
static lv_style_t s_focus_float; // 通用聚焦悬浮(阴影+上移)，替代外框
static lv_style_t s_screen_bg;  // 屏幕背景 (随方案换底)
static lv_style_t s_sw_track, s_sw_track_chk, s_sw_ind_chk, s_sw_knob, s_sw_knob_chk;  // 开关 pill

static void init_flag_base(lv_style_t *s)
{
    lv_style_init(s);
    lv_style_set_bg_opa(s, 255);
    // 上下留白，让彩色圆角底完整包住整行文字 (含中文字形的上下伸展)，
    // 否则贴字太紧会露出字的顶/底。调整时注意 oplist/applist 里两个竖直堆叠的
    // 角标间距 (见各屏 make_slot 的 y 坐标)，别让加高后的角标互相重叠。
    // 上多下少：字体行盒底部含 descender 空隙，数字/中文会偏上，多给顶距把字压低居中。
    lv_style_set_pad_top(s, S(5));
    lv_style_set_pad_bottom(s, S(1));
    lv_style_set_pad_left(s, S(2));
    lv_style_set_pad_right(s, S(2));
    lv_style_set_radius(s, S(15));
    lv_style_set_text_font(s, font_get(FONT_BODY, 14));
    // 角标一律彩色饱和底，配白字保证可读 (不随主题继承按钮文字色)。
    lv_style_set_text_color(s, lv_color_white());
}

static void init_fill(lv_style_t *s)
{
    lv_style_init(s);
    lv_style_set_bg_opa(s, LV_OPA_COVER);
}

static void styles_ensure(void)
{
    if (s_inited) return;
    s_inited = true;

    lv_style_init(&s_main_btn_def);
    lv_style_set_text_align(&s_main_btn_def, LV_TEXT_ALIGN_CENTER);
    // 常态轻微投影，按钮自带一点浮起感
    lv_style_set_shadow_width(&s_main_btn_def, S(4));
    lv_style_set_shadow_color(&s_main_btn_def, lv_color_black());
    lv_style_set_shadow_opa(&s_main_btn_def, LV_OPA_20);
    lv_style_set_shadow_offset_y(&s_main_btn_def, S(2));
    lv_style_init(&s_main_btn_foc);
    // 选中悬浮：阴影加深 + 按钮上移 2px（替代原焦点外框）
    lv_style_set_shadow_width(&s_main_btn_foc, S(18));
    lv_style_set_shadow_color(&s_main_btn_foc, lv_color_black());
    lv_style_set_shadow_opa(&s_main_btn_foc, LV_OPA_40);
    lv_style_set_shadow_offset_y(&s_main_btn_foc, S(8));
    lv_style_set_translate_y(&s_main_btn_foc, S(-4));

    lv_style_init(&s_main_small_def);
    lv_style_init(&s_main_small_foc);

    lv_style_init(&s_op_btn_def);
    lv_style_set_pad_all(&s_op_btn_def, S(8));
    lv_style_set_margin_top(&s_op_btn_def, 0);
    lv_style_init(&s_op_btn_foc);

    lv_style_init(&s_op_entry);
    lv_style_set_margin_top(&s_op_entry, S(5));

    init_flag_base(&s_flag_sd);
    init_flag_base(&s_flag_run);
    init_flag_base(&s_flag_fg);
    init_flag_base(&s_flag_notrun);
    init_flag_base(&s_flag_res);

    init_fill(&s_fill_primary);
    init_fill(&s_fill_warning);
    init_fill(&s_fill_danger);
    init_fill(&s_fill_success);
    init_fill(&s_fill_neutral);

    lv_style_init(&s_spinner_arc);
    lv_style_init(&s_log_text);

    lv_style_init(&s_focus);
    lv_style_set_outline_width(&s_focus, S(4)); // 非按钮控件(滑条/开关/下拉)用外框确认选中
    lv_style_set_outline_pad(&s_focus, S(2));
    lv_style_set_outline_opa(&s_focus, LV_OPA_COVER);

    // 通用聚焦悬浮：阴影加深 + 上移 4px，套到所有可聚焦按钮
    lv_style_init(&s_focus_float);
    lv_style_set_shadow_width(&s_focus_float, S(18));
    lv_style_set_shadow_color(&s_focus_float, lv_color_black());
    lv_style_set_shadow_opa(&s_focus_float, LV_OPA_40);
    lv_style_set_shadow_offset_y(&s_focus_float, S(8));
    lv_style_set_translate_y(&s_focus_float, S(-4));

    lv_style_init(&s_screen_bg);
    lv_style_set_bg_opa(&s_screen_bg, LV_OPA_COVER);

    // 开关 pill: 轨道圆角满, 开启指示器黄, 旋钮白带轻投影
    lv_style_init(&s_sw_track);
    lv_style_set_radius(&s_sw_track, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&s_sw_track, LV_OPA_COVER);
    lv_style_init(&s_sw_track_chk);
    lv_style_set_radius(&s_sw_track_chk, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&s_sw_track_chk, LV_OPA_COVER);
    lv_style_init(&s_sw_ind_chk);
    lv_style_set_radius(&s_sw_ind_chk, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&s_sw_ind_chk, LV_OPA_COVER);
    lv_style_init(&s_sw_knob);
    lv_style_set_radius(&s_sw_knob, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&s_sw_knob, LV_OPA_COVER);
    lv_style_set_shadow_width(&s_sw_knob, S(3));
    lv_style_set_shadow_opa(&s_sw_knob, LV_OPA_30);
    lv_style_set_shadow_offset_y(&s_sw_knob, S(1));
    lv_style_init(&s_sw_knob_chk);
    lv_style_set_radius(&s_sw_knob_chk, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&s_sw_knob_chk, LV_OPA_COVER);

    styles_apply_palette();
}

void styles_apply_palette(void)
{
    if (!s_inited) { styles_ensure(); return; } // ensure 末尾会回调本函数

    // 主按钮常态与聚焦都用中性浅灰打底，选中不加深，靠焦点外框(s_focus)区分；
    // 主题主色留给角标/状态条做点缀。
    lv_style_set_bg_color(&s_main_btn_def,   ui_color(UI_C_NEUTRAL));
    lv_style_set_bg_color(&s_main_btn_foc,   ui_color(UI_C_NEUTRAL));
    // 主按钮继承 LVGL 主题默认白字，黄底上不可读；统一按 ON_ACCENT(黑) 走。
    lv_style_set_text_color(&s_main_btn_def, ui_color(UI_C_ON_ACCENT));
    lv_style_set_text_color(&s_main_btn_foc, ui_color(UI_C_ON_ACCENT));
    lv_style_set_bg_color(&s_main_small_def, ui_color(UI_C_DANGER));
    lv_style_set_bg_color(&s_main_small_foc, ui_color(UI_C_DANGER_FOCUS));
    lv_style_set_bg_color(&s_op_btn_def,     ui_color(UI_C_NEUTRAL));
    lv_style_set_bg_color(&s_op_btn_foc,     ui_color(UI_C_PRIMARY));  // 聚焦黄底 (参考图签名式选中态)
    // 列表条目里的 name/desc 标签不自带文字色，会继承按钮的文字色。LVGL 主题给每个按钮
    // 都叠了 bg_color_primary(primary 底 + 白字)，白字在浅色方案的浅灰中性底上几乎看不清。
    // 用 LVGL 主题标题/正文同款 color_text 覆盖 —— 与顶部 "干员列表" 标题完全一致
    // (深色: 亮浅灰 / 浅色: 深灰)，两套方案都可读。聚焦态底是 accent 高亮，保持白字。
    lv_color_t list_text = ui_theme_is_dark() ? lv_palette_lighten(LV_PALETTE_GREY, 5)
                                              : lv_palette_darken(LV_PALETTE_GREY, 4);
    lv_style_set_text_color(&s_op_btn_def, list_text);
    // 聚焦底是 accent 高亮：白底主题里 accent=黑，ON_ACCENT(黑) 会黑底黑字，
    // 统一白字 (各主题 accent 均为中亮色，白字可读)。
    lv_style_set_text_color(&s_op_btn_foc, ui_color(UI_C_ON_ACCENT)); // 黄底黑字

    lv_style_set_bg_color(&s_flag_sd,     ui_color(UI_C_INFO));
    lv_style_set_bg_color(&s_flag_run,    ui_color(UI_C_SUCCESS));
    lv_style_set_bg_color(&s_flag_fg,     ui_color(UI_C_WARNING));
    lv_style_set_bg_color(&s_flag_notrun, ui_color(UI_C_MUTED));
    lv_style_set_bg_color(&s_flag_res,    ui_color(UI_C_PRIMARY));

    lv_style_set_bg_color(&s_fill_primary, ui_color(UI_C_PRIMARY));
    lv_style_set_bg_color(&s_fill_warning, ui_color(UI_C_WARNING));
    lv_style_set_bg_color(&s_fill_danger,  ui_color(UI_C_DANGER));
    lv_style_set_bg_color(&s_fill_success, ui_color(UI_C_SUCCESS));
    lv_style_set_bg_color(&s_fill_neutral, ui_color(UI_C_MUTED));
    // 饱和强调底一律配 on_accent(白)字，保证深/浅方案下按钮文字都可读；
    // 中性底不强制文字色，随 LVGL 主题深浅走 (灰底配灰底该有的字色)。
    lv_style_set_text_color(&s_fill_primary, ui_color(UI_C_ON_ACCENT));
    lv_style_set_text_color(&s_fill_warning, ui_color(UI_C_ON_ACCENT));
    // danger 底为黑/深红，统一白字 (ON_ACCENT 在黄底主题里是黑字)。
    lv_style_set_text_color(&s_fill_danger,  lv_color_white());
    lv_style_set_text_color(&s_fill_success, lv_color_white());

    lv_style_set_arc_color(&s_spinner_arc, ui_color(UI_C_MUTED));
    lv_style_set_text_color(&s_log_text,   ui_color(UI_C_MUTED));

    lv_style_set_outline_color(&s_focus, ui_color(UI_C_ACCENT));
    lv_style_set_bg_color(&s_screen_bg,  ui_color(UI_C_BG));
    // 开关: 轨道中性灰, 开启指示器主色(黄), 旋钮白
    lv_style_set_bg_color(&s_sw_track,    ui_color(UI_C_NEUTRAL));
    lv_style_set_bg_color(&s_sw_track_chk, ui_color(UI_C_NEUTRAL));
    lv_style_set_bg_color(&s_sw_ind_chk,   ui_color(UI_C_PRIMARY));
    lv_style_set_bg_color(&s_sw_knob,      lv_color_white());
    lv_style_set_bg_color(&s_sw_knob_chk,  lv_color_white());
}

// 屏蔽 LVGL 主题默认的按钮焦点外框(黄色)，按钮选中只用悬浮效果
static void btn_disable_theme_focus(lv_obj_t *obj)
{
    lv_obj_set_style_outline_width(obj, 0, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_outline_width(obj, 0, LV_PART_MAIN | LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_outline_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_FOCUS_KEY);
}

void add_style_main_btn(lv_obj_t *obj)
{
    styles_ensure();
    lv_obj_add_style(obj, &s_main_btn_def, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, &s_main_btn_foc, LV_PART_MAIN | LV_STATE_FOCUSED);
    btn_disable_theme_focus(obj);
}

void add_style_main_small_btn(lv_obj_t *obj)
{
    styles_ensure();
    lv_obj_add_style(obj, &s_main_small_def, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, &s_main_small_foc, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_add_style(obj, &s_focus_float, LV_PART_MAIN | LV_STATE_FOCUSED);
    btn_disable_theme_focus(obj);
}

void add_style_op_btn(lv_obj_t *obj)
{
    styles_ensure();
    lv_obj_add_style(obj, &s_op_btn_def, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, &s_op_btn_foc, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_add_style(obj, &s_focus_float, LV_PART_MAIN | LV_STATE_FOCUSED);
    btn_disable_theme_focus(obj);
}

void add_style_op_entry(lv_obj_t *obj)
{
    styles_ensure();
    lv_obj_add_style(obj, &s_op_entry, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void add_style_sd_flag(lv_obj_t *obj)
{
    styles_ensure();
    lv_obj_add_style(obj, &s_flag_sd, LV_PART_MAIN | LV_STATE_DEFAULT);
}
void add_style_res_flag(lv_obj_t *obj)
{
    styles_ensure();
    lv_obj_add_style(obj, &s_flag_res, LV_PART_MAIN | LV_STATE_DEFAULT);
}
void add_style_app_bg_running(lv_obj_t *obj)
{
    styles_ensure();
    lv_obj_add_style(obj, &s_flag_run, LV_PART_MAIN | LV_STATE_DEFAULT);
}
void add_style_app_fg(lv_obj_t *obj)
{
    styles_ensure();
    lv_obj_add_style(obj, &s_flag_fg, LV_PART_MAIN | LV_STATE_DEFAULT);
}
void add_style_app_bg_notrunning(lv_obj_t *obj)
{
    styles_ensure();
    lv_obj_add_style(obj, &s_flag_notrun, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void add_style_fill(lv_obj_t *obj, ui_sem_t sem)
{
    styles_ensure();
    lv_style_t *s = NULL;
    switch (sem) {
        case UI_SEM_PRIMARY: s = &s_fill_primary; break;
        case UI_SEM_WARNING: s = &s_fill_warning; break;
        case UI_SEM_DANGER:  s = &s_fill_danger;  break;
        case UI_SEM_SUCCESS: s = &s_fill_success; break;
        case UI_SEM_NEUTRAL: s = &s_fill_neutral; break;
        case UI_SEM_DEFAULT: default: return; // 走主题默认底色
    }
    if (s) lv_obj_add_style(obj, s, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, &s_focus_float, LV_PART_MAIN | LV_STATE_FOCUSED);
    btn_disable_theme_focus(obj);
}

void add_style_spinner_arc(lv_obj_t *obj)
{
    styles_ensure();
    lv_obj_add_style(obj, &s_spinner_arc, LV_PART_INDICATOR | LV_STATE_DEFAULT);
}

void add_style_log_text(lv_obj_t *obj)
{
    styles_ensure();
    lv_obj_add_style(obj, &s_log_text, LV_PART_MAIN | LV_STATE_DEFAULT);
}

// 焦点外框。进屏 (autofocus) 每次都会调，先 remove 再 add 保证只挂一份不叠加。
void add_style_focus(lv_obj_t *obj)
{
    styles_ensure();
    lv_obj_remove_style(obj, &s_focus, LV_PART_MAIN | LV_STATE_FOCUS_KEY);
    // 按钮用悬浮效果表示选中(见 s_focus_float)，不加外框；
    // 非按钮控件(滑条/开关/下拉/弧形)保留外框确认。
    if (lv_obj_check_type(obj, &lv_button_class)) return;
    lv_obj_add_style(obj, &s_focus, LV_PART_MAIN | LV_STATE_FOCUS_KEY);
}

void add_style_screen_bg(lv_obj_t *obj)
{
    styles_ensure();
    lv_obj_add_style(obj, &s_screen_bg, LV_PART_MAIN | LV_STATE_DEFAULT);
}

// 开关 -> 黄色 pill (参考游戏 UI): 轨道浅灰圆角, 开启指示器黄, 旋钮白带投影
void add_style_switch(lv_obj_t *obj)
{
    styles_ensure();
    lv_obj_add_style(obj, &s_sw_track,    LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, &s_sw_track_chk, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_style(obj, &s_sw_ind_chk,   LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_style(obj, &s_sw_knob,     LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, &s_sw_knob_chk,  LV_PART_KNOB | LV_STATE_CHECKED);
}
