/*
 *
 * atsc3_llt.c:  driver for ATSC 3.0 LLS listener over udp
 *
 */

#include "atsc3_lls.h"
#include "xml.h"
#include "atsc3_gzip.h"

#define __UNIT_TEST 1
#ifdef __UNIT_TEST

//forward declare our testcases
int test_lls_create_slt_route_dash(char* base64_payload);

//slt with packet_id=2
static char* __get_test_slt()					{ return "010100021f8b08089217185c0003534c5400b5d55b6f82301400e0f7fd0ad2e70d4a41370d609c9ac5448d092ed99ba9d06117685d5bcdfcf73ba8cbe2bc44167d229c4bcfe9f70041ebabc8ad15539a4b1122d7c6c86222912917598896e6fde109b5a2bb201e4c2ca8143a4486664d6a74624b95dd13ecd69b6fc3419ccc5941b5d39ec41dcfe9b29cc3996b07da1c38d341d64cf33444358ca220666ac51366e9edb30f7117631759592e6734dfa5fb5d98afc466547357ca537865059b1685994243413fa4eacca9102c1fc9f2188871b11f433f833ad09b49b5dec6e65299dda8112d5888da93deb0670d8713ab4ce7265e2531fb1c2d8b10955b3f2b49d384ea4d9c6782e64004757aaca49189cc4344ca3edd65da70410d80f617ed34554c831af11a36a9d56c17dbeedfb2d77431866d8067eb00d9582e1518fcf6bb8fc476eb36c165bf1305ce6ef7539ca42a27b98c9354e724b7e53c28dbe324d7e1f4aa727a97717ad539bddb727a6739bdeb70fa5539fdcb38fdea9cfe6d39fdb39cfe15386b18372ee3643a3be2e31ff3e9c52fff7439f8ba1d7121d86e9c76219b0b557329ff34d1dd372e0efb8fce060000";}
//system_time_message with packet_id=1
static char* __get_test_system_time_message()	{ return "030100011f8b08089717185c000353797374656d54696d6500358dcb0a82401440f77ec570f77a0b89227c10151428056350cb61bc3e601cc3b966fe7d6eda1e38e744e9b733e243836b7b1bc33a588120abfbb2b5750c2357fe0ed2c48be4ec98baa2ed482c82753134ccef3de2344d8162a7837ea8f199675237d4298787421e433c916997f88cf2258b6b7ec6658020f4380c64f9c1fa56558e3886700b62649df55a993ff3efc5e602a27492158fcbb252c61160e2fd003518c11fb6000000"; }

static char* _get_2019_01_07_slt_route_dash()	{ return "010100151f8b08080000000000ff534c5400cd92514bc3301485df05ff43c8b3a64d66c73a5ac7dc10065b19b4135f631bba489acc241beedf7bb756870a437df2e9c239f7de9cfb9164f4da28b413d649a3534c498891d0a5a9a4ae53bc2aeeaf071839cf75c595d122c57be1f0e8f6f222c9e7058259ed52ec793de4de95c4d8fa8a85b43f7c5cccf3722d1aee8271914f7ac1542809afec03180be09500a32727ab148718962184925cd89d2c05726d9d814729460d7f3676b2e65a0b959914df44a049fd4983b66e6ac2bda88dddb7dada58dfadcd7803e1b37191cdc1503edf95b978c9b6cd29c121c49d35bc2ab93bfab2d65c0107e8774b6bbc298d6a172bb738c47a171f3ef0b5de21de372f3c7a53e1bcd4dc8334db8cabca0a0700592f262c8a0825f46bd7aada2ce10c383ca6113bbab9d95a00741aa73123b43f20514cfa0c071dd0a03bfd0c5ff633beec377cd9bfe6cbcef2edfd916f72f8d650df00f37a26b44e030000"; }

