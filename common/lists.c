#include <common.h>
#include <malloc.h>
#include <lists.h>

#define MAX(a,b) 	(((a)>(b)) ? (a) : (b))
#define MIN(a,b) 	(((a)<(b)) ? (a) : (b))
#define CAT4CHARS(a,b,c,d)	((a<<24) | (b<<16) | (c<<8) | d)

/* increase list size by 10% every time it is full */
#define kDefaultAllocationPercentIncrease	10

/* always increase list size by 4 items when it is full */
#define kDefaultAllocationminNumItemsIncrease	4

/*
 * how many items to expand the list by when it becomes full
 * = current listSize (in items) + (hiword percent of list size) + loword
 */
#define NUMITEMSPERALLOC(list)	MAX(((*list)->listSize * \
				    ((*list)->percentIncrease + 100)) / 100, \
				    (*list)->minNumItemsIncrease )

#define ITEMPTR(list,item)	&(((char *)&(*list)->itemList)[(*(list))->itemSize * (item)])

#define LIST_SIGNATURE		CAT4CHARS('L', 'I', 'S', 'T');

#define calloc(size,num)	malloc(size*num)

/********************************************************************/

Handle NewHandle (unsigned int numBytes)
{
	void *memPtr;
	HandleRecord *hanPtr;

	memPtr = calloc (numBytes, 1);
	hanPtr = (HandleRecord *) calloc (sizeof (HandleRecord), 1);
	if (hanPtr && (memPtr || numBytes == 0)) {
		hanPtr->ptr = memPtr;
		hanPtr->size = numBytes;
		return (Handle) hanPtr;
	} else {
		free (memPtr);
		free (hanPtr);
		return NULL;
	}
}
/********************************************************************/

void DisposeHandle (Handle handle)
{
	if (handle) {
		free (*handle);
		free ((void *) handle);
	}
}
/********************************************************************/

unsigned int GetHandleSize (Handle handle)
{
	return ((HandleRecord *) handle)->size;
}
/********************************************************************/

int SetHandleSize (Handle handle, unsigned int newSize)
{
	HandleRecord *hanRecPtr = (HandleRecord *) handle;
	void *newPtr, *oldPtr;
	unsigned int oldSize;


	oldPtr = hanRecPtr->ptr;
	oldSize = hanRecPtr->size;

	if (oldSize == newSize)
		return 1;

	if (oldPtr == NULL) {
		newPtr = malloc (newSize);
	} else {
		newPtr = realloc (oldPtr, newSize);
	}
	if (newPtr || (newSize == 0)) {
		hanRecPtr->ptr = newPtr;
		hanRecPtr->size = newSize;
		if (newSize > oldSize)
			memset ((char *) newPtr + oldSize, 0, newSize - oldSize);
		return 1;
	} else
		return 0;
}

/*
 * If numNewItems == 0 then expand the list by the number of items
 * indicated by its allocation policy.
 * If numNewItems > 0 then expand the list by exactly the number of
 * items indicated.
 * If numNewItems < 0 then expand the list by the absolute value of
 * numNewItems plus the number of items indicated by its allocation
 * policy.
 * Returns 1 for success, 0 if out of memory
*/
static int ExpandListSpace (list_t list, int numNewItems)
{
	if (numNewItems == 0) {
		numNewItems = NUMITEMSPERALLOC (list);
	} else if (numNewItems < 0) {
		numNewItems = (-numNewItems) + NUMITEMSPERALLOC (list);
	}

	if (SetHandleSize ((Handle) list,
			   sizeof (ListStruct) +
			   ((*list)->listSize +
			   numNewItems) * (*list)->itemSize)) {
		(*list)->listSize += numNewItems;
		return 1;
	} else {
		return 0;
	}
}

