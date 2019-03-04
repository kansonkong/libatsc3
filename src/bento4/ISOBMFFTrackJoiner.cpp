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
#define __DROP_SIDX_BOX__

//we don't want to do this, as some route streams need the tfdt base media decode time
//#define __DROP_TFDT_BOX__


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

//	//first, we map our 2 input files into uint8_t* payloads,
//	ISOBMFFTrackJoinerFileResouces_t* fileResources = loadFileResources(argv[1], argv[2]);
//
//	//then we setup our output writer
//	const char* output_filename = "jjout.m4v";
//	AP4_ByteStream* output_stream = NULL;
//	result = AP4_FileByteStream::Create(output_filename, AP4_FileByteStream::STREAM_MODE_WRITE,output_stream);
//
//	//and remux into one unified fragment.  if you have already sent the initn b
//	parseAndBuildJoinedBoxes(fileResources, output_stream);

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
   // parseAndBuildJoinedBoxesFromMemory(audio_output_buffer->p_buffer, audio_output_buffer->i_pos, video_output_buffer->p_buffer, video_output_buffer->i_pos, memoryOutputByteStream);
    parseAndBuildJoinedBoxes_from_lls_sls_monitor_output_buffer(lls_sls_monitor_output_buffer, output_stream_p);
}


void parseAndBuildJoinedBoxes(ISOBMFFTrackJoinerFileResouces* isoBMFFTrackJoinerFileResouces, AP4_ByteStream* output_stream) {

   // parseAndBuildJoinedBoxesFromMemory(isoBMFFTrackJoinerFileResouces->file1_payload, isoBMFFTrackJoinerFileResouces->file1_size, isoBMFFTrackJoinerFileResouces->file2_payload, isoBMFFTrackJoinerFileResouces->file2_size, output_stream);
}



