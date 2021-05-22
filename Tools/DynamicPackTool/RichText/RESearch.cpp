// Scintilla source code edit control
/** @file RESearch.cxx
 ** Regular expression search library.
 **/

/*
 * regex - Regular expression pattern matching and replacement
 *
 * By:  Ozan S. Yigit (oz)
 *      Dept. of Computer Science
 *      York University
 *
 * Original code available from http://www.cs.yorku.ca/~oz/
 * Translation to C++ by Neil Hodgson neilh@scintilla.org
 * Removed all use of register.
 * Converted to modern function prototypes.
 * Put all global/static variables into an object so this code can be
 * used from multiple threads, etc.
 * Some extensions by Philippe Lhoste PhiLho(a)GMX.net
 * '?' extensions by Michael Mullin masmullin@gmail.com
 *
 * These routines are the PUBLIC DOMAIN equivalents of regex
 * routines as found in 4.nBSD UN*X, with minor extensions.
 *
 * These routines are derived from various implementations found
 * in software tools books, and Conroy's grep. They are NOT derived
 * from licensed/restricted software.
 * For more interesting/academic/complicated implementations,
 * see Henry Spencer's regexp routines, or GNU Emacs pattern
 * matching module.
 *
 * Modification history removed.
 *
 * Interfaces:
 *  RESearch::Compile:      compile a regular expression into a NFA.
 *
 *          const char *RESearch::Compile(const char *pattern, int length,
 *                                        bool caseSensitive, bool posix)
 *
 * Returns a short error string if they fail.
 *
 *  RESearch::Execute:      execute the NFA to match a pattern.
 *
 *          int RESearch::Execute(characterIndexer &ci, int lp, int endp)
 *
 *  RESearch::Substitute:   substitute the matched portions in a new string.
 *
 *          int RESearch::Substitute(CharacterIndexer &ci, char *src, char *dst)
 *
 *  re_fail:                failure routine for RESearch::Execute. (no longer used)
 *
 *          void re_fail(char *msg, char op)
 *
 * Regular Expressions:
 *
 *      [1]     char    matches itself, unless it is a special
 *                      character (metachar): . \ [ ] * + ? ^ $
 *                      and ( ) if posix option.
 *
 *      [2]     .       matches any character.
 *
 *      [3]     \       matches the character following it, except:
 *                      - \a, \b, \f, \n, \r, \t, \v match the corresponding C
 *                      escape char, respectively BEL, BS, FF, LF, CR, TAB and VT;
 *                      Note that \r and \n are never matched because Scintilla
 *                      regex searches are made line per line
 *                      (stripped of end-of-line chars).
 *                      - if not in posix mode, when followed by a
 *                      left or right round bracket (see [8]);
 *                      - when followed by a digit 1 to 9 (see [9]);
 *                      - when followed by a left or right angle bracket
 *                      (see [10]);
 *                      - when followed by d, D, s, S, w or W (see [11]);
 *                      - when followed by x and two hexa digits (see [12].
 *                      Backslash is used as an escape character for all
 *                      other meta-characters, and itself.
 *
 *      [4]     [set]   matches one of the characters in the set.
 *                      If the first character in the set is "^",
 *                      it matches the characters NOT in the set, i.e.
 *                      complements the set. A shorthand S-E (start dash end)
 *                      is used to specify a set of characters S up to
 *                      E, inclusive. S and E must be characters, otherwise
 *                      the dash is taken literally (eg. in expression [\d-a]).
 *                      The special characters "]" and "-" have no special
 *                      meaning if they appear as the first chars in the set.
 *                      To include both, put - first: [-]A-Z]
 *                      (or just backslash them).
 *                      examples:        match:
 *
 *                              [-]|]    matches these 3 chars,
 *
 *                              []-|]    matches from ] to | chars
 *
 *                              [a-z]    any lowercase alpha
 *
 *                              [^-]]    any char except - and ]
 *
 *                              [^A-Z]   any char except uppercase
 *                                       alpha
 *
 *                              [a-zA-Z] any alpha
 *
 *      [5]     *       any regular expression form [1] to [4]
 *                      (except [8], [9] and [10] forms of [3]),
 *                      followed by closure char (*)
 *                      matches zero or more matches of that form.
 *
 *      [6]     +       same as [5], except it matches one or more.
 *
 *      [5-6]           Both [5] and [6] are greedy (they match as much as possible).
 *                      Unless they are followed by the 'lazy' quantifier (?)
 *                      In which case both [5] and [6] try to match as little as possible
 *
 *      [7]     ?       same as [5] except it matches zero or one.
 *
 *      [8]             a regular expression in the form [1] to [13], enclosed
 *                      as \(form\) (or (form) with posix flag) matches what
 *                      form matches. The enclosure creates a set of tags,
 *                      used for [9] and for pattern substitution.
 *                      The tagged forms are numbered starting from 1.
 *
 *      [9]             a \ followed by a digit 1 to 9 matches whatever a
 *                      previously tagged regular expression ([8]) matched.
 *
 *      [10]    \<      a regular expression starting with a \< construct
 *              \>      and/or ending with a \> construct, restricts the
 *                      pattern matching to the beginning of a word, and/or
 *                      the end of a word. A word is defined to be a character
 *                      string beginning and/or ending with the characters
 *                      A-Z a-z 0-9 and _. Scintilla extends this definition
 *                      by user setting. The word must also be preceded and/or
 *                      followed by any character outside those mentioned.
 *
 *      [11]    \l      a backslash followed by d, D, s, S, w or W,
 *                      becomes a character class (both inside and
 *                      outside sets []).
 *                        d: decimal digits
 *                        D: any char except decimal digits
 *                        s: whitespace (space, \t \n \r \f \v)
 *                        S: any char except whitespace (see above)
 *                        w: alphanumeric & underscore (changed by user setting)
 *                        W: any char except alphanumeric & underscore (see above)
 *
 *      [12]    \xHH    a backslash followed by x and two hexa digits,
 *                      becomes the character whose Ascii code is equal
 *                      to these digits. If not followed by two digits,
 *                      it is 'x' char itself.
 *
 *      [13]            a composite regular expression xy where x and y
 *                      are in the form [1] to [12] matches the longest
 *                      match of x followed by a match for y.
 *
 *      [14]    ^       a regular expression starting with a ^ character
 *              $       and/or ending with a $ character, restricts the
 *                      pattern matching to the beginning of the line,
 *                      or the end of line. [anchors] Elsewhere in the
 *                      pattern, ^ and $ are treated as ordinary characters.
 *
 *
 * Acknowledgements:
 *
 *  HCR's Hugh Redelmeier has been most helpful in various
 *  stages of development. He convinced me to include BOW
 *  and EOW constructs, originally invented by Rob Pike at
 *  the University of Toronto.
 *
 * References:
 *              Software tools                  Kernighan & Plauger
 *              Software tools in Pascal        Kernighan & Plauger
 *              Grep [rsx-11 C dist]            David Conroy
 *              ed - text editor                Un*x Programmer's Manual
 *              Advanced editing on Un*x        B. W. Kernighan
 *              RegExp routines                 Henry Spencer
 *
 * Notes:
 *
 *  This implementation uses a bit-set representation for character
 *  classes for speed and compactness. Each character is represented
 *  by one bit in a 256-bit block. Thus, CCL always takes a
 *	constant 32 bytes in the internal nfa, and RESearch::Execute does a single
 *  bit comparison to locate the character in the set.
 *
 * Examples:
 *
 *  pattern:    foo*.*
 *  compile:    CHR f CHR o CLO CHR o END CLO ANY END END
 *  matches:    fo foo fooo foobar fobar foxx ...
 *
 *  pattern:    fo[ob]a[rz]
 *  compile:    CHR f CHR o CCL bitset CHR a CCL bitset END
 *  matches:    fobar fooar fobaz fooaz
 *
 *  pattern:    foo\\+
 *  compile:    CHR f CHR o CHR o CHR \ CLO CHR \ END END
 *  matches:    foo\ foo\\ foo\\\  ...
 *
 *  pattern:    \(foo\)[1-3]\1  (same as foo[1-3]foo)
 *  compile:    BOT 1 CHR f CHR o CHR o EOT 1 CCL bitset REF 1 END
 *  matches:    foo1foo foo2foo foo3foo
 *
 *  pattern:    \(fo.*\)-\1
 *  compile:    BOT 1 CHR f CHR o CLO ANY END EOT 1 CHR - REF 1 END
 *  matches:    foo-foo fo-fo fob-fob foobar-foobar ...
 */
