#include <LCDWIKI_GUI.h>  // Core graphics library
#include <LCDWIKI_KBV.h>  // Hardware-specific library
#include <TouchScreen.h>  // Touch library
#include "stdlib.h"

enum GameState {
    MAIN_MENU,
    PLAYING,
    COMPUTER_GAME,
    PAUSED,
    GAME_OVER
};

GameState currentState = MAIN_MENU;  // 起始於主選單

// 遊戲開始畫面設置只需要跑一次
bool GameStart = false;
bool MENU = false;
bool Pause = false;
bool GameOver = false;

// 紀錄當前哪些棋子被吃
extern uint8_t deadRedPiece[5] = {0, 0, 0, 0, 0};
extern uint8_t deadBlackPiece[5] = {0, 0, 0, 0, 0};

#include "piece.h"
#include "UI.h"
#include "board.h"
#include "computer_player.h"
#include "buzzer.h"
#include "LED.h"

// TFT显示屏与触摸屏设置
LCDWIKI_KBV my_lcd(ILI9486, A3, A2, A1, A0, A4); // 屏幕控制
TouchScreen ts = TouchScreen(8, A3, A2, 9, 300);  // 触摸屏控制

// 保存棋子要下的位置與當前位置
uint8_t selectRow = -1, selectCol = -1, chooseRow = -1, chooseCol = -1;
// 選擇落子位置時所需暫存
uint8_t prevSelectRow = -1, prevSelectCol = -1, prevChooseRow = -1, prevChooseCol = -1;

// 紀錄選擇之棋子
uint8_t piece_code = 0;

// 紀錄國王位置 檢查有無被將軍
uint8_t redKingRow = -1, redKingCol = -1;
uint8_t blackKingRow = -1, blackKingCol = -1;

// 红方先行
bool isRedTurn = true;

// 先手選擇
bool redFirst = true;

#define MAX_METEORS 8 // 最大流星數量
#define METEOR_LENGTH 10 // 流星的長度
#define METEOR_SPEED 3   // 流星速度
#define SCREEN_WIDTH 320  // 屏幕寬度
#define SCREEN_HEIGHT 480 // 屏幕高度

// 結構體保存每個流星的狀態
struct Meteor {
    int x;
    int y;
    int dx;
    int dy;
    bool active;
};

Meteor meteors[MAX_METEORS];  // 儲存多個流星

unsigned long previousMillis = 0;  // 儲存上次重繪的時間
const unsigned long interval = 10000;  // 10 秒的間隔

uint8_t board[BOARD_SIZE][BOARD_SIZE] = {
  {10,8,9,11,12,9,8,10},
  {7,7,7,7,7,7,7,7},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {1,1,1,1,1,1,1,1},
  {4,2,3,5,6,3,2,4},
};

// 觸控壓力範圍
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// 觸控位置座標
int16_t px, py;

// 獲取觸控
TSPoint getTouchPoint() {
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);
    pinMode(A3, OUTPUT);  // 恢復輸出模式
    pinMode(A2, OUTPUT);  // 恢復輸出模式
    return p;
}

