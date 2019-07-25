#pragma once
#include <stack>
#include <memory>
#include "board.h"

typedef enum Direction {
	LEFT,
	RIGHT,
	UP,
	DOWN
}dir;

class player
{
private:
	player(void);
	virtual ~player() = default;
	bool _undo(std::stack<std::pair<dir, bool>>&, std::stack<std::pair<dir, bool>>&);
protected:
	unsigned int mXpos, mYpos;
	unsigned int mMoveCount;
	int mLevel;
	std::unique_ptr<board> mpBoard;
	std::stack<std::pair<dir, bool>> mBehaviorBackwardStack;
	std::stack<std::pair<dir, bool>> mBehaviorForwardStack;
	void setBoard();
public:
	static player& instance();
	void setLevel(unsigned int);
	unsigned int getLevel() const noexcept;
	unsigned int getMoveCount() const noexcept;
	bool move(dir);
	bool undo();
	bool redo();
	bool reset();
	bool isClear() noexcept;
};