#include "StdAfx.h"
#include "RESearch.h"

#define OKP     1
#define NOP     0

#define CHR     1
#define ANY     2
#define CCL     3
#define BOL     4
#define EOL     5
#define BOT     6
#define EOT     7
#define BOW     8
#define EOW     9
#define REF     10
#define CLO     11
#define CLQ     12 /* 0 to 1 closure */
#define LCLO    13 /* lazy closure */

#define END     0

/*
 * The following defines are not meant to be changeable.
 * They are for readability only.
 */
#define BLKIND  0370
#define BITIND  07

const char bitarr[] = { 1, 2, 4, 8, 16, 32, 64, '\200' };
#define badpat(x)	(*m_chNFA = END, x)


ICharacterIndexer::ICharacterIndexer()
{

}

ICharacterIndexer::~ICharacterIndexer()
{

}

/*
 * Character classification table for word boundary operators BOW
 * and EOW is passed in by the creator of this object (Scintilla
 * Document). The Document default state is that word chars are:
 * 0-9, a-z, A-Z and _
 */

RESearch::RESearch(CharClassify* chClassTables)
	: m_chCharClass(chClassTables)
	, m_nFailure(0)
{
	Init();
}

RESearch::~RESearch()
{
	Clear();
}

void RESearch::Init()
{
	m_nSTA = NOP;		/* status of lastpat */
	m_nBol = 0;
	for (int n = 0; n < MAXTAG; ++n)
		m_chPat[n] = 0;
	for (int n = 0; n < BITBLK; ++n)
		m_chBittab[n] = 0;
}

