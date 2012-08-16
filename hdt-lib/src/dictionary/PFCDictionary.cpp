/*
 * File: PFCDictionary.cpp
 * Last modified: $Date$
 * Revision: $Revision$
 * Last modified by: $Author$
 *
 * Copyright (C) 2012, Mario Arias, Javier D. Fernandez, Miguel A. Martinez-Prieto
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the authors:
 *   Mario Arias:               mario.arias@gmail.com
 *   Javier D. Fernandez:       jfergar@infor.uva.es
 *   Miguel A. Martinez-Prieto: migumar2@infor.uva.es
 *
 */

#include "PFCDictionary.hpp"
#include <HDTVocabulary.hpp>

#include "../libdcs/CSD_PFC.h"
#include "../libdcs/CSD_HTFC.h"
#include "../libdcs/CSD_Cache.h"

namespace hdt {

PFCDictionary::PFCDictionary() : blocksize(8)
{
	subjects = new csd::CSD_PFC();
	predicates = new csd::CSD_PFC();
	objects = new csd::CSD_PFC();
	shared = new csd::CSD_PFC();
}

PFCDictionary::PFCDictionary(HDTSpecification & spec) : blocksize(8)
{
	subjects = new csd::CSD_PFC();
	predicates = new csd::CSD_PFC();
	objects = new csd::CSD_PFC();
	shared = new csd::CSD_PFC();

	string blockSizeStr = spec.get("dict.block.size");
	if(blockSizeStr!=""){
		//blocksize = atoi((const char*)blockSizeStr.c_str());
	}
}

PFCDictionary::~PFCDictionary()
{
	delete subjects;
	delete predicates;
	delete objects;
	delete shared;
}

csd::CSD *loadSection(IteratorUCharString *iterator, uint32_t blocksize, ProgressListener *listener) {
	return new csd::CSD_PFC(iterator, blocksize, listener);
	//return new csd::CSD_HTFC(iterator, blocksize, listener);
}


std::string PFCDictionary::idToString(unsigned int id, TripleComponentRole position)
{
	csd::CSD *section = getDictionarySection(id, position);

	unsigned int localid = getLocalId(id, position);

	if(localid<=section->getLength()) {
		const char * ptr = (const char *)section->extract(localid);
		if(ptr!=NULL) {
			string out = ptr;
			section->freeString((unsigned char*)ptr);
			return out;
		} else {
			//cout << "Not found: " << id << " as " << position << endl;
		}
	}

	return string();
}

unsigned int PFCDictionary::stringToId(std::string &key, TripleComponentRole position)
{
	unsigned int ret;

        if(key.length()==0) {
		return 0;
        }

	switch (position) {
	case SUBJECT:
		ret = shared->locate((const unsigned char *)key.c_str(), key.length());
		if( ret != 0) {
			return getGlobalId(ret,SHARED_SUBJECT);
		}
		ret = subjects->locate((const unsigned char *)key.c_str(), key.length());
		if(ret != 0) {
			return getGlobalId(ret,NOT_SHARED_SUBJECT);
		}
        return 0;
	case PREDICATE:
		ret = predicates->locate((const unsigned char *)key.c_str(), key.length());
		if(ret!=0) {
			return getGlobalId(ret, NOT_SHARED_PREDICATE);
		}
        return 0;

	case OBJECT:
		ret = shared->locate((const unsigned char *)key.c_str(), key.length());
		if( ret != 0) {
			return getGlobalId(ret,SHARED_OBJECT);
		}
		ret = objects->locate((const unsigned char *)key.c_str(), key.length());
		if(ret != 0) {
			return getGlobalId(ret,NOT_SHARED_OBJECT);
		}
		ret = objects->locate((const unsigned char *)key.c_str(), key.length());
		if(ret != 0) {
			return getGlobalId(ret,NOT_SHARED_OBJECT);
		}
        return 0;
	}
}


void PFCDictionary::load(std::istream & input, ControlInformation & ci, ProgressListener *listener)
{
	this->mapping = ci.getUint("$mapping");
	this->sizeStrings = ci.getUint("$sizeStrings");

	ifstream *in = dynamic_cast<ifstream *>(&input);

	IntermediateListener iListener(listener);

	iListener.setRange(0,25);
	iListener.notifyProgress(0, "Dictionary read shared area.");
	//cout << "Load shared " << in->tellg() << endl;
	delete shared;
	shared = csd::CSD::load(*in);
	if(shared==NULL){
		shared = new csd::CSD_PFC();
		throw "Could not read shared.";
	}
	shared = new csd::CSD_Cache(shared);

	iListener.setRange(25,50);
	iListener.notifyProgress(0, "Dictionary read subjects.");
	//cout << "Load subjects " << in->tellg() << endl;
	delete subjects;
	subjects = csd::CSD::load(*in);
	if(subjects==NULL){
		subjects = new csd::CSD_PFC();
		throw "Could not read subjects.";
	}
	subjects = new csd::CSD_Cache(subjects);

	iListener.setRange(50,75);
	iListener.notifyProgress(0, "Dictionary read predicates.");
	//cout << "Load predicates " << in->tellg() << endl;
	delete predicates;
	predicates = csd::CSD::load(*in);
	if(predicates==NULL){
		predicates = new csd::CSD_PFC();
		throw "Could not read predicates.";
	}
	subjects = new csd::CSD_Cache(subjects);

	iListener.setRange(75,100);
	iListener.notifyProgress(0, "Dictionary read objects.");
	//cout << "Load objects " << in->tellg() << endl;
	delete objects;
	objects = csd::CSD::load(*in);
	if(objects==NULL){
		objects = new csd::CSD_PFC();
		throw "Could not read objects.";
	}
	objects = new csd::CSD_Cache(objects);
	//cout << "Dictionary loaded " << in->tellg() << endl;
}


void PFCDictionary::import(Dictionary *other, ProgressListener *listener) {

	try {
		IntermediateListener iListener(listener);

		//NOTIFY(listener, "DictionaryPFC loading subjects", 0, 100);
		iListener.setRange(0, 20);
		IteratorUCharString *itSubj = other->getSubjects();
		delete subjects;
		subjects = loadSection(itSubj, blocksize, &iListener);
		delete itSubj;

		//NOTIFY(listener, "DictionaryPFC loading predicates", 25, 30);
		iListener.setRange(20, 21);
		IteratorUCharString *itPred = other->getPredicates();
		delete predicates;

		predicates = loadSection(itPred, blocksize, &iListener);

		//NOTIFY(listener, "DictionaryPFC loading objects", 30, 90);
		iListener.setRange(21, 90);
		IteratorUCharString *itObj = other->getObjects();
		delete objects;
		objects = loadSection(itObj, blocksize, &iListener);
		delete itObj;

		//NOTIFY(listener, "DictionaryPFC loading shared", 90, 100);
		iListener.setRange(90, 100);
		IteratorUCharString *itShared = other->getShared();
		delete shared;
		shared = loadSection(itShared, blocksize, &iListener);
		delete itShared;

		this->sizeStrings = other->size();
		this->mapping = other->getMapping();
	} catch (const char *e) {
		delete subjects;
		delete predicates;
		delete objects;
		delete shared;
		subjects = new csd::CSD_PFC();
		predicates = new csd::CSD_PFC();
		objects = new csd::CSD_PFC();
		shared = new csd::CSD_PFC();
		throw e;
	}

	#if 0
		cout << "Shared: " << shared->getLength() << endl;
		cout << "Subjects: " << subjects->getLength() << endl;
		cout << "Predicates: " << predicates->getLength() << endl;
		cout << "Objects: " << objects->getLength() << endl;

		cout << "Ensure same: " << endl;
		for(unsigned int i=1;i<getMaxObjectID();i++){
			string str1 = other->idToString(i, OBJECT);
			string str2 = this->idToString(i, OBJECT);
			unsigned int id1 = other->stringToId(str1, OBJECT);
			unsigned int id2 = this->stringToId(str1, OBJECT);

			if( (str1!=str2) || (id1!=id2)) {
				cout << i << " Objects difer: " << endl;
				cout << "\tPlain: " << id1 << " => " << str1 << endl;
				cout << "\tPFC__: " << id2 << " => " << str2 << endl;
			}
		}
		for(unsigned int i=1;i<getMaxObjectID();i++){
			string str2 = this->idToString(i, OBJECT);
			unsigned int id2 = this->stringToId(str2, OBJECT);

			//cout << "ID: "<< i << " Back id: "<< id2 << " => " << str2 << endl;
		}
	#endif
}

IteratorUCharString *PFCDictionary::getSubjects() {
	throw "Not implemented";
}

IteratorUCharString *PFCDictionary::getPredicates() {
	throw "Not implemented";
}

IteratorUCharString *PFCDictionary::getObjects() {
	throw "Not implemented";
}

IteratorUCharString *PFCDictionary::getShared() {
	throw "Not implemented";
}

void PFCDictionary::save(std::ostream & output, ControlInformation & controlInformation, ProgressListener *listener)
{
	controlInformation.set("codification", HDTVocabulary::DICTIONARY_TYPE_PFC);
	controlInformation.set("format", "text/plain");

	controlInformation.setUint("$mapping", this->mapping);
	controlInformation.setUint("$sizeStrings", this->sizeStrings);

	controlInformation.save(output);

	ofstream *out = dynamic_cast<ofstream *>(&output);
	IntermediateListener iListener(listener);

	iListener.setRange(0,10);
	iListener.notifyProgress(0, "Dictionary save shared area.");
	//cout << "Save shared " << out->tellp() << endl;
	shared->save(*out);

	iListener.setRange(10,45);
	iListener.notifyProgress(0, "Dictionary save subjects.");
	//cout << "Save subjects " << out->tellp() << endl;
	subjects->save(*out);

	iListener.setRange(45,60);
	iListener.notifyProgress(0, "Dictionary save predicates.");
	//cout << "Save predicates " << out->tellp() << endl;
	predicates->save(*out);

	iListener.setRange(60,100);
	iListener.notifyProgress(0, "Dictionary save objects.");
	//cout << "Save objects " << out->tellp() << endl;
	objects->save(*out);

	//cout << "Dictionary saved " << out->tellp() << endl;
}


void PFCDictionary::populateHeader(Header & header, string rootNode)
{
	header.insert(rootNode, HDTVocabulary::DICTIONARY_TYPE, HDTVocabulary::DICTIONARY_TYPE_PFC);
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMSUBJECTS, getNsubjects());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMPREDICATES, getNpredicates());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMOBJECTS, getNobjects());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_NUMSHARED, getNshared());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXSUBJECTID, getMaxSubjectID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXPREDICATEID, getMaxPredicateID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAXOBJECTTID, getMaxObjectID());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_MAPPING, this->mapping);
	header.insert(rootNode, HDTVocabulary::DICTIONARY_SIZE_STRINGS, size());
	header.insert(rootNode, HDTVocabulary::DICTIONARY_BLOCK_SIZE, this->blocksize);
}

