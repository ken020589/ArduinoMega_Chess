#ifndef BOARD_H
#define BOARD_H

#include <LCDWIKI_KBV.h>
#include "piece.h"
#include "UI.h"

extern LCDWIKI_KBV my_lcd;

#define BOARD_SIZE 8
#define CELL_SIZE 39.5

// 定義顏色
#define BLACK     0x0000
#define GRAY      0XAD55	
#define DARKGRAY  0X8410
#define WHITE     0xFFFF
#define RED2      0xd020
#define RED       0xdaed
#define RED3      0xe4d3
#define GREEN     0x07E0
#define BLUE      0x001F
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0

// 紀錄選中棋子的起始位置
extern uint8_t selectRow, selectCol;
extern uint8_t chooseRow, chooseCol;
extern uint8_t prevSelectRow, prevSelectCol;  // 保存上一次藍色框框的選擇位置
extern uint8_t prevChooseRow, prevChooseCol;  // 保存上一次紅色框框的選擇位置

// 紀錄選擇之棋子
extern uint8_t piece_code;

// 紀錄國王位置 檢查有無被將軍
extern uint8_t redKingRow, redKingCol;
extern uint8_t blackKingRow, blackKingCol;

// 紀錄國王跟城堡有無移動過 王車易位
bool redKingMoved = false, blackKingMoved = false;
bool redRookMovedLeft = false, redRookMovedRight = false;
bool blackRookMovedLeft = false, blackRookMovedRight = false;

// 红方先行
extern bool isRedTurn;

// 先手選擇
extern bool redFirst;

extern uint8_t board[BOARD_SIZE][BOARD_SIZE];

extern GameState currentState;

// 棋盤初始化函數
void init_board(bool redFirst) {
    uint8_t initB_RED[BOARD_SIZE][BOARD_SIZE] = {
        {10,8,9,11,12,9,8,10},
        {7,7,7,7,7,7,7,7},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {1,1,1,1,1,1,1,1},
        {4,2,3,5,6,3,2,4},
    };
    uint8_t initB_BLACK[BOARD_SIZE][BOARD_SIZE] = {
        {4,2,3,6,5,3,2,4},
        {1,1,1,1,1,1,1,1},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {7,7,7,7,7,7,7,7},
        {10,8,9,12,11,9,8,10},
    };

    if (redFirst) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                board[i][j] = initB_RED[i][j];
                if (initB_RED[i][j] == 6) {  // RED KING
                    redKingRow = i;
                    redKingCol = j;
                } else if (initB_RED[i][j]== 12) {  // BLACK KING
                    blackKingRow = i;
                    blackKingCol = j;
                }
            }
        }
    } else {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                board[i][j] = initB_BLACK[i][j];
                if (initB_BLACK[i][j] == 6) {  // RED KING
                    redKingRow = i;
                    redKingCol = j;
                } else if (initB_BLACK[i][j]== 12) {  // BLACK KING
                    blackKingRow = i;
                    blackKingCol = j;
                }
            }
        }
    }

}

void drawBoard() {
    my_lcd.Fill_Rect(0, 0, 320, 320, BLACK);  // 清屏黑色
    for (int i = 0; i <= BOARD_SIZE; i++) {
        my_lcd.Fill_Rect(0, i * CELL_SIZE, BOARD_SIZE * CELL_SIZE, 2.5,WHITE);
        my_lcd.Fill_Rect(i * CELL_SIZE, 0, 2.5, BOARD_SIZE * CELL_SIZE, WHITE);
    }
}

void draw_scale_pix(int x, int y, int scale, uint16_t color) {
    my_lcd.Set_Draw_color(color);
    my_lcd.Fill_Rectangle(x, y, x + scale - 1, y + scale - 1);
}

void drawIMG(int x, int y, int scale, uint16_t piece[16][16]) {
  for (int row = 0; row < 16; row++) {
    for (int col = 0; col < 16; col++) {
      // use [pgm_read_word] read the data in Flash
      uint16_t color = pgm_read_word(&(piece[row][col]));
      draw_scale_pix(x + col * scale, y + row * scale, scale, color);
    }
  }
}

