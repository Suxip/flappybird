#include "GameController.hpp"
#include "MyDisplay.hpp"
#include "MyStation.hpp"
#include "mixr/graphics/Translator.hpp"
#include "mixr/graphics/Polygon.hpp"
#include "mixr/graphics/Page.hpp"
#include "mixr/base/Pair.hpp"
#include "mixr/base/PairStream.hpp"
#include <random>

IMPLEMENT_SUBCLASS(GameController, "GameController")
EMPTY_COPYDATA(GameController)
EMPTY_DELETEDATA(GameController)
EMPTY_SLOTTABLE(GameController)


//TODO: Improve on ball collision
//TODO: Blocks pop up behind the DefeatScreen 

GameController::GameController() {
	STANDARD_CONSTRUCTOR()
}
void GameController::updateData(const double dt) {
	BaseClass::updateData(dt);

	if (falling == true) {
		dropFish(dt);
		//score++;
	}

	if (getPage()){
		SendData scoreSD;
		SendData scoreSD2;
		SendData scoreSD3;
		
     	getPage()->send("score", UPDATE_VALUE, score, scoreSD);
		auto test2 = getPage()->findByName("DefeatScreen");
		auto test3 = getPage()->findByName("WinScreen");
		if (test2 != nullptr) {
			mixr::graphics::Polygon* DefeatScreen = dynamic_cast<mixr::graphics::Polygon*>(test2->object());
			if (DefeatScreen != nullptr) {getDefeatScreen()->send("score", UPDATE_VALUE, score, scoreSD2); }

		}
		if (test3 != nullptr) {
			mixr::graphics::Polygon* WinScreen = dynamic_cast<mixr::graphics::Polygon*>(test3->object());
			if (WinScreen != nullptr) { getWinScreen()->send("score", UPDATE_VALUE, score, scoreSD3); }

		}

	}

	current = getDisplay()->subpageName();
	
	if (hasStarted) ++frame;

	if (current == easy && std::fmod(frame, 200) == 0 && hasStarted) {
       std::cout << "we're in the easy mode";
       if (polyCount == 1) { addPoly(-1, 6); ++polyCount; }
       else if (polyCount == 2) { addPoly(-4, 6); ++polyCount; }
       else if (polyCount == 3) { addPoly(-8, 6); ++polyCount; }
       else if (polyCount == 4) { addPoly(1, 6); ++polyCount; }
	   if (score >= 4) {
		   
		   win();
	   }
	}
	else if (current == medium && std::fmod(frame, 160) == 0 && hasStarted) { // this is to increase the rate at which polygons are generated for the medium and hard levels
		std::cout << "we're in the medium mode";
		if (polyCount == 1) { addPoly(-4, 5); ++polyCount; }
		else if (polyCount == 2) { addPoly(-2, 5); ++polyCount; }
		else if (polyCount == 3) { addPoly(-6, 5); ++polyCount; }
		else if (polyCount == 4) { addPoly(-4, 5); ++polyCount; }  
		else if (polyCount == 5) { addPoly(-2, 5); ++polyCount; }
		if (score >= 5) {
			
			win();
		}
	}
	else if (current == hard && std::fmod(frame, 140) == 0 && hasStarted) {
		std::cout << "we're in the hard mode";
		if (polyCount == 1) { addPoly(-4, 5); ++polyCount; }
		else if (polyCount == 2) { addPoly(-2, 5); ++polyCount; }
		else if (polyCount == 3) { addPoly(-6, 5); ++polyCount; }
		else if (polyCount == 4) { addPoly(-2, 5); ++polyCount; }
		else if (polyCount == 5) { addPoly(-2, 5); ++polyCount; }
		else if (polyCount == 6) { addPoly(-4, 5); ++polyCount; }
		if (score >= 6) {
			win();
		}
	}
	else if (current == endless && std::fmod(frame, 160) == 0 && hasStarted) {
		// call addPoly each time with a different number. and don't stop
		// such that every time updateData runs and every 200 frames, 
		// a new polygon will be generated and will keep generating

		// generate a random number from -9 to 9

		std::cout << "we're in the endless mode";
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 gen(rd()); // seed the generator
		std::uniform_int_distribution<> distr(-9, 5); // define the range
		int randInt = distr(gen);
		addPoly(randInt, 6);
	}

	for (int i = 1; i < blockCount; ++i) {     // not blockCount + 1 because addPoly called first
		std::string slotname = "blocks" + std::to_string(i);
		mixr::base::String slot(slotname.c_str());
		mixr::base::Pair* pair = getDisplay()->subpage()->findByName(slot);
		/*moveBlock(pair, i);*/
		if (current == easy) moveBlock(pair, -0.05);
		else if (current == medium || current == endless) moveBlock(pair, -0.07);
		else if (current == hard) moveBlock(pair, -0.09);

		if (i % 2 != 0) {

			if (collisionCheck(pair)) {
				

			}
		}

	}
}


