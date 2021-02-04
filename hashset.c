#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(elemSize > 0);
	assert(numBuckets > 0);
	assert(hashfn != NULL && comparefn != NULL);
	h->bucketSize = numBuckets;
	h->elementSize = elemSize;
	h->bucket = malloc(sizeof(vector) * numBuckets);
	for (int i = 0; i < numBuckets; i++) {
		VectorNew((char*)h->bucket + (i*sizeof(vector)), elemSize, freefn, 0);
	}
	h->hashFreeFn = freefn;
	h->hashSetFn = hashfn;
	h->HashCompareFn = comparefn;
	h->totalElements = 0;
}

void HashSetDispose(hashset *h)
{
	for (int i = 0; i < h->bucketSize; i++) {
		VectorDispose((char*)h->bucket + (i * sizeof(vector)));
	}
	free(h->bucket);
	h->totalElements = 0;
}

int HashSetCount(const hashset *h)
{ return h->totalElements; }

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
	assert(mapfn != NULL);
	for (int i = 0; i < h->bucketSize; i++) {
		VectorMap((char*)h->bucket + (i * sizeof(vector)),mapfn,auxData);
	}
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
	assert(elemAddr != NULL);
	int hash = h->hashSetFn(elemAddr, h->bucketSize);
	assert(hash >= 0 && hash < h->bucketSize);
	int elemPos = VectorSearch((char*)h->bucket + (hash * sizeof(vector)), elemAddr, h->HashCompareFn, 0, false);
	if (elemPos == -1) {
		VectorAppend((char*)h->bucket + (hash * sizeof(vector)), elemAddr);
		h->totalElements += 1;
	}
	else {
		VectorReplace((char*)h->bucket + (hash * sizeof(vector)), elemAddr, elemPos);
	}
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{
	assert(elemAddr != NULL);
	int hash = h->hashSetFn(elemAddr, h->bucketSize);
	assert(hash >= 0 && hash < h->bucketSize);
	int elemPos = VectorSearch((char*)h->bucket + (hash * sizeof(vector)), elemAddr, h->HashCompareFn, 0, false);
	if (elemPos != -1) {
		void* found = VectorNth((char*)h->bucket + (hash * sizeof(vector)), elemPos);
		return found;
	}
	return NULL; 
}
