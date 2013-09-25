// 3dlbaker
// Bakes colour corrections into a .3dl LUT
// lewis@lewissaunders.com
// TODO:
//   o accept 16 float input
//   o prettier pattern, preferably same as Nuke's
//   o accuracy down to 1 12-bit code value if possible

#include "half.h"

#ifdef __APPLE__
	#include "/usr/discreet/smoke_2013.2.53/sparks/spark.h"
#else
	#include "/usr/discreet/flame_2013.0.2/sparks/spark.h"
#endif

unsigned long *bam(int what, SparkInfoStruct si);
unsigned long *toggle(int what, SparkInfoStruct si);

SparkBooleanStruct SparkBoolean14 = {
	0,
	(char *) "Output test pattern",
	toggle
};

SparkStringStruct SparkString16 = {
	"/tmp/t.3dl",
	(char *) "Output to: %s",
	0,
	NULL
};

SparkPushStruct SparkPush30 = {
	(char *) "Write .3dl",
	bam
};

// Check a buffer is gonna work
int getbuf(int n, SparkMemBufStruct *b) {
	if(!sparkMemGetBuffer(n, b)) {
		printf("Failed to get buffer %d\n", n);
		return(0);
	}
	if(!(b->BufState & MEMBUF_LOCKED)) {
		printf("Failed to lock buffer %d\n", n);
		return(0);
	}
	return(1);
}

// Write the LUT
unsigned long *bam(int what, SparkInfoStruct si) {
	FILE *fd;
	const char *header = "#Tokens required by applications - do not edit\n"
						 "\n"
						 "3DMESH\n"
						 "Mesh 4 12\n"
						 "0 64 128 192 256 320 384 448 512 576 640 704 768 832 896 960 1023\n"
						 "\n";
	const char *footer = "\n" 
						 "#Tokens required by applications - do not edit\n"
						 "\n"
						 "LUT8\n"
						 "gamma 1.0\n"
						 "\n";

	fd = fopen(SparkString16.Value, "w");
	fprintf(fd, "%s", header);

	SparkMemBufStruct input;
	if(!getbuf(2, &input)) return(NULL);

	char *p;
	long i = 0;
	for(long y = 0; y < input.BufHeight; y++) {
		for(long x = 0; x < input.BufWidth; x++) {
			p = ((char *) input.Buffer) + y * input.Stride + x * input.Inc;

			unsigned short *r, *g, *b;
			r = (unsigned short *) p;
			g = (unsigned short *) (p + sizeof(short));
			b = (unsigned short *) (p + 2 * sizeof(short));

			fprintf(fd, "%d %d %d\n", *r / 16, *g / 16, *b / 16);

			i++;

			if(i >= (17 * 17 * 17)) {
				goto phew;
			}
		}
	}

	phew:
	fprintf(fd, "%s", footer);
	fclose(fd);
	return(NULL);
}

// Switch display
unsigned long *toggle(int what, SparkInfoStruct si) {
	sparkReprocess();
	return(NULL);
}

// Work
unsigned long *SparkProcess(SparkInfoStruct si) {
	SparkMemBufStruct result, input;

	if(!getbuf(1, &result)) return(NULL);
	if(!getbuf(2, &input)) return(NULL);

	if(SparkBoolean14.Value) {
		char *p;
		unsigned int steps[17] = {0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960, 1023};
		long i = 0;
		for(long y = 0; y < result.BufHeight; y++) {
			for(long x = 0; x < result.BufWidth; x++) {
				p = ((char *) result.Buffer) + y * result.Stride + x * result.Inc;

				unsigned short *r, *g, *b;
				r = (unsigned short *) p;
				g = (unsigned short *) (p + sizeof(short));
				b = (unsigned short *) (p + 2 * sizeof(short));

				*r = steps[(i / (17 * 17)) % 17] * (4095.0/1023.0) * 16;
				*g = steps[(i / (17 *  1)) % 17] * (4095.0/1023.0) * 16;
				*b = steps[(i / ( 1 *  1)) % 17] * (4095.0/1023.0) * 16;

				i++;
			}
		}
	} else {
		sparkCopyBuffer(input.Buffer, result.Buffer);
	}
	return(result.Buffer);
}

// Number of clips required
int SparkClips(void) {
	return(1);
}

// New memory interface to keep Batch happy
void SparkMemoryTempBuffers(void) {
}

// Module level, not desktop
unsigned int SparkInitialise(SparkInfoStruct si) {
	return(SPARK_MODULE);
}

// Bit depths yo
int SparkIsInputFormatSupported(SparkPixelFormat fmt) {
	switch(fmt) {
		case SPARKBUF_RGB_48_3x12:
			return(1);
		default:
			return(0);
	}
}

// Stop
void SparkUnInitialise(SparkInfoStruct sparkInfo) {
}
