#include "MyDisplay.hpp"
#include "MyStation.hpp"
#include "GameController.hpp"
#include "mixr/base/Pair.hpp"
#include "mixr/base/PairStream.hpp"
#include "mixr/graphics/Polygon.hpp"
#include "mixr/graphics/Translator.hpp"
#include "mixr/graphics/BmpTexture.hpp"
#include "mixr/graphics/Page.hpp"
#include <cmath>
#include <random>

IMPLEMENT_SUBCLASS(MyDisplay, "MyDisplay")
EMPTY_DELETEDATA(MyDisplay)
EMPTY_SLOTTABLE(MyDisplay)
EMPTY_COPYDATA(MyDisplay)

BEGIN_EVENT_HANDLER(MyDisplay)
    ON_EVENT(SPACE_BAR, onSpaceBar)
END_EVENT_HANDLER()


MyDisplay::MyDisplay() {
    STANDARD_CONSTRUCTOR()
}

void MyDisplay::updateData(const double dt)
{
	BaseClass::updateData(dt);

	mixr::base::Pair* obj1 = subpage()->findByName("floor");

	if (obj1 != nullptr) {
		mixr::graphics::Polygon* poly1 = dynamic_cast<mixr::graphics::Polygon*>(obj1->object());
		poly1->setTexture(count); 
	}

	mixr::base::Pair* obj2 = subpage()->findByName("sand");

	if (obj2 != nullptr) {
		mixr::graphics::Polygon* poly2 = dynamic_cast<mixr::graphics::Polygon*>(obj2->object());
		if (count % 2 == 0) {
			poly2->setTexture(9);
		}
		else {
			poly2->setTexture(10);
		}
	}

    // Not sure if we need to do this?
   getController()->updateData(dt);


}

/* Each time the user switches back to the home page, everything must be reset in the game. 
The fish must be back at the starting point, the blocks are not created yet, and the score 
is 0. */
void MyDisplay::reset() {
    BaseClass::reset();

    getController()->reset();
  
}

/* Triggers changes between pages as a result of button events. */
void MyDisplay::buttonEvent(const int b) {
    switch (b) {
    case 3001: //Play Button
        newSubpage("gameScreen", nullptr);
        break;
    case 3002: //Difficulty Button
        newSubpage("levelTest", nullptr);
        break;
    case 3003: //Easy Level
        newSubpage("gameScreen", nullptr);
        break;
    case 3004: //Medium Level
        newSubpage("Medium", nullptr);
        break;
    case 3005: //Hard Level
        newSubpage("Hard", nullptr);
        break;
    case 3006: //Endless Level
        newSubpage("endlessLevel", nullptr);
        break;
    case 3007: //GameScreen Back to Home Button
        newSubpage("startScreen", nullptr);
        reset();
        break;
    case 3008: //Try Again Button
        newSubpage("startScreen", nullptr);
        reset();
        break;
    case 3009: //Home Button on Defeat Screen
        newSubpage("startScreen", nullptr);
        reset();
        break;
    }
}

bool MyDisplay::onSpaceBar() {
    //getController();
    //Put 0.1 as filler numbers. Want to go back to change these. Might not be needed at all
    getController()->Jump(0.1);
    getController()->dropFish(0.1);
   // hasStarted = true;
    return true;
}

GameController* MyDisplay::getController(){
    MyStation* station = dynamic_cast<MyStation*>(container());
    if (station != nullptr) {
        GameController* controller = dynamic_cast<GameController*>(station->findByName("gameController")->object());
        if (controller != nullptr) {
            return controller;
        }
    }
}
