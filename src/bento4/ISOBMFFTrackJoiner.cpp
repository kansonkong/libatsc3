/*
 * ISOBMFFTrackJoiner.cpp
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 *
 *      ISOBMFF ftyp/moov/moof/mdat track joiner
 */


#include "ISOBMFFTrackJoiner.h"

#include "../atsc3_utils.h"

int _ISOBMFFTRACKJOINER_DEBUG_ENABLED = 0;
int _ISOBMFFTRACKJOINER_TRACE_ENABLED = 0;


/*****************************************************************
|
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

//glue structs...

typedef struct AP4_Atom_And_Offset {
	AP4_Atom* 	atom;
	uint32_t	start_offset;
	uint32_t	end_offset;
	bool 		write_manually;
} AP4_Atom_And_Offset_t;


list<AP4_Atom_And_Offset*> ISOBMFFTrackParseAndBuildOffset(block_t* isobmff_track_block);

/*----------------------------------------------------------------------
 |   constants
 +---------------------------------------------------------------------*/
#define BANNER "ISOBMFFTrackJoiner - libatsc3 player output\n"


#define __DROP_HINT_TRACKS__
#define __DROP_SIDX_BOX__

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



void dumpFullMetadataAndOffsets(list<AP4_Atom_And_Offset_t*> atomList) {

	if(_ISOBMFFTRACKJOINER_DEBUG_ENABLED) {
		AP4_ByteStream* boxDumpConsoleOutput = NULL;
		AP4_FileByteStream::Create("isobmff.debug", AP4_FileByteStream::STREAM_MODE_WRITE, boxDumpConsoleOutput);
		AP4_AtomInspector* inspector = new AP4_PrintInspector(*boxDumpConsoleOutput);
		inspector->SetVerbosity(3);

		std::list<AP4_Atom_And_Offset_t*>::iterator it;
		for (it = atomList.begin(); it != atomList.end(); it++) {
			(*it)->atom->Inspect(*inspector);
		}

		boxDumpConsoleOutput->WriteString("---\r\n");

		if (boxDumpConsoleOutput) boxDumpConsoleOutput->Release();
		delete inspector;
	}

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
//
//trun_sample_entry_vector_t* parseMoofBoxForTrunSampleEntries(block_t* moof_box) {
//
//	list<AP4_Atom*> isobmff_atom_list  = ISOBMFFTrackParse(moof_box);
//	AP4_AtomParent* moofAtomParent = NULL;
//	AP4_TrunAtom* trunAtom = NULL;
//
//	trun_sample_entry_vector_t* trun_sample_entry_vector = (trun_sample_entry_vector_t*) calloc(1, sizeof(trun_sample_entry_vector_t));
//
//	std::list<AP4_Atom*>::iterator it;
//	for (it = isobmff_atom_list.begin(); it != isobmff_atom_list.end(); it++) {
//
//		if((*it)->GetType() == AP4_ATOM_TYPE_MOOF) {
//			moofAtomParent = AP4_DYNAMIC_CAST(AP4_ContainerAtom, *it);
//
//			AP4_ContainerAtom* tmpTrafToCheck;
//			int trafIdx = 0;
//			while((tmpTrafToCheck = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moofAtomParent->GetChild(AP4_ATOM_TYPE_TRAF, trafIdx++)))) {
//				 trunAtom = AP4_DYNAMIC_CAST(AP4_TrunAtom, tmpTrafToCheck->GetChild(AP4_ATOM_TYPE_TRUN));
//				 if(trunAtom) {
//					//check for first sample duration
//					const AP4_Array<AP4_TrunAtom::Entry>& sampleEntries = trunAtom->GetEntries();
//					if(sampleEntries.ItemCount() > 0 && sampleEntries[0].sample_size) {
//						trun_sample_entry_vector->data = (trun_sample_entry_t**) calloc(sampleEntries.ItemCount(), sizeof(trun_sample_entry_vector->data));
//
//						for(int i=0; i < sampleEntries.ItemCount(); i++) {
//							trun_sample_entry_vector->data[i] = (trun_sample_entry_t*) calloc(1, sizeof(trun_sample_entry_t));
//							trun_sample_entry_vector->data[i]->sample_composition_time_offset = sampleEntries[i].sample_composition_time_offset;
//							trun_sample_entry_vector->data[i]->sample_duration = sampleEntries[i].sample_duration;
//							trun_sample_entry_vector->data[i]->sample_flags = sampleEntries[i].sample_flags;
//							trun_sample_entry_vector->data[i]->sample_size = 0; //sampleEntries[i].sample_size;
//							trun_sample_entry_vector->size++;
//						}
//					}
//				}
//			}
//		}
//	}
//
//	return trun_sample_entry_vector;
//}

/**
 *
 * ISOBMFF_track_joiner_monitor_output_buffer_parse_and_build_joined_alc_boxes
 *
 * prebuilt moof/trun with alc for single isobmff fragmented player output
 *
 */
void ISOBMFF_track_joiner_monitor_output_buffer_parse_and_build_joined_alc_boxes(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, AP4_MemoryByteStream** output_stream_p)
{
	block_t* audio_output_buffer = lls_sls_monitor_output_buffer_copy_alc_full_isobmff_box(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff);
	block_t* video_output_buffer = lls_sls_monitor_output_buffer_copy_alc_full_isobmff_box(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff);

    parseAndBuildJoinedBoxes_from_lls_sls_monitor_output_buffer(lls_sls_monitor_output_buffer, audio_output_buffer, video_output_buffer, output_stream_p);
}


/*
 * ISOBMFF_track_joiner_monitor_output_buffer_parse_and_build_joined_mmt_boxes
 *
 * dynamically built moof/trun with mmt for single isobmff fragmented player output (MPU reassembly)
 *
 */

#define __ENABLE_OOO_MFU_REBUILD__ false

void ISOBMFF_track_joiner_monitor_output_buffer_parse_and_build_joined_mmt_boxes(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, AP4_MemoryByteStream** output_stream_p)
{
	/** tood - magic happens here **/
	block_t* audio_output_buffer = NULL; // = lls_sls_monitor_output_buffer_copy_alc_full_isobmff_box(lls_sls_monitor_output_buffer);
	block_t* video_output_buffer = NULL; // = lls_sls_monitor_output_buffer_copy_video_alc_full_isobmff_box(lls_sls_monitor_output_buffer);

	if(__ENABLE_OOO_MFU_REBUILD__) {

	} else {
		audio_output_buffer = lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box(&lls_sls_monitor_output_buffer->audio_output_buffer_isobmff);
		video_output_buffer = lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box(&lls_sls_monitor_output_buffer->video_output_buffer_isobmff);
	}

    parseAndBuildJoinedBoxes_from_lls_sls_monitor_output_buffer(lls_sls_monitor_output_buffer, audio_output_buffer, video_output_buffer, output_stream_p);
}


void ISOBMFF_track_joiner_monitor_output_buffer_parse_and_build_joined_mmt_rebuilt_boxes(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, AP4_MemoryByteStream** output_stream_p)
{
	/** tood - magic happens here **/
	block_t* audio_output_buffer = block_Duplicate(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mmt_mpu_rebuilt);
	block_t* video_output_buffer = block_Duplicate(lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mmt_mpu_rebuilt);


    parseAndBuildJoinedBoxes_from_lls_sls_monitor_output_buffer(lls_sls_monitor_output_buffer, audio_output_buffer, video_output_buffer, output_stream_p);
}


//todo: add in mpu_presentation time logic...
uint32_t ISOBMFF_rebuild_moof_from_sample_data(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff, AP4_MemoryByteStream** output_stream_p) {

	block_t* temp_output_buffer = lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box_no_patching_trailing_mdat(lls_sls_monitor_buffer_isobmff);
	if(!temp_output_buffer) {
		__ISOBMFF_JOINER_INFO("rebuilding moof from sample, lls_sls_monitor_output_buffer_copy_mmt_moof_from_flow_isobmff_box_no_patching_trailing_mdat returned null");
		return 0;
	}

	AP4_DataBuffer* dataBuffer = new AP4_DataBuffer(temp_output_buffer->i_pos);
	AP4_MemoryByteStream* memoryOutputByteStream = new AP4_MemoryByteStream(dataBuffer);

	*output_stream_p = memoryOutputByteStream;

	uint32_t final_mdat_size = 0;

	list<AP4_Atom_And_Offset_t*> isobmff_atom_list = ISOBMFFTrackParseAndBuildOffset(temp_output_buffer);
	std::list<AP4_Atom_And_Offset_t*>::iterator it;
	AP4_Atom* moofAtom = NULL;

	for (it = isobmff_atom_list.begin(); it != isobmff_atom_list.end(); it++) {
		AP4_Atom* top_level_atom = (*it)->atom;
		if(top_level_atom->GetType() == AP4_ATOM_TYPE_MOOF) {

			AP4_AtomParent* moofAtom = AP4_DYNAMIC_CAST(AP4_ContainerAtom, top_level_atom);
			AP4_ContainerAtom* trafContainerAtom = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moofAtom->GetChild(AP4_ATOM_TYPE_TRAF));
			AP4_TfhdAtom* tfhdAtom = AP4_DYNAMIC_CAST(AP4_TfhdAtom, trafContainerAtom->GetChild(AP4_ATOM_TYPE_TFHD));
			AP4_TrunAtom* trunAtom = AP4_DYNAMIC_CAST(AP4_TrunAtom, trafContainerAtom->GetChild(AP4_ATOM_TYPE_TRUN));

			if (lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.count) {
				AP4_Array<AP4_TrunAtom::Entry>& to_walk_entries = trunAtom->UseEntries();

				for (int i = 0;	i < lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.count; i++) {
					trun_sample_entry_t* trun_sample_entry = lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.data[i];

					if(to_walk_entries[i].sample_size != trun_sample_entry->sample_length) {
						__ISOBMFF_JOINER_INFO("REBUILD MOOF: setting sample %u from size: %u to size: %u," i, to_walk_entries[i].sample_size, trun_sample_entry->sample_length);
					}
					to_walk_entries[i].sample_size = trun_sample_entry->sample_length;
					final_mdat_size += to_walk_entries[i].sample_size;
				}

				//handle any missing samples by zeroing out size
				for(int j = lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.count-1; j < to_walk_entries.ItemCount(); j++) {
					__ISOBMFF_JOINER_INFO("REBUILD MOOF: end of trun, setting sample %u from size: %u to size: %u," j, to_walk_entries[j].sample_size, 0);

					to_walk_entries[j].sample_size = 0;
				}
			}
		}
	}

	/**todo:
	 * if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_set && lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time_set) {

		//fractional component is already at 1000000 (uS), so just multiply and add the seconds...
		uint64_t audio_mpu_presentation_time_s = lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_s * 1000000;
		uint64_t audio_mpu_presentation_time_ms = lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_us % 1000000; //just to be safe..
		uint64_t audio_mpu_presentation_time_final_uS =  audio_mpu_presentation_time_s + audio_mpu_presentation_time_ms;

		audio_tfdt_atom_mdhd_timescale = new AP4_TfdtAtom(1, audio_mpu_presentation_time_final_uS);
	 *
	 */

	//re-write out our isobmff track..

	for (it = isobmff_atom_list.begin(); it != isobmff_atom_list.end(); it++) {
		AP4_Atom* top_level_atom = (*it)->atom;
		top_level_atom->Write(*memoryOutputByteStream);

	}

	block_Release(&temp_output_buffer);


	return final_mdat_size;
}