list_t ListCreate (int elementSize)
{
	list_t list;

	list = (list_t) (NewHandle (sizeof (ListStruct)));  /* create empty list */
	if (list) {
		(*list)->signature = LIST_SIGNATURE;
		(*list)->numItems = 0;
		(*list)->listSize = 0;
		(*list)->itemSize = elementSize;
		(*list)->percentIncrease = kDefaultAllocationPercentIncrease;
		(*list)->minNumItemsIncrease =
				kDefaultAllocationminNumItemsIncrease;
	}

	return list;
}

/*******************************/

void ListSetAllocationPolicy (list_t list, int minItemsPerAlloc,
			      int percentIncreasePerAlloc)
{
	(*list)->percentIncrease = percentIncreasePerAlloc;
	(*list)->minNumItemsIncrease = minItemsPerAlloc;
}

/*******************************/

void ListDispose (list_t list)
{
	DisposeHandle ((Handle) list);
}

/*
 * returns 1 if the item is inserted, returns 0 if out of memory or
 * bad arguments were passed.
 */
int ListInsertItem (list_t list, void *ptrToItem, int itemPosition)
{
	return ListInsertItems (list, ptrToItem, itemPosition, 1);
}

/*******************************/

int ListInsertItems (list_t list, void *ptrToItems, int firstItemPosition,
		     int numItemsToInsert)
{
	int numItems = (*list)->numItems;

	if (firstItemPosition == numItems + 1)
		firstItemPosition = LIST_END;
	else if (firstItemPosition > numItems)
		return 0;

	if ((*list)->numItems >= (*list)->listSize) {
		if (!ExpandListSpace (list, -numItemsToInsert))
			return 0;
	}

	if (firstItemPosition == LIST_START) {
		if (numItems == 0) {
			/* special case for empty list */
			firstItemPosition = LIST_END;
		} else {
			firstItemPosition = 1;
		}
	}

	if (firstItemPosition == LIST_END) {	/* add at the end of the list */
		if (ptrToItems)
			memcpy (ITEMPTR (list, numItems), ptrToItems,
					(*list)->itemSize * numItemsToInsert);
		else
			memset (ITEMPTR (list, numItems), 0,
					(*list)->itemSize * numItemsToInsert);

		(*list)->numItems += numItemsToInsert;
	} else {					/* move part of list up to make room for new item */
		memmove (ITEMPTR (list, firstItemPosition - 1 + numItemsToInsert),
			 ITEMPTR (list, firstItemPosition - 1),
			 (numItems + 1 - firstItemPosition) * (*list)->itemSize);

		if (ptrToItems)
			memmove (ITEMPTR (list, firstItemPosition - 1), ptrToItems,
					 (*list)->itemSize * numItemsToInsert);
		else
			memset (ITEMPTR (list, firstItemPosition - 1), 0,
					(*list)->itemSize * numItemsToInsert);

		(*list)->numItems += numItemsToInsert;
	}

	return 1;
}

void ListGetItems (list_t list, void *itemsDestination,
		   int firstItemPosition, int numItemsToGet)
{

	if (firstItemPosition == LIST_START)
		firstItemPosition = 1;
	else if (firstItemPosition == LIST_END)
		firstItemPosition = (*list)->numItems;

	memcpy (itemsDestination,
		ITEMPTR (list, firstItemPosition - 1),
		(*list)->itemSize * numItemsToGet);
}

/*******************************/

/*
 * Returns a pointer to the item at itemPosition. returns null if an
 * errors occurred.
 */
void *ListGetPtrToItem (list_t list, int itemPosition)
{
	if (itemPosition == LIST_START)
		itemPosition = 1;
	else if (itemPosition == LIST_END)
		itemPosition = (*list)->numItems;

	return ITEMPTR (list, itemPosition - 1);
}

void *ListGetDataPtr (list_t list)
{
	return &((*list)->itemList[0]);
}

int ListGetItemSize (list_t list)
{
	return (*list)->itemSize;
}

int ListNumItems (list_t list)
{
	return (*list)->numItems;
}
