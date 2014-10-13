#pragma once

class Item {
public:
	unsigned long type;
	std::string filename;
	
	Item() {
		type = -1;
		filename = "";
	}
	
	Item(unsigned long t, std::string f) {
		type = t;
		filename = f;
	}
	
	Item(const Item &i) {
		type = i.type;
		filename = i.filename;
	}
};

class Triplet {
public:
	vector<Item> items;
	std::string params;
	
	Triplet() {
		params = "";
	}
	
	Triplet(std::string p) {
		params = p;
	}
	
	Triplet(const Triplet &t) {
		items = t.items;
		params = t.params;
	}
};