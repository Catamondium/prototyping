#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>   // any_of
#include <functional>  // std::function
#include <cctype>      // tolower
#include <unistd.h>    // *nix api
#include <sys/types.h> // *nix types
#include <fstream>
#include <pwd.h>       // working directory stuff
#include "lib/parsing.hpp"
#include "lib/note.hpp"

/* TODO
 * user IO
 * * Traverse by ID, parse com by letter
 * callable COM struct?
 *
 * Debugging / clarity
 * * find a substitute for 'i' to get index
 * * Find way to integrate argv dispatch with 'user'IO
 */

const std::string  DATAFILE = "/.notes";
typedef std::optional<std::string> optstring;
const std::string COMS = "lare";

enum Com : char
{
	LIST   = 'l',
	ADD    = 'a',
	REMOVE = 'r',
	EDIT   = 'e'
};

bool com_ls(std::string fname, std::optional<Note> note, std::optional<unsigned int> index)
{
	std::vector<Note> notes = notelib::parse(fname);
	for(auto i = 0; i < notes.size(); i++) {
		std::cout << "[" << i << "] " << notes[i].unmarshal() << std::endl;
	}
	return false;
}
bool com_add(std::string fname, std::optional<Note> note, std::optional<unsigned int> index)
{
	std::ofstream file(fname, std::ios_base::app);
	if(note)
		file << note.value().unmarshal() << std::endl;
	else
		return true;
	return false;
}
bool com_rm(std::string fname, std::optional<Note> note, std::optional<unsigned int> index)
{
	std::vector<Note> notes = notelib::parse(fname);
	if(index && index.value() < notes.size())
		notes.erase(notes.begin() + index.value());
	else
		return true;
	notelib::unmarshAll(notes, fname);
	return false;
}
bool com_edit(std::string fname, std::optional<Note> note, std::optional<unsigned int> index)
{
	std::vector<Note> notes = notelib::parse(fname);
	if(note && index && index.value() < notes.size())
		notes[index.value()] = note.value();
	else
		return true;
	notelib::unmarshAll(notes, fname);
	return false;
}

typedef std::function<bool/*HasError*/(std::string, std::optional<Note>, std::optional<unsigned int>)> com_functor;
std::unordered_map<Com, com_functor> dispatch
{
	{LIST, com_ls},
	{ADD, com_add},
	{REMOVE, com_rm},
	{EDIT, com_edit}
};

std::ostream& operator<<(std::ostream& stream, Com c)
{
	switch(c) {
		case LIST:
			stream << "LIST";
			break;
		case ADD:
			stream << "ADD";
			break;
		case REMOVE:
			stream << "REMOVE";
			break;
		case EDIT:
			stream << "EDIT";
	}
	return stream;
}

void usage(std::string prog) {
	exit(1);
}

std::string getHome()
{
	char *homedir;
	if((homedir = getenv("HOME")) == NULL)
		homedir = getpwuid(getuid())->pw_dir;

	return std::string(homedir);
}

int main(int argc, char **argv)
{
	std::string file = getHome() + DATAFILE;

	optstring head;
	optstring body;
	std::optional<note_time> event;
	std::optional<unsigned int> index;

	bool user = false; // prompt user by default

	int c;
	while((c = getopt(argc, argv, "ui:h:b:e:")) != -1) {
		std::string holder;
		switch(c) {
			case 'h':
				user = false;
				holder = optarg;
				head = notelib::trim(holder);
				break;
			case 'b':
				holder = optarg;
				body = notelib::trim(holder);
				break;
			case 'e':
				holder = optarg;
				event = notelib::makeEvent(notelib::trim(holder));
				break;
			case 'u':
				user = true;
				break;
			case 'i':
				index = std::stoi(optarg);
				break;
			/*case 'f': // Doesn't work with relative paths
				std::cout << "File:\t" << optarg << std::endl;
				file = optarg;
				break;*/
			default:
				usage(argv[0]);
		}
	}

	if(!user && optind < argc) { // if user, ignore opts
		char c = std::tolower(argv[optind][0]);
		if(std::any_of(COMS.cbegin(), COMS.cend(), [&c](auto o){return c == o;})) {
			std::optional<Note> note;
			Com target = static_cast<Com>(c);
			if(head)
				note = Note(head.value(), body, event);
			
			std::cout << target << std::endl;
			if(dispatch[target](file, note, index))
				usage(argv[0]);
		} else
			usage(argv[0]);
	} else
		std::cout << "user" << std::endl;
}
