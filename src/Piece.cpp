#include "Piece.h"
#include <iostream>
#include <cmath>

#include "C_Files/bishop.c"
#include "C_Files/king.c"
#include "C_Files/knight.c"
#include "C_Files/pawn.c"
#include "C_Files/queen.c"
#include "C_Files/rook.c"

#include "C_Files/bishop_bl.c"
#include "C_Files/king_bl.c"
#include "C_Files/knight_bl.c"
#include "C_Files/pawn_bl.c"
#include "C_Files/queen_bl.c"
#include "C_Files/rook_bl.c"

Piece::Piece(SDL_Renderer* Renderer, PieceType pieceType, bool pieceTeam, float xPos, float yPos)
	:m_Renderer(Renderer), m_pieceType(pieceType), m_pieceTeam(pieceTeam), m_XPos(xPos), m_YPos(yPos)
{
	RenderThePiece(Renderer, pieceType, pieceTeam, xPos, yPos);
}
Piece::~Piece()
{
	SDL_DestroyTexture(this->GetPieceTexture());
	SDL_FreeRW(this->GetPieceRW());
	SDL_FreeSurface(this->GetPieceSurface());
	this->PossibleMovesVector().clear();
}

void Piece::RenderThePiece(SDL_Renderer* Renderer, const PieceType& pieceType, const bool& pieceTeam, const float& xPos, const float& yPos)
{
	//Rendering Piece
	std::string fileName = "";
	switch (this->m_pieceType)
	{
	case PAWN:
		fileName = "pawn";
		if (!this->m_pieceTeam)
			this->m_pieceRW = SDL_RWFromMem((void*)pawn_bl_png, sizeof(pawn_bl_png));
		else
			this->m_pieceRW = SDL_RWFromMem((void*)pawn_png, sizeof(pawn_png));
		break;
	case KNIGHT:
		fileName = "knight";
		if (!this->m_pieceTeam)
			this->m_pieceRW = SDL_RWFromMem((void*)knight_bl_png, sizeof(knight_bl_png));
		else
			this->m_pieceRW = SDL_RWFromMem((void*)knight_png, sizeof(knight_png));
		break;
	case BISHOP:
		fileName = "bishop";
		if (!this->m_pieceTeam)
			this->m_pieceRW = SDL_RWFromMem((void*)bishop_bl_png, sizeof(bishop_bl_png));
		else
			this->m_pieceRW = SDL_RWFromMem((void*)bishop_png, sizeof(bishop_png));
		break;
	case ROOK:
		fileName = "rook";
		if (!this->m_pieceTeam)
			this->m_pieceRW = SDL_RWFromMem((void*)rook_bl_png, sizeof(rook_bl_png));
		else
			this->m_pieceRW = SDL_RWFromMem((void*)rook_png, sizeof(rook_png));
		break;
	case QUEEN:
		fileName = "queen";
		if (!this->m_pieceTeam)
			this->m_pieceRW = SDL_RWFromMem((void*)queen_bl_png, sizeof(queen_bl_png));
		else
			this->m_pieceRW = SDL_RWFromMem((void*)queen_png, sizeof(queen_png));
		break;
	case KING:
		fileName = "king";
		if (!this->m_pieceTeam)
			this->m_pieceRW = SDL_RWFromMem((void*)king_bl_png, sizeof(king_bl_png));
		else
			this->m_pieceRW = SDL_RWFromMem((void*)king_png, sizeof(king_png));
		break;
	default:
		break;
	}
	//SDL_Surface* is set here
	this->m_pieceSurface = IMG_Load_RW(this->m_pieceRW, 1);
	if (!this->m_pieceSurface)
	{
		std::cout << "surf error\n";
		if (!this->m_pieceTeam)
			Chess::MissingTexture(false, fileName + "_bl.png");
		else
			Chess::MissingTexture(false, fileName + ".png");
		return;
	}
	//SDL_Texture* is set here
	this->m_pieceTexture = SDL_CreateTextureFromSurface(Renderer, this->m_pieceSurface);
	if (!this->m_pieceTexture)
	{
		std::cout << "texture error\n";
		if (!this->m_pieceTeam)
			Chess::MissingTexture(false, fileName + "_bl.png");
		else
			Chess::MissingTexture(false, fileName + ".png");
		return;
	}

	SDL_Rect pieceRect{};
	pieceRect.w = WIDTH / 8;
	pieceRect.h = HEIGHT / 8;
	pieceRect.x = (int)(m_XPos * pieceRect.w);
	pieceRect.y = (int)(m_YPos * pieceRect.h);

	SDL_RenderCopy(Renderer, m_pieceTexture, nullptr, &pieceRect);
}

void Piece::MoveThePiece(SDL_Renderer* Renderer, int boardPositionToMove, bool& currentTurn)
{
	if (currentTurn != this->GetPieceTeam())
		return;

	Piece** const boardPosition = Chess::GetBoardPos();
	
	this->m_hasMoved = true;

	int xStart = static_cast<int>(this->GetPieceX());
	int yStart = static_cast<int>(this->GetPieceY());

	int xEnd = Chess::GetBlockX(boardPositionToMove);
	int yEnd = Chess::GetBlockY(boardPositionToMove);

	// Set the number of steps for the animation
	float steps = 16.0f;
	
	// Calculate the x and y step sizes for the animation
	float xStep = (xEnd - xStart) / steps;
	float yStep = (yEnd - yStart) / steps;

	for (int i = 0; i < steps; i++)
	{
		this->AddToX(xStep);
		this->AddToY(yStep);

		//The piece is rerenderer here
		SDL_RenderClear(Renderer);
		Chess::DestroyAllPieceTextures();
		Chess::DrawChessBoard(Renderer);
		Chess::RenderAllPiece(Renderer);
		SDL_RenderPresent(Renderer);
	}

	int startPosition = xStart + (yStart * 8);
	int endPosition = xEnd + (yEnd * 8);

	//if piece exists then destroy it
	if (boardPosition[endPosition])
		delete boardPosition[endPosition];
	//assign new position to the piece
	boardPosition[endPosition] = boardPosition[startPosition];
	//set prev piece position to null
	boardPosition[startPosition] = nullptr;

	//change the turn
	currentTurn = !currentTurn;
}

void Piece::RenderPossMovesBlock(SDL_Renderer* Renderer)
{
	Piece** const boardPosition = Chess::GetBoardPos();
	//Render the possible Moves Box here
	for (int i = 0; i < this->PossibleMovesVector().size(); i++)
	{
		SDL_Rect temp1{ Chess::GetBlockX(this->PossibleMovesVector()[i]) * (WIDTH / 8), Chess::GetBlockY(this->PossibleMovesVector()[i]) * (HEIGHT / 8),  WIDTH / 8 , HEIGHT / 8 };
		//enemy piece found
		if (boardPosition[this->PossibleMovesVector()[i]] && boardPosition[this->PossibleMovesVector()[i]]->GetPieceTeam() != this->GetPieceTeam())
			SDL_SetRenderDrawColor(Renderer, 255, 20, 25, 96);
		//empty piece found
		else
			SDL_SetRenderDrawColor(Renderer, 0, 255, 25, 96);
		SDL_RenderFillRect(Renderer, &temp1);
	}
}