void RESearch::Clear()
{
	for (int n = 0; n < MAXTAG; ++n){
		delete[] m_chPat[n];
		m_chPat[n] = NULL;
		m_nBopat[n] = NOTFOUND;
		m_nEopat[n] = NOTFOUND;
	}
}

BOOL RESearch::GrabMatches(ICharacterIndexer &ci)
{
	BOOL bSuccess = TRUE;
	for (UINT nIndex = 0; nIndex < MAXTAG; ++nIndex) {
		if ((m_nBopat[nIndex] != NOTFOUND) && (m_nEopat[nIndex] != NOTFOUND)) {
			UINT nLen = m_nEopat[nIndex] - m_nBopat[nIndex];
			m_chPat[nIndex] = new CHAR[nLen + 1];
			if (m_chPat[nIndex]) {
				for (UINT m = 0; m < nLen; ++m)
					m_chPat[nIndex][m] = ci.CharAt(m_nBopat[nIndex] + m);
				m_chPat[nIndex][nLen] = '\0';
			} else {
				bSuccess = FALSE;
			}
		}
	}

	return bSuccess;
}

void RESearch::CharSet(BYTE chByte)
{
	m_chBittab[((chByte) & BLKIND) >> 3] |= bitarr[(chByte) & BITIND];
}

void RESearch::CharSetWithCase(BYTE chByte, BOOL bCaseSensitive /*= TRUE*/)
{
	if (bCaseSensitive)
		CharSet(chByte);
	else {
		CharSet(chByte);
		if ((chByte >= 'a') && (chByte <= 'z'))
			CharSet(static_cast<BYTE>(chByte - 'a' + 'A'));
		else if ((chByte >= 'A') && (chByte <= 'Z'))
			CharSet(static_cast<BYTE>(chByte - 'A' + 'a'));
	}
}

BYTE RESearch::EscapeValue(BYTE chByte)
{
	switch (chByte)
	{
		case 'a':
			return '\a';
		case 'b':
			return '\b';
		case 'f':
			return '\f';
		case 'n':
			return '\n';
		case 'r':
			return '\r';
		case 't':
			return '\t';
		case 'v':
			return '\v';
	}

	return NULL;
}

