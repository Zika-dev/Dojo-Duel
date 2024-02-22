struct Keys {
	bool w, a, s, d, x, f, space;
};

Keys key;

void buttonDown(unsigned char keyIn, int x, int y) {
	if (keyIn == 'w')
		key.w = true;
	if (keyIn == 'a')
		key.a = true;
	if (keyIn == 's')
		key.s = true;
	if (keyIn == 'd')
		key.d = true;
	if (keyIn == 'x')
		key.x = true;
	if (keyIn == 'f')
		key.f = true;
	if (keyIn == ' ')
		key.space = true;
}

void buttonUp(unsigned char keyIn, int x, int y) {
	if (keyIn == 'w')
		key.w = false;
	if (keyIn == 'a')
		key.a = false;
	if (keyIn == 's')
		key.s = false;
	if (keyIn == 'd')
		key.d = false;
	if (keyIn == 'x')
		key.x = false;
	if (keyIn == 'f')
		key.f = false;
	if (keyIn == ' ')
		key.space = false;
}