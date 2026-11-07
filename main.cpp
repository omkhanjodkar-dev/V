// CURRENT
// no scroll yet
// offset bound implementation is pending
// have only implemented y level customisation of position and size since i haven't done offset bounds yet
// line bound has a bug: press enter n times then press backspace then press down arrow key then try to type anything, that gives segfault

// OLD
// the only problem in this entire program for functionality is the stupid DLL Edit class i made, i haven't figured out lines, add and remove yet properly with all its edge cases
// future problems include mem leaks

// GOOD THINGS I COULD DO
// should have enums for magic numbers
// memory management, smart pointers
// using namespace std; !?
// NULL = nullptr
// off by one indexing of lines
// you should write functions completely considering all the edge cases before going to the next part the code, if you skip some edge cases and come back to it later, you won't remember it correctly and won't be able to solve it
// keep a book with you while doing these kinds of programs so that you aren't finding edges cases in clouds
// make sure you define in comments what you are going to use something for and against and stick to it, you messed up lines here, you could've defined it and stuck to it with comments
// i jump into coding too early, advanced coders actually spend a long time thinking, coding after you know the whole algo is trivial and fast
// what i realised from this code is that designing and thinking through the algo and the system is the main task of a programmer, not actually coding it, because coding it is quite trivial once you are good in that language.
// i should also define laws around any complex data structure i use. then after any operation on that DS, i see if all the laws are follwed and true.
// it should be more like 'can this ever become incorrect?' than 'make it work.'
// should use destructors


#include <cstdlib>
// #include <windows.h>
#include <vector>
#include <conio.h>
#include <string>
#include <iostream>
#include <map>

using namespace std;

struct Node {
	char ch;
	Node* next;
	Node* prev;
	Node(char c, Node* n, Node* p): ch(c), next(n), prev(p) {}
};

// RULES for using 'lines':
// 1. lines is 1 indexed

class Edit {
public:
	Node* pre_head;
	Node* head;
	Node* tail;
	vector<Node*> lines;

	Edit() {
		pre_head = new Node(' ', head, NULL);
		head = NULL;
		tail = NULL;
		lines = {pre_head};
	}

	// obv mem leak
	void clear() {
		pre_head = new Node(' ', head, NULL);
		head = NULL;
		tail = NULL;
		lines = {pre_head};
	}

	bool reverse() {
		Node* temp = head;
		while (temp != NULL) {
			Node* temp2 = temp->prev;
			temp->prev = temp->next;
			temp->next = temp2;
			temp = temp->prev;
		}
		Node* holder = head;
		head = tail;
		tail = holder;
		update_lines();
		return true;
	}

	vector<pair<int, int>> search(string text) {
		vector<pair<int, int>> ret;

		int l = 1;
		int o = 1;
		Node* temp = head;
		bool out = false;
		while (temp != NULL) {
			Node* ctr = temp;
			bool found = true;
			for (auto it: text) {
				if (ctr == NULL) {
					out = true;
					break;
				}
				if (ctr->ch != it) {
					found = false;
					break;
				}
				ctr = ctr->next;
			}
			if (out) {
				break;
			}
			if (found) {
				ret.push_back({l, o});
			}
			if (temp->ch == '\n') {
				l++;
				o = 1;
			} else {
				o++;
			}
			temp = temp->next;
		}

		return ret;
	}

	void add(char ch, int line, int offset) {
		update_lines();

		// indexing fix
		line--;
		// offset--; why?
		
		if (head == NULL) {
			Node* new_node = new Node(ch, NULL, NULL);
			head = new_node;
			tail = new_node;
		} else if (head == tail and line + offset == 0) { // BUGBUGBUG
			Node* new_node = new Node(ch, head, NULL);
			head = new_node;
		} else if (head == tail and line + offset == 1) { // BUGBUGBUG
			Node* new_node = new Node(ch, NULL, tail);
			tail = new_node;
		} else {
			Node* temp = lines[line]; // BUGBUGBUG you want to access something (Node with '\n') that you are assigned to add in the DLL, FIXED: changed driver code to 'add' before updating cursor pos
			int ctr = 0;
			while (temp != NULL and ctr++<offset) {
				temp = temp->next;
			}

			// edge case to add before head ??
			// add new node just before temp curr position
			if (temp == NULL) {
				Node* new_node = new Node(ch, NULL, tail);
				tail->next = new_node;
				tail = new_node;
			} else if (temp->next == NULL) {
				// this case is automatically handled in else, unnecessary
				Node* new_node = new Node(ch, tail, tail->prev);
				tail->prev->next = new_node;
				tail->prev = new_node;
			} else if (temp->prev == NULL) {
				Node* new_node = new Node(ch, head, NULL);
				head->prev = new_node;
				head = new_node;
			} else {
				Node* new_node = new Node(ch, temp, temp->prev);
				temp->prev->next = new_node;
				temp->prev = new_node;
			}
		}

		update_lines();
	}