void put_piece(int board_x, int board_y, uint8_t piece_code) {
  switch ( piece_code )
  {
    case 1:
      // RedPawn
      drawIMG(board_x, board_y, 2, RedPawn);
      break;
    case 2:
      // RedKnight
      drawIMG(board_x, board_y, 2, RedKnight);
      break;
    case 3:
      // RedBishop
      drawIMG(board_x, board_y, 2, RedBishop);
      break;
    case 4:
      // RedRook
      drawIMG(board_x, board_y, 2, RedRook);
      break;
    case 5:
      // RedQueen
      drawIMG(board_x, board_y, 2, RedQueen);
      break;
    case 6:
      // RedKing
      drawIMG(board_x, board_y, 2, RedKing);
      break;
    case 7:
      // BlackPawn
      drawIMG(board_x, board_y,2, BlackPawn);
      break;
    case 8:
      // BlackKnight
      drawIMG(board_x, board_y, 2, BlackKnight);
      break;
    case 9:
      // BlackBishop
      drawIMG(board_x, board_y, 2, BlackBishop);
      break;
    case 10:
      // BlackRook
      drawIMG(board_x, board_y, 2, BlackRook);
      break;
    case 11:
      // BlackQueen
      drawIMG(board_x, board_y, 2, BlackQueen);
      break;
    case 12:
      // BlackKing
      drawIMG(board_x, board_y, 2, BlackKing);
      break;
    default:
      //Serial.println("Unknown piece code!");
      break;
  }
}

void draw_all_piece() {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            int board_y = row * CELL_SIZE + 6;
            int board_x = col * CELL_SIZE + 5.5;
            put_piece(board_x, board_y, board[row][col]); // 更新棋子
        }
    }
}

// 紀錄上一個移動的兵是否是向前兩步的特殊移動
int last_pawn_double_row = -1;
int last_pawn_double_col = -1;
bool last_move_double_step = false;  // 是否有兵剛走了兩布

bool can_pawn_move(int start_row, int start_col, int dest_row, int dest_col, bool isRed) {
    int direction = isRed ? -1 : 1;
    int opponent_pawn = isRed ? 7 : 1;  // 對方的兵

    // 只能前進一步，不能後退，不能橫向移動
    if (start_col == dest_col && board[dest_row][dest_col] == 0) {
        if ((isRed && start_row == 6) || (!isRed && start_row == 1)) {
            // 第一行起始位置，小兵可以走一或兩步
            if (dest_row == start_row + direction || dest_row == start_row + 2 * direction) {
                // 記錄走兩步的兵
                if (dest_row == start_row + 2 * direction) {
                    last_pawn_double_row = dest_row;
                    last_pawn_double_col = dest_col;
                    last_move_double_step = true;
                } else {
                    last_move_double_step = false;
                }
                return true;
            }
        } else if (dest_row == start_row + direction) {
            // 其餘只能走一步
            last_move_double_step = false;
            return true;
        }
    }

    // 檢查是否可以吃子
    if (abs(dest_col - start_col) == 1 && dest_row == start_row + direction) {
        // 普通吃子
        if (board[dest_row][dest_col] != 0 && (board[dest_row][dest_col] > 6 || board[dest_row][dest_col] <= 6)) {
            last_move_double_step = false;
            return true;
        }

        // 過路吃子檢測
        if (last_move_double_step && board[start_row][dest_col] == opponent_pawn && last_pawn_double_row == start_row &&
            last_pawn_double_col == dest_col) {
            // 模擬移動棋子，並刪除過路吃子的目標兵
            board[last_pawn_double_row][last_pawn_double_col] = 0;  // 吃掉過路兵
            last_move_double_step = false;
            // 清除舊位置
            my_lcd.Fill_Rect(last_pawn_double_col * CELL_SIZE + 5.5, last_pawn_double_row * CELL_SIZE + 6, 32, 32, BLACK);
            return true;
        }
    }
    
    return false;
}

bool can_knight_move(int start_row, int start_col, int dest_row, int dest_col) {
    int row_diff = abs(dest_row - start_row);
    int col_diff = abs(dest_col - start_col);
    return (row_diff == 2 && col_diff == 1) || (row_diff == 1 && col_diff == 2);
}