uint32_t __rebuild_trun_sample_box(AP4_TrunAtom* temp_trunAtom, lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff) {
	return 0;

	uint32_t old_fragments_size = 0;
	uint32_t new_fragments_size = 0;
	uint32_t total_sample_duration = 0;

	if (lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.count) {
		AP4_Array<AP4_TrunAtom::Entry>& to_walk_entries = temp_trunAtom->UseEntries();
		AP4_Array<AP4_TrunAtom::Entry> rebuilt_container;

		bool has_updated_entries = false;

		for (int i = 0;	i < lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.count; i++) {
			trun_sample_entry_t* trun_sample_entry = lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.data[i];

			uint32_t last_sample_size = to_walk_entries[i].sample_size;
			old_fragments_size += last_sample_size;
			total_sample_duration += to_walk_entries[i].sample_duration;

			if (trun_sample_entry->to_remove_sample_entry ||
				!trun_sample_entry->has_matching_sample) {
				__ISOBMFF_JOINER_INFO("track_id: %u: MISSING SAMPLE: setting sample %u from size: %u to size: %u, duration: %u:, sample_flags: 0x%x", lls_sls_monitor_buffer_isobmff->track_id, i, last_sample_size, to_walk_entries[i].sample_size, total_sample_duration, to_walk_entries[i].sample_flags);

				has_updated_entries = true;
				to_walk_entries[i].sample_size = 0;
				to_walk_entries[i].sample_composition_time_offset = 0;
				to_walk_entries[i].sample_flags |= 0x000100;
				to_walk_entries[i].sample_flags |= 0x000200;

				uint32_t empty_sample_duration = to_walk_entries[i].sample_duration;

//				if(i > 0) {
//					to_walk_entries[i].sample_duration = 0;
//					to_walk_entries[i-1].sample_duration += empty_sample_duration;
//				}

			} else if (true) { //lls_sls_monitor_buffer_isobmff->moof_box_is_from_last_mpu) {
				__ISOBMFF_JOINER_INFO("track_id: %u: REBUILD MOOF: setting sample %u from size: %u to size: %u, duration: %u, sample_flags: 0x%x", lls_sls_monitor_buffer_isobmff->track_id, i, last_sample_size, to_walk_entries[i].sample_size, total_sample_duration, to_walk_entries[i].sample_flags);

				has_updated_entries = true;
				to_walk_entries[i].sample_size = trun_sample_entry->sample_length;
				to_walk_entries[i].sample_flags |= 0x000200;

			}
			rebuilt_container.Append(to_walk_entries[i]);
		}

		for(int i=0; i < rebuilt_container.ItemCount(); i++) {
			new_fragments_size += rebuilt_container[i].sample_size;
		}

		//recompute our box and mdat size
		if (has_updated_entries) {
			to_walk_entries.Clear();
			temp_trunAtom->SetEntries(rebuilt_container);

			__ISOBMFF_JOINER_INFO("track_id: %u trun: contains %u entries, sample size was: %u, now sample size: %u",
					lls_sls_monitor_buffer_isobmff->track_id, rebuilt_container.ItemCount(), old_fragments_size, new_fragments_size);
		} else {
			__ISOBMFF_JOINER_INFO("track_id: %u, recompute mdat size: sample size was: %u, now sample size: %u",
					lls_sls_monitor_buffer_isobmff->track_id, old_fragments_size, new_fragments_size);
		}
	}

	return new_fragments_size;
}

