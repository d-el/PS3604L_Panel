#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <string>
#include <algorithm>
#include <optional>
#include <json/json.h>

struct NamedParam{
	std::string name;
	Json::Value param;
};

std::optional<NamedParam> find(const Json::Value& from, std::uint16_t addr){
	using namespace std;
	for(const auto& group : from){
		for(auto param = group.begin(); param != group.end(); ++param)
			if(stoul((*param)["address"].asString(), nullptr, 0) == addr)
				return NamedParam { param.key().asString(), *param };
	}
	return{};
}

int main(int argc, char* argv[]){
	using namespace std;
	ifstream in(argv[1]);
	uint16_t address = strtol(argv[2], nullptr, 16);

	Json::Reader reader;
	Json::Value root;
	reader.parse(in, root);

    cout << hex << uppercase << setfill( '0' );
	auto p = find( root[ "parameters" ], address );
	if(p){
		cout << "[" << p->name << "]\n" << p->param << "\n";
	}
	else{
		cout << "no parameter\n";
	}
}