	pair<int, char> remove(int line, int offset) {
		update_lines();

		// indexing fix
		line--;
		offset--;

		if (head == NULL) {
			return {-1, ' '};
		} else if (head == tail and line + offset != 0) {
			char ret = head->ch;
			head = NULL;
			tail = NULL;
			return {0, ret};
		} else if (head == tail and line + offset == 0) {
			return {-1, ' '};
		} else if (offset == 0 and lines[line] == head) {
			return {-1, ' '};
		} else {
			Node* temp = lines[line];
			int ctr = 0;
			while (temp != NULL and ctr++<offset) {
				temp = temp->next;
			}

			char ret;
			// int part = 0;

			if (temp == NULL) {
				Node* del = tail;
				
				tail = tail->prev;
				
				ret = del->ch;
				delete del;
			} else if (temp->prev == NULL and offset == 1) {
				Node* del = head;
				
				head = head->next;
				
				ret = del->ch;
				delete del;
			} else if (temp->prev == NULL and offset == 0) {
				return {-1, ' '};
			} else if (temp == tail) {
				// part = 1;
				Node* del = tail;
				
				tail = tail->prev;
				tail->next = NULL;

				ret = del->ch;
				delete del;
			} else {
				Node* del = temp;
				
				temp->next->prev = temp->prev;
				temp->prev->next = temp->next;
				
				ret = del->ch;
				delete del;
			}

			return {0, ret};
		}

		update_lines();
	}

	void update_lines() {
		// temp fix
		// temporary bad time complexity func, haven't figured out how to configure lines without this.
		
		Node* del = pre_head;
		Node* new_pre_head = new Node(' ', head, NULL);
		pre_head = new_pre_head;
		delete del;

		lines = {pre_head};
		Node* temp = head;
		while (temp != NULL) {
			if (temp->ch == '\n') {
				lines.push_back(temp);
			}
			temp = temp->next;
		}
	}

	vector<Node*> get_lines() {
		return lines;
	}

	string get_buffer() {
		Node* temp = head;
		string buffer = "";
		while (temp != NULL) {
			buffer += temp->ch;
			temp = temp->next;
		}
		return buffer;
	}
};

class CursorManagement {
public:
	int line;
	int offset;
	// vector<Node*> lines;

	// CursorManagement(vector<Node*> l) {
	// 	line = 0;
	// 	offset = 0;
	// 	lines = l;
	// }

	CursorManagement() {
		line = 1;
		offset = 1;
	}

	void bound(vector<Node*> lines) {
		if (line < 1) {
			line = 1;
		}
		if (line > lines.size()) {
			line = lines.size();
		}
		if (offset < 1) {
			offset = 1;
		}
	}

	// void router() {

	// }

	void up(vector<Node*> lines) {
		line--;
		offset = 1;
		bound(lines);
	}

	void down(vector<Node*> lines) {
		line++;
		offset = 1;
		bound(lines);
	}

	void right(vector<Node*> lines) {
		offset++;
		bound(lines);
	}

	void left(vector<Node*> lines) {
		offset--;
		bound(lines);
	}

	int get_line() {
		return line;
	}

	int get_offset() {
		return offset;
	}

	void inc_line(vector<Node*> lines) {
		line++;
		bound(lines);
	}

	void dec_line(vector<Node*> lines) {
		line--;
		bound(lines);
	}

	void inc_offset(vector<Node*> lines) {
		offset++;
		bound(lines);
	}

	void dec_offset(vector<Node*> lines) {
		offset--;
		bound(lines);
	}

