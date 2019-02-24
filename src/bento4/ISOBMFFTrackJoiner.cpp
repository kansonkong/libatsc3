/*
 * ISOBMFFTrackJoiner.cpp
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 *
 *      ISOBMFF ftyp/moov/moof/mdat track joiner
 */


#include "ISOBMFFTrackJoiner.h"



/*****************************************************************
|
    AP4 - MP4 File Dumper
|
|    Copyright 2002-2008 Axiomatic Systems, LLC
|
|
|    This file is derived from Bento4/AP4 (MP4 Atom Processing Library).
|
|    Unless you have obtained Bento4 under a difference license,
|    this version of Bento4 is Bento4|GPL.
|    Bento4|GPL is free software; you can redistribute it and/or modify
|    it under the terms of the GNU General Public License as published by
|    the Free Software Foundation; either version 2, or (at your option)
|    any later version.
|
|    Bento4|GPL is distributed in the hope that it will be useful,
|    but WITHOUT ANY WARRANTY; without even the implied warranty of
|    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|    GNU General Public License for more details.
|
|    You should have received a copy of the GNU General Public License
|    along with Bento4|GPL; see the file COPYING.  If not, write to the
|    Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
|    02111-1307, USA.
|
 ****************************************************************/

using namespace std;

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

/*----------------------------------------------------------------------
 |   constants
 +---------------------------------------------------------------------*/
#define BANNER "ISOBMFFTrackJoiner - jjustman\n"





// Alternative in-process memory buffer reader/writers
//    AP4_DataBuffer* dataBuffer = new AP4_DataBuffer(65535);
//    AP4_MemoryByteStream* memoryOutputByteStream = new AP4_MemoryByteStream(dataBuffer);
//	now go the other way...
//	  (*it)->Write(*memoryOutputByteStream);

#ifndef __ISOBMFF_LIB

int main(int argc, char** argv) {

	if(argc != 3) {
		__ISOBMFF_JOINER_INFO("test harness: (file1) (file2)\n\nwill show jointed tracks as text output dump");
		return 1;
	}
	int result = 0;

	//first, we map our 2 input files into uint8_t* payloads,
	ISOBMFFTrackJoinerFileResouces_t* fileResources = loadFileResources(argv[1], argv[2]);

	//then we setup our output writer
	const char* output_filename = "jjout.m4v";
	AP4_ByteStream* output_stream = NULL;
	result = AP4_FileByteStream::Create(output_filename, AP4_FileByteStream::STREAM_MODE_WRITE,output_stream);

	//and remux into one unified fragment.  if you have already sent the initn b
	parsrseAndBuildJoinedBoxes(fileResources, output_stream);

	return 0;
}
#endif

ISOBMFFTrackJoinerFileResouces_t* loadFileResources(const char* file1, const char* file2) {

	ISOBMFFTrackJoinerFileResouces_t* isoBMFFTrackJoinerResources = (ISOBMFFTrackJoinerFileResouces_t*)calloc(1, sizeof(ISOBMFFTrackJoinerFileResouces_t));

	isoBMFFTrackJoinerResources->file1_name = (char*)calloc(strlen(file1)+1, sizeof(char));
	strncpy(isoBMFFTrackJoinerResources->file1_name, file1, strlen(file1));

	isoBMFFTrackJoinerResources->file2_name = (char*)calloc(strlen(file2)+1, sizeof(char));
	strncpy(isoBMFFTrackJoinerResources->file2_name, file2, strlen(file2));

	//read these as one show fread's
	struct stat st1;
	stat(file1, &st1);
	int64_t file1_size = st1.st_size;
	isoBMFFTrackJoinerResources->file1_size = file1_size;
	isoBMFFTrackJoinerResources->file1_payload = (uint8_t*)calloc(file1_size, sizeof(uint8_t*));
	FILE *f1 = fopen(file1, "r");
	int block_read_size1 = fread(isoBMFFTrackJoinerResources->file1_payload, 1, file1_size, f1);
    fclose(f1);
	isoBMFFTrackJoinerResources->file1_target_track_num = 1;
    __ISOBMFF_JOINER_INFO("block_read 1 size: %u", block_read_size1);

	struct stat st2;
	stat(file2, &st2);
	int64_t file2_size = st2.st_size;
	isoBMFFTrackJoinerResources->file2_size = file2_size;
    isoBMFFTrackJoinerResources->file2_payload = (uint8_t*)calloc(file2_size, sizeof(uint8_t*));
	FILE *f2 = fopen(file2, "r");
	int block_read_size2 = fread(isoBMFFTrackJoinerResources->file2_payload, 1, file2_size, f2);
    fclose(f2);
	isoBMFFTrackJoinerResources->file1_target_track_num = 2;
    __ISOBMFF_JOINER_INFO("block_read 2 size: %u", block_read_size2);

	return isoBMFFTrackJoinerResources;
}

