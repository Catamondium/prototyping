#include <optional>
#include <chrono>
#include "note.hpp"
#include <sstream>
#include <iomanip>

std::optional<std::string> Note::printEvent() {
	if(!event) return {};
	std::time_t tt = std::chrono::system_clock::to_time_t(event.value());

	struct std::tm *tm = std::gmtime(&tt);
	std::stringstream ss;
	ss << std::put_time(tm, "%d/%m/%Y");
	return ss.str();
}

std::string Note::unmarshal() {
	std::stringstream ss;

	ss << "Heading:\t" << heading << std::endl;
	if(event)
		ss << "Event:\t" << printEvent().value() << std::endl;
	if(body)
		ss << body.value();

	ss << "##";
	return ss.str();
}