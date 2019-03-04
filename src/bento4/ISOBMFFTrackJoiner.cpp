/*
 * ISOBMFFTrackJoiner.cpp
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 *
 *      ISOBMFF ftyp/moov/moof/mdat track joiner
 */


#include "ISOBMFFTrackJoiner.h"

int _ISOBMFFTRACKJOINER_DEBUG_ENABLED = 0;
int _ISOBMFFTRACKJOINER_TRACE_ENABLED = 0;


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


#define __DROP_HINT_TRACKS__


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
	parseAndBuildJoinedBoxes(fileResources, output_stream);

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


void ISOBMFF_track_joiner_monitor_output_buffer_parse_and_build_joined_boxes(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, AP4_MemoryByteStream** output_stream_p)
{
	block_t* audio_output_buffer = lls_sls_monitor_output_buffer_copy_audio_full_isobmff_box(lls_sls_monitor_output_buffer);
	block_t* video_output_buffer = lls_sls_monitor_output_buffer_copy_video_full_isobmff_box(lls_sls_monitor_output_buffer);

	//we shouldn't be bigger than this for our return..
	AP4_DataBuffer* dataBuffer = new AP4_DataBuffer(audio_output_buffer->i_pos + video_output_buffer->i_pos );
	AP4_MemoryByteStream* memoryOutputByteStream = new AP4_MemoryByteStream(dataBuffer);

	*output_stream_p = memoryOutputByteStream;

    parseAndBuildJoinedBoxesFromMemory(audio_output_buffer->p_buffer, audio_output_buffer->i_pos, video_output_buffer->p_buffer, video_output_buffer->i_pos, memoryOutputByteStream);

    block_Release(&audio_output_buffer);
    block_Release(&video_output_buffer);
}


void parseAndBuildJoinedBoxes(ISOBMFFTrackJoinerFileResouces* isoBMFFTrackJoinerFileResouces, AP4_ByteStream* output_stream) {

    parseAndBuildJoinedBoxesFromMemory(isoBMFFTrackJoinerFileResouces->file1_payload, isoBMFFTrackJoinerFileResouces->file1_size, isoBMFFTrackJoinerFileResouces->file2_payload, isoBMFFTrackJoinerFileResouces->file2_size, output_stream);
}

//HACK

AP4_UI32 trakMediaAtomSecondFileId = 0;
AP4_UI32 trakMediaAtomOriginalId = 0;