// 判斷是否觸控了棋盤某個格子
void checkTouch() {
    TSPoint p = getTouchPoint();
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
        // 映射觸控點到螢幕座標
        px = map(p.x, 112, 930, 0, my_lcd.Get_Display_Width());
        py = map(p.y, 90, 960, my_lcd.Get_Display_Height(), 0);

        // 檢測有無在棋盤範圍內
        if (px >= 0 && px < BOARD_SIZE * CELL_SIZE && py >= 0 && py < BOARD_SIZE * CELL_SIZE) {
            int row = py / CELL_SIZE;
            int col = px / CELL_SIZE;
            //Serial.print("row:");Serial.print(row);Serial.print("col:");Serial.print(col);Serial.print("\r\n");
            //Serial.print(board[row][col]);Serial.print("\r\n");
            
            // 如果之前沒選擇棋子
            if (piece_code == 0) {
                if (board[row][col] != 0){
                    my_lcd.Set_Text_Back_colour(BLACK);

                    if (isRedTurn && board[row][col] > 6) {
                        my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
                        my_lcd.Set_Text_colour(WHITE);
                        my_lcd.Set_Text_Size(2);
                        my_lcd.Print_String("RED turn now!!\n", 5, 340);
                        return; // RED can't pick BLACK
                    }
                    if (!isRedTurn && board[row][col] <= 6) {
                        my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
                        my_lcd.Set_Text_colour(WHITE);
                        my_lcd.Set_Text_Size(2);
                        my_lcd.Print_String("BLACK turn now!!\n", 5, 340);
                        return; // BLACK can't pick RED
                    }

                    selectRow = row; selectCol = col;
                    piece_code = board[row][col];
                    my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
                    my_lcd.Set_Text_colour(WHITE);
                    my_lcd.Set_Text_Size(2);
                    my_lcd.Print_String("pick up!!", 5, 340);

                    // 繪製藍色框框
                    my_lcd.Set_Draw_color(BLUE);
                    my_lcd.Draw_Rectangle((selectCol * CELL_SIZE) + 3, (selectRow * CELL_SIZE) + 3, (selectCol * CELL_SIZE) + 37, (selectRow * CELL_SIZE) + 37);
                    // 保存當前選擇之棋子
                    prevSelectRow = selectRow;
                    prevSelectCol = selectCol;
                    //Serial.print(piece_code);
                    delay(200);
                }
            // check if pick up and put down same color piece
            } else if ((piece_code == 6 && board[row][col] == 4) || (piece_code == 12 && board[row][col] == 10)) {
                // 清除之前的紅色框框
                clear_previous_selection(2);
                // 玩家選擇城堡
                chooseRow = row;
                chooseCol = col;

                // 繪製紅色框框
                my_lcd.Set_Draw_color(RED);
                my_lcd.Draw_Rectangle((col * CELL_SIZE) + 3, (row * CELL_SIZE) + 3, (col * CELL_SIZE) + 37, (row * CELL_SIZE) + 37);

                // 保存當前選擇
                prevChooseRow = chooseRow;
                prevChooseCol = chooseCol;

                delay(200);

            }else if (!(((piece_code <= 6) && (board[row][col] <= 6) && (piece_code > 0 && board[row][col] > 0)) || (piece_code > 6) && (board[row][col] > 6))) {
                // 清除之前的紅色框框
                clear_previous_selection(2);
                
                // 玩家選擇移動目標位置
                chooseRow = row;
                chooseCol = col;

                // 繪製紅色框框
                my_lcd.Set_Draw_color(RED);
                my_lcd.Draw_Rectangle((col * CELL_SIZE) + 3, (row * CELL_SIZE) + 3, (col * CELL_SIZE) + 37, (row * CELL_SIZE) + 37);

                // 保存當前選擇
                prevChooseRow = chooseRow;
                prevChooseCol = chooseCol;

                delay(200);
                
            } else if (selectRow == row && selectCol == col) {
                // 清除之前的藍色框框
                clear_previous_selection(1);

                my_lcd.Set_Text_colour(WHITE);
                my_lcd.Set_Text_Back_colour(BLACK);
                my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
                my_lcd.Set_Text_colour(WHITE);
                my_lcd.Set_Text_Size(2);
                my_lcd.Print_String("Put down this piece.", 5, 340);
                selectRow = -1; selectCol = -1;
                prevChooseRow = -1; prevChooseCol = -1;
                piece_code = 0;
                delay(200);
            } else if (selectRow != row || selectCol != col) {
                if (board[row][col] != 0){
                    my_lcd.Set_Text_Back_colour(BLACK);
                    // 清除之前的藍色框框
                    clear_previous_selection(1);

                    if (isRedTurn && board[row][col] > 6) {
                        my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
                        my_lcd.Set_Text_colour(WHITE);
                        my_lcd.Set_Text_Size(2);
                        my_lcd.Print_String("RED turn now!!\n", 5, 340);
                        return; // RED can't pick BLACK
                    }
                    if (!isRedTurn && board[row][col] <= 6) {
                        my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
                        my_lcd.Set_Text_colour(WHITE);
                        my_lcd.Set_Text_Size(2);
                        my_lcd.Print_String("BLACK turn now!!\n", 5, 340);
                        return; // BLACK can't pick RED
                    }

                    selectRow = row; selectCol = col;
                    piece_code = board[row][col];
                    my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
                    my_lcd.Set_Text_colour(WHITE);
                    my_lcd.Set_Text_Size(2);
                    my_lcd.Print_String("Pick up this piece!!", 5, 340);

                    // 繪製藍色框框
                    my_lcd.Set_Draw_color(BLUE);
                    my_lcd.Draw_Rectangle((selectCol * CELL_SIZE) + 3, (selectRow * CELL_SIZE) + 3, (selectCol * CELL_SIZE) + 37, (selectRow * CELL_SIZE) + 37);
                    // 保存當前選擇之棋子
                    prevSelectRow = selectRow;
                    prevSelectCol = selectCol;
                    //Serial.print(piece_code);
                    delay(200);
                }
            }
                
        } else {
            
            // 檢查是否按下按鈕
            checkGoButton(px, py); 
            checkPauseButton(px, py);
            
        }
        
    } else {
        // 棋盘外触控，可以忽略或提供提示

    }
}

