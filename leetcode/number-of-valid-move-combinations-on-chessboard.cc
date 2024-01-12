class Solution {
	using InputPiece = std::string;
	using InputPosition = std::vector<int>;

	enum PieceType { Rook, Bishop, Queen };

	struct Position {
		unsigned index; // 0-63
		Position (int y, int x) : index{y*8 + x} {}
		int getY() const { return index/8; }
		int getX() const { return index%8; }
		bool operator<(Position o) const { index < o.index; }
	}

	struct Piece {
		PieceType type;
		Position position;
		std::map<Position, unsigned> moves;

		Piece(InputPiece t, InputPosition p) {
			type = (t == "rook"  ) ? Rook
			:      (t == "bishop") ? Bishop
			:     /*t == "queen" */? Queen;
			position = {p[0], p[1]};

			moves[position] = 0;
			for (int distance=1; distance<8; distance++) {
				int y0 = position.getY();
				int x0 = position.getX();

				auto setPos = [&](int y, int x) {
					if ((0 <= y&&y < 8) && (0 <= x&&x < 8)) moves[{y,x}] = distance;
				};

				if (type == Rook || type == Queen) {
					setPos(y0, x0 + distance);
					setPos(y0, x0 - distance);
					setPos(y0 + distance, x0);
					setPos(y0 - distance, x0);
				}

				if (type == Bishop || type == Queen) {
					setPos(x0 + distance, y0 + distance);
					setPos(x0 + distance, y0 - distance);
					setPos(x0 - distance, y0 + distance);
					setPos(x0 - distance, y0 - distance);
				}
			}
		}
	};

public:
	int countCombinations(
		std::vector<InputPiece>& inTypes,
		std::vector<InputPosition>& inPositions
	) {
		std::vector<Piece> pieces;
		for (int i=0; i<inTypes.size(); i++) {
			pieces.push_back(inTypes[i], inPositions[i]);
		}

		// UPPER BOUND:
		std::reduce(pieces.begin(), pieces.end(), 1,
			[](Piece a, Piece b) { return a.moves.size() * b.moves.size(); }
		);
	}
};