list<AP4_Atom_And_Offset*> ISOBMFFTrackParseAndBuildOffset(block_t* isobmff_track_block) {

	__ISOBMFF_JOINER_DEBUG("::ISOBMFFTrackParse: payload size is: %u", isobmff_track_block->i_pos);

	list<AP4_Atom_And_Offset_t*> atomList;
    AP4_Atom* atom;

    AP4_MemoryByteStream* memoryInputByteStream = new AP4_MemoryByteStream(isobmff_track_block->p_buffer, isobmff_track_block->i_pos);

    // inspect the atoms one by one
    AP4_Position start_position;
    AP4_Position end_position;

    AP4_DefaultAtomFactory atom_factory;

    memoryInputByteStream->Tell(start_position);

    while (atom_factory.CreateAtomFromStream(*memoryInputByteStream, atom) == AP4_SUCCESS) {
        memoryInputByteStream->Tell(end_position);
        AP4_Atom_And_Offset_t* ap4_atom_and_offset = (AP4_Atom_And_Offset_t*)calloc(1, sizeof(AP4_Atom_And_Offset_t));
        ap4_atom_and_offset->atom = atom;
        ap4_atom_and_offset->start_offset = start_position;
        ap4_atom_and_offset->end_offset = end_position;

        atomList.push_back(ap4_atom_and_offset);
        printBoxType(atom);

        //re-seek if our printBoxType processed deeper into the box hierarchy
        memoryInputByteStream->Seek(end_position);
        start_position = end_position;
    }

    if (memoryInputByteStream) memoryInputByteStream->Release();

    return atomList;
}