bool can_bishop_move(int start_row, int start_col, int dest_row, int dest_col) {
    int row_diff = abs(dest_row - start_row);
    int col_diff = abs(dest_col - start_col);
    // Ckeck if it's on a diagonal
    if (row_diff != col_diff) {
        return false;
    }
    // Get move direction
    int row_direction = (dest_row > start_row) ? 1 : -1;
    int col_direction = (dest_col > start_col) ? 1 : -1;

    // Check if there have anyother pieces on the direction
    int check_row = start_row + row_direction;
    int check_col = start_col + col_direction;
    while (check_row != dest_row && check_col != dest_col) {
        if (board[check_row][check_col] != 0) {
            return false;  // Barrier
        }
        check_row += row_direction;
        check_col += col_direction;
    }

    return true;  // No barrier, can move.
}

bool can_rook_move(int start_row, int start_col, int dest_row, int dest_col) {
    int row_diff = abs(dest_row - start_row);
    int col_diff = abs(dest_col - start_col);
    // Ckeck if it's on a straight line
    if (row_diff != 0 && col_diff != 0) {
        return false;
    }
    
    // Check if there have anyother pieces on the direction
    if (row_diff == 0) {  // Move in same col
        int col_direction = (dest_col > start_col) ? 1 : -1;
        for (int col = start_col + col_direction; col != dest_col; col += col_direction) {
            if (board[start_row][col] != 0) {
                return false;  // Exist Barrier
            }
        }
    } else {  // Move in same row
        int row_direction = (dest_row > start_row) ? 1 : -1;
        for (int row = start_row + row_direction; row != dest_row; row += row_direction) {
            if (board[row][start_col] != 0) {
                return false;  // Exist Barrier
            }
        }
    }

    return true;  // No barrier, can move.
}

bool can_queen_move(int start_row, int start_col, int dest_row, int dest_col) {
    return can_bishop_move(start_row, start_col, dest_row, dest_col) || 
           can_rook_move(start_row, start_col, dest_row, dest_col);
}

bool can_king_move(int start_row, int start_col, int dest_row, int dest_col) {
    int row_diff = abs(dest_row - start_row);
    int col_diff = abs(dest_col - start_col);
    return (row_diff <= 1 && col_diff <= 1);
}

bool is_valid_move(int start_row, int start_col, int dest_row, int dest_col, uint8_t piece) {
    switch (piece) {
        case 1: // RedPawn
            return can_pawn_move(start_row, start_col, dest_row, dest_col, redFirst);
        case 7: // BlackPawn
            return can_pawn_move(start_row, start_col, dest_row, dest_col, !redFirst);
        case 2: // RedKnight
        case 8: // BlackKnight
            return can_knight_move(start_row, start_col, dest_row, dest_col);
        case 3: // RedBishop
        case 9: // BlackBishop
            return can_bishop_move(start_row, start_col, dest_row, dest_col);
        case 4: // RedRook
        case 10: // BlackRook
            return can_rook_move(start_row, start_col, dest_row, dest_col);
        case 5: // RedQueen
        case 11: // BlackQueen
            return can_queen_move(start_row, start_col, dest_row, dest_col);
        case 6: // RedKing
        case 12: // BlackKing
            return can_king_move(start_row, start_col, dest_row, dest_col);
        default:
            //Serial.println("Unknown piece movement!");
            return false;
    }
}

void clear_previous_selection(int color) {
    // 如果之前有藍色框框，抹除它
    if (color == 1) {
        my_lcd.Fill_Rect((prevSelectCol * CELL_SIZE) + 3, (prevSelectRow * CELL_SIZE) + 3, 36, 36, BLACK);
        put_piece(prevSelectCol * CELL_SIZE + 5.5, prevSelectRow * CELL_SIZE + 6, board[prevSelectRow][prevSelectCol]);
        my_lcd.Fill_Rect((prevChooseCol * CELL_SIZE) + 3, (prevChooseRow * CELL_SIZE) + 3, 36, 36, BLACK);
        put_piece(prevChooseCol * CELL_SIZE + 5.5, prevChooseRow * CELL_SIZE + 6, board[prevChooseRow][prevChooseCol]);
        prevSelectRow = -1; prevSelectCol = -1; prevChooseRow = -1; prevChooseCol = -1;
    }

    // 如果之前有紅色框框，抹除它
    if (color == 2) {
        my_lcd.Fill_Rect((prevChooseCol * CELL_SIZE) + 3, (prevChooseRow * CELL_SIZE) + 3, 36, 36, BLACK);
        put_piece(prevChooseCol * CELL_SIZE + 5.5, prevChooseRow * CELL_SIZE + 6, board[prevChooseRow][prevChooseCol]);
    }
}

