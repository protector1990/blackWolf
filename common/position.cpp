#include "position.h"
//#include <boost/assert.hpp>

position::position()
{
    init();
}

position::position(const position &givenPos, const int row1, const int col1, const int row2, const int col2):
    turnColor(-givenPos.turnColor),
    whiteCastleQueen(givenPos.whiteCastleQueen),
    whiteCastleKing(givenPos.whiteCastleKing),
    blackCastleQueen(givenPos.blackCastleQueen),
    blackCastleKing(givenPos.blackCastleKing),
    wasCastle(false),
    wasEnPassant(false),
    wasPromotion(false)
{
    //makes a new position out of the given one, moving the piece on first to square to second    

    for (int i=0; i<8; ++i){
        for (int j=0; j<8; ++j){
            const int givenPiece = givenPos[i][j];
            if ((givenPiece == 7)||(givenPiece==-7)) cells[i][j] = 0; //clear shadow pawn
            else cells[i][j] = givenPiece;
        }
    }

    const int pieceCode = cells[row1][col1];
    const int destPiece = givenPos[row2][col2];

    //check if en passant capture
    if ((destPiece==7)||(destPiece==-7)){ //about to nick a shadow pawn
        wasEnPassant = true;
        cells[row2+turnColor][col2] = 0; //remove the true pawn
    }

    cells[row2][col2] = pieceCode;
    cells[row1][col1] = 0;

    //this will perform the castle regardless of whether the side has castling rights
    //will produce gibberish if castling rights have been lost or way is obstructed
    if (pieceCode==6){
        if ((row1==0)&&(row2==0)){
            if (col1==4){
                if (col2==6){
                    cells[0][5] = cells[0][7];
                    cells[0][7] = 0;
                    wasCastle = true;
                }else if (col2==2){
                    cells[0][3] = cells[0][0];
                    cells[0][0] = 0;
                    wasCastle = true;
                }
            }
        }
    }else if (pieceCode==-6){
        if ((row1==7)&&(row2==7)){
            if (col1==4){
                if (col2==6){
                    cells[7][5] = cells[7][7];
                    cells[7][7] = 0;
                    wasCastle = true;
                }else if (col2==2){
                    cells[7][3] = cells[7][0];
                    cells[7][0] = 0;
                    wasCastle = true;
                }
            }
        }
    }

    //checking if castling rights should be lost
    switch (pieceCode) {
    case 6:
        whiteCastleQueen = false;
        whiteCastleKing = false;
        break;
    case -6:
        blackCastleQueen = false;
        blackCastleKing = false;
        break;
    case 1:
        if ((row1==0)&&(col1==0)) whiteCastleQueen = false;
        else if ((row1==0)&&(col1==7)) whiteCastleKing = false;
        break;
    case -1:
        if ((row1==7)&&(col1==0)) blackCastleQueen = false;
        else if ((row1==7)&&(col1==7)) blackCastleKing = false;
        break;
    }

    //create shadow pawn for en passant
    if (pieceCode==5){
        if ((row1==1)&&(row2==3)) cells[2][col2] = 7;
    }else if (pieceCode==-5){
        if ((row1==6)&&(row2==4)) cells[5][col2] = -7;
    }

    //check if promotion
    if (pieceCode==5){
        if (row2==7){
            wasPromotion = true;            
        }
    }else if (pieceCode==-5){
        if (row2==0){
            wasPromotion = true;            
        }
    }

}


void position::init()
{
    turnColor = 1;

    whiteCastleQueen = true;
    whiteCastleKing = true;
    blackCastleQueen = true;
    blackCastleKing = true;

    wasCastle = false;
    wasEnPassant = false;
    wasPromotion = false;

    cells[0][0] = 1;
    cells[0][1] = 3;
    cells[0][2] = 2;
    cells[0][3] = 4;
    cells[0][4] = 6;
    cells[0][5] = 2;
    cells[0][6] = 3;
    cells[0][7] = 1;
    cells[7][0] = -1;
    cells[7][1] = -3;
    cells[7][2] = -2;
    cells[7][3] = -4;
    cells[7][4] = -6;
    cells[7][5] = -2;
    cells[7][6] = -3;
    cells[7][7] = -1;

    for (int i=0; i<8; ++i){
        cells[1][i] = 5;
        cells[6][i] = -5;
        for (int j=2; j<6; ++j){
            cells[j][i] = 0;
        }
    }

}

void position::setPromotion(const int row, const int col, const int chosenPiece)
{
    cells[row][col] = chosenPiece;
}