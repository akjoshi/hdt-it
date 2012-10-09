/*
 * File: HDTEnums.cpp
 * Last modified: $Date: 2012-08-13 23:00:07 +0100 (lun, 13 ago 2012) $
 * Revision: $Revision: 222 $
 * Last modified by: $Author: mario.arias $
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


#include <HDT.hpp>
#include <HDTFactory.hpp>

#include "../src/rdf/RDFSerializerN3.hpp"

#include <getopt.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace hdt;
using namespace std;


void help() {
	cout << "$ hdtInfo [options] <hdtfile> " << endl;
	cout << "\t-h\t\t\tThis help" << endl;
	cout << "\t-o\t<output>\tSave query output to file." << endl;
}


int main(int argc, char **argv) {
	int c;
	string query, inputFile, outputFile;
	bool measure = false;

	while( (c = getopt(argc,argv,"ho:"))!=-1) {
		switch(c) {
		case 'h':
			help();
			break;
		case 'o':
			outputFile = optarg;
			break;
		default:
			cout << "ERROR: Unknown option" << endl;
			help();
			return 1;
		}
	}

	if(argc-optind<1) {
		cout << "ERROR: You must supply an HDT File" << endl << endl;
		help();
		return 1;
	}

	inputFile = argv[optind];
	try {
		ifstream in(inputFile.c_str(), ios::in | ios::binary);
		if(!in.good()){
			throw "Could not open input file.";
		}

		// Load header
		ControlInformation controlInformation;
		controlInformation.load(in);
		Header *header = HDTFactory::readHeader(controlInformation);
		header->load(in, controlInformation);
		in.close();

		// Save
		IteratorTripleString *it = header->search("","","");

		while(it->hasNext()) {
			TripleString *ts = it->next();
			cout << *ts << " ."<< endl;
		}

		if(outputFile!="") {
			ofstream out(outputFile.c_str());
			if(!out.good()){
				throw "Could not open output file.";
			}
			RDFSerializerN3 serializer(out, N3);
			serializer.serialize(it);
			out.close();
		} else {
			RDFSerializerN3 serializer(cout, N3);
			serializer.serialize(it);
		}
		delete it;

		delete header;

	} catch (char *e) {
		cout << "ERROR: " << e << endl;
	} catch (const char *e) {
		cout << "ERROR: " << e << endl;
	}
}