/**
 *
 * create dynamic moof box for mmt mfu support
 *
 * todo: validate  null out any empty broken fragments as per ISO23008-14
 *
 *
 *

        			video_mdat_size_new = __rebuild_trun_sample_box(video_trunAtom, &lls_sls_monitor_output_buffer->video_output_buffer_isobmff);
 *
 *
 * if(top_level_atom->GetType() == AP4_ATOM_TYPE_MOOF) {
			AP4_AtomParent* moofAtom = AP4_DYNAMIC_CAST(AP4_ContainerAtom, top_level_atom);
			AP4_ContainerAtom* trafContainerAtom = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moofAtom->GetChild(AP4_ATOM_TYPE_TRAF));
			//tfhd

            AP4_TfhdAtom* tfhdTempAtom = AP4_DYNAMIC_CAST(AP4_TfhdAtom, trafContainerAtom->GetChild(AP4_ATOM_TYPE_TFHD));
            if(tfhdTempAtom && audio_track_id_to_remap) {
            	tfhdTempAtom->SetTrackId(audio_track_id_to_remap);
            }

			audio_trafList.push_back(trafContainerAtom);

			AP4_TrunAtom* temp_trunAtom = AP4_DYNAMIC_CAST(AP4_TrunAtom, trafContainerAtom->GetChild(AP4_ATOM_TYPE_TRUN));
			audio_mdat_size_new = __rebuild_trun_sample_box(temp_trunAtom, &lls_sls_monitor_output_buffer->audio_output_buffer_isobmff);

			audio_trunList.push_back(temp_trunAtom);
		}
 *
 */

/*
 *
 * build into single mdat box
 *
 * from either alc:
 *
	block_t* audio_output_buffer = lls_sls_monitor_output_buffer_copy_audio_alc_full_isobmff_box(lls_sls_monitor_output_buffer);
	block_t* video_output_buffer = lls_sls_monitor_output_buffer_copy_video_alc_full_isobmff_box(lls_sls_monitor_output_buffer);

	or mmt recon:

	...
	block_t* audio_output_buffer = lls_sls_monitor_output_buffer_copy_audio_..._full_isobmff_box(lls_sls_monitor_output_buffer);
	block_t* video_output_buffer = lls_sls_monitor_output_buffer_copy_video_.._full_isobmff_box(lls_sls_monitor_output_buffer);

 *
 */

