#include <QApplication>
#include "Game.h"

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);

	// Sounds::instance();

	// create and show the game
	Game::instance()->show();

	// launch Qt event loop
	return app.exec();

}