void checkGoButton(int px, int py) {
    if (is_pressed(265, 433, 318, 478, px, py)) {
        if ((piece_code == 6 && board[chooseRow][chooseCol] == 4) || (piece_code == 12 && board[chooseRow][chooseCol] == 10)) {
            if (can_castle(piece_code < 7, chooseCol == 0)) {  // 調用易位檢測函數 (確定沒問題 更新全域bool
                perform_castle(isRedTurn, chooseCol == 0);  // 執行易位
                // 清除之前的選擇格子
                clear_previous_selection(1);
                my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
                my_lcd.Set_Text_colour(WHITE);
                my_lcd.Set_Text_Size(2);
                my_lcd.Print_String("Castling!", 5, 340);
                // 切換回合
                playThinkingBGM();
                isRedTurn = !isRedTurn;
                delay(200);
                return;  // 易位完成后直接返回
            } else {
                // 清除之前的選擇格子
                clear_previous_selection(1);
                my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
                my_lcd.Set_Text_colour(WHITE);
                my_lcd.Set_Text_Size(2);
                my_lcd.Print_String("Can't Castle!", 5, 340);
                selectRow = -1; selectCol = -1;
                piece_code = 0;
                delay(200);
                return;  // 易位完成后直接返回
            }
        }

        // 確認移動
        if (is_valid_move(selectRow, selectCol, chooseRow, chooseCol, piece_code) && piece_code != 0) {

            if (is_legal_move(selectRow, selectCol, chooseRow, chooseCol, isRedTurn)) {
                bool eatPawn = false;
                // 檢查有無吃子的狀況
                if (board[chooseRow][chooseCol] != 0) {
                    int captured_piece = board[chooseRow][chooseCol];
                    int piece_value = get_piece_value(captured_piece);
                    if (captured_piece > 6) {
                        redScore += piece_value;  // RED eat BLACK
                    } else {
                        blackScore += piece_value;  // BLACK eat RED
                    }
                    display_ate_piece(captured_piece);
                    eatPawn = true;
                    eatPawnBGM();
                    count_value_display();
                }

                // 看國王&城堡有無移動過
                updateMoveStatus(piece_code, selectCol, isRedTurn);

                // 更新國王位置
                update_king_position(piece_code, chooseRow, chooseCol);

                board[selectRow][selectCol] = 0;
                board[chooseRow][chooseCol] = piece_code;

                // 顯示上一回合移動
                display_last_move(piece_code, selectRow, selectCol, chooseRow, chooseCol);
            
                // 清除舊位置
                my_lcd.Fill_Rect(selectCol * CELL_SIZE + 5.5, selectRow * CELL_SIZE + 6, 32, 32, BLACK);

                // 在新位置繪製棋子
                clear_previous_selection(1);
                put_piece(chooseCol * CELL_SIZE + 5.5, chooseRow * CELL_SIZE + 6, piece_code);

                // 小兵升變特殊規則檢測
                if (board[chooseRow][chooseCol] == 1 || board[chooseRow][chooseCol] == 7) {
                    // 假設 can_pawn_move 檢測後的小兵成功走到底
                    if (chooseRow == 0 || chooseRow == 7) {
                        show_promotion_window(chooseRow, chooseCol); // 彈出升變選擇窗口
                    }
                }

                // 重置選擇狀態
                selectRow = -1; selectCol = -1;
                chooseRow = -1; chooseCol = -1;

                piece_code = 0;

                my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
                my_lcd.Set_Text_colour(WHITE);
                my_lcd.Set_Text_Size(2);
                my_lcd.Print_String("Good Job!", 5, 340);

                // 切換回合
                if (!eatPawn) {
                    if (isRedTurn) playRedBGM();
                    else playBlackBGM();
                }
                isRedTurn = !isRedTurn;

                // 確認有無被將軍
                checkGameStatus(redKingRow, redKingCol, blackKingRow, blackKingCol, isRedTurn);

                delay(200);
            } else{
                // 清除之前的選擇格子
                clear_previous_selection(1);
                // Release the piece
                my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
                my_lcd.Set_Text_colour(WHITE);
                my_lcd.Set_Text_Size(2);
                my_lcd.Print_String("Can't suicide!!", 5, 340);
                selectRow = -1; selectCol = -1;
                piece_code = 0;
                delay(200);
            }
            
        } else if (piece_code != 0) {
            // 清除之前的選擇格子
            clear_previous_selection(1);
            // Release the piece
            my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
            my_lcd.Set_Text_colour(WHITE);
            my_lcd.Set_Text_Size(2);
            my_lcd.Print_String("this is not a valid move", 5, 340);
            selectRow = -1; selectCol = -1;
            piece_code = 0;
            delay(200);
        } else if (piece_code == 0) {
            my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
            my_lcd.Set_Text_colour(WHITE);
            my_lcd.Set_Text_Size(2);
            my_lcd.Print_String("What is your move??", 5, 340);
        }
    }
}