unsigned int PFCDictionary::getNsubjects(){
	return shared->getLength()+subjects->getLength();
}
unsigned int PFCDictionary::getNpredicates(){
	return predicates->getLength();
}
unsigned int PFCDictionary::getNobjects(){
	return shared->getLength()+objects->getLength();
}
unsigned int PFCDictionary::getNshared(){
	return shared->getLength();
}


unsigned int PFCDictionary::getMaxID()
{
	unsigned int s = subjects->getLength();
	unsigned int o = objects->getLength();
	unsigned int sh = shared->getLength();
	unsigned int max = s>o ? s : o;

	if(mapping ==MAPPING2) {
		return sh+max;
	} else {
		return sh+s+o;
	}
}

unsigned int PFCDictionary::getMaxSubjectID()
{
	return getNsubjects();
}

unsigned int PFCDictionary::getMaxPredicateID()
{
	return predicates->getLength();
}

unsigned int PFCDictionary::getMaxObjectID()
{
	unsigned int s = subjects->getLength();
	unsigned int o = objects->getLength();
	unsigned int sh = shared->getLength();

	if(mapping ==MAPPING2) {
		return sh+o;
	} else {
		return sh+s+o;
	}
}

unsigned int PFCDictionary::getNumberOfElements()
{
	return shared->getLength()+subjects->getLength()+predicates->getLength()+objects->getLength();
}