	void reset_offset(vector<Node*> lines) {
		offset = 1;
		bound(lines);
	}

	void reset_line(vector<Node*> lines) {
		line = 1;
		bound(lines);
	}
};

struct triplet {
	int flag;
	char ch;
	int sec_inp;
	triplet(int f, char c, int si): flag(f), ch(c), sec_inp(si) {}
	void disp() {cout << flag << "\t:" << ch << "\t:" << sec_inp << endl;}
};

class InputManagement {
public:
	triplet* input() {
		// 1 = up
		// 2 = down
		// 3 = right
		// 4 = left
		// 5 = backspace
		// 6 = esc
		int inp = getch();
		// if (char(inp) == 'q') {exit(EXIT_FAILURE);} // only debug
		int inp_int;
		if (inp == 224) {
			inp = getch();
			switch (inp) {
			case 72:
				inp_int = 1;
				break;
			case 80:
				inp_int = 2;
				break;
			case 75:
				inp_int = 3;
				break;
			case 77:
				inp_int = 4;
				break;
			}
			return new triplet(1, ' ', inp_int);
		}
		
		char inp_ch;
		switch (char(inp)) {
		case '\r':
			inp_ch = '\n';
			break;
		case '\b':
			return new triplet(1, ' ', 5);
		case char(27):
			return new triplet(1, ' ', 6);
		default:
			inp_ch = char(inp);
		}
		return new triplet(0, inp_ch, -1);
	}
};


// this class can be improved by linking it directly to the input class, so as to take line and offset as input and erase or draw a character at that specific position as the input demands and only redraw when commands are used
class Display {
public:
	pair<int, int> size;
	pair<int, int> position;

	Display(pair<int, int> s, pair<int, int> pos) {
		size = s;
		position = pos;
	}

	string slice(string text, int start, int end) {
		int size = text.size();
		string ret = text;
		if (start < size and end <= size and start >= 0 and end >= 0 and end >= start) {
			ret = text.substr(start, end-start);
		}
		return ret;
	}

	// the update functions are very inefficient currently because they are drawing the whole box for every update
	void update(string buffer, int line, int offset, string comments) {
		// line 	= 1 onwards
		// offset 	= 1 onwards
		// cout << "\033[2J\033[H" << buffer << "\n\n\n" << stats << "\033[H\033[" << line << ";" << offset << "H";

		if (buffer.empty()) {
			buffer = comments;
		}

		line--;
		offset--;
		update(buffer, comments);
		cout << "\033[" << position.second + line << ";" << position.first + offset << "H" << flush;
	}

	void update(string buffer, string comments) {
		if (buffer.empty()) {
			buffer = comments;
		}

		for (int i = 0; i < size.second; i++) {
			// with vertical border
			cout << "\033[" << position.second + i << ";" << position.first - 1 << "H" << "|";
			cout << "\033[" << position.second + i << ";" << position.first + size.first << "H" << "|";
			cout << "\033[" << position.second + i << ";" << position.first << "H" << "\033[" << size.first << "X";
		}

		// horizontal border
		string horizontal = "";
		for (int i = 0; i < size.first; i++) {
			horizontal += "-";
		}
		cout << "\033[" << position.second - 1 << ";" << position.first << "H" << horizontal;
		cout << "\033[" << position.second + size.second << ";" << position.first << "H" << horizontal;

		string line = "";
		int ctr = 0;
		bool sad_end = false;
		for (auto it: buffer) {
			if (it == '\n') {
				// cout << "\033[" << position.second + ctr << ";" << position.first << "H" << "\033[" << size.first << "X";
				cout << "\033[" << position.second + ctr << ";" << position.first << "H" << slice(line, 0, size.first);
				line = "";
				ctr++;
			} else {
				line += it;
			}
			if (ctr >= size.second) {
				sad_end = true;
				break;
			}
		}
		if (!sad_end) {
			cout << "\033[" << position.second + ctr << ";" << position.first << "H" << slice(line, 0, size.first);
			line = "";
			ctr++;
		}
		cout << flush;
	}
};

enum Mode {
	INSERT,
	COMMAND,
	ROUTER,
	NONE
};

enum Config {
	UP,
	DOWN,
	RIGHT,
	LEFT,
	REMOVE,
	ESC_BREAK,
	ENTER_REMAP,
	ADD,
	INPUT_INTERCEPT,
	INSTRUCTIONS
};

