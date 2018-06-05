#include "midiplayer.hpp"
#include <QApplication>
#include <QStyle>
#include <QStyleFactory>


int main(int argc, char *argv[])
{
  // TODO
	QApplication app(argc, argv);

	MIDIPlayer midi_p;

	midi_p.show();
	
	app.setStyle(QStyleFactory::create("Fusion"));
	
	return app.exec();
	
}


