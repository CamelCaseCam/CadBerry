/* Generated By:JJTree&JavaCC: Do not edit this line. GuiTestScriptParserConstants.java */
 package ur_rna.GUITester.ScriptParser;


/**
 * Token literal values and constants.
 * Generated by org.javacc.parser.OtherFilesGen#start()
 */
public interface GuiTestScriptParserConstants {

  /** End of File. */
  int EOF = 0;
  /** RegularExpression Id. */
  int NEWLINE = 3;
  /** RegularExpression Id. */
  int SINGLE_LINE_COMMENT = 5;
  /** RegularExpression Id. */
  int MULTI_LINE_COMMENT = 6;
  /** RegularExpression Id. */
  int TRUE = 7;
  /** RegularExpression Id. */
  int FALSE = 8;
  /** RegularExpression Id. */
  int NULL = 9;
  /** RegularExpression Id. */
  int INTEGER_LITERAL = 10;
  /** RegularExpression Id. */
  int DECIMAL_LITERAL = 11;
  /** RegularExpression Id. */
  int HEX_LITERAL = 12;
  /** RegularExpression Id. */
  int DIGIT = 13;
  /** RegularExpression Id. */
  int HEX_DIGIT = 14;
  /** RegularExpression Id. */
  int OCTAL_LITERAL = 15;
  /** RegularExpression Id. */
  int INT_SUFFIX = 16;
  /** RegularExpression Id. */
  int FLOAT_LITERAL = 17;
  /** RegularExpression Id. */
  int EXPONENT = 18;
  /** RegularExpression Id. */
  int FLOAT_SUFFIX = 19;
  /** RegularExpression Id. */
  int STRING_LITERAL = 20;
  /** RegularExpression Id. */
  int ESC_CHAR = 21;
  /** RegularExpression Id. */
  int STRING_CHAR = 22;
  /** RegularExpression Id. */
  int ESCAPED_CHAR = 23;
  /** RegularExpression Id. */
  int HEX_CHAR = 24;
  /** RegularExpression Id. */
  int UNICODE_CHAR = 25;
  /** RegularExpression Id. */
  int DIRECTIVE_PREFIX = 26;
  /** RegularExpression Id. */
  int METHOD_PREFIX = 27;
  /** RegularExpression Id. */
  int IDENTIFIER = 28;
  /** RegularExpression Id. */
  int WORD_START_CHAR = 29;
  /** RegularExpression Id. */
  int WORD_CHAR = 30;
  /** RegularExpression Id. */
  int LETTER = 31;
  /** RegularExpression Id. */
  int LPAREN = 32;
  /** RegularExpression Id. */
  int RPAREN = 33;
  /** RegularExpression Id. */
  int LBRACE = 34;
  /** RegularExpression Id. */
  int RBRACE = 35;
  /** RegularExpression Id. */
  int LBRACKET = 36;
  /** RegularExpression Id. */
  int RBRACKET = 37;
  /** RegularExpression Id. */
  int SEMICOLON = 38;
  /** RegularExpression Id. */
  int COMMA = 39;
  /** RegularExpression Id. */
  int DOT = 40;
  /** RegularExpression Id. */
  int ARROW = 41;
  /** RegularExpression Id. */
  int ASSIGN = 42;
  /** RegularExpression Id. */
  int ASSIGN_REF = 43;
  /** RegularExpression Id. */
  int GT = 44;
  /** RegularExpression Id. */
  int LT = 45;
  /** RegularExpression Id. */
  int NOT = 46;
  /** RegularExpression Id. */
  int BIT_NOT = 47;
  /** RegularExpression Id. */
  int HOOK = 48;
  /** RegularExpression Id. */
  int COLON = 49;
  /** RegularExpression Id. */
  int EQ = 50;
  /** RegularExpression Id. */
  int LE = 51;
  /** RegularExpression Id. */
  int GE = 52;
  /** RegularExpression Id. */
  int NE = 53;
  /** RegularExpression Id. */
  int SC_OR = 54;
  /** RegularExpression Id. */
  int SC_AND = 55;
  /** RegularExpression Id. */
  int INCR = 56;
  /** RegularExpression Id. */
  int DECR = 57;
  /** RegularExpression Id. */
  int PLUS = 58;
  /** RegularExpression Id. */
  int MINUS = 59;
  /** RegularExpression Id. */
  int POW = 60;
  /** RegularExpression Id. */
  int STAR = 61;
  /** RegularExpression Id. */
  int SLASH = 62;
  /** RegularExpression Id. */
  int BIT_AND = 63;
  /** RegularExpression Id. */
  int BIT_OR = 64;
  /** RegularExpression Id. */
  int XOR = 65;
  /** RegularExpression Id. */
  int MOD = 66;
  /** RegularExpression Id. */
  int PARENT = 67;
  /** RegularExpression Id. */
  int ANCESTOR = 68;
  /** RegularExpression Id. */
  int CHILD = 69;
  /** RegularExpression Id. */
  int SIBLING = 70;
  /** RegularExpression Id. */
  int REL_LABEL = 71;
  /** RegularExpression Id. */
  int DESCENDANT = 72;
  /** RegularExpression Id. */
  int PLUSASSIGN = 73;
  /** RegularExpression Id. */
  int MINUSASSIGN = 74;
  /** RegularExpression Id. */
  int STARASSIGN = 75;
  /** RegularExpression Id. */
  int SLASHASSIGN = 76;
  /** RegularExpression Id. */
  int ANDASSIGN = 77;
  /** RegularExpression Id. */
  int ORASSIGN = 78;
  /** RegularExpression Id. */
  int XORASSIGN = 79;
  /** RegularExpression Id. */
  int REMASSIGN = 80;
  /** RegularExpression Id. */
  int LSHIFTASSIGN = 81;
  /** RegularExpression Id. */
  int RSIGNEDSHIFTASSIGN = 82;
  /** RegularExpression Id. */
  int RUNSIGNEDSHIFTASSIGN = 83;