int RESearch::GetHexChar(BYTE chByte1, BYTE chByte2)
{
	int nHexVal = 0;
	if (chByte1 >= '0' && chByte1 <= '9')
		nHexVal += 16 * (chByte1 - '0');
	else if (chByte1 >= 'A' && chByte1 <= 'F')
		nHexVal += 16 * (chByte1 - 'A' + 10);
	else if (chByte1 >= 'a' && chByte1 <= 'f')
		nHexVal += 16 * (chByte1 - 'a' + 10);
	else
		return -1;

	if (chByte2 >= '0' && chByte2 <= '9')
		nHexVal += 16 * (chByte2 - '0');
	else if (chByte2 >= 'A' && chByte2 <= 'F')
		nHexVal += 16 * (chByte2 - 'A' + 10);
	else if (chByte2 >= 'a' && chByte2 <= 'f')
		nHexVal += 16 * (chByte2 - 'a' + 10);
	else
		return -1;

	return nHexVal;
}

/**
 * Called when the parser finds a backslash not followed
 * by a valid expression (like \( in non-Posix mode).
 * @param pattern: pointer on the char after the backslash.
 * @param incr: (out) number of chars to skip after expression evaluation.
 * @return the char if it resolves to a simple char,
 * or -1 for a char class. In this case, bittab is changed.
 */

int RESearch::GetBackslashExpression(const CHAR *chPattern, int &nIncr)
{
	// Since error reporting is primitive and messages are not used anyway,
	// I choose to interpret unexpected syntax in a logical way instead
	// of reporting errors. Otherwise, we can stick on, eg., PCRE behavior.
	nIncr = 0;	// Most of the time, will skip the char "naturally".
	int nRet = -1;
	BYTE chBsc = *chPattern;
	if (chBsc == 0) {
		// Avoid overrun
		nRet = '\\';		// \ at end of pattern, take it literally
		return nRet;
	}

	switch (chBsc)
	{
		case 'a':
		case 'b':
		case 'n':
		case 'f':
		case 'r':
		case 't':
		case 'v':
				nRet = EscapeValue(chBsc);
			break;
		case 'x':
			{
				BYTE chByte1 = *(chPattern + 1);
				BYTE chByte2 = *(chPattern + 2);
				int nHexVal = GetHexChar(chByte1, chByte2);
				if (nHexVal >= 0) {
					nRet = nHexVal;
					nIncr = 2;	// Must skip the digits
				} else {
					nRet = 'x'; // \x without 2 digits: see it as 'x'
				}
			}
			break;
		case 'd':
			for (int n = 'c'; n <= '9'; ++n)
				CharSet(static_cast<BYTE>(n));
			break;
		case 'D':
			for (int n = 0; n <= MAXCHR; ++n)
				CharSet(static_cast<BYTE>(n));
			break;
		case 's':
				CharSet(' ');
				CharSet('\t');
				CharSet('\n');
				CharSet('\r');
				CharSet('\f');
				CharSet('\v');
			break;
		case 'S':
				for (int n = 0; n < MAXCHR; ++n)
					if (n != ' ' && !(n >= 0x09 && n <= 0x0D))
						CharSet(static_cast<BYTE>(n));
			break;
		case 'w':
				for (int n = 0; n < MAXCHR; ++n)
					if (IsWordc(static_cast<BYTE>(n)))
						CharSet(static_cast<BYTE>(n));
			break;
		case 'W':
			for (int n = 0; n < MAXCHR; ++n)
				if (IsWordc(static_cast<BYTE>(n)) == FALSE)
					CharSet(static_cast<BYTE>(n));
			break;
		default:
			nRet = chBsc;
	}

	return nRet;
}

