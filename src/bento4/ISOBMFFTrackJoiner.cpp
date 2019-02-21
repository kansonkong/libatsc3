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

int main(int argc, char** argv) {
	if(argc != 3) {
		__ISOBMFF_JOINER_INFO("test harness: (file1) (file2)\n\nwill show jointed tracks as text output dump");
		return 1;
	}

	ISOBMFFTrackJoinerFileResouces_t* fileResources = loadFileResources(argv[1], argv[2]);
	parsrseAndBuildJoinedBoxes(fileResources);


	return 0;
}


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

void parsrseAndBuildJoinedBoxes(ISOBMFFTrackJoinerFileResouces* isoBMFFTrackJoinerFileResouces) {

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

	AP4_ContainerAtom* mvexAtomToCopy;
	AP4_TrakAtom* trakAtomToCopy;

	AP4_AtomParent* moofSecondFile;

	AP4_ContainerAtom* trafFirstFile;
	AP4_TrunAtom* trunFirstFile;

	AP4_ContainerAtom* trafSecondFile;
	AP4_TrunAtom* trunSecondFile;

	AP4_Atom* mdatFirstFile;
	AP4_Atom* mdatSecondFile;


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
    
    AP4_Atom* moofSecondFileAtom = AP4_DYNAMIC_CAST(AP4_Atom, moofSecondFile);
    trunSecondFile->SetDataOffset((AP4_UI32)moofSecondFileAtom->GetSize()+AP4_ATOM_HEADER_SIZE);
    
    //first file is written out last...
    trunFirstFile->SetDataOffset((AP4_UI32)moofSecondFileAtom->GetSize()+AP4_ATOM_HEADER_SIZE+mdatSecondFile->GetSize());

    //apend by hand
    isoBMFFList2.push_back(mdatFirstFile);
    
    

	//now we write...
	const char* output_filename = "jjout.m4v";
	//    AP4_DataBuffer* dataBuffer = new AP4_DataBuffer(65535);
	//    AP4_MemoryByteStream* memoryOutputByteStream = new AP4_MemoryByteStream(dataBuffer);
    //	now go the other way...
	//	  (*it)->Write(*memoryOutputByteStream);

	AP4_ByteStream* output_stream = NULL;
	result = AP4_FileByteStream::Create(output_filename,
                                          AP4_FileByteStream::STREAM_MODE_WRITE,
                                          output_stream);

	for (it = isoBMFFList2.begin(); it != isoBMFFList2.end(); it++) {
        (*it)->Write(*output_stream);
	}
    __ISOBMFF_JOINER_INFO("Final output re-muxed MPU:");
    dumpFullMetadata(isoBMFFList2);

    if (output_stream) output_stream->Release();

}



/***
 *
        // write mdat
        output_stream.WriteUI32(fragment->m_MdatSize);
        output_stream.WriteUI32(AP4_ATOM_TYPE_MDAT);
        AP4_DataBuffer sample_data;
        AP4_Sample     sample;
        for (unsigned int i=0; i<fragment->m_SampleIndexes.ItemCount(); i++) {
            // get the sample
            result = fragment->m_Samples->GetSample(fragment->m_SampleIndexes[i], sample);
            if (AP4_FAILED(result)) {
                fprintf(stderr, "ERROR: failed to get sample %d (%d)\n", fragment->m_SampleIndexes[i], result);
                return;
            }

            // read the sample data
            result = sample.ReadData(sample_data);
            if (AP4_FAILED(result)) {
                fprintf(stderr, "ERROR: failed to read sample data for sample %d (%d)\n", fragment->m_SampleIndexes[i], result);
                return;
            }

            // write the sample data
            result = output_stream.Write(sample_data.GetData(), sample_data.GetDataSize());
            if (AP4_FAILED(result)) {
                fprintf(stderr, "ERROR: failed to write sample data (%d)\n", result);
                return;
            }
        }
    }

    // update the index and re-write it if needed
    if (create_segment_index) {
        unsigned int segment_index = 0;
        AP4_SidxAtom::Reference reference;
        for (AP4_List<FragmentInfo>::Item* item = fragments.FirstItem();
                                           item;
                                           item = item->GetNext()) {
            FragmentInfo* fragment = item->GetData();
            reference.m_ReferencedSize     = (AP4_UI32)(fragment->m_Moof->GetSize()+fragment->m_MdatSize);
            reference.m_SubsegmentDuration = fragment->m_Duration;
            reference.m_StartsWithSap      = true;
            sidx->SetReference(segment_index++, reference);
        }
        AP4_Position here = 0;
        output_stream.Tell(here);
        output_stream.Seek(sidx_position);
        sidx->Write(output_stream);
        output_stream.Seek(here);
        delete sidx;
    }

    // create an mfra container and write out the index
    AP4_ContainerAtom mfra(AP4_ATOM_TYPE_MFRA);
    for (unsigned int i=0; i<cursors.ItemCount(); i++) {
        if (track_id && cursors[i]->m_Track->GetId() != track_id) {
            continue;
        }
        mfra.AddChild(cursors[i]->m_Tfra);
        cursors[i]->m_Tfra = NULL;
    }
    AP4_MfroAtom* mfro = new AP4_MfroAtom((AP4_UI32)mfra.GetSize()+16);
    mfra.AddChild(mfro);
    result = mfra.Write(output_stream);
    if (AP4_FAILED(result)) {
        fprintf(stderr, "ERROR: failed to write 'mfra' (%d)\n", result);
        return;
    }

    // cleanup
    fragments.DeleteReferences();
    for (unsigned int i=0; i<cursors.ItemCount(); i++) {
        delete cursors[i];
    }
    for (AP4_List<FragmentInfo>::Item* item = fragments.FirstItem();
                                       item;
                                       item = item->GetNext()) {
        FragmentInfo* fragment = item->GetData();
        delete fragment->m_Moof;
    }
    delete output_movie;
}
 */




