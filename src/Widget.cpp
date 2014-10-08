#include "Widget.h"

unsigned long Widget::s_id = 0;

Widget::Widget() {
	// So that there can never be a widget with id 0
	id = ++s_id;
}