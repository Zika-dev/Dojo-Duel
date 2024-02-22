struct Position {
	long double x, y;
};

struct intPos {
	int x, y;
};

struct Color {
	float r = 0, g = 0, b = 0;
};

struct Sprite {
    int type;
    int state;
    int texture;
    Position pos;
    double z;
};