void Start(bool GS) {
    if (GS) {
        playMainMenuBGM();
        redKingRow = -1, redKingCol = -1;
        blackKingRow = -1, blackKingCol = -1;
        isRedTurn = true;
        initDeadPieceArray();
        my_lcd.Fill_Screen(BLACK);
        drawBoard();  // Draw Board
        init_board(redFirst);
        draw_all_piece();
        count_value_display();
        Draw_Button("GO", 285, 453, 4, WHITE);
        Draw_Button("MENU", 285, 386, 2, CYAN);
        redKingMoved = false, blackKingMoved = false;
        redRookMovedLeft = false, redRookMovedRight = false;
        blackRookMovedLeft = false, blackRookMovedRight = false;
        GameStart = false;
    }
}

// 初始化流星數據
void initMeteor(Meteor &m) {
    m.x = random(0, SCREEN_WIDTH);  // 流星的起始X座標
    m.y = random(0, SCREEN_HEIGHT); // 流星的起始Y座標
    m.dx = -METEOR_SPEED;  // X方向的速度
    m.dy = METEOR_SPEED;   // Y方向的速度
    m.active = true;
}

// 更新流星位置並繪製
void updateMeteor(Meteor &m) {
    if (!m.active) return;

    // 用黑色覆蓋流星的前一幀
    my_lcd.Set_Draw_color(BLACK);
    my_lcd.Draw_Line(m.x, m.y, m.x + METEOR_LENGTH, m.y - METEOR_LENGTH);

    // 更新位置
    m.x += m.dx;
    m.y += m.dy;

    // 檢查流星是否超出邊界
    if (m.x < -5 || m.y > SCREEN_HEIGHT + 5) {
        initMeteor(m);  // 重置流星位置
    }

    // 再加上橙色的尾部，模擬出更長的漸層效果
    my_lcd.Set_Draw_color(WHITE);
    my_lcd.Draw_Line(m.x - 10, m.y + 10, m.x + METEOR_LENGTH - 10, m.y - METEOR_LENGTH + 10);
    // 再加上橙色的尾部，模擬出更長的漸層效果
    my_lcd.Set_Draw_color(YELLOW);
    my_lcd.Draw_Line(m.x - 8, m.y + 8, m.x + METEOR_LENGTH - 8, m.y - METEOR_LENGTH + 8);
    // 再加上橙色的尾部，模擬出更長的漸層效果
    my_lcd.Set_Draw_color(CYAN);
    my_lcd.Draw_Line(m.x - 4, m.y + 4, m.x + METEOR_LENGTH - 4, m.y - METEOR_LENGTH + 4);
    // 在白色後接上黃色的尾部
    my_lcd.Set_Draw_color(MAGENTA);
    my_lcd.Draw_Line(m.x - 2, m.y + 2, m.x + 6, m.y - 6);
    // 在白色後接上黃色的尾部
    my_lcd.Set_Draw_color(RED2);
    my_lcd.Draw_Line(m.x, m.y, m.x + 6, m.y - 6);
}