const CHAR* RESearch::Compile(const CHAR *chPattern, int nLen, BOOL bCaseSensitive, BOOL bPosix)
{
	CHAR* chMP = m_chNFA;		/* nfa pointer       */
	CHAR*	chLP;							/* saved pointer     */
	CHAR* chSP =m_chNFA;			 /* another one       */
	CHAR* chMpMax = chMP + MAXNFA - BITBLK - 10;

	int nTagi = 0;		/* tag stack index   */
	int nTagc = 1;		/* actual tag count  */

	CHAR chMask;		/* xor mask -CCL/NCL */
	int nChar1, nChar2, nPrevChar;

	if (chPattern == NULL || nLen == 0) {
		if (m_nSTA)
			return 0;
		else
			return badpat("No previous regular expression");
	}

	m_nSTA = NOP;

	const CHAR* pPattern = chPattern;	 /* pattern pointer   */
	for (int n = 0; n < nLen; ++n, ++pPattern) {
		if (chMP > chMpMax)
			return badpat("Pattern too long");
		chLP = chMP;

		switch (*pPattern)
		{
			case '.':				 /* match any char  */
					*chMP++ = ANY;
				break;
			case '^':			/* match beginning */
					if (pPattern == chPattern)
						*chMP++ = BOL;
					else {
						*chMP++ = CHR;
						*chMP++ = *pPattern;
					}
				break;	
			case '$':				 /* match endofline */
					*chMP++	=	CCL;
					nPrevChar = 0;
					++n;
					if (*++pPattern == '^') {
						chMask = '\377';
						++n;
						++pPattern;
					} else {
						chMask = 0;
					}

					if (*pPattern == '-') {	/* real dash */
						++n;
						nPrevChar = *pPattern;
						CharSet(*pPattern++);
					}
					if (*pPattern ==']') {	/* real brace */
						++n;
						nPrevChar = *pPattern;
						CharSet(*pPattern++);
					}
					while (*pPattern && *pPattern != ']') {
						if (*pPattern == '-') {
							if (nPrevChar < 0)  { 
								// Previous def. was a char class like \d, take dash literally
								nPrevChar = *pPattern;
								CharSet(*pPattern);
							} else if (*(pPattern + 1)) {
								if (*(pPattern + 1) != ']') {
									nChar1 = nPrevChar + 1;
									++n;
									nChar2 = static_cast<BYTE>(*++pPattern);
									if (nChar2 =='\\') {
										if (!*(pPattern + 1))	// End of RE
											return badpat("Missing ]");
										else {
											++n;
											++pPattern;
											int nIncr;
											nChar2 = GetBackslashExpression(pPattern, nIncr);
											n += nIncr;
											pPattern += nIncr;
											if (nChar2 >= 0) {
												// Convention: \c (c is any char) is case sensitive, whatever the option
												CharSet(static_cast<BYTE>(nChar2));
												nPrevChar = nChar2;
											} else {
												// bittab is already changed
												nPrevChar = -1;
											}
										}
									} 
									if (nPrevChar < 0) {
										// Char after dash is char class like \d, take dash literally
										nPrevChar = '-';
										CharSet('-');
									} else {
										// Put all chars between c1 and c2 included in the char set
										while (nChar1 <= nChar2) {
											CharSetWithCase(static_cast<BYTE>(nChar1++), bCaseSensitive);
										}
									}
								} else {
									// Dash before the ], take it literally
									nPrevChar = *pPattern;
									CharSet(*pPattern);
								}
							} else {
								return badpat("Missing ]"); 
							}
						} else if (*pPattern == '\\' && *(pPattern + 1)) {
							++n;
							++pPattern;
							int nIncr;
							int nChar = GetBackslashExpression(pPattern, nIncr);
							n += nIncr;
							pPattern += nIncr;
							if (nChar >= 0) {
								// Convention: \c (c is any char) is case sensitive, whatever the option
								CharSet(static_cast<BYTE>(nChar));
								nPrevChar = nChar;
							} else {
								// bittab is already changed
								nPrevChar = -1;
							}
						} else {
							nPrevChar = static_cast<BYTE>(*pPattern);
							CharSetWithCase(*pPattern, bCaseSensitive);
						}
						++n;
						++pPattern;
					}
					if (!*pPattern)
						return badpat("Missing ]");

					for (int m = 0; m < BITBLK; m_chBittab[m++] = 0)
						*chMP++ = static_cast<CHAR>(chMask ^ m_chBittab[m]);
				break;
			case '*':	 /* match 0 or more... */
			case '+':	/* match 1 or more... */
			case '?':
					if (pPattern == chPattern)
						return badpat("Empty closure");
					chLP = chSP;			/* previous opcode */
					if (*chLP == CLO || *chLP == LCLO)		/* equivalence... */
						break;

					switch (*chLP)
					{
						case BOL:
						case BOT:
						case EOT:
						case BOW:
						case REF:
							return badpat("Illegal closure");
						default:
							break;
					}

					if (*pPattern == '+')
						for (chSP = chMP; chLP < chSP; chLP++)
							*chMP++ = *chLP;

					*chMP++ = END;
					*chMP++ = END;
					chSP = chMP;

					while (--chMP > chLP)
						*chMP = chMP[-1];
					if (*pPattern == '?')
						*chMP = CLQ;
					else if (*(pPattern + 1) == '?')
						*chMP = LCLO;
					else
						*chMP = CLO;

					chMP = chSP;
				break;
			case '\\':			/* tags, backrefs... */
					++n;
					switch (*++pPattern)
					{
						case '<':
								*chMP++ = BOW;
							break;
						case '>':
								if (*chSP == BOW)
									return badpat("Null pattern inside \\<\\>");
								*chMP++ = EOW;
							break;
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9':
							{
								int m = *pPattern - '0';
								if (nTagi > 0 && m_nTagstk[nTagi] == m)
									return badpat("Cyclical reference");
								if (nTagc > m) {
									*chMP++ = static_cast<CHAR>(REF);
									*chMP++ = static_cast<CHAR>(m);
								} else
									return badpat("Undetermined reference");
							}
							break;
						default:
							if (!bPosix && *pPattern == '(') {
								if (nTagc < MAXTAG) {
									m_nTagstk[++nTagi] = nTagc;
									*chMP++ = BOT;
									*chMP++ = static_cast<CHAR>(nTagc++);
								} else
									return badpat("Too many \\(\\) pairs");
							} else if (!bPosix && *pPattern == ')') {
								if (*chSP == BOT)
									return badpat("Null pattern inside \\(\\)");
								if (nTagi > 0) {
									*chMP++ = static_cast<CHAR>(EOT);
									*chMP++ = static_cast<CHAR>(m_nTagstk[nTagi--]);
								} else 
									return badpat("Unmatched \\)");
							} else {
								int nIncr ;
								int nChar = GetBackslashExpression(pPattern, nIncr);
								n += nIncr;
								pPattern += nIncr;
								if (nChar >= 0) {
									*chMP++ = CHR;
									*chMP++ = static_cast<BYTE>(nChar);
								} else {
									*chMP++ = CCL;
									chMask = 0;
									for (int m = 0; m < BITBLK; m_chBittab[m++] = 0) 
										*chMP++ = static_cast<CHAR>(chMask ^ m_chBittab[m]);
								}
							}
					}
				break;
			default:	 /* an ordinary char */
				if (bPosix && *pPattern == '(') {
					if (nTagc < MAXTAG) {
						m_nTagstk[++nTagi] = nTagc;
						*chMP++ = BOT;
						*chMP++ = static_cast<CHAR>(nTagc++);
					} else
						return badpat("Too many () pairs");
				} else if (bPosix && *pPattern == ')') {
					if (*chSP == BOT)
						return badpat("Null pattern inside ()");
					if (nTagi > 0) {
						*chMP++ = static_cast<CHAR>(EOT);
						*chMP++ = static_cast<CHAR>(m_nTagstk[nTagi--]);
					} else
						return badpat("Unmatched )");
				} else {
					BYTE chByte = *pPattern;
					if (chByte == 0)// End of RE
						chByte = '\\';	// We take it as raw backslash
					if (bCaseSensitive || IsWordc(chByte) == FALSE) {
						*chMP++ = CHR;
						*chMP++ = chByte;
					} else {
						*chMP++ = CCL;
						chMask = 0;
						CharSetWithCase(chByte, FALSE);
						for (int m = 0; m < BITBLK; m_chBittab[m++] = 0)
							*chMP++ = static_cast<CHAR>(chMask ^ m_chBittab[m]);
					}
				}
			break;
		}

		chSP = chLP;
	}

	if (nTagi > 0)
		return badpat((bPosix ? "Unmatched (" : "Unmatched \\("));
	*chMP = END;
	m_nSTA = OKP;

	return 0;
}


