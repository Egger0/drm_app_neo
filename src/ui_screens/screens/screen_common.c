#include "screen_common.h"
#include "screen_manager.h"
#include "styles.h"
#include "ui/ui_theme.h"
#include "ui_metrics.h"
#ifndef LOGO_PRTS_PATH
#include "utils/respath.h"
#endif

lv_obj_t *ui_screen_root_bare(void)
{
    lv_obj_t *root = lv_obj_create(NULL);
    lv_obj_set_size(root, S(UI_BASE_WIDTH), S(UI_BASE_HEIGHT));
    lv_obj_set_style_pad_all(root, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    add_style_screen_bg(root);  // 方案背景底 (confirm/warning 之后自己盖 fill)
    return root;
}

lv_obj_t *ui_screen_root(void)
{
    lv_obj_t *root = ui_screen_root_bare();
    lv_obj_add_event_cb(root, ui_autofocus_cb, LV_EVENT_ALL, NULL);
    return root;
}

void ui_header(lv_obj_t *root, const char *title)
{
    // 页头亮黄横带(#d8c839)：logo/标题统一放在横带上
    lv_obj_t *bar = lv_obj_create(root);
    lv_obj_remove_style_all(bar);
    lv_obj_set_pos(bar, 0, 0);
    lv_obj_set_size(bar, S(360), S(40));
    lv_obj_set_style_bg_color(bar, ui_color(UI_C_GRAD_START), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 页头亮黄向下渐变到屏幕底部(黄 #d8c839 -> 白)，缓解白底单调
    lv_obj_t *fade = lv_obj_create(root);
    lv_obj_remove_style_all(fade);
    lv_obj_set_pos(fade, 0, S(40));
    lv_obj_set_size(fade, S(360), S(600)); // 渐变直通屏幕底部(640-40)
    lv_obj_set_style_bg_color(fade, ui_color(UI_C_GRAD_START), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(fade, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(fade, ui_color(UI_C_GRAD_END), LV_PART_MAIN | LV_STATE_DEFAULT);  // 黄->浅灰渐变终点
    lv_obj_set_style_bg_grad_dir(fade, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    // 渐变在对象前 60% 完成(600px 中约 360px 从黄到深灰)，之后保持深灰
    lv_obj_set_style_bg_grad_stop(fade, 153, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(fade, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(fade, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(fade, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 背景等高线装饰: 白色稀疏曲线, 落在黄->白渐变区, 随渐变隐入白底 (全部页面共用)
    lv_obj_t *contour = lv_image_create(root);
    lv_obj_set_pos(contour, 0, 0);
    lv_obj_set_size(contour, S(360), S(640));
    lv_obj_set_style_opa(contour, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
#ifdef BG_CONTOUR_PATH
    lv_image_set_src(contour, BG_CONTOUR_PATH);
#else
    lv_image_set_src(contour, respath_lvfs(BG_CONTOUR_FILE));
#endif

    lv_obj_t *logo = lv_image_create(root);
    // logo 放右上角，避开系统层电池图标(电池从 x=330 起)：294 时右缘 330 重叠，左移到 286 留 8px
    lv_obj_set_pos(logo, S(286), S(3));
#ifdef LOGO_PRTS_PATH
    lv_image_set_src(logo, LOGO_PRTS_PATH);
#else
    lv_image_set_src(logo, respath_lvfs(LOGO_PRTS_FILE));
#endif
    lv_image_set_pivot(logo, 0, 0);
    // 源图 64x64：160=LVGL 62.5% 显示 40px，略大；144=56.25% (36px)
    lv_image_set_scale(logo, 144 * UI_SCALE);

    lv_obj_t *t = lv_label_create(root);
    lv_obj_set_pos(t, S(25), S(8));
    add_style_label_large(t);
    lv_label_set_text(t, title);
}

lv_obj_t *ui_text_button(lv_obj_t *root, int x, int y, int w, int h,
                         ui_sem_t sem, const char *text, lv_event_cb_t cb)
{
    lv_obj_t *o = lv_button_create(root);
    lv_obj_set_pos(o, S(x), S(y));
    lv_obj_set_size(o, S(w), S(h));
    add_style_fill(o, sem);
    if (cb) lv_obj_add_event_cb(o, cb, LV_EVENT_PRESSED, NULL);

    lv_obj_t *lbl = lv_label_create(o);
    add_style_label_large(lbl);
    lv_obj_set_style_align(lbl, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(lbl, text);
    return o;
}

lv_obj_t *ui_small_text_button(lv_obj_t *root, int x, int y, int w, int h,
    ui_sem_t sem, const char *text, lv_event_cb_t cb)
{
    lv_obj_t *o = lv_button_create(root);
    lv_obj_set_pos(o, S(x), S(y));
    lv_obj_set_size(o, S(w), S(h));
    add_style_fill(o, sem);
    if (cb) lv_obj_add_event_cb(o, cb, LV_EVENT_PRESSED, NULL);

    lv_obj_t *lbl = lv_label_create(o);
    add_style_label_small(lbl);
    lv_obj_set_style_align(lbl, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(lbl, text);
    return o;
}

static void add_focusables(lv_obj_t *parent, lv_group_t *g)
{
    uint32_t n = lv_obj_get_child_count(parent);
    for (uint32_t i = 0; i < n; i++) {
        lv_obj_t *c = lv_obj_get_child(parent, i);
        if (lv_obj_check_type(c, &lv_button_class) ||
            lv_obj_check_type(c, &lv_dropdown_class) ||
            lv_obj_check_type(c, &lv_switch_class) ||
            lv_obj_check_type(c, &lv_slider_class) ||
            lv_obj_check_type(c, &lv_roller_class)) {
            lv_group_add_obj(g, c);
            add_style_focus(c);
        }
        add_focusables(c, g);
    }
}

void ui_autofocus_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_SCREEN_LOAD_START) return;
    lv_group_t *g = screens_group();
    if (!g) return;
    lv_group_remove_all_objs(g);
    // 复位导航模式：group 是全屏共享的，若上一屏 (如文件管理器) 把它设成 editing,
    // 编码器会把 1/2(LEFT/RIGHT) 当成"编辑当前控件"而非按钮间导航，导致本屏翻页失效。
    // 通用屏 (主菜单/设置等) 均以导航态进入，需要编辑时按 3(ENTER) 进入。
    lv_group_set_editing(g, false);
    add_focusables(lv_event_get_target(e), g);
}
