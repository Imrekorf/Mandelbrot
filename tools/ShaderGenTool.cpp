#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "ConsoleArgumentCpp/ArgumentParser.hpp"

#include <regex>

#include "git_rev.h"
#include <stdio.h>

#define STR(x) #x
#define STRX(x) STR(x)


#ifndef GIT_HASH
#define GIT_HASH 0000000-dirty
#endif

using namespace ArgPar;

int main(int argc, const char* argv[]) {

	ArgumentParser AP("ShaderGenTool", 1, 0);
	
	AP.addFlag("-G", "--Git")
		.Help("Displays the git commit hash this software was build with")
		.Action([](const std::vector<std::string>&){
				std::cout << "Software build with git commit: " << std::hex << STRX(GIT_HASH) << std::endl;
				exit(0);
		}, false)
		.ParseAlways();

	AP.addArgument<std::string>("-S", "--Shader")
		.Help("Input shader file full path")
		.Required()
		.ParameterName("Input File")
		.Validator([](const std::vector<std::string>& parameters){
			if (FILE *file = fopen(parameters[0].c_str(), "r")) {
				fclose(file);
				return 0;
			} else {
				std::cout << "\"" << parameters[0] << "\" does not exist!" << std::endl;
				return 1;
			}
		});

	AP.addArgument<std::string>("-O", "--output")
		.Help("Specifies the output directory")
		.DefaultValue(".");


	try{
		AP.ParseArguments(argc, argv);
	}
	catch(const ValidatorException& VE){
		std::cout << "Validation for " << VE.ArgumentName() << " failed at position: " << VE.ArgumentPosition() << std::endl;
		return -1;
	}
	catch(const MissingRequiredParameter& MRPE){
		std::cout << MRPE.what() << std::endl;
		std::cout << "MissingRequiredParameter: ";
		for(auto MRP : MRPE.missingArguments()){
			std::cout << MRP;
		}
		std::cout << std::endl;
		return -1;
	}

	std::string file_path = AP["--Shader"].Parse<std::string>(0);
	std::string file_name;
	std::string file_ext = ""; // will be .frag, .vert, etc...
	std::size_t pos = file_path.find_last_of("\\/");
	
	if (pos != std::string::npos)
		file_name = file_path.substr(pos+1);
	else
		file_name = file_path;

	pos = file_name.find_last_of(".");
	if (pos != std::string::npos) {
		file_ext = file_name.substr(pos);
		file_name = file_name.substr(0, pos);
	}
	
	std::cout << "Parsing shader file: " << file_name << file_ext << std::endl;

	std::ifstream shader_file;
	shader_file.open(file_path, std::ios::in);

	std::string file_content;

	std::vector<std::string> uniforms;

	if (shader_file.is_open()) {
		std::string temp_line;
		while(std::getline(shader_file, temp_line)) {
			if (temp_line.find("uniform") != std::string::npos)
				uniforms.push_back(temp_line);
			file_content += "\t" + temp_line + "\n";
		}
		shader_file.close();
	} else {
		std::cout << "unable to open " << file_path << std::endl;
		return -2;
	}


	std::string header_output, source_output;
	header_output += 	"#ifndef __GEN_@FILE_NAME@_H__\n"
					  	"#define __GEN_@FILE_NAME@_H__\n"
						"\n"
						"/* File generated from @file_name@ */\n"
						"\n"
						"#ifdef __cplusplus\n"
						"extern \"C\" {\n"
						"#endif\n"
						"\n"
						"/* GSV: Generated Shader Value */\n"
						"namespace GSV {\n"
						"\n"
						"#ifndef GSV_TYPEDEF\n"
						"#define GSV_TYPEDEF\n"
						"\n"
						"typedef const char* const shader_program_t;\n"
						"typedef const char* const uniform_name_t  ;\n"
						"\n"
						"#endif\n"
						"\n"
						"extern shader_program_t @file_name@_src;\n"
						"@extern_uniforms@"
						"\n"
						"}; // namespace GSV\n"
						"\n"
						"#ifdef __cplusplus\n"
						"}\n"
						"#endif\n"
						"\n"
						"#endif /* __GEN_@FILE_NAME@_H__ */\n";

	source_output += 	"/* File generated from @file_name@ */\n"
						"\n"
						"#include \"@file_name@.h\"\n"
						"\n"
						"#ifdef __cplusplus\n"
						"extern \"C\" {\n"
						"#endif\n"
						"\n"
						"/* GSV: Generated Shader Value */\n"
						"namespace GSV {\n"
						"\n"
						"@uniforms@"
						"shader_program_t @file_name@_src = R\"(\n"
						"@file_content@"
						")\";\n"
						"\n"
						"}; // namespace GSV\n"
						"\n"
						"#ifdef __cplusplus\n"
						"}\n"
						"#endif\n"
						"\n";

	std::string extern_uniforms;
	std::string str_uniforms;
	for (size_t i = 0; i < uniforms.size(); i++) {
		size_t pos = uniforms[i].find_last_of(" ");
		uniforms[i] = uniforms[i].substr(pos+1);
		pos = uniforms[i].find_first_of(";");
		uniforms[i] = uniforms[i].substr(0, pos);
		pos = uniforms[i].find_first_of("[");
		if (pos != std::string::npos)
			uniforms[i] = uniforms[i].substr(0, pos);
		extern_uniforms += "extern uniform_name_t " + uniforms[i] + ";\n";
		str_uniforms += "uniform_name_t " + uniforms[i] + " = \"" + uniforms[i] + "\";\n";
	}

	std::string uppercase_file_name = file_name;
	std::transform(uppercase_file_name.begin(), uppercase_file_name.end(), uppercase_file_name.begin(), ::toupper);
	header_output = std::regex_replace(header_output, std::regex("@FILE_NAME@"), uppercase_file_name);
	header_output = std::regex_replace(header_output, std::regex("@file_name@"), file_name);
	header_output = std::regex_replace(header_output, std::regex("@extern_uniforms@"), extern_uniforms);

	source_output = std::regex_replace(source_output, std::regex("@file_name@"), file_name);
	source_output = std::regex_replace(source_output, std::regex("@uniforms@"), str_uniforms);
	source_output = std::regex_replace(source_output, std::regex("@file_content@"), file_content);

	std::ofstream header_file, source_file;
	std::string path = AP["-O"].Parse<std::string>(0);
	if (*path.rend() == '/') {
		path += file_name;
	} else {
		path += "/" + file_name;
	}

	std::cout << "saving to " << path << ".h" << std::endl;
	header_file.open(path + ".h");
	if (header_file.is_open()) {
		header_file << header_output;
		header_file.close();
	} else {
		std::cout << "unable to create " << path << ".h" << std::endl;
		return -3;
	}

	std::cout << "saving to " << path << ".cpp" << std::endl;
	source_file.open(path + ".cpp");
	if (source_file.is_open()) {
		source_file << source_output;
		source_file.close();
	} else {
		std::cout << "unable to create " << path << ".cpp" << std::endl;
		return -3;
	}

	return 0;
}