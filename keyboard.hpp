#pragma once
struct Keys {
	bool w, a, s, d, x, f, space;
};


extern Keys key;

void buttonDown(unsigned char keyIn, int x, int y);
void buttonUp(unsigned char keyIn, int x, int y);