void parsrseAndBuildJoinedBoxes(ISOBMFFTrackJoinerFileResouces* isoBMFFTrackJoinerFileResouces, AP4_ByteStream* output_stream) {

	list<AP4_Atom*> isoBMFFList1  = ISOBMFFTrackParse(isoBMFFTrackJoinerFileResouces->file1_payload, isoBMFFTrackJoinerFileResouces->file1_size);

	list<AP4_Atom*> isoBMFFList2 =  ISOBMFFTrackParse(isoBMFFTrackJoinerFileResouces->file2_payload, isoBMFFTrackJoinerFileResouces->file2_size);

	__ISOBMFF_JOINER_INFO("Dumping box 1: %s", isoBMFFTrackJoinerFileResouces->file1_name);
	dumpFullMetadata(isoBMFFList1);

	__ISOBMFF_JOINER_INFO("Dumping box 2: %s", isoBMFFTrackJoinerFileResouces->file2_name);
	dumpFullMetadata(isoBMFFList2);


	/**
     top level AP4_ContainerAtoms:
     
	bento4/ISOBMFFTrackJoiner.cpp:363:DEBUG :printBoxType: atom type: ftyp, size: 36
	bento4/ISOBMFFTrackJoiner.cpp:363:DEBUG :printBoxType: atom type: moov, size: 608
	bento4/ISOBMFFTrackJoiner.cpp:363:DEBUG :printBoxType: atom type: styp, size: 24
	bento4/ISOBMFFTrackJoiner.cpp:363:DEBUG :printBoxType: atom type: moof, size: 1220
	bento4/ISOBMFFTrackJoiner.cpp:363:DEBUG :printBoxType: atom type: mdat, size: 96765
	 
     
     steps to combine two tracks:
     ----------------------------
     in Moov box
            -> copy mvex box
            -> Copy trak box

     in Moof box
			-> Copy traf box
                <- detatch both tfdt boxes
     
            -> update trunSecondFile dataOffset from moof->getsize() + moof header size (+8)
            -> update trunfirstFile dataOffset  from moof->getsize() + moof header size (+8) +2nd mdat size
     
      append 1st Copy mdat box

	 */

	AP4_Result   result;

	AP4_ContainerAtom* mvexAtomToCopy = NULL;
	AP4_TrakAtom* trakAtomToCopy = NULL;

	AP4_AtomParent* moofSecondFile = NULL;;

	AP4_ContainerAtom* trafFirstFile = NULL;;
	AP4_TrunAtom* trunFirstFile = NULL;;

	AP4_ContainerAtom* trafSecondFile = NULL;;
	AP4_TrunAtom* trunSecondFile = NULL;;

	AP4_Atom* mdatFirstFile = NULL;;
	AP4_Atom* mdatSecondFile = NULL;;


	//from isoBMFFList1 list
	std::list<AP4_Atom*>::iterator it;
	for (it = isoBMFFList1.begin(); it != isoBMFFList1.end(); it++) {

		//In the moov box->get a ref for the trak box
		if((*it)->GetType() == AP4_ATOM_TYPE_MOOV) {
			AP4_MoovAtom* moovAtom = AP4_DYNAMIC_CAST(AP4_MoovAtom, *it);
			mvexAtomToCopy = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moovAtom->GetChild(AP4_ATOM_TYPE_MVEX));

			trakAtomToCopy = AP4_DYNAMIC_CAST(AP4_TrakAtom, moovAtom->GetChild(AP4_ATOM_TYPE_TRAK));
		}

		if((*it)->GetType() == AP4_ATOM_TYPE_MOOF) {
			AP4_AtomParent* moofAtom = AP4_DYNAMIC_CAST(AP4_ContainerAtom, *it);
			trafFirstFile = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moofAtom->GetChild(AP4_ATOM_TYPE_TRAF));
			trunFirstFile = AP4_DYNAMIC_CAST(AP4_TrunAtom, trafFirstFile->GetChild(AP4_ATOM_TYPE_TRUN));
		}
		if((*it)->GetType() == AP4_ATOM_TYPE_MDAT) {
			mdatFirstFile = (*it);
		}
	}

	//now go the other way...
	for (it = isoBMFFList2.begin(); it != isoBMFFList2.end(); it++) {

		//In the moov box->get a ref for the trak box
		if((*it)->GetType() == AP4_ATOM_TYPE_MOOV) {
			AP4_MoovAtom* moovAtom = AP4_DYNAMIC_CAST(AP4_MoovAtom, *it);
			if(mvexAtomToCopy) {
				mvexAtomToCopy->Detach();
				moovAtom->AddChild(mvexAtomToCopy, -1);
			}
			if(trakAtomToCopy) {
				trakAtomToCopy->Detach();
				moovAtom->AddChild(trakAtomToCopy, -1);
			}
		}

		if((*it)->GetType() == AP4_ATOM_TYPE_MOOF) {
			moofSecondFile = AP4_DYNAMIC_CAST(AP4_ContainerAtom, *it);
			trafSecondFile = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moofSecondFile->GetChild(AP4_ATOM_TYPE_TRAF));
            AP4_TfdtAtom* tfdtSecondFile = AP4_DYNAMIC_CAST(AP4_TfdtAtom, trafSecondFile->GetChild(AP4_ATOM_TYPE_TFDT));
            tfdtSecondFile->Detach();

			if(trafFirstFile) {
                //remove our tfdt's
                AP4_TfdtAtom* tfdtFirstAtom = AP4_DYNAMIC_CAST(AP4_TfdtAtom, trafFirstFile->GetChild(AP4_ATOM_TYPE_TFDT));
                tfdtFirstAtom->Detach();
                
                trafFirstFile->Detach();
				moofSecondFile->AddChild(trafFirstFile);
			}

			trunSecondFile = AP4_DYNAMIC_CAST(AP4_TrunAtom, trafSecondFile->GetChild(AP4_ATOM_TYPE_TRUN));
		}

		if((*it)->GetType() == AP4_ATOM_TYPE_MDAT) {
			mdatSecondFile = (*it);
		}
	}

	if(moofSecondFile) {
		AP4_Atom* moofSecondFileAtom = AP4_DYNAMIC_CAST(AP4_Atom, moofSecondFile);
		trunSecondFile->SetDataOffset((AP4_UI32)moofSecondFileAtom->GetSize()+AP4_ATOM_HEADER_SIZE);

		//first file is written out last...
		if(mdatSecondFile) {
			trunFirstFile->SetDataOffset((AP4_UI32)moofSecondFileAtom->GetSize()+AP4_ATOM_HEADER_SIZE+mdatSecondFile->GetSize());
		}
	}
    //apend by hand
    if(mdatFirstFile) {
        isoBMFFList2.push_back(mdatFirstFile);
    }
    
    //push our packets to out output_stream writer, and we're done...
    
	for (it = isoBMFFList2.begin(); it != isoBMFFList2.end(); it++) {
        (*it)->Write(*output_stream);
	}
    __ISOBMFF_JOINER_INFO("Final output re-muxed MPU:");
    dumpFullMetadata(isoBMFFList2);

