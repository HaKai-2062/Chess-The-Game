#pragma once
#include "Piece.h"

class Knight : public Piece
{
public:
	//store vars from piece to bishop
	Knight(SDL_Renderer*, int, int, int);
	//void DrawBishops();
	//bool isValidMove();
	~Knight();
};