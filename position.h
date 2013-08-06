#ifndef POSITION_H
#define POSITION_H
#include <vector>

class position{
public:

    position();

    position(const position &givenPos, const int row1, const int col1, const int row2, const int col2);

    class positionRow{
    private:
        const position &parent;
        int rowId;

    public:
        positionRow(const position &p, int theRowId):
            parent(p), rowId(theRowId)
        {

        }

        const int& operator[](int colId)  const {
            return parent.pget(rowId,colId);
        }
    };

    positionRow operator[](int rowId) const{
        return positionRow(*this, rowId);
    }

private:
    int cells[8][8];

    friend class positionRow;
    const int&   pget(int row,int col) const{
        return cells[row][col];
    }

};

#endif // POSITION_H
