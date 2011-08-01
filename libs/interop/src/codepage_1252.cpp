#include <boost/interop/conversion_iterator_adapters.hpp>
#include <boost/static_assert.hpp>

namespace boost
{
namespace interop
{
namespace detail
{
const boost::u16_t  to_utf16[] =
{
  0x0000,  // 0x00 NULL
  0x0001,  // 0x01 START OF HEADING
  0x0002,  // 0x02 START OF TEXT
  0x0003,  // 0x03 END OF TEXT
  0x0004,  // 0x04 END OF TRANSMISSION
  0x0005,  // 0x05 ENQUIRY
  0x0006,  // 0x06 ACKNOWLEDGE
  0x0007,  // 0x07 BELL
  0x0008,  // 0x08 BACKSPACE
  0x0009,  // 0x09 HORIZONTAL TABULATION
  0x000A,  // 0x0A LINE FEED
  0x000B,  // 0x0B VERTICAL TABULATION
  0x000C,  // 0x0C FORM FEED
  0x000D,  // 0x0D CARRIAGE RETURN
  0x000E,  // 0x0E SHIFT OUT
  0x000F,  // 0x0F SHIFT IN
  0x0010,  // 0x10 DATA LINK ESCAPE
  0x0011,  // 0x11 DEVICE CONTROL ONE
  0x0012,  // 0x12 DEVICE CONTROL TWO
  0x0013,  // 0x13 DEVICE CONTROL THREE
  0x0014,  // 0x14 DEVICE CONTROL FOUR
  0x0015,  // 0x15 NEGATIVE ACKNOWLEDGE
  0x0016,  // 0x16 SYNCHRONOUS IDLE
  0x0017,  // 0x17 END OF TRANSMISSION BLOCK
  0x0018,  // 0x18 CANCEL
  0x0019,  // 0x19 END OF MEDIUM
  0x001A,  // 0x1A SUBSTITUTE
  0x001B,  // 0x1B ESCAPE
  0x001C,  // 0x1C FILE SEPARATOR
  0x001D,  // 0x1D GROUP SEPARATOR
  0x001E,  // 0x1E RECORD SEPARATOR
  0x001F,  // 0x1F UNIT SEPARATOR
  0x0020,  // 0x20 SPACE
  0x0021,  // 0x21 EXCLAMATION MARK
  0x0022,  // 0x22 QUOTATION MARK
  0x0023,  // 0x23 NUMBER SIGN
  0x0024,  // 0x24 DOLLAR SIGN
  0x0025,  // 0x25 PERCENT SIGN
  0x0026,  // 0x26 AMPERSAND
  0x0027,  // 0x27 APOSTROPHE
  0x0028,  // 0x28 LEFT PARENTHESIS
  0x0029,  // 0x29 RIGHT PARENTHESIS
  0x002A,  // 0x2A ASTERISK
  0x002B,  // 0x2B PLUS SIGN
  0x002C,  // 0x2C COMMA
  0x002D,  // 0x2D HYPHEN-MINUS
  0x002E,  // 0x2E FULL STOP
  0x002F,  // 0x2F SOLIDUS
  0x0030,  // 0x30 DIGIT ZERO
  0x0031,  // 0x31 DIGIT ONE
  0x0032,  // 0x32 DIGIT TWO
  0x0033,  // 0x33 DIGIT THREE
  0x0034,  // 0x34 DIGIT FOUR
  0x0035,  // 0x35 DIGIT FIVE
  0x0036,  // 0x36 DIGIT SIX
  0x0037,  // 0x37 DIGIT SEVEN
  0x0038,  // 0x38 DIGIT EIGHT
  0x0039,  // 0x39 DIGIT NINE
  0x003A,  // 0x3A COLON
  0x003B,  // 0x3B SEMICOLON
  0x003C,  // 0x3C LESS-THAN SIGN
  0x003D,  // 0x3D EQUALS SIGN
  0x003E,  // 0x3E GREATER-THAN SIGN
  0x003F,  // 0x3F QUESTION MARK
  0x0040,  // 0x40 COMMERCIAL AT
  0x0041,  // 0x41 LATIN CAPITAL LETTER A
  0x0042,  // 0x42 LATIN CAPITAL LETTER B
  0x0043,  // 0x43 LATIN CAPITAL LETTER C
  0x0044,  // 0x44 LATIN CAPITAL LETTER D
  0x0045,  // 0x45 LATIN CAPITAL LETTER E
  0x0046,  // 0x46 LATIN CAPITAL LETTER F
  0x0047,  // 0x47 LATIN CAPITAL LETTER G
  0x0048,  // 0x48 LATIN CAPITAL LETTER H
  0x0049,  // 0x49 LATIN CAPITAL LETTER I
  0x004A,  // 0x4A LATIN CAPITAL LETTER J
  0x004B,  // 0x4B LATIN CAPITAL LETTER K
  0x004C,  // 0x4C LATIN CAPITAL LETTER L
  0x004D,  // 0x4D LATIN CAPITAL LETTER M
  0x004E,  // 0x4E LATIN CAPITAL LETTER N
  0x004F,  // 0x4F LATIN CAPITAL LETTER O
  0x0050,  // 0x50 LATIN CAPITAL LETTER P
  0x0051,  // 0x51 LATIN CAPITAL LETTER Q
  0x0052,  // 0x52 LATIN CAPITAL LETTER R
  0x0053,  // 0x53 LATIN CAPITAL LETTER S
  0x0054,  // 0x54 LATIN CAPITAL LETTER T
  0x0055,  // 0x55 LATIN CAPITAL LETTER U
  0x0056,  // 0x56 LATIN CAPITAL LETTER V
  0x0057,  // 0x57 LATIN CAPITAL LETTER W
  0x0058,  // 0x58 LATIN CAPITAL LETTER X
  0x0059,  // 0x59 LATIN CAPITAL LETTER Y
  0x005A,  // 0x5A LATIN CAPITAL LETTER Z
  0x005B,  // 0x5B LEFT SQUARE BRACKET
  0x005C,  // 0x5C REVERSE SOLIDUS
  0x005D,  // 0x5D RIGHT SQUARE BRACKET
  0x005E,  // 0x5E CIRCUMFLEX ACCENT
  0x005F,  // 0x5F LOW LINE
  0x0060,  // 0x60 GRAVE ACCENT
  0x0061,  // 0x61 LATIN SMALL LETTER A
  0x0062,  // 0x62 LATIN SMALL LETTER B
  0x0063,  // 0x63 LATIN SMALL LETTER C
  0x0064,  // 0x64 LATIN SMALL LETTER D
  0x0065,  // 0x65 LATIN SMALL LETTER E
  0x0066,  // 0x66 LATIN SMALL LETTER F
  0x0067,  // 0x67 LATIN SMALL LETTER G
  0x0068,  // 0x68 LATIN SMALL LETTER H
  0x0069,  // 0x69 LATIN SMALL LETTER I
  0x006A,  // 0x6A LATIN SMALL LETTER J
  0x006B,  // 0x6B LATIN SMALL LETTER K
  0x006C,  // 0x6C LATIN SMALL LETTER L
  0x006D,  // 0x6D LATIN SMALL LETTER M
  0x006E,  // 0x6E LATIN SMALL LETTER N
  0x006F,  // 0x6F LATIN SMALL LETTER O
  0x0070,  // 0x70 LATIN SMALL LETTER P
  0x0071,  // 0x71 LATIN SMALL LETTER Q
  0x0072,  // 0x72 LATIN SMALL LETTER R
  0x0073,  // 0x73 LATIN SMALL LETTER S
  0x0074,  // 0x74 LATIN SMALL LETTER T
  0x0075,  // 0x75 LATIN SMALL LETTER U
  0x0076,  // 0x76 LATIN SMALL LETTER V
  0x0077,  // 0x77 LATIN SMALL LETTER W
  0x0078,  // 0x78 LATIN SMALL LETTER X
  0x0079,  // 0x79 LATIN SMALL LETTER Y
  0x007A,  // 0x7A LATIN SMALL LETTER Z
  0x007B,  // 0x7B LEFT CURLY BRACKET
  0x007C,  // 0x7C VERTICAL LINE
  0x007D,  // 0x7D RIGHT CURLY BRACKET
  0x007E,  // 0x7E TILDE
  0x007F,  // 0x7F DELETE
  0x20AC,  // 0x80 EURO SIGN
  0xFFFD,  // 0x81 UNDEFINED
  0x201A,  // 0x82 SINGLE LOW-9 QUOTATION MARK
  0x0192,  // 0x83 LATIN SMALL LETTER F WITH HOOK
  0x201E,  // 0x84 DOUBLE LOW-9 QUOTATION MARK
  0x2026,  // 0x85 HORIZONTAL ELLIPSIS
  0x2020,  // 0x86 DAGGER
  0x2021,  // 0x87 DOUBLE DAGGER
  0x02C6,  // 0x88 MODIFIER LETTER CIRCUMFLEX ACCENT
  0x2030,  // 0x89 PER MILLE SIGN
  0x0160,  // 0x8A LATIN CAPITAL LETTER S WITH CARON
  0x2039,  // 0x8B SINGLE LEFT-POINTING ANGLE QUOTATION MARK
  0x0152,  // 0x8C LATIN CAPITAL LIGATURE OE
  0xFFFD,  // 0x8D UNDEFINED
  0x017D,  // 0x8E LATIN CAPITAL LETTER Z WITH CARON
  0xFFFD,  // 0x8F UNDEFINED
  0xFFFD,  // 0x90 UNDEFINED
  0x2018,  // 0x91 LEFT SINGLE QUOTATION MARK
  0x2019,  // 0x92 RIGHT SINGLE QUOTATION MARK
  0x201C,  // 0x93 LEFT DOUBLE QUOTATION MARK
  0x201D,  // 0x94 RIGHT DOUBLE QUOTATION MARK
  0x2022,  // 0x95 BULLET
  0x2013,  // 0x96 EN DASH
  0x2014,  // 0x97 EM DASH
  0x02DC,  // 0x98 SMALL TILDE
  0x2122,  // 0x99 TRADE MARK SIGN
  0x0161,  // 0x9A LATIN SMALL LETTER S WITH CARON
  0x203A,  // 0x9B SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
  0x0153,  // 0x9C LATIN SMALL LIGATURE OE
  0xFFFD,  // 0x9D UNDEFINED
  0x017E,  // 0x9E LATIN SMALL LETTER Z WITH CARON
  0x0178,  // 0x9F LATIN CAPITAL LETTER Y WITH DIAERESIS
  0x00A0,  // 0xA0 NO-BREAK SPACE
  0x00A1,  // 0xA1 INVERTED EXCLAMATION MARK
  0x00A2,  // 0xA2 CENT SIGN
  0x00A3,  // 0xA3 POUND SIGN
  0x00A4,  // 0xA4 CURRENCY SIGN
  0x00A5,  // 0xA5 YEN SIGN
  0x00A6,  // 0xA6 BROKEN BAR
  0x00A7,  // 0xA7 SECTION SIGN
  0x00A8,  // 0xA8 DIAERESIS
  0x00A9,  // 0xA9 COPYRIGHT SIGN
  0x00AA,  // 0xAA FEMININE ORDINAL INDICATOR
  0x00AB,  // 0xAB LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
  0x00AC,  // 0xAC NOT SIGN
  0x00AD,  // 0xAD SOFT HYPHEN
  0x00AE,  // 0xAE REGISTERED SIGN
  0x00AF,  // 0xAF MACRON
  0x00B0,  // 0xB0 DEGREE SIGN
  0x00B1,  // 0xB1 PLUS-MINUS SIGN
  0x00B2,  // 0xB2 SUPERSCRIPT TWO
  0x00B3,  // 0xB3 SUPERSCRIPT THREE
  0x00B4,  // 0xB4 ACUTE ACCENT
  0x00B5,  // 0xB5 MICRO SIGN
  0x00B6,  // 0xB6 PILCROW SIGN
  0x00B7,  // 0xB7 MIDDLE DOT
  0x00B8,  // 0xB8 CEDILLA
  0x00B9,  // 0xB9 SUPERSCRIPT ONE
  0x00BA,  // 0xBA MASCULINE ORDINAL INDICATOR
  0x00BB,  // 0xBB RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
  0x00BC,  // 0xBC VULGAR FRACTION ONE QUARTER
  0x00BD,  // 0xBD VULGAR FRACTION ONE HALF
  0x00BE,  // 0xBE VULGAR FRACTION THREE QUARTERS
  0x00BF,  // 0xBF INVERTED QUESTION MARK
  0x00C0,  // 0xC0 LATIN CAPITAL LETTER A WITH GRAVE
  0x00C1,  // 0xC1 LATIN CAPITAL LETTER A WITH ACUTE
  0x00C2,  // 0xC2 LATIN CAPITAL LETTER A WITH CIRCUMFLEX
  0x00C3,  // 0xC3 LATIN CAPITAL LETTER A WITH TILDE
  0x00C4,  // 0xC4 LATIN CAPITAL LETTER A WITH DIAERESIS
  0x00C5,  // 0xC5 LATIN CAPITAL LETTER A WITH RING ABOVE
  0x00C6,  // 0xC6 LATIN CAPITAL LETTER AE
  0x00C7,  // 0xC7 LATIN CAPITAL LETTER C WITH CEDILLA
  0x00C8,  // 0xC8 LATIN CAPITAL LETTER E WITH GRAVE
  0x00C9,  // 0xC9 LATIN CAPITAL LETTER E WITH ACUTE
  0x00CA,  // 0xCA LATIN CAPITAL LETTER E WITH CIRCUMFLEX
  0x00CB,  // 0xCB LATIN CAPITAL LETTER E WITH DIAERESIS
  0x00CC,  // 0xCC LATIN CAPITAL LETTER I WITH GRAVE
  0x00CD,  // 0xCD LATIN CAPITAL LETTER I WITH ACUTE
  0x00CE,  // 0xCE LATIN CAPITAL LETTER I WITH CIRCUMFLEX
  0x00CF,  // 0xCF LATIN CAPITAL LETTER I WITH DIAERESIS
  0x00D0,  // 0xD0 LATIN CAPITAL LETTER ETH
  0x00D1,  // 0xD1 LATIN CAPITAL LETTER N WITH TILDE
  0x00D2,  // 0xD2 LATIN CAPITAL LETTER O WITH GRAVE
  0x00D3,  // 0xD3 LATIN CAPITAL LETTER O WITH ACUTE
  0x00D4,  // 0xD4 LATIN CAPITAL LETTER O WITH CIRCUMFLEX
  0x00D5,  // 0xD5 LATIN CAPITAL LETTER O WITH TILDE
  0x00D6,  // 0xD6 LATIN CAPITAL LETTER O WITH DIAERESIS
  0x00D7,  // 0xD7 MULTIPLICATION SIGN
  0x00D8,  // 0xD8 LATIN CAPITAL LETTER O WITH STROKE
  0x00D9,  // 0xD9 LATIN CAPITAL LETTER U WITH GRAVE
  0x00DA,  // 0xDA LATIN CAPITAL LETTER U WITH ACUTE
  0x00DB,  // 0xDB LATIN CAPITAL LETTER U WITH CIRCUMFLEX
  0x00DC,  // 0xDC LATIN CAPITAL LETTER U WITH DIAERESIS
  0x00DD,  // 0xDD LATIN CAPITAL LETTER Y WITH ACUTE
  0x00DE,  // 0xDE LATIN CAPITAL LETTER THORN
  0x00DF,  // 0xDF LATIN SMALL LETTER SHARP S
  0x00E0,  // 0xE0 LATIN SMALL LETTER A WITH GRAVE
  0x00E1,  // 0xE1 LATIN SMALL LETTER A WITH ACUTE
  0x00E2,  // 0xE2 LATIN SMALL LETTER A WITH CIRCUMFLEX
  0x00E3,  // 0xE3 LATIN SMALL LETTER A WITH TILDE
  0x00E4,  // 0xE4 LATIN SMALL LETTER A WITH DIAERESIS
  0x00E5,  // 0xE5 LATIN SMALL LETTER A WITH RING ABOVE
  0x00E6,  // 0xE6 LATIN SMALL LETTER AE
  0x00E7,  // 0xE7 LATIN SMALL LETTER C WITH CEDILLA
  0x00E8,  // 0xE8 LATIN SMALL LETTER E WITH GRAVE
  0x00E9,  // 0xE9 LATIN SMALL LETTER E WITH ACUTE
  0x00EA,  // 0xEA LATIN SMALL LETTER E WITH CIRCUMFLEX
  0x00EB,  // 0xEB LATIN SMALL LETTER E WITH DIAERESIS
  0x00EC,  // 0xEC LATIN SMALL LETTER I WITH GRAVE
  0x00ED,  // 0xED LATIN SMALL LETTER I WITH ACUTE
  0x00EE,  // 0xEE LATIN SMALL LETTER I WITH CIRCUMFLEX
  0x00EF,  // 0xEF LATIN SMALL LETTER I WITH DIAERESIS
  0x00F0,  // 0xF0 LATIN SMALL LETTER ETH
  0x00F1,  // 0xF1 LATIN SMALL LETTER N WITH TILDE
  0x00F2,  // 0xF2 LATIN SMALL LETTER O WITH GRAVE
  0x00F3,  // 0xF3 LATIN SMALL LETTER O WITH ACUTE
  0x00F4,  // 0xF4 LATIN SMALL LETTER O WITH CIRCUMFLEX
  0x00F5,  // 0xF5 LATIN SMALL LETTER O WITH TILDE
  0x00F6,  // 0xF6 LATIN SMALL LETTER O WITH DIAERESIS
  0x00F7,  // 0xF7 DIVISION SIGN
  0x00F8,  // 0xF8 LATIN SMALL LETTER O WITH STROKE
  0x00F9,  // 0xF9 LATIN SMALL LETTER U WITH GRAVE
  0x00FA,  // 0xFA LATIN SMALL LETTER U WITH ACUTE
  0x00FB,  // 0xFB LATIN SMALL LETTER U WITH CIRCUMFLEX
  0x00FC,  // 0xFC LATIN SMALL LETTER U WITH DIAERESIS
  0x00FD,  // 0xFD LATIN SMALL LETTER Y WITH ACUTE
  0x00FE,  // 0xFE LATIN SMALL LETTER THORN
  0x00FF   // 0xFF LATIN SMALL LETTER Y WITH DIAERESIS
};

BOOST_STATIC_ASSERT(sizeof(to_utf16) == 2*256);

const unsigned char to_char[] =
{
  //  slice 0 - characters with no codepage representation
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 00 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 10 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 20 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 30 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 40 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 50 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 60 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 70 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 80 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 90 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // A0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // B0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // C0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // D0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // E0
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // F0

  //  slice 1 - 0x0000
  0x00,  // 0x0000 NULL
  0x01,  // 0x0001 START OF HEADING
  0x02,  // 0x0002 START OF TEXT
  0x03,  // 0x0003 END OF TEXT
  0x04,  // 0x0004 END OF TRANSMISSION
  0x05,  // 0x0005 ENQUIRY
  0x06,  // 0x0006 ACKNOWLEDGE
  0x07,  // 0x0007 BELL
  0x08,  // 0x0008 BACKSPACE
  0x09,  // 0x0009 HORIZONTAL TABULATION
  0x0A,  // 0x000A LINE FEED
  0x0B,  // 0x000B VERTICAL TABULATION
  0x0C,  // 0x000C FORM FEED
  0x0D,  // 0x000D CARRIAGE RETURN
  0x0E,  // 0x000E SHIFT OUT
  0x0F,  // 0x000F SHIFT IN
  0x10,  // 0x0010 DATA LINK ESCAPE
  0x11,  // 0x0011 DEVICE CONTROL ONE
  0x12,  // 0x0012 DEVICE CONTROL TWO
  0x13,  // 0x0013 DEVICE CONTROL THREE
  0x14,  // 0x0014 DEVICE CONTROL FOUR
  0x15,  // 0x0015 NEGATIVE ACKNOWLEDGE
  0x16,  // 0x0016 SYNCHRONOUS IDLE
  0x17,  // 0x0017 END OF TRANSMISSION BLOCK
  0x18,  // 0x0018 CANCEL
  0x19,  // 0x0019 END OF MEDIUM
  0x1A,  // 0x001A SUBSTITUTE
  0x1B,  // 0x001B ESCAPE
  0x1C,  // 0x001C FILE SEPARATOR
  0x1D,  // 0x001D GROUP SEPARATOR
  0x1E,  // 0x001E RECORD SEPARATOR
  0x1F,  // 0x001F UNIT SEPARATOR
  0x20,  // 0x0020 SPACE
  0x21,  // 0x0021 EXCLAMATION MARK
  0x22,  // 0x0022 QUOTATION MARK
  0x23,  // 0x0023 NUMBER SIGN
  0x24,  // 0x0024 DOLLAR SIGN
  0x25,  // 0x0025 PERCENT SIGN
  0x26,  // 0x0026 AMPERSAND
  0x27,  // 0x0027 APOSTROPHE
  0x28,  // 0x0028 LEFT PARENTHESIS
  0x29,  // 0x0029 RIGHT PARENTHESIS
  0x2A,  // 0x002A ASTERISK
  0x2B,  // 0x002B PLUS SIGN
  0x2C,  // 0x002C COMMA
  0x2D,  // 0x002D HYPHEN-MINUS
  0x2E,  // 0x002E FULL STOP
  0x2F,  // 0x002F SOLIDUS
  0x30,  // 0x0030 DIGIT ZERO
  0x31,  // 0x0031 DIGIT ONE
  0x32,  // 0x0032 DIGIT TWO
  0x33,  // 0x0033 DIGIT THREE
  0x34,  // 0x0034 DIGIT FOUR
  0x35,  // 0x0035 DIGIT FIVE
  0x36,  // 0x0036 DIGIT SIX
  0x37,  // 0x0037 DIGIT SEVEN
  0x38,  // 0x0038 DIGIT EIGHT
  0x39,  // 0x0039 DIGIT NINE
  0x3A,  // 0x003A COLON
  0x3B,  // 0x003B SEMICOLON
  0x3C,  // 0x003C LESS-THAN SIGN
  0x3D,  // 0x003D EQUALS SIGN
  0x3E,  // 0x003E GREATER-THAN SIGN
  0x3F,  // 0x003F QUESTION MARK
  0x40,  // 0x0040 COMMERCIAL AT
  0x41,  // 0x0041 LATIN CAPITAL LETTER A
  0x42,  // 0x0042 LATIN CAPITAL LETTER B
  0x43,  // 0x0043 LATIN CAPITAL LETTER C
  0x44,  // 0x0044 LATIN CAPITAL LETTER D
  0x45,  // 0x0045 LATIN CAPITAL LETTER E
  0x46,  // 0x0046 LATIN CAPITAL LETTER F
  0x47,  // 0x0047 LATIN CAPITAL LETTER G
  0x48,  // 0x0048 LATIN CAPITAL LETTER H
  0x49,  // 0x0049 LATIN CAPITAL LETTER I
  0x4A,  // 0x004A LATIN CAPITAL LETTER J
  0x4B,  // 0x004B LATIN CAPITAL LETTER K
  0x4C,  // 0x004C LATIN CAPITAL LETTER L
  0x4D,  // 0x004D LATIN CAPITAL LETTER M
  0x4E,  // 0x004E LATIN CAPITAL LETTER N
  0x4F,  // 0x004F LATIN CAPITAL LETTER O
  0x50,  // 0x0050 LATIN CAPITAL LETTER P
  0x51,  // 0x0051 LATIN CAPITAL LETTER Q
  0x52,  // 0x0052 LATIN CAPITAL LETTER R
  0x53,  // 0x0053 LATIN CAPITAL LETTER S
  0x54,  // 0x0054 LATIN CAPITAL LETTER T
  0x55,  // 0x0055 LATIN CAPITAL LETTER U
  0x56,  // 0x0056 LATIN CAPITAL LETTER V
  0x57,  // 0x0057 LATIN CAPITAL LETTER W
  0x58,  // 0x0058 LATIN CAPITAL LETTER X
  0x59,  // 0x0059 LATIN CAPITAL LETTER Y
  0x5A,  // 0x005A LATIN CAPITAL LETTER Z
  0x5B,  // 0x005B LEFT SQUARE BRACKET
  0x5C,  // 0x005C REVERSE SOLIDUS
  0x5D,  // 0x005D RIGHT SQUARE BRACKET
  0x5E,  // 0x005E CIRCUMFLEX ACCENT
  0x5F,  // 0x005F LOW LINE
  0x60,  // 0x0060 GRAVE ACCENT
  0x61,  // 0x0061 LATIN SMALL LETTER A
  0x62,  // 0x0062 LATIN SMALL LETTER B
  0x63,  // 0x0063 LATIN SMALL LETTER C
  0x64,  // 0x0064 LATIN SMALL LETTER D
  0x65,  // 0x0065 LATIN SMALL LETTER E
  0x66,  // 0x0066 LATIN SMALL LETTER F
  0x67,  // 0x0067 LATIN SMALL LETTER G
  0x68,  // 0x0068 LATIN SMALL LETTER H
  0x69,  // 0x0069 LATIN SMALL LETTER I
  0x6A,  // 0x006A LATIN SMALL LETTER J
  0x6B,  // 0x006B LATIN SMALL LETTER K
  0x6C,  // 0x006C LATIN SMALL LETTER L
  0x6D,  // 0x006D LATIN SMALL LETTER M
  0x6E,  // 0x006E LATIN SMALL LETTER N
  0x6F,  // 0x006F LATIN SMALL LETTER O
  0x70,  // 0x0070 LATIN SMALL LETTER P
  0x71,  // 0x0071 LATIN SMALL LETTER Q
  0x72,  // 0x0072 LATIN SMALL LETTER R
  0x73,  // 0x0073 LATIN SMALL LETTER S
  0x74,  // 0x0074 LATIN SMALL LETTER T
  0x75,  // 0x0075 LATIN SMALL LETTER U
  0x76,  // 0x0076 LATIN SMALL LETTER V
  0x77,  // 0x0077 LATIN SMALL LETTER W
  0x78,  // 0x0078 LATIN SMALL LETTER X
  0x79,  // 0x0079 LATIN SMALL LETTER Y
  0x7A,  // 0x007A LATIN SMALL LETTER Z
  0x7B,  // 0x007B LEFT CURLY BRACKET
  0x7C,  // 0x007C VERTICAL LINE
  0x7D,  // 0x007D RIGHT CURLY BRACKET
  0x7E,  // 0x007E TILDE
  0x7F,  // 0x007F DELETE
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 80 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 90 
  0xA0,  // 0x00A0 NO-BREAK SPACE
  0xA1,  // 0x00A1 INVERTED EXCLAMATION MARK
  0xA2,  // 0x00A2 CENT SIGN
  0xA3,  // 0x00A3 POUND SIGN
  0xA4,  // 0x00A4 CURRENCY SIGN
  0xA5,  // 0x00A5 YEN SIGN
  0xA6,  // 0x00A6 BROKEN BAR
  0xA7,  // 0x00A7 SECTION SIGN
  0xA8,  // 0x00A8 DIAERESIS
  0xA9,  // 0x00A9 COPYRIGHT SIGN
  0xAA,  // 0x00AA FEMININE ORDINAL INDICATOR
  0xAB,  // 0x00AB LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
  0xAC,  // 0x00AC NOT SIGN
  0xAD,  // 0x00AD SOFT HYPHEN
  0xAE,  // 0x00AE REGISTERED SIGN
  0xAF,  // 0x00AF MACRON
  0xB0,  // 0x00B0 DEGREE SIGN
  0xB1,  // 0x00B1 PLUS-MINUS SIGN
  0xB2,  // 0x00B2 SUPERSCRIPT TWO
  0xB3,  // 0x00B3 SUPERSCRIPT THREE
  0xB4,  // 0x00B4 ACUTE ACCENT
  0xB5,  // 0x00B5 MICRO SIGN
  0xB6,  // 0x00B6 PILCROW SIGN
  0xB7,  // 0x00B7 MIDDLE DOT
  0xB8,  // 0x00B8 CEDILLA
  0xB9,  // 0x00B9 SUPERSCRIPT ONE
  0xBA,  // 0x00BA MASCULINE ORDINAL INDICATOR
  0xBB,  // 0x00BB RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
  0xBC,  // 0x00BC VULGAR FRACTION ONE QUARTER
  0xBD,  // 0x00BD VULGAR FRACTION ONE HALF
  0xBE,  // 0x00BE VULGAR FRACTION THREE QUARTERS
  0xBF,  // 0x00BF INVERTED QUESTION MARK
  0xC0,  // 0x00C0 LATIN CAPITAL LETTER A WITH GRAVE
  0xC1,  // 0x00C1 LATIN CAPITAL LETTER A WITH ACUTE
  0xC2,  // 0x00C2 LATIN CAPITAL LETTER A WITH CIRCUMFLEX
  0xC3,  // 0x00C3 LATIN CAPITAL LETTER A WITH TILDE
  0xC4,  // 0x00C4 LATIN CAPITAL LETTER A WITH DIAERESIS
  0xC5,  // 0x00C5 LATIN CAPITAL LETTER A WITH RING ABOVE
  0xC6,  // 0x00C6 LATIN CAPITAL LETTER AE
  0xC7,  // 0x00C7 LATIN CAPITAL LETTER C WITH CEDILLA
  0xC8,  // 0x00C8 LATIN CAPITAL LETTER E WITH GRAVE
  0xC9,  // 0x00C9 LATIN CAPITAL LETTER E WITH ACUTE
  0xCA,  // 0x00CA LATIN CAPITAL LETTER E WITH CIRCUMFLEX
  0xCB,  // 0x00CB LATIN CAPITAL LETTER E WITH DIAERESIS
  0xCC,  // 0x00CC LATIN CAPITAL LETTER I WITH GRAVE
  0xCD,  // 0x00CD LATIN CAPITAL LETTER I WITH ACUTE
  0xCE,  // 0x00CE LATIN CAPITAL LETTER I WITH CIRCUMFLEX
  0xCF,  // 0x00CF LATIN CAPITAL LETTER I WITH DIAERESIS
  0xD0,  // 0x00D0 LATIN CAPITAL LETTER ETH
  0xD1,  // 0x00D1 LATIN CAPITAL LETTER N WITH TILDE
  0xD2,  // 0x00D2 LATIN CAPITAL LETTER O WITH GRAVE
  0xD3,  // 0x00D3 LATIN CAPITAL LETTER O WITH ACUTE
  0xD4,  // 0x00D4 LATIN CAPITAL LETTER O WITH CIRCUMFLEX
  0xD5,  // 0x00D5 LATIN CAPITAL LETTER O WITH TILDE
  0xD6,  // 0x00D6 LATIN CAPITAL LETTER O WITH DIAERESIS
  0xD7,  // 0x00D7 MULTIPLICATION SIGN
  0xD8,  // 0x00D8 LATIN CAPITAL LETTER O WITH STROKE
  0xD9,  // 0x00D9 LATIN CAPITAL LETTER U WITH GRAVE
  0xDA,  // 0x00DA LATIN CAPITAL LETTER U WITH ACUTE
  0xDB,  // 0x00DB LATIN CAPITAL LETTER U WITH CIRCUMFLEX
  0xDC,  // 0x00DC LATIN CAPITAL LETTER U WITH DIAERESIS
  0xDD,  // 0x00DD LATIN CAPITAL LETTER Y WITH ACUTE
  0xDE,  // 0x00DE LATIN CAPITAL LETTER THORN
  0xDF,  // 0x00DF LATIN SMALL LETTER SHARP S
  0xE0,  // 0x00E0 LATIN SMALL LETTER A WITH GRAVE
  0xE1,  // 0x00E1 LATIN SMALL LETTER A WITH ACUTE
  0xE2,  // 0x00E2 LATIN SMALL LETTER A WITH CIRCUMFLEX
  0xE3,  // 0x00E3 LATIN SMALL LETTER A WITH TILDE
  0xE4,  // 0x00E4 LATIN SMALL LETTER A WITH DIAERESIS
  0xE5,  // 0x00E5 LATIN SMALL LETTER A WITH RING ABOVE
  0xE6,  // 0x00E6 LATIN SMALL LETTER AE
  0xE7,  // 0x00E7 LATIN SMALL LETTER C WITH CEDILLA
  0xE8,  // 0x00E8 LATIN SMALL LETTER E WITH GRAVE
  0xE9,  // 0x00E9 LATIN SMALL LETTER E WITH ACUTE
  0xEA,  // 0x00EA LATIN SMALL LETTER E WITH CIRCUMFLEX
  0xEB,  // 0x00EB LATIN SMALL LETTER E WITH DIAERESIS
  0xEC,  // 0x00EC LATIN SMALL LETTER I WITH GRAVE
  0xED,  // 0x00ED LATIN SMALL LETTER I WITH ACUTE
  0xEE,  // 0x00EE LATIN SMALL LETTER I WITH CIRCUMFLEX
  0xEF,  // 0x00EF LATIN SMALL LETTER I WITH DIAERESIS
  0xF0,  // 0x00F0 LATIN SMALL LETTER ETH
  0xF1,  // 0x00F1 LATIN SMALL LETTER N WITH TILDE
  0xF2,  // 0x00F2 LATIN SMALL LETTER O WITH GRAVE
  0xF3,  // 0x00F3 LATIN SMALL LETTER O WITH ACUTE
  0xF4,  // 0x00F4 LATIN SMALL LETTER O WITH CIRCUMFLEX
  0xF5,  // 0x00F5 LATIN SMALL LETTER O WITH TILDE
  0xF6,  // 0x00F6 LATIN SMALL LETTER O WITH DIAERESIS
  0xF7,  // 0x00F7 DIVISION SIGN
  0xF8,  // 0x00F8 LATIN SMALL LETTER O WITH STROKE
  0xF9,  // 0x00F9 LATIN SMALL LETTER U WITH GRAVE
  0xFA,  // 0x00FA LATIN SMALL LETTER U WITH ACUTE
  0xFB,  // 0x00FB LATIN SMALL LETTER U WITH CIRCUMFLEX
  0xFC,  // 0x00FC LATIN SMALL LETTER U WITH DIAERESIS
  0xFD,  // 0x00FD LATIN SMALL LETTER Y WITH ACUTE
  0xFE,  // 0x00FE LATIN SMALL LETTER THORN
  0xFF,  // 0x00FF LATIN SMALL LETTER Y WITH DIAERESIS
  //  slice 2 - 0x0100
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 00 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 10 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 20 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 30 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 40 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 50 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 60 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 70 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 80 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 90 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // A0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // B0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // C0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // D0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // E0
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // F0
  //  slice 3 - 0x0200
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 00 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 10 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 20 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 30 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 40 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 50 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 60 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 70 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 80 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 90 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // A0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // B0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // C0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // D0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // E0
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // F0
  //  slice 4 - 0x2000
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 00 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 10 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 20 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 30 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 40 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 50 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 60 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 70 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 80 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 90 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // A0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // B0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // C0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // D0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // E0
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // F0
  //  slice 5 - 0x2100
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 00 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 10 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 20 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 30 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 40 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 50 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 60 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 70 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 80 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // 90 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // A0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // B0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // C0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // D0 
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?',  // E0
  '?','?','?','?','?','?','?','?','?','?','?','?','?','?','?','?'   // F0
};

BOOST_STATIC_ASSERT(sizeof(to_char) == 6*256);

const boost::uint8_t to_slice[] =
{
  1,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 00
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 10
  5,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 20
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 30
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 40
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 50
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 60
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 70
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 80
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 90
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // A0
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // B0
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // C0
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // D0
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // E0
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // F0
};

BOOST_STATIC_ASSERT(sizeof(to_slice) == 256);

}  // namesapce detail

}  // namespace interop

}  // namespace boost
