/* CSD.cpp
 * Copyright (C) 2011, Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * Abstract class for implementing Compressed String Dictionaries following:
 *
 *   ==========================================================================
 *     "Compressed String Dictionaries"
 *     Nieves R. Brisaboa, Rodrigo Canovas, Francisco Claude, 
 *     Miguel A. Martinez-Prieto and Gonzalo Navarro.
 *     10th Symposium on Experimental Algorithms (SEA'2011), p.136-147, 2011.
 *   ==========================================================================
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
 *   Rodrigo Canovas:  rcanovas@dcc.uchile.cl
 *   Miguel A. Martinez-Prieto:  migumar2@infor.uva.es
 */

#include "CSD.h"

#include "CSD_Cache.h"

namespace csd
{

CSD::CSD() : length(0) {

}

CSD * CSD::load(ifstream & fp)
{
	uint32_t r = loadValue<uint32_t>(fp);
	//uint32_t pos = fp.tellg();
	//fp.seekg(pos-sizeof(uint32_t));
	//fp.seekg(-sizeof(uint32_t), ios_base::cur);
	switch(r)
	{
	case HTFC: return new CSD_Cache(CSD_HTFC::load(fp));
	//case PFC: return CSD_PFC::load(fp);
	case PFC: return new CSD_Cache(CSD_PFC::load(fp));
	//	case REPAIRDAC: return CSD_RePairDAC::load(fp);
	case FMINDEX: return new CSD_Cache(CSD_FMIndex::load(fp));
	//	case HASHHUFF: return CSD_HashHuff::load(fp);
	/*
	 * case DELTA: return URICDDelta::load(fp);
	 * case DELTA_HT: return URICDDelta_HT::load(fp);
	 */
	}

	return NULL;
}

uint32_t CSD::getLength()
{
	return length;
}

};


