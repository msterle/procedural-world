#ifndef FILTER_H
#define FILTER_H

class Filter {
protected:
	unsigned int kernelSize;
	float kernel[];
public:
	Filter(kernelSize, float* kernel) : kernelSize(kernelSize), kernel(kernel) { }
	
}

#endif