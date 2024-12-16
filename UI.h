#ifndef UI_H
#define UI_H

#include <LCDWIKI_KBV.h>
#include "piece.h"
#include "board.h"
#include "buzzer.h"

extern LCDWIKI_KBV my_lcd;
extern TSPoint getTouchPoint();

// 紀錄當前雙方分數
extern uint8_t redScore = 0;
extern uint8_t blackScore = 0;

#define BOARD_SIZE 8
#define CELL_SIZE 39.5

// 红方先行
extern bool isRedTurn;

// 先手選擇
extern bool redFirst;

extern uint8_t board[BOARD_SIZE][BOARD_SIZE];

extern GameState currentState;
extern bool Pause;

// 觸控壓力範圍
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// check if your button is pressed
bool is_pressed(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t px, int16_t py) {
  return ((px >= x1) && (px <= x2)) && ((py >= y1) && (py <= y2));
}

// UI 繪圖函式
void show_string(uint8_t *str, int16_t x, int16_t y, uint8_t font_size, uint16_t font_color, uint16_t back_color, boolean mode) {
    // if mode=1 then the button only show string without background_color, mode=0 then button will show background_color
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(font_size);
    my_lcd.Set_Text_colour(font_color);
    my_lcd.Set_Text_Back_colour(back_color);
    my_lcd.Print_String(str, x, y);
}

// 在屏幕上顯示上一回合的移動
void display_last_move(uint8_t piece, int startRow, int startCol, int endRow, int endCol) {
    // 清除之前的顯示
    my_lcd.Fill_Rect(0, 440, 250, 40, BLACK);
    
    // 顯示棋子移動信息
    my_lcd.Set_Text_colour(WHITE);
    my_lcd.Set_Text_Size(2);
    
    // 獲取棋子的名稱 (根據piece_code決定)
    const char* piece_name;
    switch(piece) {
        case 1: piece_name = "Red Pawn"; break;
        case 7: piece_name = "Black Pawn"; break;
        case 2: piece_name = "Red Knight"; break;
        case 8: piece_name = "Black Knight"; break;
        case 3: piece_name = "Red Bishop"; break;
        case 9: piece_name = "Black Bishop"; break;
        case 4: piece_name = "Red Rook"; break;
        case 10: piece_name = "Black Rook"; break;
        case 5: piece_name = "Red Queen"; break;
        case 11: piece_name = "Black Queen"; break;
        case 6: piece_name = "Red King"; break;
        case 12: piece_name = "Black King"; break;
        default: piece_name = "Unknown"; break;
    }
    
    // 在屏幕上顯示移動信息
    my_lcd.Print_String(piece_name, 3, 440);  // 顯示棋子的名稱
    my_lcd.Print_String("from ", 2, 460);

    // 根據 redFirst 決定是否翻轉列的字母表示
    char start_col = redFirst ? ('a' + startCol) : ('h' - startCol);  // 列轉換成字母
    int start_row = redFirst ? (8 - startRow) : (startRow + 1);       // 行轉換成西洋棋行
    char end_col = redFirst ? ('a' + endCol) : ('h' - endCol);        // 列轉換成字母
    int end_row = redFirst ? (8 - endRow) : (endRow + 1);             // 行轉換成西洋棋行
    
    char buffer[6];
    sprintf(buffer, "%c%d", start_col, start_row);  // 起始位置
    my_lcd.Print_String(buffer, 60, 460);
    
    my_lcd.Print_String(" to ", 80, 460);
    sprintf(buffer, "%c%d", end_col, end_row);  // 終點位置
    my_lcd.Print_String(buffer, 125, 460);
}

void Draw_Button(uint8_t *str ,int button_x, int button_y, int button_size, uint16_t color) {
    my_lcd.Set_Draw_color(color);
    my_lcd.Draw_Round_Rectangle(button_x + strlen((char*)str) * button_size * 6 / 2 + 5, 
                                button_y + button_size * 8 / 2 + 5, 
                                button_x - strlen((char*)str) * button_size * 6 / 2 - 2, 
                                button_y - button_size * 8 / 2 - 2, 5);
    show_string(str,
                // ((count button string len.) * (button string size) * 6 / 2) in LCDWIKI lib string width = 6*string size
                button_x - strlen((char*)str) * button_size * 6 / 2 + button_size / 2 + 1,
                button_y - button_size * 8 / 2 + button_size / 2 + 1,
                button_size, color, BLACK, 1);
}

int get_piece_value(int piece_code) {
    // Weights are used to calculate the current game situation
    switch (piece_code) {
        case 1: case 7: return 1;  // Pawn
        case 2: case 8: return 3;  // Knight
        case 3: case 9: return 3;  // Bishop
        case 4: case 10: return 5;  // Rook
        case 5: case 11: return 9;  // Queen
        default: return 0;
    }
}