//
//
//
//int sequenceNumber = 1;
//AP4_TrunAtom* trunToComputeDataOffset = NULL;
//
//void traverseChildren(AP4_Atom* toCheckAtom, list<AP4_Atom*> atomList) {
//    AP4_AtomParent* parent = AP4_DYNAMIC_CAST(AP4_ContainerAtom, toCheckAtom);
//
//    if(!parent) {
//        return;
//    }
//
//    AP4_List<AP4_Atom>::Item* child = parent->GetChildren().FirstItem();
//    while (child) {
//        if(child->GetData()) {
//            AP4_Atom* atom = child->GetData();
//            AP4_UI32 m_Type2 = atom->GetType();
//
//            atomList.push_back(atom);
//
//            char name[5];
//            AP4_FormatFourCharsPrintable(name, m_Type2);
//            name[4] = '\0';
//
//            // printf("atom is: %s\n", name);
//            const char* toFindMfhd = "mfhd";
//            if(strncmp(toFindMfhd, name, 4) == 0) {
//                AP4_MfhdAtom* mfhdAtom = AP4_DYNAMIC_CAST(AP4_MfhdAtom, atom);
//                mfhdAtom->SetSequenceNumber(sequenceNumber++);
//            }
//
//            const char* toFindTrun = "trun";
//            const char* toRemoveTrak = "trak";
//            const char* toRemoveTrex = "trex";
//            const char* toRemoveTfdt = "tfdt";
//            const char* toRemoveEdts = "edts";
//            //remove tfhd?
//            const char* toRemoveTraf = "traf";
//
//            if(strncmp(toRemoveEdts, name, 4) == 0) {
//                printf("removing %s\n", name);
//                atom->Detach();
//                //set atom to null so we don't traverse to this boxes children
//                atom = NULL;
//
//            } else if (strncmp(toRemoveTfdt, name, 4) == 0) {
//                printf("removing %s\n", name);
//                atom->Detach();
//                atom = NULL; //no atom->children to traverse after the Tfdt..
//            } else if(strncmp(toFindTrun, name, 4) == 0) {
//                AP4_TrunAtom* trunAtom = AP4_DYNAMIC_CAST(AP4_TrunAtom, atom);
//                //todo check with parent/tfhd.track id==1
//                trunToComputeDataOffset = trunAtom;
//                printf("setting trunToComputeDataOffset to: %p\n", trunToComputeDataOffset);
//               // child = NULL;
//
//            } else if(strncmp(toRemoveTrex, name, 4) == 0 ) {
//                AP4_TrexAtom* trexAtom = AP4_DYNAMIC_CAST(AP4_TrexAtom, atom);
//                if(trexAtom->GetTrackId() == 2) {
//                    printf("removing %s\n", name);
//                    atom->Detach();
//                    atom = NULL;
//
//                    //seems to throw up a segfault? delete atom;
//
//                   // child = NULL;
//
//                 //   return;
//                }
//            } else if(strncmp(toRemoveTrak, name, 4) == 0 ) {
//                //dynamic cast to figure out our track id
//                AP4_TrakAtom* trakAtom = AP4_DYNAMIC_CAST(AP4_TrakAtom, atom);
//                AP4_TkhdAtom* tkhdAtom = AP4_DYNAMIC_CAST(AP4_TkhdAtom, trakAtom->GetChild(AP4_ATOM_TYPE_TKHD));
//
//                if(tkhdAtom->GetTrackId() == 2) {
//                    printf("removing %s\n", name);
//
//                    trakAtom->Detach();
//                    //don't iterate over our children...
//                    atom = NULL;
//                    child = NULL;
//
//                }
//            } else if(strncmp(toRemoveTraf, name, 4) == 0 ) {
//                AP4_ContainerAtom* traf = AP4_DYNAMIC_CAST(AP4_ContainerAtom, atom);
//                AP4_TfhdAtom* tfhdAtom = AP4_DYNAMIC_CAST(AP4_TfhdAtom, traf->GetChild(AP4_ATOM_TYPE_TFHD));
//
//                if(tfhdAtom->GetTrackId() == 2) {
//                    printf("toRemoveTraf: removing %s\n", name);
//                    if(trunToComputeDataOffset) {
//
//                        //20 is the size of the tfdt box removed earlier
//                        trunToComputeDataOffset->SetDataOffset(trunToComputeDataOffset->GetDataOffset() - traf->GetSize() - 20);
//                        printf("setting trunToComputeDataOffset to: %d\n", trunToComputeDataOffset->GetDataOffset());
//
//                        trunToComputeDataOffset = NULL;
//                    }
//                    //adjust the other trun offset
//                    atom->Detach();
//                    atom = NULL;
////                    child = NULL;
//                   // child = NULL;
//                    //seems to throw up a segfault? delete atom;
//
//                 //   return;
//                }
//            }
//            if(atom) {
//                traverseChildren(atom, atomList);
//            }
//        }
//        if(child) {
//            child = child->GetNext();
//        }
//    }
//}

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
/**

AP4_DataBuffer* mpuToDumpISOBMFFBoxes(uint8_t* full_mpu_payload, uint32_t full_mpu_payload_size, int mdat_size) {
    printf("in mpuToDumpISOBMFFBoxes");

    //AP4_Result result = AP4_FileByteStream::Create(filename, AP4_FileByteStream::STREAM_MODE_READ, input);
    //  AP4_MemoryByteStream
    //    AP4_MemoryByteStream(const AP4_UI08* buffer, AP4_Size size);
    //
    //    char* filename = (char*)calloc(20, sizeof(char));
    //    snprintf(filename, 20, "mpudump-%d", mpu_dump_count++);
    //    FILE* f = fopen(filename, "w");
    //    for(int i=0; i < full_mpu_payload_size; i++) {
    //        fwrite(&full_mpu_payload[i], 1, 1, f);
    //
    //    }
    //    fclose(f);


    if(mdat_size > 0) {
		memoryOutputByteStream->WriteUI32(mdat_size+AP4_ATOM_HEADER_SIZE);
		memoryOutputByteStream->WriteUI32(AP4_ATOM_TYPE_MDAT);
    }

    //clean up our atom list
    while(atomList.size()) {

        AP4_Atom* toClear = atomList.front();
        atomList.pop_front();
    //    delete toClear;
    }

    if (boxDumpConsoleOutput) boxDumpConsoleOutput->Release();

    AP4_ByteStream* boxDumpConsoleOutput = NULL;
    AP4_FileByteStream::Create("-stdout", AP4_FileByteStream::STREAM_MODE_WRITE, boxDumpConsoleOutput);

    AP4_AtomInspector* inspector = new AP4_PrintInspector(*boxDumpConsoleOutput);
    inspector->SetVerbosity(3);

    AP4_MemoryByteStream* memoryInputByteStream = new AP4_MemoryByteStream(full_mpu_payload, full_mpu_payload_size);

    AP4_DataBuffer* dataBuffer = new AP4_DataBuffer(4096);

    // open the output memory buffer, assume we won't be bigger than our ingest payload size for now
    AP4_MemoryByteStream* memoryOutputByteStream = new AP4_MemoryByteStream(dataBuffer);

    // inspect the atoms one by one
    AP4_Atom* atom;
    list<AP4_Atom*> atomList;

    AP4_DefaultAtomFactory atom_factory;
    while (atom_factory.CreateAtomFromStream(*memoryInputByteStream, atom) == AP4_SUCCESS) {
        // remember the current stream position because the Inspect method
        // may read from the stream (there may be stream references in some
        // of the atoms
        AP4_Position position;
        memoryInputByteStream->Tell(position);

 traverseChildren(atom, atomList);
        atom->Write(*memoryOutputByteStream);
 memoryInputByteStream->Seek(position);
        atom->Inspect(*inspector);


        atom->Inspect(*inspector);

        memoryInputByteStream->Seek(position);

    }

    if (boxDumpConsoleOutput) boxDumpConsoleOutput->Release();
    if (memoryInputByteStream) memoryInputByteStream->Release();

    delete inspector;



    return dataBuffer;
}

**/

void printBoxType(AP4_Atom* atom) {

	AP4_UI32 m_Type = atom->GetType();
	char name[5];
	AP4_FormatFourCharsPrintable(name, m_Type);

	name[4] = '\0';
	__ISOBMFF_JOINER_DEBUG("printBoxType: atom type: %s, size: %llu\n", name, atom->GetSize());
}

