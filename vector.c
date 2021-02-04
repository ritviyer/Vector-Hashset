#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
	assert(initialAllocation >= 0);
	if (initialAllocation == 0) {
		initialAllocation = 5;
	}
	v->arr = malloc(elemSize * initialAllocation);
	v->allocatedLength = initialAllocation;
	v->initialAllocation = initialAllocation;
	v->elementSize = elemSize;
	v->logicalLength = 0;
	v->freeElement = freeFn;
}

void VectorDispose(vector *v)
{
	if (v->freeElement != NULL) {
		for (int i = 0; i < v->logicalLength; i++) {
			void* temp = (char*)v->arr + (i * v->elementSize);
			v->freeElement(temp);
		}
	}
	v->logicalLength = 0;
	free(v->arr);
}

int VectorLength(const vector *v)
{ 
	return v->logicalLength; 
}

void *VectorNth(const vector *v, int position)
{ 
	assert(position >= 0 && position < v->logicalLength);
	void* temp = (char*)v->arr + (position * v->elementSize);
	return temp;
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
	assert(position >= 0 && position < v->logicalLength);
	void* temp = (char*)v->arr + (position * v->elementSize);
	if (v->freeElement != NULL) {
		v->freeElement(temp);
	}
	memcpy(temp, elemAddr, v->elementSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
	assert(position >= 0 && position <= v->logicalLength);
	if (v->logicalLength == v->allocatedLength) {
		v->allocatedLength += v->initialAllocation;
		v->arr = realloc(v->arr, (v->elementSize * v->allocatedLength));
	}
	void* temp = (char*)v->arr + (position * v->elementSize);
	if (position != v->logicalLength) {
		void* dest = (char*)temp + v->elementSize;
		int size = (v->logicalLength - position) * v->elementSize;
		memmove(dest, temp, size);
	}
	memcpy(temp, elemAddr, v->elementSize);
	v->logicalLength += 1;
}

void VectorAppend(vector *v, const void *elemAddr)
{
	if (v->logicalLength == v->allocatedLength) {
		v->allocatedLength += v->initialAllocation;
		v->arr = realloc(v->arr, (v->elementSize * v->allocatedLength));
	}
	void* temp = (char*)v->arr + (v->logicalLength * v->elementSize);
	memcpy(temp, elemAddr, v->elementSize);
	v->logicalLength += 1;
}

void VectorDelete(vector *v, int position)
{
	assert(position >= 0 && position < v->logicalLength);
	void* temp = (char*)v->arr + (position * v->elementSize);
	if (v->freeElement != NULL) {
		v->freeElement(temp);
	}
	if (position != v->logicalLength - 1) {
		void* src = (char*)temp + v->elementSize;
		int size = (v->logicalLength - (position+1)) * v->elementSize;
		memmove(temp, src, size);
	}
	v->logicalLength -= 1;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
	assert(compare != NULL);
	qsort(v->arr, v->logicalLength, v->elementSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
	assert(mapFn != NULL);
	for (int i = 0; i < v->logicalLength; i++) {
		void* temp = (char*)v->arr + (i * v->elementSize);
			mapFn(temp, auxData);
	}
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{
	assert(startIndex >= 0 && startIndex <= v->logicalLength);
	assert(key != NULL && searchFn != NULL);
	void* base = (char*)v->arr + (startIndex * v->elementSize);
	int size = v->logicalLength - startIndex;
	if (isSorted) {
		void* found = bsearch(key, base, size, v->elementSize, searchFn);
		if (found != NULL) {
			int elemPos = abs((char*)v->arr - (char*)found) / v->elementSize;
			return elemPos;
		}
	}
	else {
		for (int i = startIndex; i < v->logicalLength; i++) {
			void* temp = (char*)v->arr + (i * v->elementSize);
			int found = searchFn(temp, key);
			if (found == 0) {
				return i;
			}
		}
	}
	return -1; 
} 