void count_value_display() {
    float gap = redScore - blackScore;
    float percentage = gap / 100;
    my_lcd.Set_Draw_color(WHITE);
    my_lcd.Draw_Rectangle(2, 323, 316, 333);
    my_lcd.Fill_Rect(3, 324, 313, 9, BLACK);
    my_lcd.Fill_Rect(3, 324, 158 + percentage * 340, 9, WHITE);
    //my_lcd.print_Number_Int(blackScore, 305, 326);
    
}

void display_ate_piece(int captured_piece) {
    // record which color piece is been ate
    if (captured_piece > 6) {
        // 如果吃掉的是黑色棋子
        switch (captured_piece) {
            case 7: deadBlackPiece[0]++; break; // 小兵
            case 8: deadBlackPiece[1]++; break; // 騎士
            case 9: deadBlackPiece[2]++; break; // 主教
            case 10: deadBlackPiece[3]++; break; // 城堡
            case 11: deadBlackPiece[4]++; break; // 皇后
        }
    } else if (captured_piece > 0 && captured_piece <= 6) {
        // 如果吃掉的是紅色棋子
        switch (captured_piece) {
            case 1: deadRedPiece[0]++; break; // 小兵
            case 2: deadRedPiece[1]++; break; // 騎士
            case 3: deadRedPiece[2]++; break; // 主教
            case 4: deadRedPiece[3]++; break; // 城堡
            case 5: deadRedPiece[4]++; break; // 皇后
        }
    }

    // 繪製紅色被吃掉的棋子
    // 固定位置設定
    int redPieceX[5] = {2, 42, 82, 122, 162};  // 紅方各類型被吃掉的棋子x坐標
    int yRed = 360;  // 紅方棋子的y坐標
    int yBlack = 395;  // 黑方棋子的y坐標

    my_lcd.Set_Text_colour(WHITE);
    my_lcd.Set_Text_Size(1);
    // 繪製被吃掉的紅棋子
    for (int i = 0; i < 5; i++) {
        if (deadRedPiece[i] > 0) {
            switch (i) {
                case 0: drawIMG(redPieceX[i], yRed, 2, RedPawn); 
                        my_lcd.Fill_Rect(redPieceX[i] + 32, yRed + 16, 8, 10, BLACK);
                        my_lcd.Print_Number_Int(deadRedPiece[i], redPieceX[i] + 32, yRed + 16, 2, " ", 10);
                        break;  // 小兵
                case 1: drawIMG(redPieceX[i], yRed, 2, RedKnight); 
                        my_lcd.Fill_Rect(redPieceX[i] + 32, yRed + 16, 8, 10, BLACK);
                        my_lcd.Print_Number_Int(deadRedPiece[i], redPieceX[i] + 32, yRed + 16, 2, " ", 10);
                        break;  // 騎士
                case 2: drawIMG(redPieceX[i], yRed, 2, RedBishop); 
                        my_lcd.Fill_Rect(redPieceX[i] + 32, yRed + 16, 8, 10, BLACK);
                        my_lcd.Print_Number_Int(deadRedPiece[i], redPieceX[i] + 32, yRed + 16, 2, " ", 10);
                        break;  // 主教
                case 3: drawIMG(redPieceX[i], yRed, 2, RedRook); 
                        my_lcd.Fill_Rect(redPieceX[i] + 32, yRed + 16, 8, 10, BLACK);
                        my_lcd.Print_Number_Int(deadRedPiece[i], redPieceX[i] + 32, yRed + 16, 2, " ", 10);
                        break;  // 城堡
                case 4: drawIMG(redPieceX[i], yRed, 2, RedQueen); 
                        my_lcd.Fill_Rect(redPieceX[i] + 32, yRed + 16, 8, 10, BLACK);
                        my_lcd.Print_Number_Int(deadRedPiece[i], redPieceX[i] + 32, yRed + 16, 2, " ", 10);
                        break;  // 皇后
            }
        }
    }

    // 繪製被吃掉的黑棋子
    for (int i = 0; i < 5; i++) {
        if (deadBlackPiece[i] > 0) {
            switch (i) {
                case 0: drawIMG(redPieceX[i], yBlack, 2, BlackPawn); 
                        my_lcd.Fill_Rect(redPieceX[i] + 32, yBlack + 16, 8, 10, BLACK);
                        my_lcd.Print_Number_Int(deadBlackPiece[i], redPieceX[i] + 32, yBlack + 16, 2, " ", 10);
                        break;  // 小兵
                case 1: drawIMG(redPieceX[i], yBlack, 2, BlackKnight); 
                        my_lcd.Fill_Rect(redPieceX[i] + 32, yBlack + 16, 8, 10, BLACK);
                        my_lcd.Print_Number_Int(deadBlackPiece[i], redPieceX[i] + 32, yBlack + 16, 2, " ", 10);
                        break;  // 騎士
                case 2: drawIMG(redPieceX[i], yBlack, 2, BlackBishop); 
                        my_lcd.Fill_Rect(redPieceX[i] + 32, yBlack + 16, 8, 10, BLACK);
                        my_lcd.Print_Number_Int(deadBlackPiece[i], redPieceX[i] + 32, yBlack + 16, 2, " ", 10);
                        break;  // 主教
                case 3: drawIMG(redPieceX[i], yBlack, 2, BlackRook); 
                        my_lcd.Fill_Rect(redPieceX[i] + 32, yBlack + 16, 8, 10, BLACK);
                        my_lcd.Print_Number_Int(deadBlackPiece[i], redPieceX[i] + 32, yBlack + 16, 2, " ", 10);
                        break;  // 城堡
                case 4: drawIMG(redPieceX[i], yBlack, 2, BlackQueen); 
                        my_lcd.Fill_Rect(redPieceX[i] + 32, yBlack + 16, 8, 10, BLACK);
                        my_lcd.Print_Number_Int(deadBlackPiece[i], redPieceX[i] + 32, yBlack + 16, 2, " ", 10);
                        break;  // 皇后
            }
        }
    }
}
    