class TextBox {
public:
	Edit edit;
	CursorManagement cursor_management;
	Display* display;
	InputManagement input_management;
	Mode mode;
	Mode activate_on_mode;
	pair<int, int> size;
	pair<int, int> position;
	map<Config, bool> configuration;
	map<Config, string> constants;

	TextBox(Mode aom, Mode current_mode, pair<int, int> s, pair<int, int> pos) {
		activate_on_mode = aom;
		mode = current_mode;
		size = s;
		position = pos;
		display = new Display(s, pos);
		constants = {{INSTRUCTIONS, "Instructions: Esc to exit insert mode."}};
		configuration = {{UP, true}, {DOWN, true}, {RIGHT, true}, {LEFT, true}, {REMOVE, true}, {ESC_BREAK, true}, {ENTER_REMAP, false}, {ADD, true}, {INPUT_INTERCEPT, true}};
	}

	TextBox(Mode aom, Mode current_mode, pair<int, int> s, pair<int, int> pos, map<Config, bool> config, map<Config, string> c) {
		activate_on_mode = aom;
		mode = current_mode;
		size = s;
		position = pos;
		display = new Display(s, pos);
		constants = c;
		configuration = config;
	}

	string get_text() {
		return edit.get_buffer();
	}

	vector<pair<int, int>> search_text(string text) {
		return edit.search(text);
	}

	map<Config, string> get_constants() {
		return constants;
	}

	void set_constants(map<Config, string> c) {
		constants = c;
	}

	bool reverse() {
		return edit.reverse();
	}

	void clear() {
		edit.clear();
		cursor_management.reset_offset(edit.get_lines());
		cursor_management.reset_line(edit.get_lines());
	}

	void update() {
		display->update(edit.get_buffer(), cursor_management.get_line(), cursor_management.get_offset(), constants[INSTRUCTIONS]);
	}

	Mode loop(Mode m) {
		mode = m;
		// string stats = "Stats: \n"; // only debug
		if (configuration[INPUT_INTERCEPT]) {
			triplet* inp = input_management.input();
			if (inp->flag == 1) {
				switch (inp->sec_inp) {
				case 1:
					if (configuration[UP]) {
						cursor_management.up(edit.get_lines());
					}
					break;
				case 2:
					if (configuration[DOWN]) {
						cursor_management.down(edit.get_lines());
					}
					break;
				case 3:
					if (configuration[LEFT]) {
						cursor_management.left(edit.get_lines());
					}
					break;
				case 4:
					if (configuration[RIGHT]) {
						cursor_management.right(edit.get_lines());
					}
					break;
				case 5:
					if (configuration[REMOVE]) {
						pair<int, char> ret = edit.remove(cursor_management.get_line(), cursor_management.get_offset());
						// stats += to_string(ret.first) + "\t" + to_string(ret.second); // only debug
						if (ret.first == 0) {
							switch (ret.second) {
							case '\n':
								cursor_management.dec_line(edit.get_lines());
								cursor_management.reset_offset(edit.get_lines());
								break;
							default:
								cursor_management.dec_offset(edit.get_lines());
							}
						}
					}
					break;
				case 6:
					if (configuration[ESC_BREAK]) {
						mode = COMMAND;
					}
					break;
				}
			} else if (inp->flag == 0) {
				if (configuration[ADD]) {
					bool add = false;
					
					if (configuration[ENTER_REMAP] and inp->ch != '\n') {
						edit.add(inp->ch, cursor_management.get_line(), cursor_management.get_offset());
						add = true;
					} else if (!configuration[ENTER_REMAP]) {
						edit.add(inp->ch, cursor_management.get_line(), cursor_management.get_offset());
						add = true;
					}

					if (add) {
						switch (inp->ch) {
						case '\n':
							if (!configuration[ENTER_REMAP]) {
								cursor_management.inc_line(edit.get_lines());
								cursor_management.reset_offset(edit.get_lines());
							} else {
								mode = ROUTER;
							}
							break;
						default:
							cursor_management.inc_offset(edit.get_lines());
						}
					} else {
						if (configuration[ENTER_REMAP] and inp->ch == '\n') {
							mode = ROUTER;
						}
					}
				}
			}
			// stats += "\n" + to_string(cursor_management.get_line()) + "\t" + to_string(cursor_management.get_offset()) + "\n"; // only debug
			// display.update(edit.get_buffer(), cursor_management.get_line(), cursor_management.get_offset(), stats + "\n\n\n\n\n" + instructions); // only debug
		}
		display->update(edit.get_buffer(), cursor_management.get_line(), cursor_management.get_offset(), constants[INSTRUCTIONS]);
		return mode;
	}
};

