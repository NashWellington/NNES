#include "util.h"

// TODO maybe add addressing mode?
// TODO unordered_map?
std::map<byte,std::string> instructions = 
{
    {0x00,"BRK"}, {0x01,"ORA"}, /*{0x02,""}*/ {0X03,"SLO"}, {0X04,"NOP"}, {0X05,"ORA"}, {0X06,"ASL"}, {0X07,"SLO"}, {0X08,"PHP"}, {0X09,"ORA"}, {0X0A,"ASL"}, {0X0B,"ANC"}, {0X0C,"NOP"}, {0X0D,"ORA"}, {0X0E,"ASL"}, {0X0F,"SLO"},
    {0x10,"BPL"}, {0x11,"ORA"}, /*{0x12,""}*/ {0X13,"SLO"}, {0X14,"NOP"}, {0X15,"ORA"}, {0X16,"ASL"}, {0X17,"SLO"}, {0X18,"CLC"}, {0X19,"ORA"}, {0X1A,"NOP"}, {0X1B,"SLO"}, {0X1C,"NOP"}, {0X1D,"ORA"}, {0X1E,"ASL"}, {0X1F,"SLO"},
    {0x20,"JSR"}, {0x21,"AND"}, /*{0x22,""}*/ {0x23,"RLA"}, {0X24,"BIT"}, {0X25,"AND"}, {0X26,"ROL"}, {0X27,"RLA"}, {0X28,"PLP"}, {0X29,"AND"}, {0X2A,"ROL"}, {0X2B,"ANC"}, {0X2C,"BIT"}, {0X2D,"AND"}, {0X2E,"ROL"}, {0X2F,"RLA"},
    {0x30,"BMI"}, {0x31,"AND"}, /*{0x32,""}*/ {0x33,"RLA"}, {0X34,"NOP"}, {0X35,"AND"}, {0X36,"ROL"}, {0X37,"RLA"}, {0X38,"SEC"}, {0X39,"AND"}, {0X3A,"NOP"}, {0X3B,"RLA"}, {0X3C,"NOP"}, {0X3D,"AND"}, {0X3E,"ROL"}, {0X3F,"RLA"},
    {0x40,"RTI"}, {0x41,"EOR"}, /*{0x42,""}*/ {0x43,"SRE"}, {0X44,"NOP"}, {0X45,"EOR"}, {0X46,"LSR"}, {0X47,"SRE"}, {0X48,"PHA"}, {0X49,"EOR"}, {0X4A,"LSR"}, {0X4B,"ALR"}, {0X4C,"JMP"}, {0X4D,"EOR"}, {0X4E,"LSR"}, {0X4F,"SRE"},
    {0x50,"BVC"}, {0x51,"EOR"}, /*{0x52,""}*/ {0x53,"SRE"}, {0X54,"NOP"}, {0X55,"EOR"}, {0X56,"LSR"}, {0X57,"SRE"}, {0X58,"CLI"}, {0X59,"EOR"}, {0X5A,"NOP"}, {0X5B,"SRE"}, {0X5C,"NOP"}, {0X5D,"EOR"}, {0X5E,"LSR"}, {0X5F,"SRE"},
    {0x60,"RTS"}, {0x61,"ADC"}, /*{0x62,""}*/ {0x63,"RRA"}, {0X64,"NOP"}, {0X65,"ADC"}, {0X66,"ROR"}, {0X67,"RRA"}, {0X68,"PLA"}, {0X69,"ADC"}, {0X6A,"ROR"}, {0X6B,"ARR"}, {0X6C,"JMP"}, {0X6D,"ADC"}, {0X6E,"ROR"}, {0X6F,"RRA"},
    {0x70,"BVS"}, {0x71,"ADC"}, /*{0x72,""}*/ {0x73,"RRA"}, {0X74,"NOP"}, {0X75,"ADC"}, {0X76,"ROR"}, {0X77,"RRA"}, {0X78,"SEI"}, {0X79,"ADC"}, {0X7A,"NOP"}, {0X7B,"RRA"}, {0X7C,"NOP"}, {0X7D,"ADC"}, {0X7E,"ROR"}, {0X7F,"RRA"},
    {0x80,"NOP"}, {0x81,"STA"}, {0x82,"NOP"}, {0x83,"SAX"}, {0X84,"STY"}, {0X85,"STA"}, {0X86,"STX"}, {0X87,"SAX"}, {0X88,"DEY"}, {0X89,"NOP"}, {0X8A,"TXA"}, /*{0X8B,""}*/ {0X8C,"STY"}, {0X8D,"STA"}, {0X8E,"STX"}, {0X8F,"SAX"},
    {0x90,"BCC"}, {0x91,"STA"}, /*{0x92,""}*/ {0x93,"SHA"}, {0X94,"STY"}, {0X95,"STA"}, {0X96,"STX"}, {0X97,"SAX"}, {0X98,"TYA"}, {0X99,"STA"}, {0X9A,"TXS"}, {0X9B,"SHS"}, {0X9C,"SHY"}, {0X9D,"STA"}, {0X9E,"SHX"}, {0X9F,"SHA"},
    {0xA0,"LDY"}, {0xA1,"LDA"}, {0xA2,"LDX"}, {0xA3,"LAX"}, {0XA4,"LDY"}, {0XA5,"LDA"}, {0XA6,"LDX"}, {0XA7,"LAX"}, {0XA8,"TAY"}, {0XA9,"LDA"}, {0XAA,"TAX"}, {0XAB,"LXA"}, {0XAC,"LDY"}, {0XAD,"LDA"}, {0XAE,"LDX"}, {0XAF,"LAX"},
    {0xB0,"BCS"}, {0xB1,"LDA"}, /*{0xB2,""}*/ {0xB3,"LAX"}, {0XB4,"LDY"}, {0XB5,"LDA"}, {0XB6,"LDX"}, {0XB7,"LAX"}, {0XB8,"CLV"}, {0XB9,"LDA"}, {0XBA,"TSX"}, /*{0XBB,""}*/ {0XBC,"LDY"}, {0XBD,"LDA"}, {0XBE,"LDX"}, {0XBF,"LAX"},
    {0xC0,"CPY"}, {0xC1,"CMP"}, {0xC2,"NOP"}, {0xC3,"DCP"}, {0XC4,"CPY"}, {0XC5,"CMP"}, {0XC6,"DEC"}, {0XC7,"DCP"}, {0XC8,"INY"}, {0XC9,"CMP"}, {0XCA,"DEX"}, {0XCB,"AXS"}, {0XCC,"CPY"}, {0XCD,"CMP"}, {0XCE,"DEC"}, {0XCF,"DCP"},
    {0xD0,"BNE"}, {0xD1,"CMP"}, /*{0xD2,""}*/ {0xD3,"DCP"}, {0XD4,"NOP"}, {0XD5,"CMP"}, {0XD6,"DEC"}, {0XD7,"DCP"}, {0XD8,"CLD"}, {0XD9,"CMP"}, {0XDA,"NOP"}, {0XDB,"DCP"}, {0XDC,"NOP"}, {0XDD,"CMP"}, {0XDE,"DEC"}, {0XDF,"DCP"},
    {0xE0,"CPX"}, {0xE1,"SBC"}, {0xE2,"NOP"}, {0xE3,"ISC"}, {0XE4,"CPX"}, {0XE5,"SBC"}, {0XE6,"INC"}, {0XE7,"ISC"}, {0XE8,"INX"}, {0XE9,"SBC"}, {0XEA,"NOP"}, {0XEB,"SBC"}, {0XEC,"CPX"}, {0XED,"SBC"}, {0XEE,"INC"}, {0XEF,"ISC"},
    {0xF0,"BEQ"}, {0xF1,"SBC"}, /*{0xF2,""}*/ {0xF3,"ISC"}, {0XF4,"NOP"}, {0XF5,"SBC"}, {0XF6,"INC"}, {0XF7,"ISC"}, {0XF8,"SED"}, {0XF9,"SBC"}, {0XFA,"NOP"}, {0XFB,"ISC"}, {0XFC,"NOP"}, {0XFD,"SBC"}, {0XFE,"INC"}, {0XFF,"ISC"}
};

// TODO add addressing mode support
std::string decompile(uword address)
{
    std::string line = hex(address) + ": ";
    line += instructions[bus.cpuRead(address)];
    return line;
}