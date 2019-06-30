#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <vector>
#include <algorithm>
#include "player.h"

using namespace nana;
using namespace std;

int WinMain(HINSTANCE hInstance, HINSTANCE preInstance, LPSTR lpCmdLine, int nCmdShow) {
	array<string, 8> imageName = { "whiteSpace.bmp", "player.bmp", "hole.bmp", "playerHole.bmp", "stone.bmp", "dummy.bmp", "stoneHole.bmp", "wall.bmp" };
	int stageNumber;

	ifstream inputStream("map.data");
	if (inputStream.fail()) {
		cout << "Can't find file : ./map.data" << endl;
		return 1;
	}
	array<char, board::maxSize> buffer;
	while (!inputStream.eof()) {
		inputStream.getline(buffer.data(), buffer.size());
		if (buffer[0] == '<') {
			vector<vector<int>> map;
			while (true) {
				inputStream.getline(buffer.data(), buffer.size());
				if (buffer[0] == '>') break;
				int i = 0;
				vector<int> line;
				line.reserve(board::maxSize);
				while (i < 50 && buffer[i] != 0) {
					line.push_back(buffer[i] & 0xF);
					buffer[i++] = 0;
				}
				map.push_back(move(line));
				buffer[0] = 0;
			}
			board::sMap.push_back(move(map));
			buffer[0] = 0;
		}
	}
	inputStream.close();

	paint::graphics graph;
	for (int i = 0; i != 8; ++i) {
		try {
			board::imageAsset[i] = paint::image{ imageName[i] };
		}
		catch (...) {
			return 1;
		}
		rectangle srcRect{ {0, 0}, board::imageAsset[i].size() };
		rectangle dstRect{ {(int)(board::bmSize * i), 0}, {board::bmSize, board::bmSize} };
		board::imageAsset[i].stretch(srcRect, graph, dstRect);
	}
	
	stageNumber = board::sMap.size();

	form fm{ API::make_center(1100, 875), appear::decorate<appear::taskbar>() };
	fm.caption(L"Socoban");
	fm.bgcolor(color(0x2E, 0xFE, 0xC8));
	label lbLevel{ fm, rectangle{875, 20, 225, 20} };
	lbLevel.caption("<bold size=10>level</>");
	lbLevel.format("true");
	lbLevel.text_align(align::center);
	button pprevBtn{ fm, rectangle{925, 40, 20, 20} };
	pprevBtn.caption("<<");
	button prevBtn{ fm, rectangle{950, 40, 15, 20} };
	prevBtn.caption("<");
	label lbLevelNum{ fm, rectangle{965, 40, 40, 20} };
	lbLevelNum.caption("<bold size=10>0</>");
	lbLevelNum.format("true");
	lbLevelNum.text_align(align::center);
	button nextBtn{ fm, rectangle{1005, 40, 15, 20} };
	nextBtn.caption(">");
	button nnextBtn{ fm, rectangle{1025, 40, 20, 20} };
	nnextBtn.caption(">>");
	label lbUsage{ fm, rectangle{875, 400, 225, 100} };
	lbUsage.caption(L"<bold size=10>Control\narrow : move\nu : undo\nr : redo\nb : restart\nESC : quit</>");
	lbUsage.format(true);
	lbUsage.text_align(align::center);
	label lbCnt{ fm,  rectangle{875, 800, 225, 20} };
	lbCnt.caption(L"<bold size=10>Move Count</>");
	lbCnt.format(true);
	lbCnt.text_align(align::center);
	label lbCntNum{ fm, rectangle{875, 820, 225, 20} };
	lbCntNum.caption("<bold size=10>0</>");
	lbCntNum.text_align(align::center);
	lbCntNum.format(true);
	board::sDrawing = make_unique<drawing>(fm);
	
	player& p = player::instance();
	p.setLevel(0);

	pprevBtn.events().click([&](const nana::arg_click& click) {
		p.setLevel(max(p.getLevel() - 10, 0));
		lbLevelNum.caption("<bold size=10>"s + std::to_string(p.getLevel()) + "</>"s);
		lbCntNum.caption("<bold size=10>"s + std::to_string(p.getMoveCount()) + "</>"s);
	});
	prevBtn.events().click([&](const nana::arg_click& click) {
		p.setLevel(max(p.getLevel() - 1, 0));
		lbLevelNum.caption("<bold size=10>"s + std::to_string(p.getLevel()) + "</>"s);
		lbCntNum.caption("<bold size=10>"s + std::to_string(p.getMoveCount()) + "</>"s);
	});
	nextBtn.events().click([&](const nana::arg_click& click) {
		p.setLevel(min(p.getLevel() + 1, stageNumber - 1));
		lbLevelNum.caption("<bold size=10>"s + std::to_string(p.getLevel()) + "</>"s);
		lbCntNum.caption("<bold size=10>"s + std::to_string(p.getMoveCount()) + "</>"s);
	});
	nnextBtn.events().click([&](const nana::arg_click& click) {
		p.setLevel(min(p.getLevel() + 10, stageNumber - 1));
		lbLevelNum.caption("<bold size=10>"s + std::to_string(p.getLevel()) + "</>"s);
		lbCntNum.caption("<bold size=10>"s + std::to_string(p.getMoveCount()) + "</>"s);
	});

	fm.events().focus([&fm](const nana::arg_focus& arg) {
		if (!arg.getting)
			fm.focus();
	});

	fm.events().key_char([&](const nana::arg_keyboard& arg) {
		bool stateChanged = false;
		switch (arg.key) {
		case 'u':
		case 'U':
			stateChanged = p.undo();
			break;
		case 'r':
		case 'R':
			stateChanged = p.redo();
			break;
		case 'b':
		case 'B':
			stateChanged = p.reset();
			break;
		}
		if (stateChanged) {
			lbCntNum.caption("<bold size=10>"s + std::to_string(p.getMoveCount()) + "</>"s);
			API::update_window(fm);
		}
	});

	fm.events().key_press([&](const nana::arg_keyboard& arg) {
		bool stateChanged = false;
		switch (arg.key) {
		case keyboard::os_arrow_left:
			stateChanged = p.move(dir::LEFT);
			break;
		case keyboard::os_arrow_right:
			stateChanged = p.move(dir::RIGHT);
			break;
		case keyboard::os_arrow_up:
			stateChanged = p.move(dir::UP);
			break;
		case keyboard::os_arrow_down:
			stateChanged = p.move(dir::DOWN);
			break;
		case keyboard::escape:
			fm.close();
			return;
		}
		if (stateChanged) {
			if (p.isClear()) {
				if (p.getLevel() == stageNumber - 1) {
					msgbox info("Info"s);
					info << L"It's the last stage.";
					info.show();
				}
				else {
					p.setLevel(p.getLevel() + 1);
					lbLevelNum.caption("<bold size=10>"s + std::to_string(p.getLevel()) + "</>"s);
				}
			}
			lbCntNum.caption("<bold size=10>"s + std::to_string(p.getMoveCount()) + "</>"s);
		}
		API::update_window(fm);
	});

	fm.show();
	nana::exec();
	return 0;
}