// 初始化所有流星
void initMeteors() {
    for (int i = 0; i < MAX_METEORS; i++) {
        initMeteor(meteors[i]);
    }
}

// 更新所有流星
void updateMeteors() {
    for (int i = 0; i < MAX_METEORS; i++) {
        updateMeteor(meteors[i]);
    }
}

// 重新繪製螢幕的函數
void redrawScreen() {
    my_lcd.Set_Text_Back_colour(BLACK);
    my_lcd.Set_Draw_color(MAGENTA);
    my_lcd.Set_Text_colour(CYAN);
    my_lcd.Set_Text_Size(4);
    my_lcd.Print_String("CHESS", 102, 130);
    my_lcd.Set_Text_colour(WHITE);
    my_lcd.Set_Text_Size(3);
    my_lcd.Print_String("Start Game", 73, 230);
    my_lcd.Draw_Round_Rectangle(60, 215, 260, 265, 10);
    my_lcd.Print_String("Computer", 91, 315);
    my_lcd.Draw_Round_Rectangle(60, 300, 260, 350, 10);
}

void setup() {
    Serial.begin(9600);
    initBuzzer();  // 初始化蜂鳴器
    my_lcd.Init_LCD();
    initMeteors();  // 初始化流星
    MENU = true;
}

void loop() {
    if (currentState == MAIN_MENU) {
        //playMainMenuBGM();
        displayMainMenu(MENU);
        TSPoint p = getTouchPoint();
        if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
            // 映射觸控點到螢幕座標
            px = map(p.x, 112, 930, 0, my_lcd.Get_Display_Width());
            py = map(p.y, 90, 960, my_lcd.Get_Display_Height(), 0);

            // 檢查是否按到"Strat Game"
            if (is_pressed(60, 215, 260, 265, px, py)) {
                GameStart = true;
                currentState = PLAYING;
            }
            // 檢查是否按到"Computer"
            if (is_pressed(60, 300, 260, 350, px, py)) {
                GameStart = true;
                currentState = COMPUTER_GAME;
            }
        }

        updateMeteors();  // 更新流星效果
        //delay(10);
        // 計時器檢查
        unsigned long currentMillis = millis();  // 取得當前的時間
        // 檢查是否到達重繪的時間
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;  // 更新上次重繪時間
            redrawScreen();  // 重新繪製螢幕
        }
    
    } else if (currentState == PLAYING) {
        Start(GameStart);
        if (isRedTurn) {
            off(33);
            blink(31);
        } else {
            off(31);
            blink(33);
        }
        // 遊戲邏輯，移動、顯示棋盤、檢查狀態
        checkTouch();

    } else if (currentState == COMPUTER_GAME) {
        Start(GameStart);
        // 遊戲邏輯，移動、顯示棋盤、檢查狀態
        checkTouch();

        // 當玩家移動结束後，輪到電腦行動
        if (!isRedTurn) {
            computerTurn();  // 電腦進行移動
            isRedTurn = true;  // 切換回玩家回合
        }

    } else if (currentState == PAUSED) {
        displayPauseScreen();
        TSPoint p = getTouchPoint();
        if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
            // 映射觸控點到螢幕座標
            px = map(p.x, 112, 930, 0, my_lcd.Get_Display_Width());
            py = map(p.y, 90, 960, my_lcd.Get_Display_Height(), 0);

            // 檢查是否按下重玩
            checkReplayButton(px, py);
            checkMainMenuButton(px, py);
            checkResumeButton(px, py);
        }

    } else if (currentState == GAME_OVER) {
        displayGameOverScreen();
        TSPoint p = getTouchPoint();
        if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
            // 映射觸控點到螢幕座標
            px = map(p.x, 112, 930, 0, my_lcd.Get_Display_Width());
            py = map(p.y, 90, 960, my_lcd.Get_Display_Height(), 0);

            checkPauseButton(px, py);
        }
    }
}

