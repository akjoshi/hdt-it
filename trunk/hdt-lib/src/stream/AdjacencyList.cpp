/*
 * AdjacencyList.cpp
 *
 *  Created on: 31/05/2011
 *      Author: mck
 */

#include "AdjacencyList.hpp"

namespace hdt {

AdjacencyList::AdjacencyList(StreamElements *el, cds_static::BitSequence *bit) :
		elements(el),
		bitmap(bit) {

}

AdjacencyList::~AdjacencyList() {

}

size_t AdjacencyList::find(size_t x) {
	if(x<=0) {
		return 0;
	}
	unsigned int first1 = bitmap->select1(x);
	return bitmap->rank0(first1);
}

size_t AdjacencyList::find(size_t x, size_t y) {
	size_t begin = find(x);
	size_t end = last(x);
	return binSearch(y, begin, end);
}

size_t AdjacencyList::findListIndex(size_t globalpos) {
	return bitmap->rank1(globalpos);
}

size_t AdjacencyList::last(size_t x) {
	size_t first1 = bitmap->select1(x+1);
	return bitmap->rank0(first1)-1;
}

size_t AdjacencyList::countListsX() {
	return bitmap->countOnes();
}

size_t AdjacencyList::countItemsY(size_t x) {
	return last(x)-find(x)+1;
}

size_t AdjacencyList::binSearch(unsigned int element, size_t begin, size_t end) {
	while (begin <= end) {
		int mid = (begin + end) / 2;

		unsigned int read = elements->get(mid);

		if (element > read)
			begin = mid + 1;
		else if (element < read)
			end = mid - 1;
		else
			return mid;
	}
	throw "Not found";
}

size_t AdjacencyList::linSearch(unsigned int element, size_t begin, size_t end) {
	while (begin <= end) {
		unsigned int read = elements->get(begin);
		//cout << "\t\tPos: " << begin << " Compare " << element << " with " << read << endl;

		if (element == read)
			return begin;

		begin++;
	}
	return end;
}


unsigned int AdjacencyList::get(size_t pos) {
	return elements->get(pos);
}

size_t AdjacencyList::getSize() {
	return elements->getNumberOfElements();
}

void AdjacencyList::dump() {
	for(unsigned int i=0; i<countListsX(); i++) {
		cout << " [";
		for(unsigned int j=0; j<countItemsY(i); j++) {
			cout << get(find(i)+j);

			if(j!=countItemsY(i)-1)
				cout << ",";
		}
		cout << "] ";
	}
	cout << endl;


	cout << "List has " << getSize() << " elements in " << countListsX() << " lists" << endl;

	for(unsigned int i=0; i<countListsX(); i++) {
		cout << "List " << i << " [" << find(i) << ", " << last(i) << "] (" << countItemsY(i)<< ") "<< endl;
		for(unsigned int j=0; j<countItemsY(i); j++) {
			cout << "\tItem " << i << ", "<< j << " => " << get(find(i)+j) << " Found pos: " << find(i, get(find(i)+j)) << endl;
		}
	}
}

}