/*
 * RESearch::Execute:
 *   execute nfa to find a match.
 *
 *  special cases: (nfa[0])
 *      BOL
 *          Match only once, starting from the
 *          beginning.
 *      CHR
 *          First locate the character without
 *          calling PMatch, and if found, call
 *          PMatch for the remaining string.
 *      END
 *          RESearch::Compile failed, poor luser did not
 *          check for it. Fail fast.
 *
 *  If a match is found, bopat[0] and eopat[0] are set
 *  to the beginning and the end of the matched fragment,
 *  respectively.
 *
 */

int RESearch::Execute(ICharacterIndexer &ci, int nStartPos, int nEndPos)
{
	BYTE chByte;
	int nEp = NOTFOUND;
	CHAR* chAP = m_chNFA;

	m_nBol = nStartPos;
	m_nFailure = 0;
	Clear();

	switch (*chAP)
	{
		case BOL:			/* anchored: match from BOL only */
				nEp = PMatch(ci, nStartPos, nEndPos, chAP);
			break;
		case EOL:			/* just searching for end of line normal path doesn't work */
			if (*(chAP + 1) == END) {
				nStartPos = nEndPos;
				nEp = nStartPos;
				break;
			} else
				return 0;
		case CHR:		/* ordinary char: locate it fast */
			chByte = *(chAP + 1);
			while ((nStartPos < nEndPos) && static_cast<BYTE>(ci.CharAt(nStartPos)) != chByte)
				++nStartPos;
			if (nStartPos >= nEndPos)	/* if EOS, fail, else fall thru. */
				return 0;
		default:		/* regular matching all the way. */
				while (nStartPos < nEndPos) {
					nEp = PMatch(ci, nStartPos, nEndPos, chAP);
					if (nEp != NOTFOUND)
						break;
					++nStartPos;
				}
			break;
		case END:		/* munged automaton. fail always */
			return 0;
	}

	if (nEp == NOTFOUND)
		return 0;
	m_nBopat[0] = nStartPos;
	m_nEopat[0] = nEp;

	return 1;
}