//    if (output_stream) output_stream->Release();

}




list<AP4_Atom*> ISOBMFFTrackParse(uint8_t* full_mpu_payload, uint32_t full_mpu_payload_size) {

	__ISOBMFF_JOINER_DEBUG("::ISOBMFFTrackParse: payload size is: %u", full_mpu_payload_size);

	list<AP4_Atom*> atomList;
    AP4_Atom* atom;

    AP4_MemoryByteStream* memoryInputByteStream = new AP4_MemoryByteStream(full_mpu_payload, full_mpu_payload_size);
    // inspect the atoms one by one

    AP4_DefaultAtomFactory atom_factory;
    while (atom_factory.CreateAtomFromStream(*memoryInputByteStream, atom) == AP4_SUCCESS) {
        AP4_Position position;
        memoryInputByteStream->Tell(position);
        atomList.push_back(atom);
        printBoxType(atom);
        memoryInputByteStream->Seek(position);
    }

    if (memoryInputByteStream) memoryInputByteStream->Release();

    return atomList;
}

void dumpFullMetadata(list<AP4_Atom*> atomList) {

	AP4_ByteStream* boxDumpConsoleOutput = NULL;
	AP4_FileByteStream::Create("-stdout", AP4_FileByteStream::STREAM_MODE_WRITE, boxDumpConsoleOutput);
	AP4_AtomInspector* inspector = new AP4_PrintInspector(*boxDumpConsoleOutput);
	inspector->SetVerbosity(3);

	std::list<AP4_Atom*>::iterator it;
	for (it = atomList.begin(); it != atomList.end(); it++) {
		(*it)->Inspect(*inspector);
	}

	if (boxDumpConsoleOutput) boxDumpConsoleOutput->Release();
	delete inspector;

}


void printBoxType(AP4_Atom* atom) {

	AP4_UI32 m_Type = atom->GetType();
	char name[5];
	AP4_FormatFourCharsPrintable(name, m_Type);

	name[4] = '\0';
	__ISOBMFF_JOINER_DEBUG("printBoxType: atom type: %s, size: %llu\n", name, atom->GetSize());
}