static char* _get_2019_04_03_skt_test1() 		{ return "030100011f8b0808714f2c5c000353797374656d54696d6500358dcb0ac2301000ef7e45d8bb5da5071ff481a8a0d0a2900a7a0ce9f6016d2ac9c6dabfb717af033313a5dfbe131fb2ae1d4c0ceb6005828c1ecad6d43178ae965b4893452427c7d4176d4f62168c8ba1617eef11c7710c143b1d0cb6c6679e49dd50af1c1e0a790cf1445d3bc727942f595cf333ce0304a1bdb564f8c1fa56558e38867003c2b3ce06adba3fbb17bb0b88d24956ece763a53a4780c9e207ef022c62b5000000"; }

static char* _get_2019_04_03_skt_test2() 		{ return "010100031f8b08086b4f2c5c0003534c54005d50cb6ec23010bcf72b2c9f5b3b0e0a822809a2e45024404849a5de2a13bb892b3fa86d50f9fb2e10a912173f66677766a758fc1a8dced207e56c8919493092b67342d9bec4a7f8f532c38beaa968362d02a60d258ebccf790c1d71be7f4e1336cd3fb69ba61ba4e1812edb6635a1b5d40a665e28b4519849313a04254a9c25b82a1ae9cfaa9328dcef35e02c491846bd7607aec7f2ba067d6f6f52f948552237267e0e0223c3bf9d5f0ddc5aa9770e2600a4ec2334f6ad7894bdf3973b36381f47911d37b2c4db6d8bde6aa8e8d89cbb46feec4ea6c429587df58e8b8e871bae7acb35e402bcb0f72ebacee92bedfaaf6588caf20829ae8f4b21bc0c10553a999334cb489610f6487b17c73d18814ceecb43b571270f8bfff7b3794ad87446e064134cab828ebeafaf4d5b3dfd01bca71b4fbf010000"; }

int test_lls_create_xml_table(char* base64_payload);
void test_kvp_extraction();

int main(int argc, char* argv[]) {

	if(argc==2) {
		//use this as our text

		test_lls_create_slt_route_dash(argv[1]);

	} else {
		printf("%s - lls_hex_payload_for_decode", argv[0]);
	}
	//test_kvp_extraction();
	return 0;
}