void GameController::reset() {
	BaseClass::reset();

	if (getPage()) {
		if (getFish()) {
			getFish()->setYPos(0);
			getFish()->setVisibility(1);
			
		}
		getDefeatScreen()->setVisibility(0);
		getWinScreen()->setVisibility(0);

		deletePoly();
			hasStarted = false;
			jumpAllowed = true;
			score = 0;
			blockCount = 1;
			frame = -1;
			polyCount = 1;

	}
}

/* Creates a pair of blocks (top and bottom) and adds them to the current subpage.
blockCount is increased such that slot name of each block is different, starting
from block1 and increasing by number to block2, block3, etc. */
void GameController::addPoly(int height, int gap) {
	mixr::graphics::Translator* translator1 = new mixr::graphics::Translator();
	mixr::graphics::Translator* translator2 = new mixr::graphics::Translator();

	mixr::graphics::Polygon* polygon1 = new mixr::graphics::Polygon();
	mixr::graphics::Polygon* polygon2 = new mixr::graphics::Polygon();

	mixr::base::Identifier* color = new mixr::base::Identifier("blue");

	polygon1->setColor(color);
	polygon2->setColor(color);

	// bottom block
	mixr::base::Vec3d vertices1[4] = {
		mixr::base::Vec3d(13, height, 0),  // Top-right corner
		mixr::base::Vec3d(10, height, 0),  // Top-left corner
		mixr::base::Vec3d(10, -10, 0),   // Bottom-left corner 
		mixr::base::Vec3d(13, -10, 0)    // Bottom-right corner
	};

	// top block
	mixr::base::Vec3d vertices2[4] = {
		mixr::base::Vec3d(13, 10, 0),  // Top-right corner
		mixr::base::Vec3d(10, 10, 0),  // Top-left corner
		mixr::base::Vec3d(10, height + gap, 0),   // Bottom-left corner
		mixr::base::Vec3d(13, height + gap, 0)    // Bottom-right corner
	};

	polygon1->setVertices(vertices1, 4); //block 1
	polygon2->setVertices(vertices2, 4);

	std::string polyName1 = "poly"; //+ std::to_string(blockCount);
	mixr::base::String polySlot1(polyName1.c_str());
	mixr::base::Pair* polyPair1 = new mixr::base::Pair(polySlot1, polygon1);

	translator1->addComponent(polyPair1);

	std::string slotname1 = "blocks" + std::to_string(blockCount);
	mixr::base::String slot1(slotname1.c_str());
	mixr::base::Pair* pair1 = new mixr::base::Pair(slot1, translator1);

	getDisplay()->subpage()->addComponent(pair1);

	++blockCount; //blockCount = 2

	std::string polyName2 = "poly"; /*+ std::to_string(blockCount);*/
	mixr::base::String polySlot2(polyName2.c_str());
	mixr::base::Pair* polyPair2 = new mixr::base::Pair(polySlot2, polygon2);

	translator2->addComponent(polyPair2);

	std::string slotname2 = "blocks" + std::to_string(blockCount); // must increase again for the next iteration
	mixr::base::String slot2(slotname2.c_str());
	mixr::base::Pair* pair2 = new mixr::base::Pair(slot2, translator2);

	getDisplay()->subpage()->addComponent(pair2);

	++blockCount; //blockCount = 3

}

void GameController::moveBlock(mixr::base::Pair* pair, float movement) {
	if (pair != nullptr && hasStarted == true) {
		mixr::graphics::Translator* block = dynamic_cast<mixr::graphics::Translator*>(pair->object());
		block->setXPos(block->getXPos() + movement);

		mixr::base::PairStream* components = getDisplay()->subpage()->getComponents();

		if (block->getXPos() + 1.5 <= -23) {
			components->remove(pair);

		}
	}
}

