//
// Created by root on 17-12-23.
//

#ifndef ENGINE_SUDOKU_H
#define ENGINE_SUDOKU_H


#include "../../base/Type.h"
#include "../../base/StringPiece.h"

string solveSudoku(const StringPiece& puzzle);
const int kCells = 81;
extern const char kNoSolution[];

#endif //ENGINE_SUDOKU_H
