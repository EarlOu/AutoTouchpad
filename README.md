# AutoTouchpad

Automatically disable touchpad when using keyboard with Ubuntu on ASUS UL30vt.

## Warning

Dirty code!!! This is my early project. Only tested on Ubuntu 10.04

## Description

There is no driver of the touchpad on ASUS UL30vt for Ubuntu, so existing touchpad controllers do not work.

## Compiling
	cd src
	g++ -o bin/autoTouchpad src/x11.cpp -lX11 -lXtst -lpthread
