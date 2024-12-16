#ifndef COMPUTERPLAYER_H
#define COMPUTERPLAYER_H

#include "piece.h"
#include "board.h"
#include "UI.h"

// 红方先行
extern bool isRedTurn;
// 先手選擇
extern bool redFirst;

bool isComputerPiece(int row, int col) {
  if (redFirst) {   // 玩家是先手 紅方
    return (board[row][col] > 6);  // 棋盤上大於 6 的棋子為電腦的棋子
  } else {          // 玩家是後手 黑方
    return (board[row][col] > 0 && board[row][col] <= 6);  // 棋盤上大於 6 的棋子為電腦的棋子
  }
}

void movePiece(int startRow, int startCol, int targetRow, int targetCol) {
    // 更新棋盤
    board[targetRow][targetCol] = board[startRow][startCol];
    board[startRow][startCol] = 0;

    // 繪制新位置上的棋子
    put_piece(targetCol * CELL_SIZE + 5.5, targetRow * CELL_SIZE + 6, board[targetRow][targetCol]);

    // 清除舊位置
    my_lcd.Fill_Rect(startCol * CELL_SIZE + 5.5, startRow * CELL_SIZE + 6, 32, 32, BLACK);
}

// 處理電腦回合狀態
void computerTurn() {
    bool moveMade = false;
    
    while (!moveMade) {
        int pieceRow = random(0, BOARD_SIZE);
        int pieceCol = random(0, BOARD_SIZE);

        if (isComputerPiece(pieceRow, pieceCol)) {  // 檢查是否是電腦方棋子
            for (int targetRow = 0; targetRow < BOARD_SIZE; targetRow++) {
                for (int targetCol = 0; targetCol < BOARD_SIZE; targetCol++) {
                    if (is_valid_move(pieceRow, pieceCol, targetRow, targetCol, board[pieceRow][pieceCol]) && !isComputerPiece(targetRow, targetCol)) {
                        if (is_legal_move(pieceRow, pieceCol, targetRow, targetCol, false)) {
                            // 執行電腦的移動
                            movePiece(pieceRow, pieceCol, targetRow, targetCol);
                            moveMade = true;
                            return;
                        }
                    }
                }
            }
        }
    }
}

#endif