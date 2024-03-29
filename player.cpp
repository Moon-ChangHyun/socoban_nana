#include "player.h"
#include <iostream>
using namespace std;

const std::array<nana::point, 4> player::deflection = { {{-1, 0}, {1, 0}, {0, -1}, {0, 1}} };

player::player() : mXpos(0), mYpos(0), mMoveCount(0), mLevel(-1), mpBoard(nullptr) {}

player& player::instance()
{
	static player sInstance;
	return sInstance;
}

void player::setLevel(unsigned int inLevel)
{
	if (mLevel != inLevel) {
		mLevel = inLevel;
		setBoard();
	}
}

unsigned int player::getLevel() const noexcept{
	return mLevel;
}

void player::setBoard()
{
	while (!mBehaviorBackwardStack.empty()) mBehaviorBackwardStack.pop();
	while (!mBehaviorForwardStack.empty()) mBehaviorForwardStack.pop();
	mpBoard = make_unique<board>(mLevel);
	mXpos = mpBoard->getStartX();
	mYpos = mpBoard->getStartY();
	mMoveCount = 0;
	board::sDrawing->update();
}

unsigned int player::getMoveCount() const noexcept
{
	return mMoveCount;
}

bool player::move(dir dir)
{
	if (mpBoard == nullptr) return false;
	int dx = deflection[dir].x;
	int dy = deflection[dir].y;
	if (!mpBoard->isThereWall(mXpos + dx, mYpos + dy)) {
		bool isThereStone = mpBoard->isThereStone(mXpos + dx, mYpos + dy);
		if (isThereStone) {
			if (mpBoard->isThereWall(mXpos + (dx << 1), mYpos + (dy << 1)) || mpBoard->isThereStone(mXpos + (dx << 1), mYpos + (dy << 1))) {
				return false;
			}
			mpBoard->moveStone(mXpos + dx, mYpos + dy, dx, dy);
		}
		mpBoard->movePlayer(mXpos, mYpos, dx, dy);
		mXpos += dx;
		mYpos += dy;
		while (!mBehaviorForwardStack.empty()) mBehaviorForwardStack.pop();
		mBehaviorBackwardStack.push({ dir, isThereStone });
		++mMoveCount;
		board::sDrawing->update();
		return true;
	}
	return false;
}

bool player::_undo()
{
	if (mpBoard == nullptr) return false;
	if (mBehaviorBackwardStack.empty()) return false;
	const pair<dir, bool> behavior = mBehaviorBackwardStack.top();
	mBehaviorBackwardStack.pop();
	int dx = deflection[behavior.first].x;
	int dy = deflection[behavior.first].y;
	mpBoard->movePlayer(mXpos, mYpos, -dx, -dy);
	if (behavior.second) {
		mpBoard->moveStone(mXpos + dx, mYpos + dy, -dx, -dy);
	}
	mXpos -= dx;
	mYpos -= dy;
	mBehaviorForwardStack.push(behavior);
	--mMoveCount;
	return true;
}

bool player::undo()
{
	bool ret = _undo();
	if (ret) board::sDrawing->update();
	return ret;
}

bool player::_redo()
{
	if (mpBoard == nullptr) return false;
	if (mBehaviorForwardStack.empty()) return false;
	const pair<dir, bool> behavior = mBehaviorForwardStack.top();
	mBehaviorForwardStack.pop();
	int dx = deflection[behavior.first].x;
	int dy = deflection[behavior.first].y;
	if (behavior.second) {
		mpBoard->moveStone(mXpos + dx, mYpos + dy, dx, dy);
	}
	mpBoard->movePlayer(mXpos, mYpos, dx, dy);
	mXpos += dx;
	mYpos += dy;
	mBehaviorBackwardStack.push(behavior);
	++mMoveCount;
	return true;
}

bool player::redo()
{
	bool ret = _redo();
	if (ret) board::sDrawing->update();
	return ret;
}

bool player::reset()
{
	if (mpBoard == nullptr) return false;
	if (mBehaviorBackwardStack.empty() && mBehaviorForwardStack.empty()) return false;
	while (_undo());
	while (!mBehaviorForwardStack.empty()) mBehaviorForwardStack.pop();
	board::sDrawing->update();
	return true;
}

bool player::isClear() noexcept
{
	if (mpBoard == nullptr) return false;
	return mpBoard->isClear();
}