void parseAndBuildJoinedBoxes_from_lls_sls_monitor_output_buffer(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, AP4_MemoryByteStream** output_stream_p) {

	AP4_Result   result;

	AP4_ContainerAtom* audio_mvexAtomToCopy = NULL;
	AP4_TrakAtom* audio_trakMediaAtomToCopy = NULL;

#ifndef __DROP_HINT_TRACKS__

	//only used if recombining hint tracks
	std::list<AP4_TrakAtom*> audio_trakHintAtomToCopyList;
	std::list<AP4_TrakAtom*>::iterator itHint;
#endif

	std::list<AP4_ContainerAtom*> audio_trafList;
	std::list<AP4_ContainerAtom*>::iterator itTraf;

	std::list<AP4_TrunAtom*> audio_trunList;
	std::list<AP4_TrunAtom*>::iterator itTrunFirst;

	std::list<AP4_Atom*> audio_mdatList;
	std::list<AP4_Atom*>::iterator itMdatFirst;


	AP4_AtomParent* video_moofAtomParent = NULL;
	AP4_Atom* video_moofAtom = NULL;

	AP4_ContainerAtom* video_trafAtom = NULL;
	AP4_TrunAtom* video_trunAtom = NULL;

	std::list<AP4_Atom*> video_mdatList;
	std::list<AP4_Atom*>::iterator video_mdatIt;
	uint64_t video_mdatFileOffset = 0;

	block_t* audio_output_buffer = lls_sls_monitor_output_buffer_copy_audio_full_isobmff_box(lls_sls_monitor_output_buffer);
	block_t* video_output_buffer = lls_sls_monitor_output_buffer_copy_video_full_isobmff_box(lls_sls_monitor_output_buffer);



	if(!audio_output_buffer || !video_output_buffer) {
		__ISOBMFF_JOINER_INFO("setting *output_stream_p to null, audio_output_buffer: %p, video_output_buffer: %p", audio_output_buffer, video_output_buffer);
		*output_stream_p = NULL;
		return;
	}

	//we shouldn't be bigger than this for our return..
	AP4_DataBuffer* dataBuffer = new AP4_DataBuffer(audio_output_buffer->i_pos + video_output_buffer->i_pos );
	AP4_MemoryByteStream* memoryOutputByteStream = new AP4_MemoryByteStream(dataBuffer);

	*output_stream_p = memoryOutputByteStream;

	list<AP4_Atom*> audio_isobmff_atom_list  = ISOBMFFTrackParse(audio_output_buffer);

	list<AP4_Atom*> video_isobmff_atom_list =  ISOBMFFTrackParse(video_output_buffer);

    __ISOBMFF_JOINER_DEBUG("Dumping audio box: size: %u", audio_output_buffer->i_pos);
	dumpFullMetadata(audio_isobmff_atom_list);

	__ISOBMFF_JOINER_DEBUG("Dumping video box: %u", video_output_buffer->i_pos);
	dumpFullMetadata(video_isobmff_atom_list);

	block_Release(&audio_output_buffer);
	block_Release(&video_output_buffer);


	/**
     top level AP4_ContainerAtoms:
     
	bento4/ISOBMFFTrackJoiner.cpp:363:DEBUG :printBoxType: atom type: ftyp, size: 36
	bento4/ISOBMFFTrackJoiner.cpp:363:DEBUG :printBoxType: atom type: moov, size: 608
	bento4/ISOBMFFTrackJoiner.cpp:363:DEBUG :printBoxType: atom type: styp, size: 24
	bento4/ISOBMFFTrackJoiner.cpp:363:DEBUG :printBoxType: atom type: moof, size: 1220
	bento4/ISOBMFFTrackJoiner.cpp:363:DEBUG :printBoxType: atom type: mdat, size: 96765
	 
     
     remove sidx by defining __DROP_SIDX_BOX__

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



    /**
     to postion at end:
     
     [hdlr] size=12+40
     handler_type = hint
     handler_name = Bento4 Hint Handler
     **/

	//from isoBMFFList1 list
	std::list<AP4_Atom*>::iterator it;
	for (it = audio_isobmff_atom_list.begin(); it != audio_isobmff_atom_list.end(); it++) {

		//In the moov box->get a ref for the trak box
		if((*it)->GetType() == AP4_ATOM_TYPE_MOOV) {
			AP4_MoovAtom* moovAtom = AP4_DYNAMIC_CAST(AP4_MoovAtom, *it);
			audio_mvexAtomToCopy = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moovAtom->GetChild(AP4_ATOM_TYPE_MVEX));

			AP4_TrakAtom* tmpTrakAtom;
			int trakIndex = 0;
			while((tmpTrakAtom = AP4_DYNAMIC_CAST(AP4_TrakAtom, moovAtom->GetChild(AP4_ATOM_TYPE_TRAK, trakIndex++)))) {

				AP4_HdlrAtom* hdlrAtom = AP4_DYNAMIC_CAST(AP4_HdlrAtom, tmpTrakAtom->FindChild("mdia/hdlr", false, false));

				//todo - handle duplicate track id's

				if(hdlrAtom && hdlrAtom->GetHandlerType() == AP4_HANDLER_TYPE_SOUN) {

					lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.track_id = tmpTrakAtom->GetId();
					audio_trakMediaAtomToCopy = tmpTrakAtom;

				} else if(hdlrAtom->GetHandlerType() == AP4_HANDLER_TYPE_HINT) {
#ifndef __DROP_HINT_TRACKS__

						tmpTrakAtom->SetId(tmpTrakAtom->GetId()+10);

						audio_trakHintAtomToCopyList.push_back(tmpTrakAtom);

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

			}
		}

		if((*it)->GetType() == AP4_ATOM_TYPE_MOOF) {
			AP4_AtomParent* moofAtom = AP4_DYNAMIC_CAST(AP4_ContainerAtom, *it);
			AP4_ContainerAtom* trafContainerAtom = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moofAtom->GetChild(AP4_ATOM_TYPE_TRAF));
			audio_trafList.push_back(trafContainerAtom);
			audio_trunList.push_back(AP4_DYNAMIC_CAST(AP4_TrunAtom, trafContainerAtom->GetChild(AP4_ATOM_TYPE_TRUN)));
		}

		if((*it)->GetType() == AP4_ATOM_TYPE_MDAT) {
			audio_mdatList.push_back(*it);
		}
	}

	//now go the other way...
	for (it = video_isobmff_atom_list.begin(); it != video_isobmff_atom_list.end(); it++) {

		//In the moov box->get a ref for the trak box
		if((*it)->GetType() == AP4_ATOM_TYPE_MOOV) {
			AP4_MoovAtom* moovAtom = AP4_DYNAMIC_CAST(AP4_MoovAtom, *it);

			//remove our hints
			AP4_TrakAtom* tmpTrakAtom;
			int trakIndex = 0;
			while((tmpTrakAtom = AP4_DYNAMIC_CAST(AP4_TrakAtom, moovAtom->GetChild(AP4_ATOM_TYPE_TRAK, trakIndex++)))) {

				AP4_HdlrAtom* hdlrAtom = AP4_DYNAMIC_CAST(AP4_HdlrAtom, tmpTrakAtom->FindChild("mdia/hdlr", false, false));

				bool shouldDetatch = true;
				if(hdlrAtom && hdlrAtom->GetHandlerType() == AP4_HANDLER_TYPE_VIDE) {
					lls_sls_monitor_output_buffer->video_output_buffer_isobmff.track_id = tmpTrakAtom->GetId();
					shouldDetatch = false;
				}

				if(shouldDetatch) {
					//clear out any hint tracks
					tmpTrakAtom->Detach();
				}
			}

			AP4_ContainerAtom* mvexToClear = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moovAtom->GetChild(AP4_ATOM_TYPE_MVEX));

			AP4_TrexAtom* tmpTrexAtom;
			int trexIndex = 0;
			while((tmpTrexAtom = AP4_DYNAMIC_CAST(AP4_TrexAtom, mvexToClear->GetChild(AP4_ATOM_TYPE_TREX, trexIndex++)))) {
				if(tmpTrexAtom->GetTrackId() != lls_sls_monitor_output_buffer->video_output_buffer_isobmff.track_id) {
					tmpTrexAtom->Detach();
				}
			}

			if(audio_mvexAtomToCopy) {
				audio_mvexAtomToCopy->Detach();
				//update the mvex/trex
				moovAtom->AddChild(audio_mvexAtomToCopy, -1);
			}

			if(audio_trakMediaAtomToCopy) {
				audio_trakMediaAtomToCopy->Detach();
				moovAtom->AddChild(audio_trakMediaAtomToCopy, -1);
			}

#ifndef __DROP_HINT_TRACKS__

			//trakHintAtomToCopy
			//this track index is already offset by +10

			//looks like there is no actual hint data in these files, don't add in the hint tracks,..

			for (itHint = audio_trakHintAtomToCopyList.begin(); itHint != audio_trakHintAtomToCopyList.end(); itHint++) {
				(*itHint)->Detach();
				moovAtom->AddChild(*itHint, -1);
			}
#endif
		}

		if((*it)->GetType() == AP4_ATOM_TYPE_MOOF) {
			video_moofAtomParent = AP4_DYNAMIC_CAST(AP4_ContainerAtom, *it);

            //clear out our traf's if tfhd id != trakMediaAtomSecondFileId

			AP4_ContainerAtom* tmpTrafToClean;
			int trafIdx = 0;
			while((tmpTrafToClean = AP4_DYNAMIC_CAST(AP4_ContainerAtom, video_moofAtomParent->GetChild(AP4_ATOM_TYPE_TRAF, trafIdx++)))) {
                AP4_TfhdAtom* tfhdTempAtom = AP4_DYNAMIC_CAST(AP4_TfhdAtom, tmpTrafToClean->GetChild(AP4_ATOM_TYPE_TFHD));
                bool shouldDetachTrak = true;
                if(tfhdTempAtom && tfhdTempAtom->GetTrackId() == lls_sls_monitor_output_buffer->video_output_buffer_isobmff.track_id) {
                	video_trafAtom = tmpTrafToClean;
                	shouldDetachTrak = false;

#ifdef __DROP_TFDT_BOX__
                	AP4_TfdtAtom* tfdtSecondFile = AP4_DYNAMIC_CAST(AP4_TfdtAtom, tmpTrafToClean->GetChild(AP4_ATOM_TYPE_TFDT));
                	if(tfdtSecondFile) {
                		tfdtSecondFile->Detach();
                	}
#endif
                }
                if(shouldDetachTrak) {
                	tmpTrafToClean->Detach();
                }
			}

			for(itTraf = audio_trafList.begin(); itTraf != audio_trafList.end(); itTraf++) {

                AP4_TfhdAtom* tfhdTempAtom = AP4_DYNAMIC_CAST(AP4_TfhdAtom, (*itTraf)->GetChild(AP4_ATOM_TYPE_TFHD));
                //shift our track id's by +10 if we are not the audio track id
                if(tfhdTempAtom && tfhdTempAtom->GetTrackId() != lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.track_id) {

                	tfhdTempAtom->SetTrackId(tfhdTempAtom->GetTrackId() + 10);
                }

#ifdef __DROP_TFDT_BOX__
                //remove our tfdt's
                AP4_TfdtAtom* tfdtTempAtom = AP4_DYNAMIC_CAST(AP4_TfdtAtom, (*itTraf)->GetChild(AP4_ATOM_TYPE_TFDT));

                if(tfdtTempAtom) {
					tfdtTempAtom->Detach();
				}
#endif
                
                (*itTraf)->Detach();
				video_moofAtomParent->AddChild(*itTraf);
			}

			/**
			 * TODO: null out any empty broken fragments as per ISO23008-14
			 */

            if(video_trafAtom) {
                video_trunAtom = AP4_DYNAMIC_CAST(AP4_TrunAtom, video_trafAtom->GetChild(AP4_ATOM_TYPE_TRUN));
                if(video_trunAtom) {
                	//get our first sample duration
                	const AP4_Array<AP4_TrunAtom::Entry>& video_sampleEntries = video_trunAtom->GetEntries();
                	bool has_found_sample_duration = false;

                	for(int i=0; !has_found_sample_duration && i < video_sampleEntries.ItemCount(); i++) {
                		if(video_sampleEntries[i].sample_duration) {
                			lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fps_num = video_sampleEntries[i].sample_duration;
                			lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fps_denom = 1000000;
                			has_found_sample_duration = true;
                		}
                	}
                }
            }
		}

		if((*it)->GetType() == AP4_ATOM_TYPE_MDAT) {
			video_mdatList.push_back(*it);
		}
	}

	if(video_moofAtomParent) {
		video_moofAtom = AP4_DYNAMIC_CAST(AP4_Atom, video_moofAtomParent);
        if(video_trunAtom) {
            video_trunAtom->SetDataOffset((AP4_UI32)video_moofAtom->GetSize()+AP4_ATOM_HEADER_SIZE);
            video_trunAtom->GetEntries();
        } else {
            //this shouldn't happen
        }

		//(AP4_UI32)moofSecondFileAtom->GetSize()+AP4_ATOM_HEADER_SIZE+
		for(video_mdatIt = video_mdatList.begin(); video_mdatIt != video_mdatList.end(); video_mdatIt++) {
			video_mdatFileOffset += (*video_mdatIt)->GetSize() + AP4_ATOM_HEADER_SIZE;
		}
		video_mdatFileOffset += video_moofAtom->GetSize();
	}


	//apend by hand and update
	for(itTrunFirst = audio_trunList.begin(); itTrunFirst != audio_trunList.end(); itTrunFirst++) {
		//trunFirstFile
		(*itTrunFirst)->SetDataOffset(video_mdatFileOffset);
	}

    if(audio_mdatList.size()) {
    	video_isobmff_atom_list.insert(video_isobmff_atom_list.end(), audio_mdatList.begin(), audio_mdatList.end());
    }
    
    //push our packets to out output_stream writer, and we're done...
    
	for (it = video_isobmff_atom_list.begin(); it != video_isobmff_atom_list.end(); it++) {
		bool should_write_box = true;

#ifdef __DROP_SIDX_BOX__
		if((*it)->GetType() == AP4_ATOM_TYPE_SIDX) {
			should_write_box = false;
		}
#endif
		if(should_write_box) {
			(*it)->Write(*memoryOutputByteStream);
		}
	}

    __ISOBMFF_JOINER_INFO("Final output re-muxed MPU:");
    dumpFullMetadata(video_isobmff_atom_list);

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




list<AP4_Atom*> ISOBMFFTrackParse(block_t* isobmff_track_block) {

	__ISOBMFF_JOINER_DEBUG("::ISOBMFFTrackParse: payload size is: %u", isobmff_track_block->i_pos);

	list<AP4_Atom*> atomList;
    AP4_Atom* atom;

    AP4_MemoryByteStream* memoryInputByteStream = new AP4_MemoryByteStream(isobmff_track_block->p_buffer, isobmff_track_block->i_pos);
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