// 給王車易位檢測城堡是否移動過
void updateRookMoveStatus(uint8_t piece_code, int col, bool isRedTurn) {
    if (isRedTurn) {
        if (piece_code == 4 && col == 0) {
            redRookMovedLeft = true;  // 红方左城堡
        } else if (piece_code == 4 && col == 7) {
            redRookMovedRight = true;  // 红方右城堡
        }
    } else {
        if (piece_code == 10 && col == 0) {
            blackRookMovedLeft = true;  // 黑方左城堡
        } else if (piece_code == 10 && col == 7) {
            blackRookMovedRight = true;  // 黑方右城堡
        }
    }
}

// 給王車易位檢測國王是否移動過
void updateMoveStatus(uint8_t piece_code, int col, bool isRedTurn) {
    if (piece_code == 6) {
        redKingMoved = true;  // 红國王
    } else if (piece_code == 12) {
        blackKingMoved = true;  // 黑國王
    } else {
        updateRookMoveStatus(piece_code, col, isRedTurn);  // 更新城堡的狀態
        Serial.print(redKingMoved);Serial.print(blackKingMoved);Serial.print("\n");
        Serial.print(redRookMovedLeft);Serial.print(redRookMovedRight);Serial.print(blackRookMovedLeft);Serial.print(blackRookMovedRight);Serial.print("\n");
    }
}

void update_king_position(int piece_code, int newRow, int newCol) {
    if (piece_code == 6) {  // redKing
        redKingRow = newRow;
        redKingCol = newCol;
    } else if (piece_code == 12) {  // blackKing
        blackKingRow = newRow;
        blackKingCol = newCol;
    }
}

// 此函數中使用偵測的是同一個顏色的參數
bool is_in_check(int kingRow, int kingCol, bool isRedKing) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            int piece = board[row][col];
            if ((isRedKing && piece > 6) || (!isRedKing && piece <= 6)) {
                if (is_valid_move(row, col, kingRow, kingCol, piece)) {
                    return true;  // 國王被將軍
                }
            }
        }
    }
    return false;
}

// 國王不能自殺
bool is_legal_move(int startRow, int startCol, int destRow, int destCol, bool isRedTurn) {
    // 暫存當前的棋盤狀態
    uint8_t tempPiece = board[destRow][destCol];
    bool is_king_move = false;

    // 看移動的是否是國王
    if (board[startRow][startCol] == 6 && isRedTurn) {
        is_king_move = true;
        redKingRow = destRow;
        redKingCol = destCol;
    } else if (board[startRow][startCol] == 12 && !isRedTurn) {
        is_king_move = true;
        blackKingRow = destRow;
        blackKingCol = destCol;
    }

    // 模擬移動
    board[destRow][destCol] = board[startRow][startCol];
    board[startRow][startCol] = 0;

    // 檢查移動後國王是否仍然在將軍狀態
    int kingRow = isRedTurn ? redKingRow : blackKingRow;
    int kingCol = isRedTurn ? redKingCol : blackKingCol;

    // 檢查移動後，該方國王是否安全
    if(is_in_check(kingRow, kingCol, isRedTurn)) {
        // 恢復棋盤原狀
        board[startRow][startCol] = board[destRow][destCol];
        board[destRow][destCol] = tempPiece;
        // 如果是國王的移動，將其座標恢復
        if (is_king_move) {
            if (isRedTurn) {
                redKingRow = startRow;
                redKingCol = startCol;
            } else {
                blackKingRow = startRow;
                blackKingCol = startCol;
            }
        }
        // 如果國王移動後不安全，則該移動無效
        return false;
    }
    // 恢復棋盤原狀
    board[startRow][startCol] = board[destRow][destCol];
    board[destRow][destCol] = tempPiece;

    // 如果是國王的移動，將其座標恢復
    if (is_king_move) {
        if (isRedTurn) {
            redKingRow = startRow;
            redKingCol = startCol;
        } else {
            blackKingRow = startRow;
            blackKingCol = startCol;
        }
    }

    // 如果移動合法，返回 true
    return true;
}

