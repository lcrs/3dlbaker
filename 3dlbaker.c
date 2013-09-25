// 3dlbaker
// Bakes colour corrections into a .3dl LUT
// lewis@lewissaunders.com

#include "half.h"

#ifdef __APPLE__
	#include "/usr/discreet/smoke_2013.2.53/sparks/spark.h"
#else
	#include "/usr/discreet/flame_2013.0.2/sparks/spark.h"
#endif

unsigned long *bam(int i, SparkInfoStruct si);
unsigned long *toggle(int i, SparkInfoStruct si);

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
unsigned long *bam(int i, SparkInfoStruct si) {
	int fd;
	const char *header = "#Tokens required by applications - do not edit\n"
						 "3DMESH\n"
						 "Mesh 4 12\n"
						 "0 64 128 192 256 320 384 448 512 576 640 704 768 832 896 960 1023\n"
						 "\n";
	const char *footer = "#Tokens required by applications - do not edit\n"
						 "\n"
						 "LUT8\n"
						 "gamma 1.0\n"
						 "\n";

	fd = open(SparkString16.Value, O_CREAT | O_WRONLY | O_TRUNC, 0000666);
	write(fd, header, strlen(header));
	write(fd, footer, strlen(footer));
	close(fd);
	return(NULL);
}

// Switch display
unsigned long *toggle(int i, SparkInfoStruct si) {
	sparkReprocess();
	return(NULL);
}

// Work
unsigned long *SparkProcess(SparkInfoStruct si) {
	SparkMemBufStruct result, input;

	if(!getbuf(1, &result)) return(NULL);
	if(!getbuf(2, &input)) return(NULL);

	if(SparkBoolean14.Value) {
		unsigned long i = 0;
		char *p;
		for(int y = 0; y < result.BufHeight; y++) {
			for(int x = 0; x < result.BufWidth; x++) {
				p = ((char *) result.Buffer) + y * result.Stride + x * result.Inc;

				unsigned int *r, *g, *b;
				r = (unsigned int *) p;
				g = (unsigned int *) (p + sizeof(short));
				b = (unsigned int *) (p + 2 * sizeof(short));
				
				*r = x << 4;
				*g = y << 4;
				*b = 4095 << 4;

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
