#include "Cube.h"


Cube::Cube()
{
}


Cube::~Cube()
{
}

void Cube::customDraw() 
{
	ofBox(0.10);
	ofDrawAxis(0.20);
}
