#include <list>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <regex>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <math.h>

struct Token{
	Token(std::string t, char d): token(t), delimiter(d) {};
	std::string token;
	char delimiter;
};

using tokens_t = std::list<Token>;

struct Mask{
	struct Value{
		Value(std::string n, std::string v): name(n), value(v) {};
		std::string name;
		std::string value;
	};
	std::string name;
	std::list<Value> value;
	std::string type;
};

using masks_t = std::list<Mask>;

typedef struct{
	std::string name;
	std::string unit;
	std::string type;
	std::string chmod;
	std::string callback;
	std::string param;
	std::string addr;
	std::string def;
	std::string min;
	std::string max;
	std::string step;
	std::string bigstep;
	std::string pow;
	std::string savetype;
}parameter_t;

using parameters_t = std::list<parameter_t>;
using groupProperties_t = std::map<std::string, std::string>;

struct Group{
	Group(groupProperties_t &prop, parameters_t &param): properties(prop), parameters(param){}
	bool operator==(std::string prop){
		return this->properties["access"] == prop;
	}
	groupProperties_t properties;
	parameters_t parameters;
};

using parameterstree_t = std::list< Group >;

namespace string_parsing {
inline void trim(std::string &str) {
	auto space_is_it = [] (char c) {
		return c > '\0' && c <= ' ';
	};

	for(auto rit = str.rbegin(); rit != str.rend(); ++rit) {
		if(!space_is_it(*rit)) {
			if(rit != str.rbegin()) {
				str.erase(&*rit - &*str.begin() + 1);
			}
			for(auto fit=str.begin(); fit != str.end(); ++fit) {
				if(!space_is_it(*fit)) {
					if(fit != str.begin()) {
						str.erase(str.begin(), fit);
					}
					return;
	}	}	}	}
str.clear();
}

template <typename D>
inline std::string get_to(std::string& str, D&& delimiters, char& found_delimiter) {
	const auto pos = str.find_first_of(std::forward<D>(delimiters));
	if(pos == std::string::npos) {
		found_delimiter = '\0';
		std::string result;
		std::swap(result, str);
		trim(result);
		return result;
	}
	found_delimiter = str[pos];
	auto left = str.substr(0, pos);
	trim(left);
	str.erase(0, pos + 1);
return left;
}

template <typename D>
inline std::string get_to(std::string& str, D&& delimiters) {
	char discarded_delimiter;
	return get_to(str, std::forward<D>(delimiters), discarded_delimiter);
}

inline std::string pad_right(const std::string&     str,
                             std::string::size_type min_length,
                             char                   pad_char=' ')
{
	if(str.length() >= min_length ) return str;
	return str + std::string(min_length - str.length(), pad_char);
}

}

