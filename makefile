all:	src/autoTouchpad.cpp
	g++ -o bin/autoTouchpad src/autoTouchpad.cpp -lX11 -lXtst -lpthread

cp:	bin/autoTouchpad
	cp bin/autoTouchpad ~/.autoTouchpad