bool is_checkmate(int kingRow, int kingCol, bool isRedKing) {
    // 1. 檢查國王附近是否存在不被將軍的位置
    for (int rowDiff = -1; rowDiff <= 1; rowDiff++) {
        for (int colDiff = -1; colDiff <= 1; colDiff++) {
            if (rowDiff == 0 && colDiff == 0) continue;  // 跳過當前國王位置

            int newRow = kingRow + rowDiff;
            int newCol = kingCol + colDiff;

            // 檢查是否在棋盤範圍內
            if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE) {
                int targetPiece = board[newRow][newCol];

                // 檢查目標格子是否有隊友的棋子
                if (isRedKing && targetPiece > 0 && targetPiece < 6) continue;  // 跳過紅色隊友的格子
                if (!isRedKing && targetPiece > 6) continue;  // 跳過黑色隊友的格子

                // 模擬國王移動到新位置
                int temp = board[newRow][newCol];  // 保存目標格子原本的棋子
                board[newRow][newCol] = board[kingRow][kingCol];  // 移動國王
                board[kingRow][kingCol] = 0;  // 原位置變空

                // 檢查移動後國王是否仍然在將軍狀態
                if(!is_in_check(newRow, newCol, isRedTurn)){
                    // 恢復棋盤狀態
                    //draw_all_piece();
                    board[kingRow][kingCol] = board[newRow][newCol];  // 將國王移回原位
                    board[newRow][newCol] = temp;  // 恢復目標格子的原狀
                    return false;
                }
                
                // 恢復棋盤狀態
                board[kingRow][kingCol] = board[newRow][newCol];  // 將國王移回原位
                board[newRow][newCol] = temp;  // 恢復目標格子的原狀
            }
        }
    }
    
    // 2. 檢查其他棋子是否可以解除將軍
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            int piece = board[row][col];

            // 跳過當前一方的國王
            if (piece == 6 || piece == 12) continue;  
        
            // 如果是當前一方的棋子
            if ((isRedKing && piece < 6 && piece > 0) || (!isRedKing && piece > 6)) {
                // 檢查該棋子的所有可能移動
                for (int targetRow = 0; targetRow < BOARD_SIZE; targetRow++) {
                    for (int targetCol = 0; targetCol < BOARD_SIZE; targetCol++) {
                        // 跳過己方棋子的位置，因為不能移動到己方棋子位置
                        if ((isRedKing && board[targetRow][targetCol] > 0 && board[targetRow][targetCol] < 6) ||
                            (!isRedKing && board[targetRow][targetCol] > 6)) {
                            continue;
                        }

                        // 確認該棋子的移動是否有效
                        if (is_valid_move(row, col, targetRow, targetCol, piece)) {
                            // 保存當前狀態，模擬移動該棋子
                            int temp = board[targetRow][targetCol];
                            board[targetRow][targetCol] = piece;
                            board[row][col] = 0;
                            
                            // 檢查移動後國王是否仍然被將軍
                            if (!is_in_check(kingRow, kingCol, isRedTurn)) {
                                // 如果國王不再被將軍，則可以解除將軍
                                //draw_all_piece();
                                board[targetRow][targetCol] = temp;  // 恢復原本狀態
                                board[row][col] = piece;
                                return false;  // 有棋子可以移動來解除將軍
                            }

                            // 恢復原本狀態
                            board[targetRow][targetCol] = temp;
                            board[row][col] = piece;
                        }
                    }
                }
            }
        }
    }

    // 如果沒有任何合法移動可以解除將軍，則判定為將死
    return true;
}