/**
 *
 *
 *
203:process_xml_payload - node ptr: 0x100400250, name is: SLT, size: 2
70:dump_xml_string::xml_string: len: 3, is_self_closing: 0, val: |SLT|, attributes len: 70, val: xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/SLT/1.0/" bsid="50"
236:build_SLT_table, attributes are:

xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/SLT/1.0/" bsid="50"
70:dump_xml_string::xml_string: len: 7, is_self_closing: 0, val: |Service|, attributes len: 172, val: serviceId="1001" globalServiceID="urn:atsc:serviceid:ateme_mmt_1" majorChannelNo="10" minorChannelNo="1" serviceCategory="1" shortServiceName="ATEME MMT 1" sltSvcSeqNum="0"
70:dump_xml_string::xml_string: len: 21, is_self_closing: 1, val: |BroadcastSvcSignaling|, attributes len: 118, val: slsProtocol="2" slsDestinationIpAddress="239.255.10.1" slsDestinationUdpPort="51001" slsSourceIpAddress="172.16.200.1"
283:build_SLT_BROADCAST_SVC_SIGNALING_table - attributes are: serviceId="1001" globalServiceID="urn:atsc:serviceid:ateme_mmt_1" majorChannelNo="10" minorChannelNo="1" serviceCategory="1" shortServiceName="ATEME MMT 1" sltSvcSeqNum="0"70:dump_xml_string::xml_string: len: 7, is_self_closing: 0, val: |Service|, attributes len: 172, val: serviceId="1002" globalServiceID="urn:atsc:serviceid:ateme_mmt_2" majorChannelNo="10" minorChannelNo="2" serviceCategory="1" shortServiceName="ATEME MMT 2" sltSvcSeqNum="0"
70:dump_xml_string::xml_string: len: 21, is_self_closing: 1, val: |BroadcastSvcSignaling|, attributes len: 118, val: slsProtocol="2" slsDestinationIpAddress="239.255.10.2" slsDestinationUdpPort="51002" slsSourceIpAddress="172.16.200.1"
283:build_SLT_BROADCAST_SVC_SIGNALING_table - attributes are: serviceId="1002" globalServiceID="urn:atsc:serviceid:ateme_mmt_2" majorChannelNo="10" minorChannelNo="2" serviceCategory="1" shortServiceName="ATEME MMT 2" sltSvcSeqNum="0"70:dump_xml_string::xml_string: len: 7, is_self_closing: 0, val: |Service|, attributes len: 172, val: serviceId="1003" globalServiceID="urn:atsc:serviceid:ateme_mmt_3" majorChannelNo="10" minorChannelNo="3" serviceCategory="1" shortServiceName="ATEME MMT 3" sltSvcSeqNum="0"
70:dump_xml_string::xml_string: len: 21, is_self_closing: 1, val: |BroadcastSvcSignaling|, attributes len: 118, val: slsProtocol="2" slsDestinationIpAddress="239.255.10.3" slsDestinationUdpPort="51003" slsSourceIpAddress="172.16.200.1"
283:build_SLT_BROADCAST_SVC_SIGNALING_table - attributes are: serviceId="1003" globalServiceID="urn:atsc:serviceid:ateme_mmt_3" majorChannelNo="10" minorChannelNo="3" serviceCategory="1" shortServiceName="ATEME MMT 3" sltSvcSeqNum="0"70:dump_xml_string::xml_string: len: 7, is_self_closing: 0, val: |Service|, attributes len: 172, val: serviceId="1004" globalServiceID="urn:atsc:serviceid:ateme_mmt_4" majorChannelNo="10" minorChannelNo="4" serviceCategory="1" shortServiceName="ATEME MMT 4" sltSvcSeqNum="0"
70:dump_xml_string::xml_string: len: 21, is_self_closing: 1, val: |BroadcastSvcSignaling|, attributes len: 118, val: slsProtocol="2" slsDestinationIpAddress="239.255.10.4" slsDestinationUdpPort="51004" slsSourceIpAddress="172.16.200.1"
283:build_SLT_BROADCAST_SVC_SIGNALING_table - attributes are: serviceId="1004" globalServiceID="urn:atsc:serviceid:ateme_mmt_4" majorChannelNo="10" minorChannelNo="4" serviceCategory="1" shortServiceName="ATEME MMT 4" sltSvcSeqNum="0"70:dump_xml_string::xml_string: len: 7, is_self_closing: 0, val: |Service|, attributes len: 117, val: serviceId="5009" globalServiceID="urn:atsc:serviceid:esg" serviceCategory="4" shortServiceName="ESG" sltSvcSeqNum="0"
70:dump_xml_string::xml_string: len: 21, is_self_closing: 1, val: |BroadcastSvcSignaling|, attributes len: 118, val: slsProtocol="1" slsDestinationIpAddress="239.255.20.9" slsDestinationUdpPort="52009" slsSourceIpAddress="172.16.200.1"
283:build_SLT_BROADCAST_SVC_SIGNALING_table - attributes are: serviceId="5009" globalServiceID="urn:atsc:serviceid:esg" serviceCategory="4" shortServiceName="ESG" sltSvcSeqNum="0"base table:
-----------
lls_table_id				: 1	(0x1)
lls_group_id				: 1	(0x1)



 */
void test_kvp_extraction() {
	char* kvp_test_string_1 = "xmlns=\"tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/SLT/1.0/\" bsid=\"50\"";
	kvp_collection_t *collection_1 = kvp_collection_parse((uint8_t*)kvp_test_string_1);
	printf("%d:test_kvp_extraction - sizeof kvp collection: %d, bsid value is: %s", __LINE__, collection_1->size_n, kvp_collection_get(collection_1, "bsid"));

}