  /** Lexical state. */
  int DEFAULT = 0;

  /** Literal token values. */
  String[] tokenImage = {
    "<EOF>",
    "\" \"",
    "\"\\t\"",
    "<NEWLINE>",
    "<token of kind 4>",
    "<SINGLE_LINE_COMMENT>",
    "<MULTI_LINE_COMMENT>",
    "\"true\"",
    "\"false\"",
    "\"null\"",
    "<INTEGER_LITERAL>",
    "<DECIMAL_LITERAL>",
    "<HEX_LITERAL>",
    "<DIGIT>",
    "<HEX_DIGIT>",
    "<OCTAL_LITERAL>",
    "<INT_SUFFIX>",
    "<FLOAT_LITERAL>",
    "<EXPONENT>",
    "<FLOAT_SUFFIX>",
    "<STRING_LITERAL>",
    "\"\\\\\"",
    "<STRING_CHAR>",
    "<ESCAPED_CHAR>",
    "<HEX_CHAR>",
    "<UNICODE_CHAR>",
    "\"#\"",
    "\"@\"",
    "<IDENTIFIER>",
    "<WORD_START_CHAR>",
    "<WORD_CHAR>",
    "<LETTER>",
    "\"(\"",
    "\")\"",
    "\"{\"",
    "\"}\"",
    "\"[\"",
    "\"]\"",
    "\";\"",
    "\",\"",
    "\".\"",
    "\"->\"",
    "\"=\"",
    "\":=\"",
    "\">\"",
    "\"<\"",
    "\"!\"",
    "\"~\"",
    "\"?\"",
    "\":\"",
    "\"==\"",
    "\"<=\"",
    "\">=\"",
    "\"!=\"",
    "\"||\"",
    "\"&&\"",
    "\"++\"",
    "\"--\"",
    "\"+\"",
    "\"-\"",
    "\"**\"",
    "\"*\"",
    "\"/\"",
    "\"&\"",
    "\"|\"",
    "\"^\"",
    "\"%\"",
    "\"<<\"",
    "\"<<<\"",
    "\">>\"",
    "\">|\"",
    "\"~>\"",
    "\">>>\"",
    "\"+=\"",
    "\"-=\"",
    "\"*=\"",
    "\"/=\"",
    "\"&=\"",
    "\"|=\"",
    "\"^=\"",
    "\"%=\"",
    "\"<<=\"",
    "\">>=\"",
    "\">>>=\"",
  };

}
