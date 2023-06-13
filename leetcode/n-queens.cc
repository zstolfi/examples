class Solution {
private:
    enum { Empty = ' ', Taken = '.', Queen = 'Q' }

    struct Board {
        using Data = vector<string>;
        Data b;
        int  n;
        Board(int n) : b(n,string(4,Empty)), n(n) {}
    };

    bool operator==(const Board& a, const Board& b) {
        if (a.n != b.n) { return false; }
        for (int i=0; i<a.n; i++) {
        for (int j=0; j<a.n; j++) {
            if (a.b[i][j] != b.b[i][j]) { return false; }
        } }
        return true;
    }
    
    bool symmetryCompare(const Board& a, const Board& b) {
        for (Transformation t : SquareGroup)
            if (t(a) == b) { return true; }
        return false;
    }

public:
    std::vector<Board::Data> solveNQueens(int n) {
        std::vector<Board::Data> results;



        return results;
    }
};