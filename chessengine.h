#ifndef CHESSENGINE_H
#define CHESSENGINE_H
#include <libExecStream/exec-stream.h>
#include <iostream>
#include <tuple>

class chessEngine
{

public:
    typedef std::tuple<int,int,int,int> move;

    chessEngine();

    void makeMove(const int row1, const int col1, const int row2, const int col2);

    void newGame();

    move getMove();

    bool load();

    void unLoad();

private:
    exec_stream_t process;

    void waitForOk();

    std::string moveString(const int row1, const int col1, const int row2, const int col2) const;

    std::string colToString(const int col) const;
    int stringToCol(const std::string stringedCol) const;

    move stringToTuple(const std::string theString) const;

    std::string moveList;

};

#endif // CHESSENGINE_H