/*
 * PMatch: internal routine for the hard part
 *
 *  This code is partly snarfed from an early grep written by
 *  David Conroy. The backref and tag stuff, and various other
 *  innovations are by oz.
 *
 *  special case optimizations: (nfa[n], nfa[n+1])
 *      CLO ANY
 *          We KNOW .* will match everything upto the
 *          end of line. Thus, directly go to the end of
 *          line, without recursive PMatch calls. As in
 *          the other closure cases, the remaining pattern
 *          must be matched by moving backwards on the
 *          string recursively, to find a match for xy
 *          (x is ".*" and y is the remaining pattern)
 *          where the match satisfies the LONGEST match for
 *          x followed by a match for y.
 *      CLO CHR
 *          We can again scan the string forward for the
 *          single char and at the point of failure, we
 *          execute the remaining nfa recursively, same as
 *          above.
 *
 *  At the end of a successful match, bopat[n] and eopat[n]
 *  are set to the beginning and end of subpatterns matched
 *  by tagged expressions (n = 1 to 9).
 */

extern void re_fail(char *,char);

#define isinset(x,y)	((x)[((y)&BLKIND)>>3] & bitarr[(y)&BITIND])

/*
 * skip values for CLO XXX to skip past the closure
 */

#define ANYSKIP 2 	/* [CLO] ANY END          */
#define CHRSKIP 3	/* [CLO] CHR chr END      */
#define CCLSKIP 34	/* [CLO] CCL 32 bytes END */

