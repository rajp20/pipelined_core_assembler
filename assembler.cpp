#include <fstream>
#include <string>
#include <map>
#include <bitset>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iterator>
#include <iostream>

using namespace std;

void firstPass(string input_file, map<string, int> &labelMap);

bool isLabel(string line);

void secondPass(string input_file, map<string, int> labelMap);

void addCodeToCOE(string linesToAdd, string out_file);

string getBinaryRegDecLabel(string arg, map<string, int> labelMap, int currentIndex);

map<string, string> opCodeMapToBinary;
map<string, string> regMapToBinary;

int main(int argv, char **argc) {
  if (argv != 2) {
    cout << "Please enter input filename." << endl;
    return 0;
  }

  string input_file = argc[1];
  if (input_file.substr(input_file.length() - 4) != ".asm") {
    cout << "Invalid file. File does not end in '.asm'." << endl;
    return 0;
  }

  ifstream inFile;
  // Make sure the file is readable.
  inFile.open(input_file);
  if (!inFile) {
    cout << "File could not be read or found." << endl;
    return 0;
  }
  inFile.close();

  // All of the binary opcodes associated with their instruction.
  opCodeMapToBinary["nop"] = "0000";
  opCodeMapToBinary["sub"] = "0001";
  opCodeMapToBinary["add"] = "0010";
  opCodeMapToBinary["addi"] = "0011";
  opCodeMapToBinary["shlli"] = "0100";
  opCodeMapToBinary["shrli"] = "0101";
  opCodeMapToBinary["jump"] = "0110";
  opCodeMapToBinary["jumpl"] = "0111";
  opCodeMapToBinary["jumpg"] = "1000";
  opCodeMapToBinary["jumpe"] = "1001";
  opCodeMapToBinary["jumpne"] = "1010";
  opCodeMapToBinary["cmp"] = "1011";
  opCodeMapToBinary["load"] = "1100";
  opCodeMapToBinary["loadi"] = "1101";
  opCodeMapToBinary["store"] = "1110";
  opCodeMapToBinary["mov"] = "1111";

  // All of the registers with their associated binary value. $r0 - $r27
  regMapToBinary["$r0"] = "00000";
  regMapToBinary["$r1"] = "00001";
  regMapToBinary["$r2"] = "00010";
  regMapToBinary["$r3"] = "00011";
  regMapToBinary["$r4"] = "00100";
  regMapToBinary["$r5"] = "00101";
  regMapToBinary["$r6"] = "00110";
  regMapToBinary["$r7"] = "00111";
  regMapToBinary["$r8"] = "01000";
  regMapToBinary["$r9"] = "01001";
  regMapToBinary["$r10"] = "01010";
  regMapToBinary["$r11"] = "01011";
  regMapToBinary["$r12"] = "01100";
  regMapToBinary["$r13"] = "01101";
  regMapToBinary["$r14"] = "01110";
  regMapToBinary["$r15"] = "01111";
  regMapToBinary["$r16"] = "10000";
  regMapToBinary["$r17"] = "10001";
  regMapToBinary["$r18"] = "10010";
  regMapToBinary["$r19"] = "10011";
  regMapToBinary["$r20"] = "10100";
  regMapToBinary["$r21"] = "10101";
  regMapToBinary["$r22"] = "10110";
  regMapToBinary["$r23"] = "10111";
  regMapToBinary["$r24"] = "11000";
  regMapToBinary["$r25"] = "11001";
  regMapToBinary["$r26"] = "11010";
  regMapToBinary["$r27"] = "11011";
  regMapToBinary["$r28"] = "11100";
  regMapToBinary["$r29"] = "11101";
  regMapToBinary["$r30"] = "11110";
  regMapToBinary["$r31"] = "11111";

  map<string, int> labelMap;
  labelMap["start"] = 0;
  firstPass(input_file, labelMap);
  secondPass(input_file, labelMap);
  return 0;
}

/*
 * Used as the first pass through. Leave labels in jump functions as the label,
 * but record the address of the label when you pass through it so you can
 * change it in the second passthrough.
 */
void firstPass(string input_file, map<string, int> &labelMap) {
  string line;
  int index = 0;
  ifstream inFile;
  inFile.open(input_file);
  if (inFile.is_open()) {
    while (getline(inFile, line)) {
      line.erase(remove(line.begin(), line.end(), ' '), line.end());
      if (isLabel(line)) {
        // Remove the last char in the string
        string label = line.substr(0, line.size() - 1);
        labelMap[label] = index;
        cout << "Adding label " << label << " at index " << index << endl;
      } else {
        index++;
      }
    }
    inFile.close();
  }
}

/*
 * Helper function which returns true if the input string is a label.
 * False otherwise.
 */
bool isLabel(string line) {
  char endOfLine = line[line.length() - 1];
  return endOfLine == ':';
}

/*
 * Generates output binary string.
 */
void secondPass(string input_file, map<string, int> labelMap) {
  string line;
  int index = 0;

  // Input file
  ifstream inFile;
  inFile.open(input_file);

  // Output file
  string output = "";

  if (inFile.is_open()) {
    while (getline(inFile, line)) {
      line.erase(remove(line.begin(), line.end(), '\t'), line.end());
      line.erase(remove(line.begin(), line.end(), ','), line.end());
      if (!isLabel(line)) {
        // Split string in to tokens.
        cout << "Working on instruction " << line << endl;
        istringstream buf(line);
        istream_iterator<string> beg(buf), end;
        vector<string> tokens(beg, end);

        string binaryCode;

        // Get the op code binary string.
        binaryCode += opCodeMapToBinary[tokens[0]];

        if (tokens[0] == "nop") {
          binaryCode += "00000000";
        } else {
          // Get the binary string of reg/decimal/lable.
          string firstArg = getBinaryRegDecLabel(tokens[1], labelMap, index + 1);

          // Only set the top bit if it's an immediate.
          if (firstArg.length() < 7) {
            binaryCode += "00";
          }
          binaryCode += firstArg;

          if (tokens.size() > 2) {
            binaryCode += getBinaryRegDecLabel(tokens[2], labelMap, index + 1);
          }
        }
        binaryCode += "\n";
        output += binaryCode;
        cout << binaryCode << endl;
        index++;
      }
    }
    string out_file = input_file.substr(0, input_file.size() - 4) + ".data";
    addCodeToCOE(output, out_file);
    inFile.close();
  }
}

/*
 * Returns the binary of the operands.
 */
string getBinaryRegDecLabel(string arg, map<string, int> labelMap, int currentIndex) {

  // If the first character of the string is a $, then we know that this string is a register.
  if (arg[0] == '$') {
    cout << "Getting Register " << arg << endl;
    return regMapToBinary[arg];
  }
    // If the argument is in the label map, than arg is label.
  else if (labelMap.count(arg)) {
    cout << "Label " << arg << endl;
    int labelIndex = labelMap[arg];
    int offset = labelIndex - currentIndex;
    return bitset<12>(offset).to_string();
  }
    // If the string is a decimal.
  else {
    int value = stoi(arg);
    if (value >= -128 && value <= 127) {
      cout << "Converting value " << value << " to binary " <<  bitset<6>(value).to_string() << endl;
      return bitset<7>(value).to_string();
    } else {
      cout << "Immediate value needs to be greater than -128 and less than 127, this will cause error in the core"
           << endl;
      return "1";
    }
  }

}

/*
 * Helper function. Adds one line to the output coe file.
 */
void addCodeToCOE(string linesToAdd, string out_file) {
  ofstream outFile;
  outFile.open(out_file, ios::trunc);
  outFile << linesToAdd;
  outFile.close();
}
