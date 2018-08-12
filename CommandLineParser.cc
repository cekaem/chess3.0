#include "CommandLineParser.h"

#include <string>
#include <cassert>
#include <cstring>
#include <sstream>

using namespace std;


namespace {

// Helper function which will be used to convert c-strings to int and double
template<typename T>
T ConvertString(const char* str) {
  stringstream ss(str);
  T result;
  ss >> result;
  return result;
}

}  // unnamed namespace


CommandLineParser::~CommandLineParser() {
	while (parameters_.size()) {
		Parameter* parameter = parameters_.back();
		parameters_.pop_back();
		delete parameter;
	}
}

void CommandLineParser::CheckIfDoesNotExist(char parameter) {
	for (const auto* param: parameters_) {
		if (param->parameter == parameter) {
			throw CommandLineParserParameterExistsException(parameter);
		}
	}
}

CommandLineParser::Parameter* CommandLineParser::GetParameter(char parameter) {
	for (auto* param: parameters_) {
		if (param->parameter == parameter) {
			return param;
		}
	}
	throw CommandLineParserUnknownParameterException(parameter);
}

CommandLineParser::Parameter* CommandLineParser::GetParameter(char parameter, ParameterType type) {
	Parameter* result = GetParameter(parameter);
	if (result->type != type) {
		throw CommandLineParserBadParameterTypeException(parameter, type, result->type);
	}
	return result;
}

bool CommandLineParser::IsLetter(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

void CommandLineParser::AddBinaryParameter(char parameter, bool mandatory) {
	CheckIfDoesNotExist(parameter);
  parameters_.push_back(new BinaryParameter(parameter, mandatory));
}

void CommandLineParser::AddIntegerParameter(char parameter, int default_value, bool mandatory) {
	CheckIfDoesNotExist(parameter);
  parameters_.push_back(new IntegerParameter(parameter, mandatory, default_value));
}

void CommandLineParser::AddFloatParameter(char parameter, double default_value, bool mandatory) {
	CheckIfDoesNotExist(parameter);
  parameters_.push_back(new FloatParameter(parameter, mandatory, default_value));
}

void CommandLineParser::AddStringParameter(char parameter, const string& default_value, bool mandatory) {
	CheckIfDoesNotExist(parameter);
	parameters_.push_back(new StringParameter(parameter, mandatory, default_value));
}

bool CommandLineParser::GetBinaryValue(char param) {
	Parameter* parameter = GetParameter(param, ParameterType::BINARY);
	assert(parameter);
	assert(parameter->type == ParameterType::BINARY);
	return static_cast<BinaryParameter*>(parameter)->value;
}

const string& CommandLineParser::GetStringValue(char param) {
	Parameter* parameter = GetParameter(param, ParameterType::STRING);
	assert(parameter);
	assert(parameter->type == ParameterType::STRING);
	return static_cast<StringParameter*>(parameter)->value;
}

int CommandLineParser::GetIntegerValue(char param) {
	Parameter* parameter = GetParameter(param, ParameterType::INTEGER);
	assert(parameter);
	assert(parameter->type == ParameterType::INTEGER);
	return static_cast<IntegerParameter*>(parameter)->value;
}

double CommandLineParser::GetFloatValue(char param) {
	Parameter* parameter = GetParameter(param, ParameterType::FLOAT);
	assert(parameter);
	assert(parameter->type == ParameterType::FLOAT);
	return static_cast<FloatParameter*>(parameter)->value;
}

void CommandLineParser::Parse(int argc, const char** argv) {
	for (int i = 1; i < argc; ++i) {
		if (!argv[i] || strlen(argv[i]) != 2 || argv[i][0] != '-' || !IsLetter(argv[i][1])) {
			if (i == argc - 1) {
				last_parameter_.assign(argv[i]);
				break;
			} else {
				throw CommandLineParserParseError(argv[i]);
			}
		}
		char param = argv[i][1];
		Parameter* parameter = GetParameter(param);
		assert(parameter);
		parameter->is_set = true;
		switch (parameter->type) {
			case ParameterType::BINARY:
				static_cast<BinaryParameter*>(parameter)->value = true;
				break;
			case ParameterType::STRING: {
				++i;
				if (i == argc)
				{
					throw CommandLineParserParameterNeedsValueException(param);
				}
				static_cast<StringParameter*>(parameter)->value.assign(argv[i]);
				break;
			}
			case ParameterType::INTEGER: {
				++i;
				if (i == argc) {
					throw CommandLineParserParameterNeedsValueException(param);
				}
				static_cast<IntegerParameter*>(parameter)->value = ConvertString<int>(argv[i]);
				break;
			}
			case ParameterType::FLOAT: {
				++i;
				if (i == argc)
				{
					throw CommandLineParserParameterNeedsValueException(param);
				}
				static_cast<FloatParameter*>(parameter)->value = ConvertString<double>(argv[i]);
				break;
			}
		}
	}
	for (const auto* param: parameters_) {
		if (param->mandatory == true && param->is_set == false) {
			throw CommandLineParserMandatoryParameterNotSetException(param->parameter);
		}
	}
	if (last_parameter_mandatory_ && !last_parameter_.length()) {
		throw CommandLineParserMandatoryParameterNotSetException(' ');
	}
}
