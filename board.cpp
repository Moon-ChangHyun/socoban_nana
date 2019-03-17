#include "board.h"
using namespace std;
using namespace nana;

array<paint::image, 8> board::imageAsset;
vector<vector<vector<int>>> board::sMap;
unique_ptr<drawing> board::sDrawing;

board::board(unsigned int inStage) : mStage(inStage), mWidth(board::sMap[mStage].front().size()), mHeight(board::sMap[inStage].size()), mGoal(0)
{
	mMap = vector<vector<int>>(mHeight, vector<int>(mWidth));
	for (unsigned int y = 0; y != mHeight; ++y) {
		for (unsigned int x = 0; x != mWidth; ++x) {
			switch (mMap[y][x] = board::sMap[mStage][y][x]) {
			case board::player:
				mStartY = y;
				mStartX = x;
				break;
			case board::hole:
				++mGoal;
				break;
			case board::playerHole:
				mStartY = y;
				mStartX = x;
				++mGoal;
				break;
			default:;
			}
		}
	}
	mBitStartX = (875 - bmSize * mWidth) >> 1;
	mBitStartY = (875 - bmSize * mHeight) >> 1;

	sDrawing->clear();
	sDrawing->draw([&](paint::graphics& graph) {
		for (unsigned int i = 0; i != maxSize; ++i)
			for (unsigned int j = 0; j != maxSize; ++j)
				imageAsset[0].paste(graph, { (int)(j * bmSize), (int)(i * bmSize) });
		for (unsigned int i = 0; i != mHeight; ++i) {
			for (unsigned int j = 0; j != mWidth; ++j) {
				imageAsset[mMap[i][j]].paste(graph, { (int)(mBitStartX + j * bmSize), (int)(mBitStartY + i * bmSize) });
			}
		}
	});
}

board::board(board&& src) noexcept : mStage(src.mStage), mWidth(src.mWidth), mHeight(src.mHeight), mStartX(src.mStartX), mStartY(src.mStartY), mGoal(src.mGoal), mMap(move(src.mMap))
{
	src.mStage = 0;
	src.mHeight = 0;
	src.mWidth = 0;
	src.mGoal = 0;
	src.mStartX = 0;
	src.mStartY = 0;
}

unsigned int board::getWidth() const noexcept
{
	return mWidth;
}

unsigned int board::getHeight() const noexcept
{
	return mHeight;
}

unsigned int board::getStartX() const noexcept
{
	return mStartX;
}

unsigned int board::getStartY() const noexcept
{
	return mStartY;
}

bool board::isThereWall(unsigned int xPos, unsigned int yPos) const
{
	return mMap.at(yPos).at(xPos) == board::wall;
}

bool board::isThereStone(unsigned int xPos, unsigned int yPos) const
{
	return (mMap.at(yPos).at(xPos) & board::stone);
}

void board::moveStone(unsigned int xPos, unsigned int yPos, int xDeflection, int yDeflection) noexcept
{
	try {
		int& nextPos = mMap.at(yPos + yDeflection).at(xPos + xDeflection);
		int& curPos = mMap.at(yPos).at(xPos);
		if (nextPos == board::hole) {
			nextPos |= board::stone;
			--mGoal;
		}
		else {
			nextPos = board::stone;
		}
		if (curPos == board::stoneHole) {
			curPos &= ~board::stone;
			++mGoal;
		}
		else {
			curPos = board::empty;
		}
	}
	catch (const out_of_range& e) {
		return;
	}
}

void board::movePlayer(unsigned int xPos, unsigned int yPos, int xDeflection, int yDeflection) noexcept
{
	try {
		int& nextPos = mMap.at(yPos + yDeflection).at(xPos + xDeflection);
		int& curPos = mMap.at(yPos).at(xPos);
		if (nextPos == board::hole) {
			nextPos |= board::player;
		}
		else {
			nextPos = board::player;
		}
		if (curPos == board::playerHole) {
			curPos &= ~board::player;
		}
		else {
			curPos = board::empty;
		}
	}
	catch (const out_of_range& e) {
		return;
	}
}

bool board::isClear() const noexcept
{
	return mGoal == 0;
}