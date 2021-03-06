/*
 * This file is not part of original USBProxy.
 * Sub-class of Command. 
 * Writes a string as a keyboard will do. Param is a double-quote delimited string with the text to write.
 *
 * Author: Skazza
 */

#include "CommandWrite.h"

CommandWrite::CommandWrite() : Command() {}

CommandWrite::~CommandWrite() {}

std::list<std::pair<__u8 *, __u64>> * CommandWrite::preparePayLoad(std::string stringToWrite, __u16 maxPacketSize) {
	std::list<std::pair<__u8 *, __u64>> * payLoad = new std::list<std::pair<__u8 *, __u64>>;

	for(unsigned int i = 0; i < stringToWrite.length(); ++i) {
		__u8 * packetPressed = (__u8 *) calloc(maxPacketSize, sizeof(__u8));
		std::pair<__u8, __u8> firstAndThirdByte = findCharacter(stringToWrite[i]);

		packetPressed[0x00] = firstAndThirdByte.first;
		packetPressed[0x02] = firstAndThirdByte.second;

		payLoad->push_back(std::pair<__u8 *, __u64>(packetPressed, maxPacketSize));
	}

	for(std::list<std::pair<__u8 *, __u64>>::iterator it = payLoad->begin(); it != payLoad->end(); ++it) {
		std::list<std::pair<__u8 *, __u64>>::iterator nextPacket = std::next(it, 1);

		if(nextPacket != payLoad->end())
			if((*it).first[0x02] == (*nextPacket).first[0x02]) {
				__u8 * packetReleased = (__u8 *) calloc(maxPacketSize, sizeof(__u8));
				payLoad->insert(nextPacket, std::pair<__u8 *, __u64>(packetReleased, maxPacketSize));
			}


		std::prev(it, 1);
	}

	__u8 * packetReleased = (__u8 *) calloc(maxPacketSize, sizeof(__u8));
	payLoad->push_back(std::pair<__u8 *, __u64>(packetReleased, maxPacketSize));

	return payLoad;
}


std::vector<std::string> * CommandWrite::parseParams(const std::string &paramString) {
	std::regex paramRegex("^\"(.*)\"$", std::regex_constants::icase);
	std::smatch matches; std::regex_search(paramString, matches, paramRegex);

	if(!matches[1].str().empty()) {
		std::vector<std::string> * paramVector = new std::vector<std::string>;
		paramVector->push_back(matches[1].str());

		return paramVector;
	}

	return NULL;
}

std::list<std::pair<__u8 *, __u64>> * CommandWrite::execute(const std::string &paramString, __u16 maxPacketSize) {
	std::vector<std::string> * paramList = this->parseParams(paramString);

	if(paramList) {
		std::list<std::pair<__u8 *, __u64>> * payLoad = this->preparePayLoad(paramList->at(0), maxPacketSize);
		delete(paramList);

		return payLoad;
	}

	return new std::list<std::pair<__u8 *, __u64>>;
}

/* Autoregisters the class into the CommandFactory */
const bool hasRegistered = CommandFactory::getInstance()->registerClass("WRITE", &CommandWrite::createInstance);
