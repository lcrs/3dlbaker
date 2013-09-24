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

SparkBooleanStruct SparkBoolean14 = {
	0,
	(char *) "Output test pattern",
	NULL
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
	fd = open(SparkString16.Value, O_CREAT | O_WRONLY | O_TRUNC, 0000666);
	write(fd, "hi", 2);
	close(fd);
	return(NULL);
}

// Work
unsigned long *SparkProcess(SparkInfoStruct si) {
	SparkMemBufStruct result, input;

	if(!getbuf(1, &result)) return(NULL);
	if(!getbuf(2, &input)) return(NULL);

	sparkCopyBuffer(input.Buffer, result.Buffer);
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
		case SPARKBUF_RGB_48_3x16_FP:
			return(1);
		default:
			return(0);
	}
}

// Stop
void SparkUnInitialise(SparkInfoStruct sparkInfo) {
}
