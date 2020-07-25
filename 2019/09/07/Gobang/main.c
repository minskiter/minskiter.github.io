#include <gtk/gtk.h>
#include "GoBang.h"

/* 棋子信息结构 */
struct chess_info{
    short x,y;
    GtkWidget * image;
};

/* 全局变量方便回调函数修改 */
struct chess_info chess_praph[16][16];

/* 状态栏 */
GtkWidget * status_label;

/* 判断是否可以开始游戏 */
gboolean is_start_game=TRUE;

#define CHESS_INFO(object) ((struct chess_info *)object)

struct gb_board game_gb;

static GtkWidget * get_chess(const gchar * filename){
    GtkWidget * image = gtk_image_new_from_file(filename);
    return image;
}

/* 改变棋子在图形界面的图像 */
static gboolean change_chess(short x,short y,int type){\
    GtkImage * image;
    image=GTK_IMAGE(chess_praph[x-1][y-1].image);
    g_return_val_if_fail(image,FALSE);
    switch (type){
        case 1:
            g_object_set(image,"file","./image/五子棋黑棋.png",NULL);
            break;
        case 0:
            g_object_set(image,"file","./image/五子棋白棋.png",NULL);
            break;
        default:
            g_object_set(image,"file","./image/五子棋空格.png",NULL);
            break;
    }
    return TRUE;
}
/*  初始化 */
static void chess_init(struct gb_board * cur_board){
    gb_init_board(cur_board,15,15);
    int i,j;
    for (i=1;i<=15;++i){
        for (j=1;j<=15;++j){
            change_chess(i,j,-1);
        }
    }
}

/* 显示获胜方 */
static gboolean winner_show(int flag){
    if (flag==-1) return FALSE;
    switch(flag){
        case -2:
            gtk_label_set_text(GTK_LABEL(status_label),"平局");
        break;
        case 1:
            gtk_label_set_text(GTK_LABEL(status_label),"白棋胜");
        break;
        case 0:
            gtk_label_set_text(GTK_LABEL(status_label),"黑棋胜");
        break;
    }
    is_start_game=FALSE;
    return TRUE;
}

/* 状态标签回调函数 */
static gboolean status_show(int flag){
    switch(flag) {
        case 0:
            gtk_label_set_text(GTK_LABEL(status_label),"当前轮到白方下棋，电脑正在思考中...");
        case 1:
            gtk_label_set_text(GTK_LABEL(status_label),"当前轮到黑方下棋");
    }
    return TRUE;
}

/* 菜单重新开始回调函数 */
static gboolean chess_restart(GtkWidget *widget,gpointer data){
    gtk_label_set_text(GTK_LABEL(status_label),"游戏开始");
    chess_init(&game_gb);
    is_start_game=TRUE;
    return TRUE;
}

/* 悔棋回调函数 */
static gboolean chess_back(GtkWidget *widget,gpointer data){
    if (!is_start_game || game_gb.gb_turn<1) return TRUE;
    change_chess(game_gb.gb_chess_manual[game_gb.gb_turn].x,game_gb.gb_chess_manual[game_gb.gb_turn].y,-1);
    gb_back_turn(&game_gb);
    change_chess(game_gb.gb_chess_manual[game_gb.gb_turn].x,game_gb.gb_chess_manual[game_gb.gb_turn].y,-1);
    gb_back_turn(&game_gb);
    return TRUE;
}

/* 响应鼠标点击 */
static gboolean chess_click(GtkWidget *widget,GdkEventButton *event,gpointer data){
    if (!is_start_game) return TRUE;
    struct gb_coord chess_pos;
    chess_pos.x=CHESS_INFO(data)->x,chess_pos.y=CHESS_INFO(data)->y;
    if (gb_place_judge(&game_gb,&chess_pos)){
        change_chess(chess_pos.x,chess_pos.y,game_gb.gb_cur_is);
        gb_nxt_turn(&game_gb,&chess_pos);
        if (winner_show(gb_win_judge(&game_gb))) return TRUE;
        status_show(game_gb.gb_cur_is);
        gb_auto_nxt_turn(&game_gb,&chess_pos,3);
        change_chess(chess_pos.x,chess_pos.y,game_gb.gb_cur_is);
        gb_nxt_turn(&game_gb,&chess_pos);
        if (winner_show(gb_win_judge(&game_gb))) return TRUE;
        status_show(game_gb.gb_cur_is);
    }
    return TRUE;
}

/* 应用窗口 */
static void activate(GtkApplication *app, gpointer user_data) {
    g_print("看到此条信息证明控制台未关闭");
    GtkWidget *window;
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW (window), "五子棋");
    gtk_widget_set_size_request(GTK_WIDGET(window),400,450);
    gtk_window_set_resizable(GTK_WINDOW(window),gtk_false());

    GtkWidget * chess_board;
    chess_board = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(chess_board),4);
    gtk_grid_set_row_spacing(GTK_GRID(chess_board),4);

    GtkWidget * fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window),fixed);
    /* 初始化五子棋界面 */
    GtkWidget * image_chess_board = gtk_image_new_from_file("./image/五子棋棋盘.png");
    gint i,j;
    for (i=0;i<15;++i){
        for (j=0;j<15;++j){
            GtkWidget * event_box=gtk_event_box_new();
            chess_praph[i][j].y=j+1;
            chess_praph[i][j].x=i+1;
            chess_praph[i][j].image=get_chess("./image/五子棋空格.png");
            gtk_container_add(GTK_CONTAINER(event_box),chess_praph[i][j].image);
            gtk_grid_attach(GTK_GRID(chess_board),event_box,i,j,1,1);
            g_signal_connect(G_OBJECT(event_box),"button_press_event",G_CALLBACK(chess_click),&chess_praph[i][j]);
        }
    }
    /* 初始化 */
    chess_init(&game_gb);
    status_label = gtk_label_new("游戏开始");

    gtk_fixed_put(GTK_FIXED(fixed),image_chess_board,0,25);
    gtk_fixed_put(GTK_FIXED(fixed),chess_board,0,25);
    gtk_fixed_put(GTK_FIXED(fixed),status_label,0,430);

    /* 创建一个水平的box */
    GtkWidget * menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_fixed_put(GTK_FIXED(fixed),menu_box,0,0);

    GtkWidget * menu_bar,* menu_restart,* menu_back;
    menu_bar=gtk_menu_bar_new();
    gtk_widget_set_hexpand (menu_bar, TRUE);
    gtk_box_pack_start (GTK_BOX (menu_box),menu_bar, FALSE, TRUE, 0);

    menu_restart=gtk_menu_item_new_with_label("重新开始");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), menu_restart);

    menu_back = gtk_menu_item_new_with_label("悔棋");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), menu_back);


    /* 信号连接菜单 */
    g_signal_connect(GTK_MENU_ITEM(menu_restart),"activate",G_CALLBACK(chess_restart),NULL);
    g_signal_connect(GTK_MENU_ITEM(menu_back),"activate",G_CALLBACK(chess_back),NULL);


    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("GoBang.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION (app), argc, argv);
    g_object_unref(app);

    return status;
}