void promote_pawn(int dest_row, int dest_col, uint8_t new_piece_code) {
    if (isRedTurn) {
        board[dest_row][dest_col] = new_piece_code;  // 替換為紅方的新棋子
    } else {
        board[dest_row][dest_col] = new_piece_code + 6;  // 替換為黑方的新棋子
    }
    // 繪製新的棋子
    put_piece(dest_col * CELL_SIZE + 5.5, dest_row * CELL_SIZE + 6, board[dest_row][dest_col]);
}

// 小兵生變時彈出選擇視窗
void show_promotion_window(int dest_row, int dest_col) {

    // 清除一個區域，準備顯示選項
    my_lcd.Fill_Rect(49, 88, 220, 140, BLACK); // 定義選單大小
    my_lcd.Fill_Rect(48, 88, 220, 2, MAGENTA);
    my_lcd.Fill_Rect(48, 88, 2, 140, MAGENTA);
    my_lcd.Fill_Rect(48, 228, 221, 2, MAGENTA);
    my_lcd.Fill_Rect(267, 88, 2, 140, MAGENTA);
    
    // 顯示選擇棋子圖示
    drawIMG(65, 99, 3, isRedTurn ? RedPawn : BlackPawn);    // 小兵
    drawIMG(135, 100, 3, isRedTurn ? RedKnight : BlackKnight); // 騎士
    drawIMG(205, 100, 3, isRedTurn ? RedBishop : BlackBishop); // 主教
    drawIMG(100, 165, 3, isRedTurn ? RedRook : BlackRook);   // 城堡
    drawIMG(170, 166, 3, isRedTurn ? RedQueen : BlackQueen);  // 皇后

    my_lcd.Set_Draw_color(CYAN);
    my_lcd.Draw_Rectangle(62, 100, 114, 152);
    my_lcd.Draw_Rectangle(132, 100, 184, 152);
    my_lcd.Draw_Rectangle(202, 100, 254, 152);
    my_lcd.Draw_Rectangle(97, 165, 149, 217);
    my_lcd.Draw_Rectangle(167, 165, 219, 217);

    // 顯示選項文字
    my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
    my_lcd.Set_Text_colour(WHITE);
    my_lcd.Set_Text_Size(2);
    my_lcd.Print_String("Pawn Promotion!!", 5, 340);

    playUpgradeBGM();

    // 等待玩家點擊
    while (true) {
        TSPoint p = getTouchPoint();
        if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
            int x = map(p.x, 112, 930, 0, my_lcd.Get_Display_Width());
            int y = map(p.y, 90, 960, my_lcd.Get_Display_Height(), 0);
            
            if (is_pressed(265, 433, 318, 478, x, y)) {
                my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
                my_lcd.Set_Text_colour(WHITE);
                my_lcd.Set_Text_Size(2);
                my_lcd.Print_String("Plese Select a Piece!!", 5, 340);
            }

            // 檢查玩家點擊了哪個升變選項
            if (is_pressed(62, 100, 114, 152, x, y)) {
                promote_pawn(dest_row, dest_col, 1);  // 升變為小兵 (根據需要選擇升變的編碼)
                break;
            } else if (is_pressed(132, 100, 184, 152, x, y)) {
                promote_pawn(dest_row, dest_col, 2);  // 升變為騎士
                if (isRedTurn) redScore+=3; else blackScore+=3;
                break;
            } else if (is_pressed(202, 100, 254, 152, x, y)) {
                promote_pawn(dest_row, dest_col, 3);  // 升變為主教
                if (isRedTurn) redScore+=3; else blackScore+=3;
                break;
            } else if (is_pressed(97, 165, 149, 217, x, y)) {
                promote_pawn(dest_row, dest_col, 4);  // 升變為城堡
                if (isRedTurn) redScore+=5; else blackScore+=5;
                break;
            } else if (is_pressed(167, 165, 219, 217, x, y)) {
                promote_pawn(dest_row, dest_col, 5);  // 升變為皇后
                if (isRedTurn) redScore+=9; else blackScore+=9;
                break;
            }
        }
    }

    // 清除彈窗
    my_lcd.Fill_Rect(0, 0, 320, 320, BLACK);
    drawBoard();
    draw_all_piece();
    count_value_display();
}

