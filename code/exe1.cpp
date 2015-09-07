#include <packet/PacketBufferV.h>
#include <packet/PacketLibDefinition.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <cstdlib>



int main(int argc, char *argv[])
{
	if(argc <= 3)
	{
		std::cout << "Wrong arguments, please provide config file, raw input file and number of packets." << std::endl;
		std::cout << argv[0] << " config.xml input.raw <npackets>" << std::endl;
		return 1;
	}
	const std::string configFilename(realpath(argv[1], NULL));
	const std::string inputFilename(realpath(argv[2], NULL));
	const unsigned long numevents = std::atol(argv[3]);
	
	// load events buffer
	PacketLib::PacketBufferV events(configFilename, inputFilename);
	events.load();
	int npackets = events.size();
	std::cout << "Loaded " << npackets << " packets " << std::endl;

	PacketLib::PacketStream ps(configFilename.c_str());
	
	unsigned long event_count = 0, event_size = 0;

	//get the indexes of fields
	PacketLib::Packet* p = ps.getPacketType("triggered_telescope1_30GEN");
	int npix_idx = p->getPacketSourceDataField()->getFieldIndex("Number of pixels");
	int nsamp_idx = p->getPacketSourceDataField()->getFieldIndex("Number of samples");

	
	while(event_count++ < numevents)
	{
		PacketLib::ByteStreamPtr event = events.getNext();
		event_size += event->size();
		
		/// swap if the stream has a different endianity
#ifdef ARCH_BIGENDIAN
		if(!event->isBigendian())
			event->swapWord();
#else
		if(event->isBigendian())
			event->swapWord();
#endif
		
		/// decoding packetlib packet
		PacketLib::Packet *packet = ps.getPacket(event);
		
		/// get telescope id
		PacketLib::DataFieldHeader* dfh = packet->getPacketDataFieldHeader();
		
		unsigned int telescopeID = dfh->getFieldValue_16ui("TelescopeID");
		
		int npix = packet->getPacketSourceDataField()->getFieldValue(npix_idx);
		int nsamp = packet->getPacketSourceDataField()->getFieldValue(nsamp_idx);

		
		cout << event_count << " " <<  telescopeID  << " " << npix << " " << nsamp << endl;
		
		/// get the waveforms
		PacketLib::byte* buff = packet->getData()->getStream();
		PacketLib::dword buffSize = packet->getData()->size();
		
		

	}

}