void GameController::win() {
	falling = false;
	hasStarted = false;
	getWinScreen()->setVisibility(1);
	getFish()->setVisibility(0);
	jumpAllowed = false;
}

void GameController::crash(){
	falling = false;
	hasStarted = false;
	getDefeatScreen()->setVisibility(1);
	getFish()->setVisibility(0);
	jumpAllowed = false;
}

bool GameController::Jump(const double dt) {
	if (jumpAllowed == true) {
		falling = true;
		hasStarted = true;

		double fishY = getFish()->getYPos();
		if (fishY >= 8.5) {
			vy = 1;

		}
		else {
			vy = 5;
		}
		if (fishY >= 9.5) {
			getFish()->setYPos(9.5);
		}
		return true;

	}
	
}

bool GameController::dropFish(const double dt) {
	falling = true;
	if (jumpAllowed == true) {
		double fishY = getFish()->getYPos();
		if (fishY < -8.5) {
			crash();

		}
		vy -= 0.4;
		getFish()->setYPos(fishY + vy * dt);
		return true;

	}
	
}

bool GameController::collisionCheck(mixr::base::Pair* pair) {
	double fishY = getFish()->getYPos();
	double fishYTop = fishY + 1;
	double fishYBottom = fishY - 1;
	double fishXRight = -9; //-7
	double fishXLeft = -11; //-10

	double blockXRight;
	double blockXLeft;

	double blockY;
	double blockYMin;



	if (pair != nullptr) {
		mixr::graphics::Translator* block = dynamic_cast<mixr::graphics::Translator*>(pair->object());

		//std::cout << block->findByIndex(1) << std::endl;
		auto* poly = dynamic_cast<mixr::graphics::Polygon*>(block->findByIndex(1)->object());
		poly->getVertices();
		blockY = poly->getVertices()->y();

		blockXLeft = block->getXPos();
		blockXRight = block->getXPos() + 3;

		
		if (fishXRight > blockXLeft && fishXRight < blockXRight || fishXLeft > blockXLeft && fishXLeft < blockXRight) { //switch this with the blockCount check

			if (fishYBottom < blockY || fishYTop >(blockY + 6)) {
				
				crash();
				return true;
			}
			
		}
		
		else if (blockXRight < -11 && blockXRight > -11.05) {
			score++;
		}
	}
	return false;
}


//Helper Functions
mixr::graphics::Polygon* GameController::getDefeatScreen() {

	auto test = getPage()->findByName("DefeatScreen");
	mixr::graphics::Polygon* DefeatScreen = dynamic_cast<mixr::graphics::Polygon*>(test->object());
	if (DefeatScreen != nullptr) {
		return DefeatScreen;
	}
}

mixr::graphics::Polygon* GameController::getWinScreen() {

	auto test = getPage()->findByName("WinScreen");
	mixr::graphics::Polygon* WinScreen = dynamic_cast<mixr::graphics::Polygon*>(test->object());
	if (WinScreen != nullptr) {
		return WinScreen;
	}

}


//Helper function to get page
mixr::graphics::Page* GameController::getPage() {
	MyStation* station = dynamic_cast<MyStation*>(container());

	MyDisplay* display = dynamic_cast<MyDisplay*>(station->getDisplay());
	if (display != nullptr) {
		mixr::graphics::Page* page = dynamic_cast<mixr::graphics::Page*>(display->subpage());
		if (page != nullptr) {
			return page;
		}

	}
}

//Helper function to get the fish
mixr::graphics::Translator* GameController::getFish() {
	auto test = getPage()->findByName("fish");
	mixr::graphics::Translator* fish = dynamic_cast<mixr::graphics::Translator*>(test->object());
	return fish;
}

mixr::graphics::Display* GameController::getDisplay() {
	MyStation* station = dynamic_cast<MyStation*>(container());
	if (station != nullptr) {
		MyDisplay* display = dynamic_cast<MyDisplay*>(station->getDisplay());
		return display;

	}
}


void GameController::deletePoly() {

	mixr::base::PairStream* components = getDisplay()->subpage()->getComponents();

	for (int i = 1; i < blockCount; ++i) {
		std::cout << "blockCount:" << blockCount << std::endl;
		std::string slotname = "blocks" + std::to_string(i);
		mixr::base::String slot(slotname.c_str());
		mixr::base::Pair* pair = getDisplay()->subpage()->findByName(slot);
		components->remove(pair);
	}
}