void checkReplayButton(int px, int py) {
    if (is_pressed(73, 174, 192, 208, px, py)) {
        my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
        my_lcd.Set_Text_colour(WHITE);
        my_lcd.Set_Text_Size(2);
        my_lcd.Print_String("Replay!!", 5, 340);

        // 清除一個區域，準備顯示選項
        my_lcd.Fill_Rect(49, 88, 220, 140, BLACK); // 定義選單大小
        my_lcd.Fill_Rect(48, 88, 220, 2, MAGENTA);
        my_lcd.Fill_Rect(48, 88, 2, 140, MAGENTA);
        my_lcd.Fill_Rect(48, 228, 221, 2, MAGENTA);
        my_lcd.Fill_Rect(267, 88, 2, 140, MAGENTA);
        my_lcd.Set_Text_colour(CYAN);
        my_lcd.Print_String("WHICH COLOR YOU", 70, 100);
        my_lcd.Print_String("WANT TO CHOOSE?", 70, 125);

        // 顯示玩家要哪一方的兩個按鈕
        Draw_Button("BLACK", 118, 164, 3, WHITE);
        Draw_Button("RED", 100, 203, 3, WHITE);
        drawIMG(225, 185, 2, Return);
        my_lcd.Set_Draw_color(CYAN);
        my_lcd.Draw_Round_Rectangle(220, 180, 262, 222, 8);

        // 等待玩家點擊
        while (true) {
            TSPoint p = getTouchPoint();
            if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
                int x = map(p.x, 112, 930, 0, my_lcd.Get_Display_Width());
                int y = map(p.y, 90, 960, my_lcd.Get_Display_Height(), 0);
            
                if (is_pressed(265, 433, 318, 478, x, y)) {
                    my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
                    my_lcd.Set_Text_colour(WHITE);
                    my_lcd.Set_Text_Size(2);
                    my_lcd.Print_String("Pause, You can't move!!", 5, 340);
                }

                // 檢查玩家點擊了哪個顏色棋子來對局
                if (is_pressed(70, 145, 165, 175, x, y)) {
                    // 選擇當黑棋
                    redFirst = false;
                    // 清除彈窗
                    GameStart = true;
                    currentState = PLAYING;
                    break;
                } else if (is_pressed(70, 184, 130, 214, x, y)) {
                    // 選擇當紅棋
                    redFirst = true;
                    // 清除彈窗
                    GameStart = true;
                    currentState = PLAYING;
                    break;
                } else if (is_pressed(220, 180, 262, 222, x, y)) {
                    my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
                    my_lcd.Set_Text_colour(WHITE);
                    my_lcd.Set_Text_Size(2);
                    my_lcd.Print_String("Game Paused!!", 5, 340);
                    Pause = true;
                    break;
                }
            }
        }
    }
};  

void checkPauseButton(int px, int py) {
    if (is_pressed(255, 381, 318, 404, px, py)) {
        my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
        my_lcd.Set_Text_colour(WHITE);
        my_lcd.Set_Text_Size(2);
        my_lcd.Print_String("Game Paused!!", 5, 340);

        // 清除之前的選擇格子
        clear_previous_selection(1);
        // Release the piece
        selectRow = -1; selectCol = -1;
        chooseRow = -1; chooseCol = -1;
        prevSelectRow = -1, prevSelectCol = -1, prevChooseRow = -1, prevChooseCol = -1;
        piece_code = 0;

        Pause = true;
        currentState = PAUSED;
    }
};  

void checkMainMenuButton(int px, int py) {
    if (is_pressed(73, 114, 247, 148, px, py)) {
        MENU = true;
        currentState = MAIN_MENU;
    }
}; 

void checkResumeButton(int px, int py) {
    if (is_pressed(220, 180, 262, 222, px, py)) {
        // 清除彈窗
        my_lcd.Fill_Rect(0, 0, 320, 320, BLACK);
        drawBoard();
        draw_all_piece();
        my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
        currentState = PLAYING;
    }
}

#endif