enum def {
	DEFAULT,
	CUSTOM
};

class V {
public:
	vector<TextBox*> ui;
	Mode mode;
	bool active;

	V(def d) {
		active = true;

		if (d == DEFAULT) {
			mode = INSERT;

			TextBox* insert = new TextBox(INSERT, INSERT, {150, 49}, {2, 2}); // 150 49
			TextBox* command = new TextBox(COMMAND, INSERT, {150, 1}, {2, 52}, {{UP, false}, {DOWN, false}, {RIGHT, true}, {LEFT, true}, {REMOVE, true}, {ESC_BREAK, false}, {ENTER_REMAP, true}, {ADD, true}, {INPUT_INTERCEPT, true}}, {{INSTRUCTIONS, "Command Line"}});
			TextBox* instructions = new TextBox(NONE, NONE, {50, 25}, {153, 2}, {{UP, false}, {DOWN, false}, {RIGHT, false}, {LEFT, false}, {REMOVE, false}, {ESC_BREAK, false}, {ENTER_REMAP, false}, {ADD, false}, {INPUT_INTERCEPT, false}}, {{INSTRUCTIONS, "Instructions\n\nDefault mode is INSERT mode.\nEsc to switch to COMMAND mode.\n\nINSERT mode allows you to edit text.\nCOMMAND mode allows you to write a command in the\n command line.\n\nAvailable commands in COMMAND mode:\n\t':q' to quit.\n\t':s target_text' to search for text.\n\t':r' to reverse text."}});
			TextBox* result = new TextBox(NONE, NONE, {50, 24}, {153, 27}, {{UP, false}, {DOWN, false}, {RIGHT, false}, {LEFT, false}, {REMOVE, false}, {ESC_BREAK, false}, {ENTER_REMAP, false}, {ADD, false}, {INPUT_INTERCEPT, false}}, {{INSTRUCTIONS, "Result\n\n"}});
			TextBox* current_mode = new TextBox(NONE, NONE, {50, 1}, {153, 52}, {{UP, false}, {DOWN, false}, {RIGHT, false}, {LEFT, false}, {REMOVE, false}, {ESC_BREAK, false}, {ENTER_REMAP, false}, {ADD, false}, {INPUT_INTERCEPT, false}}, {{INSTRUCTIONS, "--INSERT--"}});

			ui.push_back(insert);
			ui.push_back(command);
			ui.push_back(instructions);
			ui.push_back(result);
			ui.push_back(current_mode);
		}
	}

	void update_mode_display() {
		map<Config, string> res = ui[4]->get_constants();
		string ret = "";
		switch (mode) {
		case INSERT:
			ret = "INSERT";
			break;
		case COMMAND:
			ret = "COMMAND";
			break;
		case ROUTER:
			ret = "ROUTER";
			break;
		case NONE:
			ret = "NONE";
			break;
		default:
			ret = "INVALID";
		}
		res[INSTRUCTIONS] = string("--") + ret + string("--");
		ui[4]->set_constants(res);
		ui[4]->update();
	}

	void v() {
		for (auto it: ui) {
			it->update();
		}

		while (active) {
			update_mode_display(); // cursor position BUGBUGBUG
			switch (mode) {
			case INSERT:
				ui[0]->update();
				mode = ui[0]->loop(mode);
				break;
			case COMMAND:
				ui[1]->update();
				mode = ui[1]->loop(mode);
				break;
			case ROUTER:
				router(ui[1]->get_text());
				break;
			}
		}

		cout << "\033[2J\033[H";
	}

	string slice(string text, int start, int end) {
		int size = text.size();
		string ret = text;
		if (start < size and end <= size and start >= 0 and end >= 0 and end >= start) {
			ret = text.substr(start, end-start);
		}
		return ret;
	}

