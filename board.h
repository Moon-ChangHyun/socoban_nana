#pragma once
#include <Windows.h>
#include <vector>
#include <array>
#include <memory>
#include <nana/gui.hpp>

class board
{
protected:
	static const unsigned int empty = 0;
	static const unsigned int player = 1;
	static const unsigned int hole = 2;
	static const unsigned int playerHole = 3;
	static const unsigned int stone = 4;
	static const unsigned int stoneHole = 6;
	static const unsigned int wall = 7;
	unsigned int mStage;
	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mStartX;
	unsigned int mStartY;
	unsigned int mGoal;
	unsigned int mBitStartX;
	unsigned int mBitStartY;
	std::vector<std::vector<int>> mMap;
public:
	static const unsigned int maxSize = 35;
	static const unsigned int bmSize = 25;
	static std::array<nana::paint::image, 8> imageAsset;
	static std::vector<std::vector<std::vector<int>>> sMap;
	static std::unique_ptr<nana::drawing> sDrawing;

	board(unsigned int);
	board(const board&) = default;
	board(board&&) noexcept;

	unsigned int getWidth() const noexcept;
	unsigned int getHeight() const noexcept;
	unsigned int getStartX() const noexcept;
	unsigned int getStartY() const noexcept;
	bool isThereWall(unsigned int, unsigned int) const;
	bool isThereStone(unsigned int, unsigned int) const;
	void moveStone(unsigned int, unsigned int, int, int) noexcept;
	void movePlayer(unsigned int, unsigned int, int, int) noexcept;
	bool isClear() const noexcept;
	virtual ~board() = default;
};