int main(int argc, char *argv[]) {
	setvbuf(stdout, NULL, _IONBF, 0);	//not buffered
	std::string fileName = argv[1];
	std::string outfileName = argv[2];
	std::string mode = argv[3];
	std::cout << "generate '" << mode << "' > " << outfileName << "\n";

	// Read file
	std::ifstream file(fileName.c_str());
	if(!file){
		std::cerr << "Cannot open the File : " << fileName << "\n";
		return false;
	}
	std::string buffer;
	std::string line;
	while(getline(file, line)){
		if(line.size() == 0 || line.substr(0,3)=="\357\273\277"){
			continue;
		}
		buffer += line;
	}
	file.close();

	// Tokenize
	tokens_t tokens;
	bool quote_opened = false;
	while(!buffer.empty()) {
		const char* delimiter_set = quote_opened ? "'" : ",:()";
		char delimiter;
		auto token = string_parsing::get_to(buffer, delimiter_set, delimiter);
		quote_opened = delimiter == '\'' && !quote_opened;
		tokens.push_back(Token(token, delimiter));
	}

	// Parse
	masks_t texts;
	parameterstree_t parameterstree;
	for(auto t = tokens.begin(); t != tokens.end(); ){
		if(t->token == "text"){
			t++;
			Mask mask;
			mask.name = t++->token;
			mask.type = t++->token;
			while(1){
				std::string value = t++->token;
				std::string name = t->token;
				mask.value.push_back(Mask::Value(name, value));
				if(t->delimiter == ')'){
					t++;
					break;
				}
				t++;
			}
			texts.push_back(mask);
		}
		else if(t->token == "group"){
		t++;
			groupProperties_t groupProp = { { "name", t++->token }, { "access", t++->token } };
			parameters_t parameters;
			while(t->token == "p"){
				t++;
				parameter_t parameter;
				parameter.name = t++->token;
				parameter.unit = t++->token;
				parameter.type = t++->token;
				parameter.chmod = t++->token;
				parameter.callback = t++->token;
				parameter.param = t++->token;
				parameter.addr = t++->token;
				parameter.def = t++->token;
				parameter.min = t++->token;
				parameter.max = t++->token;
				parameter.step = t++->token;
				parameter.bigstep = t++->token;
				parameter.pow = t++->token;
				parameter.savetype = t++->token;
				parameters.push_back(parameter);
				if(t == tokens.end()){
					break;
				}
			}
			parameterstree.push_back(Group(groupProp, parameters));
		}
	}

	// Open the File
	std::ofstream out(outfileName.c_str());
	// Check if object is valid
	if(!out){
		std::cerr << "Cannot open the out File : "<<fileName<<std::endl;
		return false;
	}

	if(mode == "json"){
		out << "{\n";

		if(!texts.empty()){
			out << "\t\"text\": {\n";
			for(auto text = texts.begin(); text != texts.end(); ){
				out << "\t\t\"" << text->name << "\": { \"type\": \"" << text->type << "\", \"value\": [ ";
				for(auto value = text->value.begin(); value != text->value.end(); ){
					out << "{ \"v\": " << value->value << ", \"n\": \"" << value->name << "\"";
					++value != text->value.end() ? out << " }, " : out << " } ";
				}
				++text != texts.end() ? out << "] },\n" : out << "] }\n";
			}
			out << "\t},\n\n";
		}

		out << "\t\"parameters\": {\n";
		for(auto group = parameterstree.begin(); group != parameterstree.end(); ){
			if(group->properties["access"] == "private"){
				group++;
				continue;
			}
			out << "\t\t\"" << group->properties["name"] << "\": {\n";
			parameters_t &parameters = group->parameters;
			for(const auto& param : parameters){
				out << "\t\t\t\"" << param.name << "\": { ";

				out << "\"unit\": " << param.unit << ", ";
				out << "\"format\": \"" << param.type << "\", ";
				out << "\"changemode\": \"" << param.chmod << "\", ";
				out << "\"address\": \"" << param.addr << "\", ";
				out << "\"default\": \"" << param.def << "\", ";
				out << "\"min\": " << param.min << ", ";
				out << "\"max\": " << param.max << ", ";
				out << "\"power\": " << param.pow;

				if(&param != &parameters.back()){
					out << " },\n";
				}else{
					out << " }\n";
				}
			}

			group++;
			auto findIter = std::find(group, parameterstree.end(), "public");
			if(findIter != parameterstree.end()){
				out << "\t\t},\n";
			}else{
				out << "\t\t}\n";
			}
		}
		out << "\t}\n";

		out << "}\n";
	}

	if(mode == "type"){
		// Mask
		for(auto text = texts.begin(); text != texts.end(); text++){
			out << "enum mask_" << text->name << "{\n";
			for(auto value = text->value.begin(); value != text->value.end(); value++){
				out << "\t" << value->name << " = " << value->value << ",\n";
			}
			out << "};\n\n";
		}

		for(auto group = parameterstree.begin(); group != parameterstree.end(); group++){
			if(group != parameterstree.begin()){
				out << "\n";
			}
			out << "// " << group->properties["name"] << "\n";
			parameters_t &parameters = group->parameters;
			for(const auto& param : parameters){
				out << "extern Val<" << param.type << "> " << param.name << ";\n";
			}
		}
	}

	if(mode == "object"){
		if(!texts.empty()){
			out << "namespace nmText {\n";
			for(auto text = texts.begin(); text != texts.end(); text++){
				if(text->type != "value") continue;
				out << "const Text<" << text->value.size() << "> " << text->name << " = {\n";
				for(auto value = text->value.begin(); value != text->value.end(); value++){
					out << "\t" << value->value << ", \"" << value->name << "\",\n";
				}
				out << "};\n";
			}
			out << "}\n";
			out << "\n";
		}

		size_t parameternumber = 0;
		for(auto group = parameterstree.begin(); group != parameterstree.end(); ){
			if(group != parameterstree.begin()){
				out << "\n";
			}
			out << "// " << group->properties["name"] << "\n";
			parameters_t &parameters = group->parameters;
			for(const auto& param : parameters){
				int power = std::pow(10, std::stoi(param.pow));
				const std::string &name = param.name;
				auto text = std::find_if(texts.begin(), texts.end(),
						[&name](Mask &m){ return name.find(m.name) != std::string::npos && m.type == "value"; });
				out << "const ValHandler<" << param.type << "> handler_" << param.name << "("
					<< "\"" << param.name << "\", "
					<< param.unit << ", "
					<< param.def << "*" << power << ", "
					<< param.min << "*" << power << ", "
					<< param.max << "*" << power << ", "
					<< param.step << "*" << power << ", "
					<< param.bigstep << "*" << power << ", "
					<< param.addr << ", "
					<< "reinterpret_cast<void*>(" << param.param << "), "
					<< param.pow << ", "
					<< param.callback << ", "
					<< (text != texts.end() ? "&nmText::" + text->name : "nullptr") << ", "
					<< param.savetype
					<< ");\n";

				out << "Val<" <<param.type << "> " << param.name << "(handler_" << param.name << ");\n\n";
				parameternumber++;
			}
			++group;
		}

		out << "static const std::array< IVal *, " << parameternumber << "> valuearray = {\n";
		for(auto group = parameterstree.begin(); group != parameterstree.end(); ){
			parameters_t &parameters = group->parameters;
			for(const auto& param : parameters){
				out << "\t&" << param.name << ",\n";
			}
			++group;
		}
		out << "};\n";
	}

	out.close();	//Close The File

	return 0;
}
