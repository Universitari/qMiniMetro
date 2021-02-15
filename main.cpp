#include <QApplication>
#include <QIcon>
#include "Game.h"


int main(int argc, char *argv[]) {

	QApplication app(argc, argv);

	QIcon icon(QPixmap(":/Graphics/icon.png"));
	app.setWindowIcon(icon);

	// create and show the game
	Game::instance()->show();

	// launch Qt event loop
	return app.exec();

}