unsigned int PFCDictionary::size()
{
	return shared->getSize()+subjects->getSize()+predicates->getSize()+objects->getSize();
}

string PFCDictionary::getType()
{
	return HDTVocabulary::DICTIONARY_TYPE_PFC;
}

unsigned int PFCDictionary::getMapping() {
	return mapping;
}


csd::CSD *PFCDictionary::getDictionarySection(unsigned int id, TripleComponentRole position) {
	switch (position) {
	case SUBJECT:
		if(id<=shared->getLength()) {
			//cout << "Section SHARED" << endl;
			return shared;
		} else {
			//cout << "Section SUBJECTS" << endl;
			return subjects;
		}

	case PREDICATE:
		return predicates;

	case OBJECT:
		if(id<=shared->getLength()) {
			//cout << "Section SHARED" << endl;
			return shared;
		} else {
			//cout << "Section OBJECTS" << endl;
			return objects;
		}
	}

	throw "Item not found";
}

unsigned int PFCDictionary::getGlobalId(unsigned int mapping, unsigned int id, DictionarySection position) {
	switch (position) {
	case NOT_SHARED_SUBJECT:
		return shared->getLength()+id;

	case NOT_SHARED_PREDICATE:
		return id;

	case NOT_SHARED_OBJECT:
		if(mapping==MAPPING2) {
			return shared->getLength()+id;
		} else {
			return shared->getLength()+subjects->getLength()+id;
		}

	case SHARED_SUBJECT:
	case SHARED_OBJECT:
		return id;
	}

	throw "Item not found";
}