	string slice(string text, int start) {
		int size = text.size();
		string ret = text;
		if (start < size and start >= 0) {
			ret = text.substr(start);
		}
		return ret;
	}

	void router(string text) {
		if (text == ":q") {
			active = false;
		} else if (slice(text, 0, 2) == ":s") {
			vector<pair<int, int>> res = ui[0]->search_text(slice(text, 3));

			string result = "Result\n\nLine\tOffset\n";
			for (auto it: res) {
				result += to_string(it.first) + "\t" + to_string(it.second) + "\n";
			}

			map<Config, string> getc = ui[3]->get_constants();
			getc[INSTRUCTIONS] = result;
			ui[3]->set_constants(getc);
			ui[3]->update();

			mode = INSERT;
		} else if (text == ":r") {
			bool ret = ui[0]->reverse();
			map<Config, string> getc = ui[3]->get_constants();
			getc[INSTRUCTIONS] = string("Result\n\n") + ((ret)?"Success":"Failed");
			ui[3]->set_constants(getc);
			ui[3]->update();
			mode = INSERT;
		} else {
			mode = INSERT;
		}
		ui[1]->clear();
		ui[0]->update();
		ui[1]->update();
	}
};

int main() {
	// Display class testing
	/*
	Display display;
	while (true) {
		for (int i = 1; i < 10; i++) {
			for (int j = 1; j < 10; j++) {
				display.update("hello world\nno way does this work\nwow!", i, j);
				Sleep(100);
			}
		}
	}
	*/

	//InputManagement class testing
	/*
	InputManagement input_management;
	while (true) {
		// cout << "\033[2J\033[H";
		input_management.input()->disp();
	}
	*/

	// CursorManagement class testing
	/*
	CursorManagement cursor_management;
	Display display;
	InputManagement input_management;
	while (true) {
		triplet* inp = input_management.input();
		if (inp->flag == 1) {
			switch (inp->sec_inp) {
			case 1:
				cursor_management.up();
				break;
			case 2:
				cursor_management.down();
				break;
			case 3:
				cursor_management.left();
				break;
			case 4:
				cursor_management.right();
				break;
			}
		}
		display.update("hello world\nno way does this work\nwow!\n\n" + to_string(cursor_management.get_line()) + " " + to_string(cursor_management.get_offset()), cursor_management.get_line(), cursor_management.get_offset());
	}
	*/

	// Edit class testing
	/*
	Edit edit;
	CursorManagement cursor_management;
	Display display;
	InputManagement input_management;
	while (true) {
		string stats = "Stats: \n";
		string instructions = "Instructions: Esc to exit insert mode.";
		triplet* inp = input_management.input();
		if (inp->flag == 1) {
			switch (inp->sec_inp) {
			case 1:
				cursor_management.up(edit.get_lines());
				break;
			case 2:
				cursor_management.down(edit.get_lines());
				break;
			case 3:
				cursor_management.left(edit.get_lines());
				break;
			case 4:
				cursor_management.right(edit.get_lines());
				break;
			case 5:
				pair<int, char> ret = edit.remove(cursor_management.get_line(), cursor_management.get_offset());
				stats += to_string(ret.first) + "\t" + to_string(ret.second);
				if (ret.first == 0) {
					switch (ret.second) {
					case '\n':
						cursor_management.dec_line(edit.get_lines());
						cursor_management.reset_offset(edit.get_lines());
						break;
					default:
						cursor_management.dec_offset(edit.get_lines());
					}
				}
				break;
			case 6:

				break;
			}
		} else if (inp->flag == 0) {
			edit.add(inp->ch, cursor_management.get_line(), cursor_management.get_offset());
			switch (inp->ch) {
			case '\n':
				cursor_management.inc_line(edit.get_lines());
				cursor_management.reset_offset(edit.get_lines());
				break;
			default:
				cursor_management.inc_offset(edit.get_lines());
			}
		}
		stats += "\n" + to_string(cursor_management.get_line()) + "\t" + to_string(cursor_management.get_offset()) + "\n";
		display.update(edit.get_buffer(), cursor_management.get_line(), cursor_management.get_offset(), stats + "\n\n\n\n\n" + instructions);
	}
	*/

	// V testing
	V v(DEFAULT);
	v.v();
}