// 統一處理當前回合的將軍與將死檢測
void checkGameStatus(int redKingRow, int redKingCol, int blackKingRow, int blackKingCol, bool isRedTurn) {
    int kingRow = isRedTurn ? redKingRow : blackKingRow;  // 根據當前回合確定國王位置
    int kingCol = isRedTurn ? redKingCol : blackKingCol;

    my_lcd.Set_Text_colour(RED2);
    my_lcd.Set_Text_Size(2);

    // 檢查國王是否被將軍
    if (is_in_check(kingRow, kingCol, isRedTurn)) {
        // 清除之前的顯示區域
        my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
        my_lcd.Print_String(isRedTurn ? "Red King is in check!" : "Black King is in check!", 5, 340);

        // 檢查是否被將死
        if (is_checkmate(kingRow, kingCol, isRedTurn)) {
            my_lcd.Fill_Rect(5, 340, 320, 20, BLACK);
            my_lcd.Print_String(isRedTurn ? "Black wins!" : "Red wins!", 5, 340);  // 宣布勝利者
            GameOver = true;
            currentState = GAME_OVER;
        }
    }
}

// 檢查是否可以進行王車易位
bool can_castle(bool isRed, bool isLeftSide) {
    if (isRed) {
        if (redKingMoved) return false;  // 國王已移動
        if (redRookMovedLeft && isLeftSide) return false;  // 左城堡已移動
        if (redRookMovedRight && !isLeftSide) return false;  // 右城堡已移動
    } else {
        if (blackKingMoved) return false;  // 國王已移動
        if (blackRookMovedLeft && isLeftSide) return false;  // 左城堡已移動
        if (blackRookMovedRight && !isLeftSide) return false;  // 右城堡已移動
    }

    // 檢查國王和城堡之間的路徑是否清空
    int kingRow = isRed ? redKingRow : blackKingRow;
    int kingCol = isRed ? redKingCol : blackKingCol;
    int rookCol = isLeftSide ? 0 : 7;
    for (int col = min(kingCol, rookCol) + 1; col < max(kingCol, rookCol); col++) {
        if (board[kingRow][col] != 0) return false;  // 路徑未清空
    }

    // 檢查路徑上是否存在被將軍的格子
    for (int col = min(kingCol, rookCol); col <= max(kingCol, rookCol); col++) {
        if (is_in_check(kingRow, col, isRed)) return false;  // 路徑被將軍
    }

    return true;
}

void perform_castle(bool isRed, bool isLeftSide) {
    int kingRow = isRed ? redKingRow : blackKingRow;
    int kingCol = isRed ? redKingCol : blackKingCol;  // 根據當前的國王位置來計算

    // 如果棋盤翻轉，左側易位將使國王移動到 kingCol + 2，右側易位移動到 kingCol - 2
    int finalKingCol = isLeftSide ? kingCol - 2 : kingCol + 2;
    int rookCol = isLeftSide ? 0 : 7;
    int newRookCol = isLeftSide ? finalKingCol + 1 : finalKingCol - 1;

    // 移動國王和城堡
    board[kingRow][finalKingCol] = isRed ? 6 : 12;  // 移動國王
    board[kingRow][newRookCol] = isRed ? 4 : 10;  // 移動城堡
    board[kingRow][kingCol] = 0;  // 清除原國王位置
    board[kingRow][rookCol] = 0;  // 清除原城堡位置

    // 更新全局變量
    if (isRed) {
        redKingCol = finalKingCol;  // 更新紅國王的位置
        redKingMoved = true;
        if (isLeftSide) redRookMovedLeft = true;
        else redRookMovedRight = true;
    } else {
        blackKingCol = finalKingCol;  // 更新黑國王的位置
        blackKingMoved = true;
        if (isLeftSide) blackRookMovedLeft = true;
        else blackRookMovedRight = true;
    }

    // 更新國王位置
    update_king_position(piece_code, kingRow, finalKingCol);

    // 繪製畫面新位子 國王與城堡原位子清空
    put_piece(finalKingCol * CELL_SIZE + 5.5, kingRow * CELL_SIZE + 6, piece_code);
    put_piece(newRookCol * CELL_SIZE + 5.5, kingRow * CELL_SIZE + 6, isRed ? 4 : 10);
    my_lcd.Fill_Rect(selectCol * CELL_SIZE + 5.5, selectRow * CELL_SIZE + 6, 32, 32, BLACK);
    my_lcd.Fill_Rect(chooseCol * CELL_SIZE + 5.5, chooseRow * CELL_SIZE + 6, 32, 32, BLACK);

    // 重置選擇狀態
    selectRow = -1; selectCol = -1;
    chooseRow = -1; chooseCol = -1;
    piece_code = 0;
}

#endif