#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>

/* function declaration */
static void die(const char *errstr, ...);
static void run();
static void setup();
static void usage();

/* variables */
static Display *dpy;
static XDevice *dev;
static Atom prop;

/* function implementations */
void
die(const char *errstr, ...) {
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

void
run() {
	Atom type;
	int format;
	unsigned char *data;
	unsigned long ntimes, bytes_after;

	XGetDeviceProperty(dpy, dev, prop, 0, 1, False, AnyPropertyType, &type,
			&format, &ntimes, &bytes_after, &data);
	data[0] = !data[0];
	XChangeDeviceProperty(dpy, dev, prop, type, format, PropModeReplace, data,
			ntimes);
	XFree(data);
	XFlush(dpy);
}

void
setup() {
	Atom type = XInternAtom(dpy, XI_TOUCHPAD, True);
	Atom *props;
	int ndevs = 0;
	int nprops = 0;
	XDeviceInfo *info = XListInputDevices(dpy, &ndevs);

	while(ndevs--) {
		if(info[ndevs].type == type) {
			dev = XOpenDevice(dpy, info[ndevs].id);
			if(!dev) {
				XFreeDeviceList(info);
				die("syntog: cannot open device '%s'\n",
						info[ndevs].name);
			}
			props = XListDeviceProperties(dpy, dev, &nprops);
			if(!props || !nprops) {
				XCloseDevice(dpy, dev);
				XFree(props);
				XFreeDeviceList(info);
				die("syntog: no properties on device '%s'\n",
						info[ndevs].name);
			}
			while(nprops--) {
				if(props[nprops] == prop)
					break;
			}
			if(!nprops) {
				XCloseDevice(dpy, dev);
				XFree(props);
				XFreeDeviceList(info);
				die("syntog: no synaptics properties on device '%s'\n",
						info[ndevs].name);
			}
			break;
		}
	}
}

void
usage() {
	fprintf(stderr, "usage: syntog [-v]\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char **argv) {
	if((argc == 2) && !strcmp("-v", argv[1]))
		die("syntog-%s, © 2012 Peter A. Shevtsov\n", VERSION);
	else if(argc != 1)
		usage();
	if(!(dpy = XOpenDisplay(NULL))) {
		die("syntog: cannot open display\n");
	}
	if(!(prop = XInternAtom(dpy, "Synaptics Off", True))) {
		die("syntog: synaptics property not available");
	}
	setup();
	run();
	XCloseDevice(dpy, dev);
	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}