void parseAndBuildJoinedBoxes_from_lls_sls_monitor_output_buffer(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, block_t* audio_output_buffer, block_t* video_output_buffer, AP4_MemoryByteStream** output_stream_p) {

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
	uint32_t audio_track_id_to_remap = 0;

	std::list<AP4_TrunAtom*> audio_trunList;
	std::list<AP4_TrunAtom*> video_trunList;

	std::list<AP4_TrunAtom*>::iterator itTrunFirst;

	std::list<AP4_Atom_And_Offset_t*> audio_mdatList;
	std::list<AP4_Atom_And_Offset_t*> video_mdatList;
	std::list<AP4_Atom_And_Offset_t*>::iterator it;

	uint32_t audio_mdat_size_new = 0;
	uint32_t video_mdat_size_new = 0;

	AP4_AtomParent* video_moofAtomParent = NULL;
	AP4_Atom* video_moofAtom = NULL;

	AP4_ContainerAtom* video_trafAtom = NULL;
	AP4_TrunAtom* video_trunAtom = NULL;

	uint32_t video_trun_last_offset = 0;

	AP4_MdhdAtom* video_mdhdAtom;

	//mpu_presentation_time support
	AP4_TfdtAtom* audio_tfdt_atom_mdhd_timescale = NULL;
	AP4_TfdtAtom* video_tfdt_atom_mdhd_timescale = NULL;

	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_set && lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time_set) {

		//fractional component is already at 1000000 (uS), so just multiply and add the seconds...
		uint64_t audio_mpu_presentation_time_s = lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_s * 1000000;
		uint64_t audio_mpu_presentation_time_ms = lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.mpu_presentation_time_us % 1000000; //just to be safe..
		uint64_t audio_mpu_presentation_time_final_uS =  audio_mpu_presentation_time_s + audio_mpu_presentation_time_ms;

		audio_tfdt_atom_mdhd_timescale = new AP4_TfdtAtom(1, audio_mpu_presentation_time_final_uS);

		//now for video
		uint64_t video_mpu_presentation_time_s = lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time_s * 1000000;
		uint64_t video_mpu_presentation_time_ms = lls_sls_monitor_output_buffer->video_output_buffer_isobmff.mpu_presentation_time_us % 1000000; //just to be safe..
		uint64_t video_mpu_presentation_time_final_uS =  video_mpu_presentation_time_s + video_mpu_presentation_time_ms;

		video_tfdt_atom_mdhd_timescale = new AP4_TfdtAtom(1, video_mpu_presentation_time_final_uS);
	}

	if(!audio_output_buffer || !video_output_buffer) {
		__ISOBMFF_JOINER_INFO("setting *output_stream_p to null, audio_output_buffer: %p, video_output_buffer: %p", audio_output_buffer, video_output_buffer);
		*output_stream_p = NULL;
		return;
	}

	//we shouldn't be bigger than this for our return..
	AP4_DataBuffer* dataBuffer = new AP4_DataBuffer(audio_output_buffer->i_pos + video_output_buffer->i_pos );
	AP4_MemoryByteStream* memoryOutputByteStream = new AP4_MemoryByteStream(dataBuffer);

	*output_stream_p = memoryOutputByteStream;

	list<AP4_Atom_And_Offset_t*> audio_isobmff_atom_list  = ISOBMFFTrackParseAndBuildOffset(audio_output_buffer);

	list<AP4_Atom_And_Offset_t*> video_isobmff_atom_list =  ISOBMFFTrackParseAndBuildOffset(video_output_buffer);

    __ISOBMFF_JOINER_DEBUG("Dumping audio box: size: %u", audio_output_buffer->i_pos);
	//dumpFullMetadata(audio_isobmff_atom_list);

	__ISOBMFF_JOINER_DEBUG("Dumping video box: %u", video_output_buffer->i_pos);
	//dumpFullMetadata(video_isobmff_atom_list);



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
                <- detatch both tfdt boxes if base_media_decode_time == 0

            -> update trunSecondFile dataOffset from moof->getsize() + moof header size (+8)
            -> update trunfirstFile dataOffset  from moof->getsize() + moof header size (+8) +2nd mdat size

      append 1st Copy mdat box interior into v mdat_box

	 */



    /**
     *
     * TODO: handle use case without a MOOV atom...
     to postion at end:

     [hdlr] size=12+40
     handler_type = hint
     handler_name = Bento4 Hint Handler
     **/

	//find our audio and video track id's first
	for (it = audio_isobmff_atom_list.begin(); it != audio_isobmff_atom_list.end(); it++) {
		AP4_Atom* top_level_atom = (*it)->atom;
		if(top_level_atom->GetType() == AP4_ATOM_TYPE_MOOV) {
			AP4_MoovAtom* moovAtom = AP4_DYNAMIC_CAST(AP4_MoovAtom, top_level_atom);

			AP4_TrakAtom* tmpTrakAtom;
			int trakIndex = 0;
			while((tmpTrakAtom = AP4_DYNAMIC_CAST(AP4_TrakAtom, moovAtom->GetChild(AP4_ATOM_TYPE_TRAK, trakIndex++)))) {
				AP4_HdlrAtom* hdlrAtom = AP4_DYNAMIC_CAST(AP4_HdlrAtom, tmpTrakAtom->FindChild("mdia/hdlr", false, false));

				if(hdlrAtom && hdlrAtom->GetHandlerType() == AP4_HANDLER_TYPE_SOUN) {
					lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.track_id = tmpTrakAtom->GetId();

					//try and find our parent's mdhd timescale and re-map as needed
					AP4_AtomParent* mdiaAtom = hdlrAtom->GetParent();
					AP4_MdhdAtom* mdhdAtom = AP4_DYNAMIC_CAST(AP4_MdhdAtom, mdiaAtom->FindChild("mdhd"));
					if(mdhdAtom) {
						if(!mdhdAtom->GetTimeScale()) {
							mdhdAtom->SetTimeScale(1000000);
						} else if(mdhdAtom->GetTimeScale() != 1000000 && audio_tfdt_atom_mdhd_timescale) {
							//rebase from 1000000 into 1/
							uint64_t audio_tfdt_atom_mdhd_presentation_time = audio_tfdt_atom_mdhd_timescale->GetBaseMediaDecodeTime();
							audio_tfdt_atom_mdhd_timescale->SetBaseMediaDecodeTime((audio_tfdt_atom_mdhd_presentation_time * mdhdAtom->GetTimeScale())/1000000);
						}
					}
				}
			}
		}
	}


	//Video track: now go the other way...
	for (it = video_isobmff_atom_list.begin(); it != video_isobmff_atom_list.end(); it++) {
		AP4_Atom* top_level_atom = (*it)->atom;
		if(top_level_atom->GetType() == AP4_ATOM_TYPE_MOOV) {
			AP4_MoovAtom* moovAtom = AP4_DYNAMIC_CAST(AP4_MoovAtom, top_level_atom);

			AP4_TrakAtom* tmpTrakAtom;
			int trakIndex = 0;
			while((tmpTrakAtom = AP4_DYNAMIC_CAST(AP4_TrakAtom, moovAtom->GetChild(AP4_ATOM_TYPE_TRAK, trakIndex++)))) {
				AP4_HdlrAtom* hdlrAtom = AP4_DYNAMIC_CAST(AP4_HdlrAtom, tmpTrakAtom->FindChild("mdia/hdlr", false, false));

				if(hdlrAtom && hdlrAtom->GetHandlerType() == AP4_HANDLER_TYPE_VIDE) {

					lls_sls_monitor_output_buffer->video_output_buffer_isobmff.track_id = tmpTrakAtom->GetId();

					//try and find our parent's mdhd timescale and re-map as needed
					AP4_AtomParent* mdiaAtom = hdlrAtom->GetParent();
					video_mdhdAtom = AP4_DYNAMIC_CAST(AP4_MdhdAtom, mdiaAtom->FindChild("mdhd"));
					if(video_mdhdAtom) {
						if(!video_mdhdAtom->GetTimeScale()) {
							video_mdhdAtom->SetTimeScale(1000000);
						} else if(video_mdhdAtom->GetTimeScale() != 1000000 && video_tfdt_atom_mdhd_timescale) {
							//rebase from 1000000 into 1/
							uint64_t video_tfdt_atom_mdhd_presentation_time = video_tfdt_atom_mdhd_timescale->GetBaseMediaDecodeTime();
							video_tfdt_atom_mdhd_timescale->SetBaseMediaDecodeTime((video_tfdt_atom_mdhd_presentation_time * video_mdhdAtom->GetTimeScale())/1000000);
						}
					}

				}
			}
		}
	}

	if(lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.track_id == lls_sls_monitor_output_buffer->video_output_buffer_isobmff.track_id) {
		audio_track_id_to_remap = lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.track_id + 1;

		__ISOBMFF_JOINER_INFO("Duplicate track_id's for v/a: %u, setting audio track id to: %u", lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.track_id, audio_track_id_to_remap);
	}


	//from isoBMFFList1 list - audio
	for (it = audio_isobmff_atom_list.begin(); it != audio_isobmff_atom_list.end(); it++) {
		AP4_Atom* top_level_atom = (*it)->atom;

		//In the moov box->get a ref for the trak box
		if(top_level_atom->GetType() == AP4_ATOM_TYPE_MOOV) {
			AP4_MoovAtom* moovAtom = AP4_DYNAMIC_CAST(AP4_MoovAtom, top_level_atom);
			audio_mvexAtomToCopy = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moovAtom->GetChild(AP4_ATOM_TYPE_MVEX));

			//filter out any hint tracks in the mvex box

			AP4_TrexAtom* tmpTrexAtom;
			int trexIndex = 0;
			while((tmpTrexAtom = AP4_DYNAMIC_CAST(AP4_TrexAtom, audio_mvexAtomToCopy->GetChild(AP4_ATOM_TYPE_TREX, trexIndex++)))) {
				if(tmpTrexAtom->GetTrackId() != lls_sls_monitor_output_buffer->audio_output_buffer_isobmff.track_id) {
					tmpTrexAtom->Detach();
				} else if(audio_track_id_to_remap) {
					//matching track id, detatch tmpTrexAtom and replace
					tmpTrexAtom->Detach();
					AP4_TrexAtom* ap4_trexAtom = new AP4_TrexAtom(audio_track_id_to_remap,
							tmpTrexAtom->GetDefaultSampleDescriptionIndex(),
							tmpTrexAtom->GetDefaultSampleDuration(),
							tmpTrexAtom->GetDefaultSampleSize(),
							tmpTrexAtom->GetDefaultSampleFlags());

					audio_mvexAtomToCopy->AddChild(ap4_trexAtom);

				}
			}


			AP4_TrakAtom* tmpTrakAtom;
			int trakIndex = 0;
			while((tmpTrakAtom = AP4_DYNAMIC_CAST(AP4_TrakAtom, moovAtom->GetChild(AP4_ATOM_TYPE_TRAK, trakIndex++)))) {

				AP4_HdlrAtom* hdlrAtom = AP4_DYNAMIC_CAST(AP4_HdlrAtom, tmpTrakAtom->FindChild("mdia/hdlr", false, false));

				if(hdlrAtom && hdlrAtom->GetHandlerType() == AP4_HANDLER_TYPE_SOUN) {
					if(audio_track_id_to_remap) {
						tmpTrakAtom->SetId(audio_track_id_to_remap);
					}
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

		if(top_level_atom->GetType() == AP4_ATOM_TYPE_MOOF) {
			AP4_AtomParent* moofAtom = AP4_DYNAMIC_CAST(AP4_ContainerAtom, top_level_atom);
			AP4_ContainerAtom* trafContainerAtom = AP4_DYNAMIC_CAST(AP4_ContainerAtom, moofAtom->GetChild(AP4_ATOM_TYPE_TRAF));
			//tfhd

            AP4_TfhdAtom* tfhdTempAtom = AP4_DYNAMIC_CAST(AP4_TfhdAtom, trafContainerAtom->GetChild(AP4_ATOM_TYPE_TFHD));
            if(tfhdTempAtom && audio_track_id_to_remap) {
            	tfhdTempAtom->SetTrackId(audio_track_id_to_remap);
            }

			audio_trafList.push_back(trafContainerAtom);

			AP4_TrunAtom* temp_trunAtom = AP4_DYNAMIC_CAST(AP4_TrunAtom, trafContainerAtom->GetChild(AP4_ATOM_TYPE_TRUN));
		
			audio_trunList.push_back(temp_trunAtom);
		}

		if(top_level_atom->GetType() == AP4_ATOM_TYPE_MDAT) {
			if(audio_mdat_size_new) {
				top_level_atom->SetSize32(audio_mdat_size_new + AP4_ATOM_HEADER_SIZE);
			}
			audio_mdatList.push_back(*it);
		}
	}

	//Video track: now go the other way...
	for (it = video_isobmff_atom_list.begin(); it != video_isobmff_atom_list.end(); it++) {
		AP4_Atom* top_level_atom = (*it)->atom;

		//In the moov box->get a ref for the trak box
		if(top_level_atom->GetType() == AP4_ATOM_TYPE_MOOV) {
			AP4_MoovAtom* moovAtom = AP4_DYNAMIC_CAST(AP4_MoovAtom, top_level_atom);

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

		if(top_level_atom->GetType() == AP4_ATOM_TYPE_MOOF) {
			video_moofAtomParent = AP4_DYNAMIC_CAST(AP4_ContainerAtom, top_level_atom);

            //clear out our traf's if tfhd id != trakMediaAtomSecondFileId

			AP4_ContainerAtom* tmpTrafToClean;
			int trafIdx = 0;
			while((tmpTrafToClean = AP4_DYNAMIC_CAST(AP4_ContainerAtom, video_moofAtomParent->GetChild(AP4_ATOM_TYPE_TRAF, trafIdx++)))) {
                AP4_TfhdAtom* tfhdTempAtom = AP4_DYNAMIC_CAST(AP4_TfhdAtom, tmpTrafToClean->GetChild(AP4_ATOM_TYPE_TFHD));
                bool shouldDetachTrak = true;
                if(tfhdTempAtom && tfhdTempAtom->GetTrackId() == lls_sls_monitor_output_buffer->video_output_buffer_isobmff.track_id) {
                	video_trafAtom = tmpTrafToClean;
                	shouldDetachTrak = false;

                    //remove our tfdt's if base media decode time is 0

                	AP4_TfdtAtom* video_tfdtTempAtom = AP4_DYNAMIC_CAST(AP4_TfdtAtom, tmpTrafToClean->GetChild(AP4_ATOM_TYPE_TFDT));

                	if(video_tfdtTempAtom && video_tfdtTempAtom->GetBaseMediaDecodeTime() == 0) {
                		if(video_tfdt_atom_mdhd_timescale) {
                			video_tfdtTempAtom->Detach();
                			tmpTrafToClean->AddChild(video_tfdt_atom_mdhd_timescale, 1);
                		} else {
                			video_tfdtTempAtom->Detach();
                		}
                	} else if(!video_tfdtTempAtom && video_tfdt_atom_mdhd_timescale) {
                		tmpTrafToClean->AddChild(video_tfdt_atom_mdhd_timescale, 1);
                	}
                }

                if(shouldDetachTrak) {
                	tmpTrafToClean->Detach();
                } else {
                	//append this into our video_trunList
                	AP4_TrunAtom* temp_trunAtom = AP4_DYNAMIC_CAST(AP4_TrunAtom, tmpTrafToClean->GetChild(AP4_ATOM_TYPE_TRUN));

                	video_trunList.push_back(temp_trunAtom);
                }
			}

			//add our audio tracks into the moofAtom
			for(itTraf = audio_trafList.begin(); itTraf != audio_trafList.end(); itTraf++) {

                AP4_TfhdAtom* tfhdTempAtom = AP4_DYNAMIC_CAST(AP4_TfhdAtom, (*itTraf)->GetChild(AP4_ATOM_TYPE_TFHD));

                //remove our tfdt's if base media decode time is 0
                AP4_TfdtAtom* audio_tfdtTempAtom = AP4_DYNAMIC_CAST(AP4_TfdtAtom, (*itTraf)->GetChild(AP4_ATOM_TYPE_TFDT));

                if(audio_tfdtTempAtom && audio_tfdtTempAtom->GetBaseMediaDecodeTime() == 0) {
                	if(audio_tfdt_atom_mdhd_timescale) {
                		//audio_tfdtTempAtom->SetBaseMediaDecodeTime(audio_tfdt_atom->GetBaseMediaDecodeTime());
                		audio_tfdtTempAtom->Detach();
                		(*itTraf)->AddChild(audio_tfdt_atom_mdhd_timescale, 1);

                	} else {
                		audio_tfdtTempAtom->Detach();
                	}
				} else if(audio_tfdt_atom_mdhd_timescale) {
					(*itTraf)->AddChild(audio_tfdt_atom_mdhd_timescale, 1);
				}

                (*itTraf)->Detach();
				video_moofAtomParent->AddChild(*itTraf);
			}


			if(video_trafAtom) {
                video_trunAtom = AP4_DYNAMIC_CAST(AP4_TrunAtom, video_trafAtom->GetChild(AP4_ATOM_TYPE_TRUN));
                if(video_trunAtom) {

                	//get our first sample duration to get an ~estimate of our fps
                	const AP4_Array<AP4_TrunAtom::Entry>& video_sampleEntries = video_trunAtom->GetEntries();
                	bool has_found_sample_duration = false;

                	for(int i=0; !has_found_sample_duration && i < video_sampleEntries.ItemCount(); i++) {
                		if(video_sampleEntries[i].sample_duration) {
                			lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fps_num = video_sampleEntries[i].sample_duration;
                			if(video_mdhdAtom) {
                				lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fps_denom = video_mdhdAtom->GetTimeScale();
                			} else {
                				lls_sls_monitor_output_buffer->video_output_buffer_isobmff.fps_denom = 1000000;
                			}
                			has_found_sample_duration = true;
                		}
                	}
                }
            }
		}

		/**
		 * this is the fun part, detatch these and build one concatenated mdat box below,
		 * we can't get the interor payload of the mdat atom via bento4 as its managed by the samples
		 */

		if(top_level_atom->GetType() == AP4_ATOM_TYPE_MDAT) {

			//update this size if we've "removed" samples
			if(video_mdat_size_new) {
				top_level_atom->SetSize32(video_mdat_size_new + AP4_ATOM_HEADER_SIZE);
			}
			video_mdatList.push_back(*it);

			//write us out by hand manually later
			top_level_atom->Detach();
			(*it)->write_manually = true;
		}
	}

	//set our video trun last offset here, and then set our audio trun DataOffset down below...
	if(video_moofAtomParent) {
		video_moofAtom = AP4_DYNAMIC_CAST(AP4_Atom, video_moofAtomParent);
        if(video_trunAtom) {
        	video_trun_last_offset = (AP4_UI32)video_moofAtom->GetSize()+AP4_ATOM_HEADER_SIZE;
            video_trunAtom->SetDataOffset(video_trun_last_offset);
        } else {
            //this shouldn't happen
        }
	}


	//final muxed output process
	//TODO: evaulate short sample interleaving for http re-fragmented delivery as ffplay complains about an incomplete file

	//first, compute up the size of all of our mdat payload sizes
	uint32_t video_mdat_payload_size_refragment = 0;
	uint32_t video_mdat_payload_size_bento_parser = 0;

	uint32_t audio_mdat_payload_size_refragment = 0;
	uint32_t audio_mdat_payload_size_bento_parser = 0;

	//these should be the same..
	uint32_t final_mdat_payload_size_refragment = 0;
	uint32_t final_mdat_payload_size_bento_parser = 0;


	//AP4_ATOM_HEADER_SIZE
	//we may have multiple video mdat's combined here..
	itTrunFirst = video_trunList.begin();

	it = video_mdatList.begin();
	for(; itTrunFirst != video_trunList.end() && it != video_mdatList.end(); ) {
		(*itTrunFirst)->SetDataOffset(video_trun_last_offset + video_mdat_payload_size_refragment);

		video_mdat_payload_size_refragment += (*it)->atom->GetSize32() - AP4_ATOM_HEADER_SIZE;
		video_mdat_payload_size_bento_parser += ((*it)->end_offset - (*it)->start_offset) - AP4_ATOM_HEADER_SIZE;

		final_mdat_payload_size_refragment += (*it)->atom->GetSize32() - AP4_ATOM_HEADER_SIZE;
		final_mdat_payload_size_bento_parser += ((*it)->end_offset - (*it)->start_offset) - AP4_ATOM_HEADER_SIZE;
		itTrunFirst++;
		it++;
	}

    //update audio segment trun box(es).. before writing them out...
	itTrunFirst = audio_trunList.begin();
	it = audio_mdatList.begin();

	for(;itTrunFirst != audio_trunList.end() && it != audio_mdatList.end();) {
		(*itTrunFirst)->SetDataOffset(video_trun_last_offset + video_mdat_payload_size_refragment + audio_mdat_payload_size_bento_parser);

		audio_mdat_payload_size_refragment += (*it)->atom->GetSize32() - AP4_ATOM_HEADER_SIZE;
		audio_mdat_payload_size_bento_parser += ((*it)->end_offset - (*it)->start_offset) - AP4_ATOM_HEADER_SIZE;

		final_mdat_payload_size_refragment += (*it)->atom->GetSize32() - AP4_ATOM_HEADER_SIZE;
		final_mdat_payload_size_bento_parser += ((*it)->end_offset - (*it)->start_offset) - AP4_ATOM_HEADER_SIZE;

		itTrunFirst++;
		it++;
	}



	//write the final combined isobmff boxes except for mdat
	for (it = video_isobmff_atom_list.begin(); it != video_isobmff_atom_list.end(); it++) {
		AP4_Atom* top_level_atom = (*it)->atom;

		bool should_write_box = true;

#ifdef __DROP_SIDX_BOX__
		if(top_level_atom->GetType() == AP4_ATOM_TYPE_SIDX) {
			should_write_box = false;
		}
#endif
		if(!(*it)->write_manually) {
			if(should_write_box) {
				top_level_atom->Write(*memoryOutputByteStream);
			}
		}
	}

	//now write our final mdat boxe

	memoryOutputByteStream->WriteUI32((AP4_UI32)final_mdat_payload_size_refragment + AP4_ATOM_HEADER_SIZE);
	memoryOutputByteStream->WriteUI32(AP4_ATOM_TYPE_MDAT);

	//now combine the interior samples....
	uint32_t mdat_to_write_size = 0;
	for(it = video_mdatList.begin(); it != video_mdatList.end(); it++) {
		mdat_to_write_size = (*it)->end_offset - ((*it)->start_offset + AP4_ATOM_HEADER_SIZE);
		memoryOutputByteStream->Write(&video_output_buffer->p_buffer[(*it)->start_offset + AP4_ATOM_HEADER_SIZE], mdat_to_write_size);
	}

	for(it = audio_mdatList.begin(); it != audio_mdatList.end(); it++) {
		mdat_to_write_size = (*it)->end_offset - ((*it)->start_offset + AP4_ATOM_HEADER_SIZE);
		memoryOutputByteStream->Write(&audio_output_buffer->p_buffer[(*it)->start_offset + AP4_ATOM_HEADER_SIZE], mdat_to_write_size);
	}

    __ISOBMFF_JOINER_INFO("Final output re-muxed MPU:");
    dumpFullMetadataAndOffsets(video_isobmff_atom_list);

	block_Release(&audio_output_buffer);
	block_Release(&video_output_buffer);

	for (it = audio_isobmff_atom_list.begin(); it != audio_isobmff_atom_list.end(); it++) {
		if((*it)->atom) {
			delete (*it)->atom;
		}
		free (*it);
	}

	for (it = video_isobmff_atom_list.begin(); it != video_isobmff_atom_list.end(); it++) {
		if((*it)->atom) {
			delete (*it)->atom;
		}
		free (*it);
	}


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
		AP4_FileByteStream::Create("isobmff.debug", AP4_FileByteStream::STREAM_MODE_WRITE, boxDumpConsoleOutput);
		AP4_AtomInspector* inspector = new AP4_PrintInspector(*boxDumpConsoleOutput);
		inspector->SetVerbosity(3);

		std::list<AP4_Atom*>::iterator it;
		for (it = atomList.begin(); it != atomList.end(); it++) {
			(*it)->Inspect(*inspector);
		}

		boxDumpConsoleOutput->WriteString("---\r\n");
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