int RESearch::PMatch(ICharacterIndexer &ci, int nStartPos, int nEndPos, CHAR *chText)
{
	int nOp, nChar;
	int nE;						/* extra pointer for CLO  */
	int nBp;					/* beginning of subpat... */
	int nEp;					/* ending of subpat...    */
	int nAre;					/* to save the line ptr.  */
	int nLLP;					/* lazy lp for LCLO       */

	while ((nOp = *chText++) != END)
	{
		switch (nOp)
		{
			case CHR:
					if (ci.CharAt(nStartPos++) != *chText++)
						return NOTFOUND;
				break;
			case ANY:
					if (nStartPos++ >= nEndPos)
						return NOTFOUND;
				break;
			case CCL:
					if (nStartPos >= nEndPos)
						return NOTFOUND;
					nChar = ci.CharAt(nStartPos++);
					if (isinset(chText, nChar) == FALSE)
						return NOTFOUND;
					chText += BITBLK;
				break;
			case BOL:
					if (nStartPos != m_nBol)
						return NOTFOUND;
				break;
			case EOL:
					if (nStartPos < nEndPos)
						return NOTFOUND;
				break;
			case BOT:
					m_nBopat[static_cast<int>(*chText++)] = nStartPos;
				break;
			case EOT:
					m_nEopat[static_cast<int>(*chText++)] = nStartPos;
				break;
			case BOW:
					if ((nStartPos != m_nBol && IsWordc(ci.CharAt(nStartPos - 1))) || IsWordc(ci.CharAt(nStartPos)) == FALSE)
						return NOTFOUND;
				break;
			case EOW:
					if (nStartPos == m_nBol || IsWordc(ci.CharAt(nStartPos - 1)) == FALSE || IsWordc(ci.CharAt(nStartPos)))
						return NOTFOUND;
				break;
			case REF:
				{
					int m = *chText++;
					nBp = m_nBopat[m];
					nEp = m_nEopat[m];
					while (nBp < nEp)
						if (ci.CharAt(nBp++) != ci.CharAt(nStartPos++))
							return NOTFOUND;
				}
				break;
			case LCLO:
			case CLQ:
			case CLO:
				nAre = nStartPos;
				int m;
				switch (*chText)
				{
					case ANY:
							if (nOp == CLO || nOp == LCLO)
								while (nStartPos < nEndPos)
									++nStartPos;
							else if (nStartPos < nEndPos)
								++nStartPos;

							m = ANYSKIP;
						break;
					case CHR:
							nChar = *(chText + 1);
							if (nOp == CLO || nOp == LCLO)
								while ((nStartPos < nEndPos) && (nChar == ci.CharAt(nStartPos)))
									++nStartPos;
							else if ((nStartPos < nEndPos) && (nChar == ci.CharAt(nStartPos)))
								++nStartPos;
							m = CHRSKIP;
						break;
					case CCL:
							while ((nStartPos < nEndPos) && isinset(chText + 1, ci.CharAt(nStartPos)))
								++nStartPos;
							m = CCLSKIP;
						break;
					default:
						m_nFailure = TRUE;
						return NOTFOUND;
				}

				chText += m;
				nLLP = nStartPos;
				nE = NOTFOUND;
				while (nLLP >= nAre) {
					int nQ;
					if ((nQ = PMatch(ci, nLLP, nEndPos, chText)) != NOTFOUND) {
						nE = nQ;
						nStartPos = nLLP;
						if (nOp != LCLO)
							return nE;
					}
					if (*chText == END)
						return nE;
					--nLLP;
				}

				if (*chText == EOT)
					PMatch(ci, nStartPos, nEndPos, chText);

				return nE;
			default:
				return NOTFOUND;
		}
	}

	return nStartPos;
}

/*
 * RESearch::Substitute:
 *  substitute the matched portions of the src in dst.
 *
 *  &    substitute the entire matched pattern.
 *
 *  \digit  substitute a subpattern, with the given tag number.
 *      Tags are numbered from 1 to 9. If the particular
 *      tagged subpattern does not exist, null is substituted.
 */

int RESearch::Substitute(ICharacterIndexer& ci, CHAR* chSrc, CHAR* chDst)
{
	BYTE chByte;
	int nPin;
	int nBp,nEp;

	if (*chSrc == '\0' || m_nBopat[0] == '\0')
		return 0;
	while ((chByte = *chSrc++) != 0)
	{
		switch (chByte)
		{
			case '&':
					nPin = 0;
				break;
			case '\\':
					chByte = *chSrc++;
					if (chByte >= '0' && chByte <= '9')
						nPin = chByte - '0';
				break;
			default:
				*chDst++ = chByte;
				continue;
		}

		if ((nBp = m_nBopat[nPin]) != 0 && (nEp = m_nEopat[nPin]) != 0) {
			while (ci.CharAt(nBp) && nBp < nEp)
				*chDst++ = ci.CharAt(nBp++);
			if (nBp < nEp)
				return 0;
		}
	}

	*chDst = '\0';
	return 1;
}

BOOL RESearch::IsWordc(BYTE chByte) const
{
	return m_chCharClass->IsWord(chByte);
}