void parseAndBuildJoinedBoxesFromMemory(uint8_t* file1_payload, uint32_t file1_size, uint8_t* file2_payload, uint32_t file2_size, AP4_ByteStream* output_stream) {
	list<AP4_Atom*> isoBMFFList1  = ISOBMFFTrackParse(file1_payload, file1_size);

	list<AP4_Atom*> isoBMFFList2 =  ISOBMFFTrackParse(file2_payload, file2_size);

    __ISOBMFF_JOINER_DEBUG("Dumping box 1: size: %u", file1_size);
	dumpFullMetadata(isoBMFFList1);

	__ISOBMFF_JOINER_DEBUG("Dumping box 2: %u", file2_size);
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

	AP4_TrakAtom* trakMediaAtomToCopy = NULL;
    std::list<AP4_TrakAtom*> trakHintAtomToCopy;
	std::list<AP4_TrakAtom*>::iterator itHint;


	std::list<AP4_ContainerAtom*> trafFirstFileList;
	std::list<AP4_ContainerAtom*>::iterator itTraf;

	std::list<AP4_TrunAtom*> trunFirstFileList;
	std::list<AP4_TrunAtom*>::iterator itTrunFirst;

	std::list<AP4_Atom*> mdatFirstFileList;
	std::list<AP4_Atom*>::iterator itMdatFirst;


	AP4_AtomParent* moofSecondFile = NULL;
	AP4_Atom* moofSecondFileAtom = NULL;

	AP4_ContainerAtom* trafSecondFile = NULL;
	AP4_TrunAtom* trunSecondFile = NULL;

	std::list<AP4_Atom*> mdatSecondFileList;
	std::list<AP4_Atom*>::iterator itMdatSecond;
	uint64_t mdatSecondFileOffset = 0;

    /**
     to postion at end:
     
     [hdlr] size=12+40
     handler_type = hint
     handler_name = Bento4 Hint Handler
     **/

	//from isoBMFFList1 list
	std::list<AP4_Atom*>::iterator it;
	for (it = isoBMFFList1.begin(); it != isoBMFFList1.end(); it++) {

		//In the moov box->get a ref for the trak box
		if((*it)->GetType() == AP4_ATOM_TYPE_MOOV) {
			AP4_MoovAtom* moovAtom = AP4_DYNAMIC_CAST(AP4_MoovAtom, *it);
			mvexAtomToCopy = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moovAtom->GetChild(AP4_ATOM_TYPE_MVEX));

			AP4_TrakAtom* tmpTrakAtom;
			int trakIndex = 0;
			while((tmpTrakAtom = AP4_DYNAMIC_CAST(AP4_TrakAtom, moovAtom->GetChild(AP4_ATOM_TYPE_TRAK, trakIndex++)))) {

            //AP4_HdlrAtom* hdlrAtom = AP4_DYNAMIC_CAST(AP4_HdlrAtom, tmpTrakAtom->GetChild(AP4_ATOM_TYPE_HDLR));

				AP4_HdlrAtom* hdlrAtom = AP4_DYNAMIC_CAST(AP4_HdlrAtom, tmpTrakAtom->FindChild("mdia/hdlr", false, false));

				//todo - handle duplicate track id's

				if(hdlrAtom)  {
					if(hdlrAtom->GetHandlerType() == AP4_HANDLER_TYPE_VIDE || hdlrAtom->GetHandlerType() == AP4_HANDLER_TYPE_SOUN) {
						trakMediaAtomToCopy = tmpTrakAtom;
						trakMediaAtomOriginalId = tmpTrakAtom->GetId();
						tmpTrakAtom->SetId(tmpTrakAtom->GetId()+10);

					} else if(hdlrAtom->GetHandlerType() == AP4_HANDLER_TYPE_HINT) {
#ifndef __DROP_HINT_TRACKS__

						tmpTrakAtom->SetId(tmpTrakAtom->GetId()+10);

						trakHintAtomToCopy.push_back(tmpTrakAtom);

						//if we have a hint ref
											/**
											 *[tref] size=8+12
												  [hint] size=8+4
													track_id_count = 1
													track id  = 2
											 */
						//tmpTrakAtom->GetChild(AP4_ATOM_TYPE_TREF)
						AP4_TrefTypeAtom* tmpTrefAtom = AP4_DYNAMIC_CAST(AP4_TrefTypeAtom, tmpTrakAtom->FindChild("tref/hint", false, false)); //(AP4_ATOM_TYPE_TREF));
                        if(tmpTrefAtom) {
                            const AP4_Array<AP4_UI32>& trefTrackIds = tmpTrefAtom->GetTrackIds();

                            AP4_TrefTypeAtom* newTempTrefAtom = new AP4_TrefTypeAtom(tmpTrefAtom->GetType());
                            for(AP4_Cardinal i=0; i < trefTrackIds.ItemCount(); i++) {
                                newTempTrefAtom->AddTrackId(trefTrackIds[i] + 10);
                            }
                            AP4_AtomParent* tmpTrefParent = tmpTrefAtom->GetParent();
                            tmpTrefAtom->Detach();
                            tmpTrefParent->AddChild(newTempTrefAtom);
                        }
#endif
					} else {
						//printf("Skipping tmpTrakAtom: %u", tmpTrakAtom->GetType());
					}

				}//otherwise drop
			}
		}

		if((*it)->GetType() == AP4_ATOM_TYPE_MOOF) {
			AP4_AtomParent* moofAtom = AP4_DYNAMIC_CAST(AP4_ContainerAtom, *it);
			AP4_ContainerAtom* trafContainerAtom = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moofAtom->GetChild(AP4_ATOM_TYPE_TRAF));
			trafFirstFileList.push_back(trafContainerAtom);
			trunFirstFileList.push_back(AP4_DYNAMIC_CAST(AP4_TrunAtom, trafContainerAtom->GetChild(AP4_ATOM_TYPE_TRUN)));
		}
		if((*it)->GetType() == AP4_ATOM_TYPE_MDAT) {
			mdatFirstFileList.push_back(*it);
		}
	}

	//now go the other way...
	for (it = isoBMFFList2.begin(); it != isoBMFFList2.end(); it++) {

		//In the moov box->get a ref for the trak box
		if((*it)->GetType() == AP4_ATOM_TYPE_MOOV) {
			AP4_MoovAtom* moovAtom = AP4_DYNAMIC_CAST(AP4_MoovAtom, *it);

			//remove our hints
			AP4_TrakAtom* tmpTrakAtom;
			int trakIndex = 0;
			while((tmpTrakAtom = AP4_DYNAMIC_CAST(AP4_TrakAtom, moovAtom->GetChild(AP4_ATOM_TYPE_TRAK, trakIndex++)))) {

				AP4_HdlrAtom* hdlrAtom = AP4_DYNAMIC_CAST(AP4_HdlrAtom, tmpTrakAtom->FindChild("mdia/hdlr", false, false));

				//todo - handle duplicate track id's
				bool shouldDetatch = true;
				if(hdlrAtom)  {
					if(hdlrAtom->GetHandlerType() == AP4_HANDLER_TYPE_VIDE || hdlrAtom->GetHandlerType() == AP4_HANDLER_TYPE_SOUN) {
						//noop
						trakMediaAtomSecondFileId = tmpTrakAtom->GetId();
						shouldDetatch = false;
					}
				}

				if(shouldDetatch) {

					//clear out any trex here

					tmpTrakAtom->Detach();

				}
			}

			AP4_ContainerAtom* mvexToClear = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moovAtom->GetChild(AP4_ATOM_TYPE_MVEX));

			AP4_TrexAtom* tmpTrexAtom;
			int trexIndex = 0;
			while((tmpTrexAtom = AP4_DYNAMIC_CAST(AP4_TrexAtom, mvexToClear->GetChild(AP4_ATOM_TYPE_TREX, trexIndex++)))) {
				//ugh, there's not SetTrackId method...

				//tmpTrexAtom->AP4_TrexAtom()
				/*
				 *   AP4_TrexAtom(AP4_UI32 track_id,
			 AP4_UI32 default_sample_description_index,
			 AP4_UI32 default_sample_duration,
			 AP4_UI32 default_sample_size,
			 AP4_UI32 default_sample_flags);
				 */
				if(tmpTrexAtom->GetTrackId() != trakMediaAtomSecondFileId) {
					tmpTrexAtom->Detach();
				}
			}

			if(mvexAtomToCopy) {
				mvexAtomToCopy->Detach();

				//update the mvex/trex
				AP4_TrexAtom* tmpTrexAtom;
				int trexIndex = 0;
				while((tmpTrexAtom = AP4_DYNAMIC_CAST(AP4_TrexAtom, mvexAtomToCopy->GetChild(AP4_ATOM_TYPE_TREX, trexIndex++)))) {
					//ugh, there's not SetTrackId method...

					//tmpTrexAtom->AP4_TrexAtom()
					/*
					 *   AP4_TrexAtom(AP4_UI32 track_id,
                 AP4_UI32 default_sample_description_index,
                 AP4_UI32 default_sample_duration,
                 AP4_UI32 default_sample_size,
                 AP4_UI32 default_sample_flags);
					 */
					tmpTrexAtom->Detach();

					if(trakMediaAtomOriginalId && trakMediaAtomOriginalId == tmpTrexAtom->GetTrackId()) {
						tmpTrexAtom = new AP4_TrexAtom(tmpTrexAtom->GetTrackId() + 10,
								tmpTrexAtom->GetDefaultSampleDescriptionIndex(),
								tmpTrexAtom->GetDefaultSampleDuration(),
								tmpTrexAtom->GetDefaultSampleSize(),
								tmpTrexAtom->GetDefaultSampleFlags());
						mvexAtomToCopy->AddChild(tmpTrexAtom, trexIndex-1);
					}
				}
				moovAtom->AddChild(mvexAtomToCopy, -1);
			}
			//copy over our media track, then copy over our hint tracks
			//this track index is already offset by +10
			if(trakMediaAtomToCopy) {
				trakMediaAtomToCopy->Detach();

				moovAtom->AddChild(trakMediaAtomToCopy, -1);
			}

			//trakHintAtomToCopy
			//this track index is already offset by +10

			//looks like there is no actual hint data in these files, don't add in the hint tracks,..
#ifndef __DROP_HINT_TRACKS__
			for (itHint = trakHintAtomToCopy.begin(); itHint != trakHintAtomToCopy.end(); itHint++) {
				(*itHint)->Detach();
				moovAtom->AddChild(*itHint, -1);
			}
#endif
		}

		/**
		 * [moof] size=8+2460
			  [mfhd] size=12+4
				sequence number = 1
			  [traf] size=8+1540
				[tfhd] size=12+4, flags=20000
				  track ID = 2

		 */

		if((*it)->GetType() == AP4_ATOM_TYPE_MOOF) {
			moofSecondFile = AP4_DYNAMIC_CAST(AP4_ContainerAtom, *it);

            //clear out our traf's if tfhd id != trakMediaAtomSecondFileId

			AP4_ContainerAtom* tmpTrafToClean;
			int trafIdx = 0;
			while((tmpTrafToClean = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moofSecondFile->GetChild(AP4_ATOM_TYPE_TRAF, trafIdx++)))) {
                AP4_TfhdAtom* tfhdTempAtom = AP4_DYNAMIC_CAST(AP4_TfhdAtom, tmpTrafToClean->GetChild(AP4_ATOM_TYPE_TFHD));
                bool shouldDetachTrak = true;
                if(tfhdTempAtom && tfhdTempAtom->GetTrackId() == trakMediaAtomSecondFileId) {
                	trafSecondFile = tmpTrafToClean;
                	shouldDetachTrak = false;
                	AP4_TfdtAtom* tfdtSecondFile = AP4_DYNAMIC_CAST(AP4_TfdtAtom, tmpTrafToClean->GetChild(AP4_ATOM_TYPE_TFDT));
                	if(tfdtSecondFile) {
                		tfdtSecondFile->Detach();
                	}
                }
                if(shouldDetachTrak) {
                	tmpTrafToClean->Detach();
                }
			}

			for(itTraf = trafFirstFileList.begin(); itTraf != trafFirstFileList.end(); itTraf++) {
				//shift our track id's by +10
                AP4_TfhdAtom* tfhdTempAtom = AP4_DYNAMIC_CAST(AP4_TfhdAtom, (*itTraf)->GetChild(AP4_ATOM_TYPE_TFHD));
                if(tfhdTempAtom) {
                	tfhdTempAtom->SetTrackId(tfhdTempAtom->GetTrackId() + 10);
                } else {
                	//error
                }
				//remove our tfdt's
                AP4_TfdtAtom* tfdtTempAtom = AP4_DYNAMIC_CAST(AP4_TfdtAtom, (*itTraf)->GetChild(AP4_ATOM_TYPE_TFDT));
				tfdtTempAtom->Detach();
                
                (*itTraf)->Detach();
				moofSecondFile->AddChild(*itTraf);
			}

            if(trafSecondFile) {
                trunSecondFile = AP4_DYNAMIC_CAST(AP4_TrunAtom, trafSecondFile->GetChild(AP4_ATOM_TYPE_TRUN));
            }
		}

		if((*it)->GetType() == AP4_ATOM_TYPE_MDAT) {
			mdatSecondFileList.push_back(*it);
		}
	}

	if(moofSecondFile) {
		moofSecondFileAtom = AP4_DYNAMIC_CAST(AP4_Atom, moofSecondFile);
        if(trunSecondFile) {
            trunSecondFile->SetDataOffset((AP4_UI32)moofSecondFileAtom->GetSize()+AP4_ATOM_HEADER_SIZE);
        } else {
            //this shouldn't happen
        }
        
		//first file is written out last...
//		if(mdatSecondFile) {
//			trunFirstFile->SetDataOffset((AP4_UI32)moofSecondFileAtom->GetSize()+AP4_ATOM_HEADER_SIZE+mdatSecondFile->GetSize());
//		}

		//(AP4_UI32)moofSecondFileAtom->GetSize()+AP4_ATOM_HEADER_SIZE+
		for(itMdatSecond = mdatSecondFileList.begin(); itMdatSecond != mdatSecondFileList.end(); itMdatSecond++) {
			mdatSecondFileOffset += (*itMdatSecond)->GetSize() + AP4_ATOM_HEADER_SIZE;
		}
		mdatSecondFileOffset += moofSecondFileAtom->GetSize();
	}


	 //apend by hand and update
	for(itTrunFirst = trunFirstFileList.begin(); itTrunFirst != trunFirstFileList.end(); itTrunFirst++) {
		//trunFirstFile
		(*itTrunFirst)->SetDataOffset(mdatSecondFileOffset);
	}

    if(mdatFirstFileList.size()) {
    	isoBMFFList2.insert(isoBMFFList2.end(), mdatFirstFileList.begin(), mdatFirstFileList.end());
    }
    
    //push our packets to out output_stream writer, and we're done...
    
	for (it = isoBMFFList2.begin(); it != isoBMFFList2.end(); it++) {
        (*it)->Write(*output_stream);
	}

    __ISOBMFF_JOINER_INFO("Final output re-muxed MPU:");
    dumpFullMetadata(isoBMFFList2);

