#include  <X11/Xlib.h>
#include  <X11/extensions/record.h>
#include  <cstdlib>
#include   <cstdio>
#include  <iostream>
#include  <fstream>
#include  <ctime>
#include  <iomanip>
#include  <sstream>
#include  <string>
using namespace std;

#define     CTRL 37
#define     ESC 9
#define     WAIT 1


void* mouse_thread(void*);
void key_pressed_cb(XPointer a,XRecordInterceptData *d);
void* lock_thread(void*);

static Display* dpy=0;		// object for x11 extensions
static XRecordContext rc;	// object for x11 extensions

bool lock =false;	//whether the touchpad is close or not
bool keyusing=false;	//whether the keyboard is using 
bool mousein=false;	//whether there is an USB mouse in 
bool ctrl=false;	//whether ctrl is press
bool superlock=false;	//whether user lock the touchpad forcely



void turnOff(){
	system("xinput set-int-prop ImPS/2\\ Logitech\\ Wheel\\ Mouse \"Device Enabled\" 8 0");
}
void turnOn(){
	system("xinput set-int-prop ImPS/2\\ Logitech\\ Wheel\\ Mouse \"Device Enabled\" 8 1");
}

ofstream* getOutputFile();


int main(int argc, char *argv[])
{

   	ofstream* of = getOutputFile();
	//close the std IO  
	fclose(stdout);
	fclose(stdin);
	//turn on the touchpad initially
	turnOn();
	//x11 extensions record
	XRecordClientSpec rcs;
	XRecordRange* rr;
	dpy= XOpenDisplay(0);
	rr = XRecordAllocRange();	
	rr->device_events.first = KeyPress;
	rr->device_events.last = MotionNotify;
	rcs = XRecordAllClients;
	rc = XRecordCreateContext(dpy,0,&rcs,1,&rr,1);
	//thread for USB mouse in
	pthread_t t;
	pthread_create(&t,NULL,mouse_thread,NULL);
	pthread_detach(t);

	//wait for events
	XRecordEnableContext(dpy,rc,key_pressed_cb,NULL);
}


////////////////////////////////////////////
// function mouse_thread(void*)
// detect the USB mouse in and close the touchpad
/////////////////////////////////////////////


void* mouse_thread(void*){
	
	while(1){
		FILE* f=popen("xinput list|grep USB\\ OPTICAL\\ MOUSE","r");
		if(fgetc(f)==EOF)mousein=false;
		else mousein=true;
		pclose(f);
		if(mousein){
			turnOff();
		}else{
			if(!lock&&!superlock){			
				turnOn();
			}
		}
		sleep(1);
	}
	return NULL;
}


//////////////////////////////////////////////////////
// funciont  key_pressed_cb 
// when x11 extensions record evevts happen
//////////////////////////////////////////////////////

void key_pressed_cb(XPointer a,XRecordInterceptData *d){
	if(d->category != XRecordFromServer) return;
	unsigned char type =((unsigned char*)d->data)[0] & 0x7F;
	if(d->data_len==8){
		if(d->data[0]==2&&d->data[1]==CTRL){
			ctrl=true;
		}
		if(d->data[0]==3&&d->data[1]==CTRL){
			ctrl=false;
		}
		
		if(d->data[0]==2&&d->data[1]==ESC&&ctrl){
			if(mousein){
				superlock=false;
			}else{
				if(superlock){
					superlock=false;
				}else{
					superlock=true;
					turnOff();
				}
			}
		}
	}	
	if(type == KeyPress){
		pthread_t t;
		pthread_create(&t,NULL,lock_thread,NULL);
 		pthread_detach(t);
	}	
	return;
}




//////////////////////////////////////////////////////////////
//function lock_thread
//called when the events is a keyevent
////////////////////////////////////////////////////////////
void* lock_thread(void*){
	keyusing=true;
	if(lock)return NULL;
	turnOff();
	lock=true;
	keyusing=true;
	while(keyusing){
		keyusing=false;
		sleep(WAIT);
	}
	lock=false;
	if(!mousein&&!superlock){
		turnOn();
	}
	return NULL;
}


//////////////////////////////////////////////////////////
//function  getOutputFile
//determine the output log file name and make an ofstream object
//return the ofstream object pointer 
/////////////////////////////////////////////////////////


ofstream* getOutputFile(){
	time_t rawTime;
	time(&rawTime);
	struct tm* now = localtime(&rawTime);
	stringstream filename;
	filename<<now->tm_year+1900<<setfill('0')<<setw(2)<<now->tm_mon+1
						  <<setw(2)<<now->tm_mday
						  <<'_'
						  <<setw(2)<<now->tm_hour
						  <<setw(2)<<now->tm_min
						  <<setw(2)<<now->tm_sec
						  <<".log";
	
	ofstream* of=new ofstream(filename.str().c_str(),ios::out);
	return of;	
}