void __create_binary_payload(char *test_payload_base64, uint8_t **binary_payload, int * binary_payload_size) {
	int test_payload_base64_length = strlen(test_payload_base64);
	int test_payload_binary_size = test_payload_base64_length/2;

	uint8_t *test_payload_binary = calloc(test_payload_binary_size, sizeof(uint8_t));

	for (size_t count = 0; count < test_payload_binary_size; count++) {
	        sscanf(test_payload_base64, "%2hhx", &test_payload_binary[count]);
	        test_payload_base64 += 2;
	}

	*binary_payload = test_payload_binary;
	*binary_payload_size = test_payload_binary_size;
}


int test_lls_create_xml_table(char* base64_payload) {

	uint8_t *binary_payload;
	int binary_payload_size;

	__create_binary_payload(base64_payload, &binary_payload, &binary_payload_size);
	block_t* lls_packet_block = block_Alloc(binary_payload_size);
	block_Write(lls_packet_block, binary_payload, binary_payload_size);
	lls_table_t *lls_table = lls_create_xml_table(lls_packet_block);

	lls_dump_instance_table(lls_table);

	block_Destroy(&lls_packet_block);

	return 0;
}



int test_lls_create_slt_table(char* base64_payload) {

	uint8_t *binary_payload;
	int binary_payload_size;

	__create_binary_payload(base64_payload, &binary_payload, &binary_payload_size);
	block_t* lls_packet_block = block_Alloc(binary_payload_size);
	block_Write(lls_packet_block, binary_payload, binary_payload_size);

	lls_table_t* lls = __lls_table_create(lls_packet_block);

	lls_dump_instance_table(lls);
	block_Destroy(&lls_packet_block);

	return 0;
}



int test_lls_create_slt_route_dash(char* base64_payload) {

	uint8_t *binary_payload;
	int binary_payload_size;

	__create_binary_payload(base64_payload, &binary_payload, &binary_payload_size);
	block_t* lls_packet_block = block_Alloc(binary_payload_size);
	block_Write(lls_packet_block, binary_payload, binary_payload_size);

	lls_table_t* lls = __lls_table_create(lls_packet_block);

	if(!lls) {
		printf("error creating lls table for %s", base64_payload);
	} else {
		lls_dump_instance_table(lls);
	}
	block_Destroy(&lls_packet_block);

	return 0;
}

int test_lls_components() {

	char* test_payload_base64;

	//test_payload_base64 = __get_test_slt();
	//test_payload_base64 = _get_2019_4_03_skt_test1();
	int test_payload_base64_length = strlen(test_payload_base64);
	int test_payload_binary_size = test_payload_base64_length/2;

	uint8_t *test_payload_binary = calloc(test_payload_binary_size, sizeof(uint8_t));

	for (size_t count = 0; count < test_payload_binary_size; count++) {
	        sscanf(test_payload_base64, "%2hhx", &test_payload_binary[count]);
	        test_payload_base64 += 2;
	}

	block_t* lls_packet_block = block_Alloc(test_payload_binary_size);
	block_Write(lls_packet_block, test_payload_binary, test_payload_binary_size);
	lls_table_t *parsed_table = lls_create_xml_table(lls_packet_block);

	lls_dump_instance_table(parsed_table);

	uint8_t *decompressed_payload;
	int ret = atsc3_unzip_gzip_payload(parsed_table->raw_xml.xml_payload, parsed_table->raw_xml.xml_payload_size, &decompressed_payload);
	//both char and %s with '\0' should be the same
	//printf("gzip ret is: %d\n", ret);
	for(int i=0; i < ret; i++) {
		printf("%c", decompressed_payload[i]);
	}

	printf("%s", decompressed_payload);
	block_Destroy(&lls_packet_block);

	return 0;
}


#endif