//    if (output_stream) output_stream->Release();

}

/**
 * todo: remove mmtp headers
 * //mfu's have time and un-timed additional DU headers, so recalc to_read_packet_len after doing (uint8_t*)extract
				//we use the du_header field
				//parse data unit header here based upon mpu timed flag

				* MFU mpu_fragmentation_indicator==1's are prefixed by the following box, need to remove
				*
				aligned(8) class MMTHSample {
				   unsigned int(32) sequence_number;
				   if (is_timed) {

					//interior block is 152 bits, or 19 bytes
					  signed int(8) trackrefindex;
					  unsigned int(32) movie_fragment_sequence_number
					  unsigned int(32) samplenumber;
					  unsigned int(8)  priority;
					  unsigned int(8)  dependency_counter;
					  unsigned int(32) offset;
					  unsigned int(32) length;
					//end interior block

					  multiLayerInfo();
				} else {
						//additional 2 bytes to chomp for non timed delivery
					  unsigned int(16) item_ID;
				   }
				}

				aligned(8) class multiLayerInfo extends Box("muli") {
				   bit(1) multilayer_flag;
				   bit(7) reserved0;
				   if (multilayer_flag==1) {
					   //32 bits
					  bit(3) dependency_id;
					  bit(1) depth_flag;
					  bit(4) reserved1;
					  bit(3) temporal_id;
					  bit(1) reserved2;
					  bit(4) quality_id;
					  bit(6) priority_id;
				   }  bit(10) view_id;
				   else{
					   //16bits
					  bit(6) layer_id;
					  bit(3) temporal_id;
					  bit(7) reserved3;
				} }




					//MMTHSample does not subclass box...
						//buf = (uint8_t*)extract(buf, &mmthsample_len, 1);
						buf = (uint8_t*)extract(buf, mmthsample_sequence_number, 4);

						uint8_t mmthsample_timed_block[19];
						buf = (uint8_t*)extract(buf, mmthsample_timed_block, 19);

						//read multilayerinfo
						uint8_t multilayerinfo_box_length[4];
						uint8_t multilayerinfo_box_name[4];
						uint8_t multilayer_flag;

						buf = (uint8_t*)extract(buf, multilayerinfo_box_length, 4);
						buf = (uint8_t*)extract(buf, multilayerinfo_box_name, 4);

						buf = (uint8_t*)extract(buf, &multilayer_flag, 1);

						int is_multilayer = (multilayer_flag >> 7) & 0x01;
						//if MSB is 1, then read multilevel struct, otherwise just pull layer info...
						if(is_multilayer) {
							uint8_t multilayer_data_block[4];
							buf = (uint8_t*)extract(buf, multilayer_data_block, 4);

						} else {
							uint8_t multilayer_layer_id_temporal_id[2];
							buf = (uint8_t*)extract(buf, multilayer_layer_id_temporal_id, 2);
						}

				*/




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

	if(_ISOBMFFTRACKJOINER_DEBUG_ENABLED) {
		AP4_ByteStream* boxDumpConsoleOutput = NULL;
		AP4_FileByteStream::Create("-stderr", AP4_FileByteStream::STREAM_MODE_WRITE, boxDumpConsoleOutput);
		AP4_AtomInspector* inspector = new AP4_PrintInspector(*boxDumpConsoleOutput);
		inspector->SetVerbosity(3);

		std::list<AP4_Atom*>::iterator it;
		for (it = atomList.begin(); it != atomList.end(); it++) {
			(*it)->Inspect(*inspector);
		}

		if (boxDumpConsoleOutput) boxDumpConsoleOutput->Release();
		delete inspector;
	}

}


void printBoxType(AP4_Atom* atom) {

    AP4_UI32 m_Type = atom->GetType();
    char name[5];
    AP4_FormatFourCharsPrintable(name, m_Type);

    name[4] = '\0';
    __ISOBMFF_JOINER_DEBUG("printBoxType: atom type: %s, size: %llu", name, atom->GetSize());
}

