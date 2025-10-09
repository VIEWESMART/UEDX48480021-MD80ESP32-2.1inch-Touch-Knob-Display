SET(SOURCES screens/ui_Screen1.c
    screens/ui_time.c
    screens/ui_working.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_1kaorou_png.c
    images/ui_img_2kaoji_png.c
    images/ui_img_3danta_png.c
    images/ui_img_4pisa_png.c
    images/ui_img_5liupai_png.c
    images/ui_img_6shutiao_png.c
    images/ui_img_black_png.c
    images/ui_img_start_png.c
    images/ui_img_1522004715.c
    images/ui_img_362584155.c
    images/ui_img_174066631.c
    images/ui_img_min_png.c
    images/ui_img_1620963198.c)

add_library(ui ${SOURCES})