void displayMainMenu(bool menu) {
    if (menu) {
        off(33);off(31);
        my_lcd.Fill_Screen(BLACK);
        my_lcd.Set_Text_Back_colour(BLACK);
        my_lcd.Set_Draw_color(MAGENTA);
        my_lcd.Set_Text_colour(CYAN);
        my_lcd.Set_Text_Size(4);
        my_lcd.Print_String("CHESS", 102, 130);
        my_lcd.Set_Text_colour(WHITE);
        my_lcd.Set_Text_Size(3);
        my_lcd.Print_String("Start Game", 73, 230);
        my_lcd.Draw_Round_Rectangle(60, 215, 260, 265, 10);
        my_lcd.Print_String("Computer", 91, 315);
        my_lcd.Draw_Round_Rectangle(60, 300, 260, 350, 10);
        MENU = false;
    }
}

void displayPauseScreen() {
    if (Pause) {
        // 清除一個區域，準備顯示選項
        off(33);off(31);
        my_lcd.Fill_Rect(49, 88, 220, 140, BLACK); // 定義選單大小
        my_lcd.Fill_Rect(48, 88, 220, 2, MAGENTA);
        my_lcd.Fill_Rect(48, 88, 2, 140, MAGENTA);
        my_lcd.Fill_Rect(48, 228, 221, 2, MAGENTA);
        my_lcd.Fill_Rect(267, 88, 2, 140, MAGENTA);
        my_lcd.Set_Text_colour(WHITE);
        my_lcd.Set_Text_Size(3);
        my_lcd.Print_String("Main Menu", 81, 120);
        my_lcd.Print_String("Replay", 81, 180);
        my_lcd.Set_Draw_color(CYAN);
        my_lcd.Draw_Round_Rectangle(73, 114, 247, 148, 8);
        my_lcd.Draw_Round_Rectangle(73, 174, 192, 208, 8);
        drawIMG(225, 185, 2, Return);
        my_lcd.Set_Draw_color(CYAN);
        my_lcd.Draw_Round_Rectangle(220, 180, 262, 222, 8);
        Pause = false;
    }
}

void displayGameOverScreen() {
    if (GameOver) {
        // 只顯示MENU按鈕
        off(33);off(31);
        my_lcd.Fill_Rect(230, 400, 320, 480, BLACK);
        Draw_Button("MENU", 285, 386, 2, CYAN);
        playWinBGM();
        GameOver = false;
    }
}

void initDeadPieceArray() {
    for (int i = 0; i < 4; i++) {
        deadRedPiece[i] = 0;
        deadBlackPiece[i] = 0;
    }
}