unsigned int PFCDictionary::getGlobalId(unsigned int id, DictionarySection position) {
	return getGlobalId(this->mapping, id, position);
}

unsigned int PFCDictionary::getLocalId(unsigned int mapping, unsigned int id, TripleComponentRole position) {
	switch (position) {
	case SUBJECT:
		if(id<=shared->getLength()) {
			return id;
		} else {
			return id-shared->getLength();
		}
		break;
	case OBJECT:
		if(id<=shared->getLength()) {
			return id;
		} else {
			if(mapping==MAPPING2) {
				return id-shared->getLength();
			} else {
				return 2+id-shared->getLength()-subjects->getLength();
			}
		}
		break;
	case PREDICATE:
		return id;
	}

	throw "Item not found";
}

unsigned int PFCDictionary::getLocalId(unsigned int id, TripleComponentRole position) {
	return getLocalId(mapping,id,position);
}

void PFCDictionary::getSuggestions(const char *base, hdt::TripleComponentRole role, std::vector<std::string> &out, int maxResults)
{
	if(role==PREDICATE) {
		predicates->fillSuggestions(base, out, maxResults);
		return;
	}

	vector<string> v1,v2;
	shared->fillSuggestions(base, v1, maxResults);
	if(role==SUBJECT) {
		subjects->fillSuggestions(base, v2, maxResults);
	} else if(role==OBJECT){
		objects->fillSuggestions(base, v2, maxResults);
	}

	// Merge results from shared and subjects/objects keeping order
	merge(v1.begin(),v1.end(), v2.begin(), v2.end(), std::back_inserter(out));

	// Remove possible extra items
	if(out.size()>maxResults) {
		out.resize(maxResults);
	}
}

}


