
#include "gh3\GH3Keys.h"
#include "gh3\GlobalMap.h"
#include "gh3\QbScript.h"
#include "gh3\QbValueType.h"
#include "core\Patcher.h"
#include "QDB.h"
#include "gh3\malloc.h"
#include <stdint.h>
#include <stdio.h>
#include <Windows.h>
#include <CommCtrl.h>
#include "gh3\GH3Functions.h"

#pragma comment(lib, "winmm.lib")
static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);
#pragma region bytecodes
#define ESCRIPTTOKEN_ENDOFFILE 0x0
#define ESCRIPTTOKEN_ENDOFLINE 0x1
#define ESCRIPTTOKEN_ENDOFLINENUMBER 0x2
#define ESCRIPTTOKEN_STARTSTRUC 0x3
#define ESCRIPTTOKEN_ENDSTRUC 0x4
#define ESCRIPTTOKEN_STARTARRAY 0x5
#define ESCRIPTTOKEN_ENDARRAY 0x6
#define ESCRIPTTOKEN_EQUALS 0x7
#define ESCRIPTTOKEN_DOT 0x8
#define ESCRIPTTOKEN_COMMA 0x9
#define ESCRIPTTOKEN_MINUS 0x0A
#define ESCRIPTTOKEN_ADD 0x0B
#define ESCRIPTTOKEN_DIVIDE 0x0C
#define ESCRIPTTOKEN_MULTIPLY 0x0D
#define ESCRIPTTOKEN_OPENPARENTH 0x0E
#define ESCRIPTTOKEN_CLOSEPARENTH 0x0F
#define ESCRIPTTOKEN_DEBUGINFO 0x10
#define ESCRIPTTOKEN_SAMEAS 0x11
#define ESCRIPTTOKEN_LESSTHAN 0x12
#define ESCRIPTTOKEN_LESSTHANEQUAL 0x13
#define ESCRIPTTOKEN_GREATERTHAN 0x14
#define ESCRIPTTOKEN_GREATERTHANEQUAL 0x15
#define ESCRIPTTOKEN_NAME 0x16
#define ESCRIPTTOKEN_INTEGER 0x17
#define ESCRIPTTOKEN_HEXINTEGER 0x18
#define ESCRIPTTOKEN_ENUM 0x19
#define ESCRIPTTOKEN_FLOAT 0x1A
#define ESCRIPTTOKEN_STRING 0x1B
#define ESCRIPTTOKEN_LOCALSTRING 0x1C
#define ESCRIPTTOKEN_ARRAY 0x1D
#define ESCRIPTTOKEN_VECTOR 0x1E
#define ESCRIPTTOKEN_PAIR 0x1F
#define ESCRIPTTOKEN_KEYWORD_BEGIN 0x20
#define ESCRIPTTOKEN_KEYWORD_REPEAT 0x21
#define ESCRIPTTOKEN_KEYWORD_BREAK 0x22
#define ESCRIPTTOKEN_KEYWORD_SCRIPT 0x23
#define ESCRIPTTOKEN_KEYWORD_ENDSCRIPT 0x24
#define ESCRIPTTOKEN_KEYWORD_ELSEIF 0x27
#define ESCRIPTTOKEN_KEYWORD_ENDIF 0x28
#define ESCRIPTTOKEN_KEYWORD_RETURN 0x29
#define ESCRIPTTOKEN_UNDEFINED 0x2A
#define ESCRIPTTOKEN_CHECKSUM_NAME 0x2B
#define ESCRIPTTOKEN_KEYWORD_ALLARGS 0x2C
#define ESCRIPTTOKEN_ARG 0x2D
#define ESCRIPTTOKEN_JUMP 0x2E
#define ESCRIPTTOKEN_KEYWORD_RANDOM 0x2F
#define ESCRIPTTOKEN_KEYWORD_RANDOM_RANGE 0x30
#define ESCRIPTTOKEN_AT 0x31
#define ESCRIPTTOKEN_OR 0x32
#define ESCRIPTTOKEN_AND 0x33
#define ESCRIPTTOKEN_XOR 0x34
#define ESCRIPTTOKEN_SHIFT_LEFT 0x35
#define ESCRIPTTOKEN_SHIFT_RIGHT 0x36
#define ESCRIPTTOKEN_KEYWORD_RANDOM2 0x37
#define ESCRIPTTOKEN_KEYWORD_RANDOM_RANGE2 0x38
#define ESCRIPTTOKEN_KEYWORD_NOT 0x39
#define ESCRIPTTOKEN_KEYWORD_AND 0x3A
#define ESCRIPTTOKEN_KEYWORD_OR 0x3B
#define ESCRIPTTOKEN_KEYWORD_SWITCH 0x3C
#define ESCRIPTTOKEN_KEYWORD_ENDSWITCH 0x3D
#define ESCRIPTTOKEN_KEYWORD_CASE 0x3E
#define ESCRIPTTOKEN_KEYWORD_DEFAULT 0x3F
#define ESCRIPTTOKEN_KEYWORD_RANDOM_NO_REPEAT 0x40
#define ESCRIPTTOKEN_KEYWORD_RANDOM_PERMUT 0x41
#define ESCRIPTTOKEN_COLON 0x42
#define ESCRIPTTOKEN_RUNTIME_CFUNCTION 0x43
#define ESCRIPTTOKEN_RUNTIME_MEMBERFUNCTION 0x44
#define ESCRIPTTOKEN_KEYWORD_IF 0x47
#define ESCRIPTTOKEN_KEYWORD_ELSE 0x48
#define ESCRIPTTOKEN_SHORTJMP 0x49
#define ESCRIPTTOKEN_QBSTRUCT 0x4A
#define ESCRIPTTOKEN_GLOBAL 0x4B
#define ESCRIPTTOKEN_WSTRING 0x4C

#pragma endregion
#pragma region LZSS

namespace LZSS {
#define N		 4096	/* size of ring buffer */
#define F		   18	/* upper limit for match_length */
#define THRESHOLD	2   /* encode string into position and length
						   if match_length is greater than this */
#define NIL			N	/* index for root of binary search trees */

	unsigned long int
		textsize = 0,	/* text size counter */
		codesize = 0,	/* code size counter */
		printcount = 0;	/* counter for reporting progress every 1K bytes */


		//unsigned char text_buf[N + F - 1];	 	//ring buffer of size N, with extra F-1 bytes to facilitate string comparison 
	int     match_position, match_length;	// of longest match.  These are 	set by the InsertNode() procedure. 
	//int		lson[N + 1], rson[N + 257], dad[N + 1];   // left & right children & parents -- These constitute binary search trees.


	unsigned char* text_buf;
	int* lson;
	int* rson;
	int* dad;


#define readc()		*pIn++
#define writec(x)	*pOut++ = x


	void InitTree(void)	 /* initialize trees */
	{
		int  i;

		//Mem::Manager::sHandle().PushContext(Mem::Manager::sHandle().TopDownHeap());
		text_buf = new unsigned char[N + F - 1];
		lson = new int[N + 1];
		rson = new int[N + 257];
		dad = new int[N + 1];
		//Mem::Manager::sHandle().PopContext(); //Mem::Manager::sHandle().TopDownHeap());	



		/* For i = 0 to N - 1, rson[i] and lson[i] will be the right and
		   left children of node i.  These nodes need not be initialized.
		   Also, dad[i] is the parent of node i.  These are initialized to
		   NIL (= N), which stands for 'not used.'
		   For i = 0 to 255, rson[N + i + 1] is the root of the tree
		   for strings that begin with character i.  These are initialized
		   to NIL.  Note there are 256 trees. */

		for (i = N + 1; i <= N + 256; i++) rson[i] = NIL;
		for (i = 0; i < N; i++) dad[i] = NIL;
	}

	void    DeInitTree(void)  /* free up the memory */
	{
		delete[] text_buf;
		delete[] lson;
		delete[] rson;
		delete[] dad;

	}

	void InsertNode(int r)
		/* Inserts string of length F, text_buf[r..r+F-1], into one of the
		   trees (text_buf[r]'th tree) and returns the longest-match position
		   and length via the global variables match_position and match_length.
		   If match_length = F, then removes the old node in favor of the new
		   one, because the old one will be deleted sooner.
		   Note r plays double role, as tree node and position in buffer. */
	{
		int  i, p, cmp;
		unsigned char* key;

		cmp = 1;  key = &text_buf[r];  p = N + 1 + key[0];
		rson[r] = lson[r] = NIL;  match_length = 0;
		for (; ; )
		{
			if (cmp >= 0)
			{
				if (rson[p] != NIL)
				{
					p = rson[p];
				}
				else
				{
					rson[p] = r;  dad[r] = p;  return;
				}
			}
			else
			{
				if (lson[p] != NIL)
				{
					p = lson[p];
				}
				else
				{
					lson[p] = r;  dad[r] = p;  return;
				}
			}

			for (i = 1; i < F; i++)
			{
				if ((cmp = key[i] - text_buf[p + i]) != 0)  break;
			}

			if (i > match_length)
			{
				match_position = p;
				if ((match_length = i) >= F)
				{
					break;
				}
			}
		}
		dad[r] = dad[p];  lson[r] = lson[p];  rson[r] = rson[p];
		dad[lson[p]] = r;  dad[rson[p]] = r;
		if (rson[dad[p]] == p)
		{
			rson[dad[p]] = r;
		}
		else
		{
			lson[dad[p]] = r;
		}
		dad[p] = NIL;  /* remove p */
	}

	void DeleteNode(int p)	/* deletes node p from tree */
	{
		int  q;

		if (dad[p] == NIL) return;  /* not in tree */
		if (rson[p] == NIL) q = lson[p];
		else if (lson[p] == NIL) q = rson[p];
		else
		{
			q = lson[p];
			if (rson[q] != NIL)
			{
				do
				{
					q = rson[q];
				} while (rson[q] != NIL);
				rson[dad[q]] = lson[q];  dad[lson[q]] = dad[q];
				lson[q] = lson[p];  dad[lson[p]] = q;
			}
			rson[q] = rson[p];  dad[rson[p]] = q;
		}
		dad[q] = dad[p];
		if (rson[dad[p]] == p) rson[dad[p]] = q;
		else lson[dad[p]] = q;
		dad[p] = NIL;
	}

	int Encode(char* pIn, char* pOut, int bytes_to_read, bool print_progress)
	{
		int  i, c, len, r, s, last_match_length, code_buf_ptr;
		unsigned char  code_buf[17], mask;

		textsize = 0;	/* text size counter */
		codesize = 0;	/* code size counter */
		printcount = 0;	/* counter for reporting progress every 1K bytes */

		InitTree();	 /* initialize trees */
		code_buf[0] = 0;  /* code_buf[1..16] saves eight units of code, and
			code_buf[0] works as eight flags, "1" representing that the unit
			is an unencoded letter (1 byte), "0" a position-and-length pair
			(2 bytes).  Thus, eight units require at most 16 bytes of code. */
		code_buf_ptr = mask = 1;
		s = 0;  r = N - F;
		for (i = s; i < r; i++) text_buf[i] = ' ';  /* Clear the buffer with
			  any character that will appear often. */
		for (len = 0; len < F && bytes_to_read; len++)
		{
			c = readc();
			bytes_to_read--;
			text_buf[r + len] = c;	/* Read F bytes into the last F bytes of
				the buffer */
		}
		if ((textsize = len) == 0)
		{
			DeInitTree();
			return 0;  /* text of size zero */
		}
		for (i = 1; i <= F; i++) InsertNode(r - i);  /* Insert the F strings,
			  each of which begins with one or more 'space' characters.  Note
			  the order in which these strings are inserted.  This way,
			  degenerate trees will be less likely to occur. */
		InsertNode(r);	/* Finally, insert the whole string just read.  The
			global variables match_length and match_position are set. */
		do
		{
			if (match_length > len) match_length = len;  /* match_length
				  may be spuriously long near the end of text. */
			if (match_length <= THRESHOLD)
			{
				match_length = 1;  /* Not long enough match.  Send one byte. */
				code_buf[0] |= mask;  /* 'send one byte' flag */
				code_buf[code_buf_ptr++] = text_buf[r];	 /* Send uncoded. */
			}
			else
			{
				code_buf[code_buf_ptr++] = (unsigned char)match_position;
				code_buf[code_buf_ptr++] = (unsigned char)
					(((match_position >> 4) & 0xf0)
						| (match_length - (THRESHOLD + 1)));  /* Send position and
							  length pair. Note match_length > THRESHOLD. */
			}
			if ((mask <<= 1) == 0)
			{  /* Shift mask left one bit. */
				for (i = 0; i < code_buf_ptr; i++)  /* Send at most 8 units of */
					writec(code_buf[i]);	 /* code together */
				codesize += code_buf_ptr;
				code_buf[0] = 0;  code_buf_ptr = mask = 1;
			}
			last_match_length = match_length;
			for (i = 0; i < last_match_length &&
				bytes_to_read; i++)
			{
				c = readc();
				bytes_to_read--;
				DeleteNode(s);		/* Delete old strings and */
				text_buf[s] = c;	/* read new bytes */
				if (s < F - 1) text_buf[s + N] = c;  /* If the position is
					  near the end of buffer, extend the buffer to make
					  string comparison easier. */
				s = (s + 1) & (N - 1);  r = (r + 1) & (N - 1);
				/* Since this is a ring buffer, increment the position
				   modulo N. */
				InsertNode(r);	/* Register the string in text_buf[r..r+F-1] */
			}
			if ((textsize += i) > printcount)
			{
				//			printf("%12ld\r", textsize);  printcount += 1024;
							/* Reports progress each time the textsize exceeds
							   multiples of 1024. */
			}
			while (i++ < last_match_length)
			{	/* After the end of text, */
				DeleteNode(s);					/* no need to read, but */
				s = (s + 1) & (N - 1);  r = (r + 1) & (N - 1);
				if (--len) InsertNode(r);		  /* buffer may not be empty. */
			}
		} while (len > 0);  /* until length of string to be processed is zero */
		if (code_buf_ptr > 1)
		{	  /* Send remaining code. */
			for (i = 0; i < code_buf_ptr; i++) writec(code_buf[i]);
			codesize += code_buf_ptr;
		}

		if (print_progress)
		{
			printf("     In : %ld bytes\n", textsize);	/* Encoding is done. */
			printf("     Out: %ld bytes\n", codesize);
			printf("     Out/In: %.3f\n", (double)codesize / textsize);
		}

		DeInitTree();
		return codesize;
	}


	///////////////////////////////////////////////////////////////////////////////////////
	//
	// Decompression code, cut & pasted from pre.cpp
	//
	///////////////////////////////////////////////////////////////////////////////////////

#define RINGBUFFERSIZE		 4096	/* N size of ring buffer */
#define MATCHLIMIT		   18	/* F upper limit for match_length */
#define THRESHOLD	2   /* encode string into position and length */

//#define WriteOut(x) 	{Dbg_MsgAssert(pOut<pIn,("Oops! Decompression overlap!\nIncrease IN_PLACE_DECOMPRESSION_MARGIN in sys\\file\\pip.cpp")); *pOut++ = x;}
#define WriteOut(x) 	{*pOut++ = x;}

#define USE_BUFFER	1		 // we don't need no stinking buffer!!!!

#if USE_BUFFER
#ifdef	__PLAT_NGPS__
// ring buffer is just over 4K 4096+17),
// so fits nicely in the PS2's 8K scratchpad
	static unsigned char    sTextBuf[RINGBUFFERSIZE + MATCHLIMIT - 1];
	// Note:  if we try to use the scratchpad, like this
	// then the code actually runs slower
	// if we want to optimize this, then it should
	// be hand crafted in assembly, using 128bit registers
	//	const unsigned char * sTextBuf = (unsigned char*) 0x70000000;
#else
	static unsigned char
		sTextBuf[RINGBUFFERSIZE + MATCHLIMIT - 1];	/* ring buffer of size N,
			with extra F-1 bytes to facilitate string comparison */
#endif
#endif


#define	ReadInto(x)		if (!Len) break; Len--; x = *pIn++
#define	ReadInto2(x)	Len--; x = *pIn++ 	  // version that knows Len is Ok


			// Decode an LZSS encoded stream
			// Runs at approx 12MB/s on PS2	 without scratchpad (which slows it down in C)
			// a 32x CD would run at 4.8MB/sec, although we seem to get a lot less than this
			// with our current file system, more like 600K per seconds.....
			// Need to write a fast streaming file system....

			// Ken: Made this return the new pOut so I can do some asserts to make sure it has
			// written the expected number of bytes.
	unsigned char* DecodeLZSS(unsigned char* pIn, unsigned char* pOut, int Len)	/* Just the reverse of Encode(). */
	{
		int  i, j, k, r, c;
		//	uint64	LongWord;
		//	int bytes = 0;
		//	unsigned char *pScratch;
		unsigned int  flags;



		//	int basetime =  (int) Tmr::ElapsedTime(0);
		//	int len = Len;

		//	int	OutBytes = 4;
		//	int	OutWord = 0;

#if USE_BUFFER
		for (i = 0; i < RINGBUFFERSIZE - MATCHLIMIT; i++)
			sTextBuf[i] = ' ';
		r = RINGBUFFERSIZE - MATCHLIMIT;
#else
		r = RINGBUFFERSIZE - MATCHLIMIT;
#endif
		flags = 0;
		for (; ; )
		{
			if (((flags >>= 1) & 256) == 0)
			{
				ReadInto(c);
				flags = c | 0xff00;			/* uses higher byte cleverly */
			}										/* to count eight */
			if (flags & 1)
			{
				ReadInto(c);
				//			putc(c, outfile);
				WriteOut(c);
#if USE_BUFFER
				sTextBuf[r++] = c;
				r &= (RINGBUFFERSIZE - 1);
#else
				r++;
				//			r &= (RINGBUFFERSIZE - 1);	  // don't need to wrap r until it is used
#endif
			}
			else
			{
				ReadInto(i);
				ReadInto2(j);			// note, don't need to check len on this one....

				i |= ((j & 0xf0) << 4);						// i is 12 bit offset

#if !USE_BUFFER
				j = (j & 0x0f) + THRESHOLD + 1;				// j is 4 bit length (above the threshold)
				unsigned char* pStream;
				r &= (RINGBUFFERSIZE - 1);					// wrap r around before it is used
				pStream = pOut - r;							// get base of block
				if (i >= r)										  // if offset > r, then
					pStream -= RINGBUFFERSIZE;				// it's the previous block
				pStream += i;									// add in the offset to the base
				r += j;												// add size to r
				while (j--)									  // copy j bytes
					WriteOut(*pStream++);
#else

				j = (j & 0x0f) + THRESHOLD;				// j is 4 bit length (above the threshold)
				for (k = 0; k <= j; k++)					  // just copy the bytes
				{
					c = sTextBuf[(i + k) & (RINGBUFFERSIZE - 1)];
					WriteOut(c);
					sTextBuf[r++] = c;
					r &= (RINGBUFFERSIZE - 1);
				}
#endif
			}
		}

		//	int Time = (int) Tmr::ElapsedTime(basetime);
		//	if (Time > 5)
		//	{
		//		printf("decomp time is %d ms, for %d bytes,  %d bytes/second\n", Time,len, len * 1000 /Time );
		//	}
		return pOut;
	}
}

#pragma endregion

int qbstrindent = 0;

char*DBGName(UINT key);

const char* qbstructOpen = "{ ";
void printStructStructIndent(char*buf,int size)
{
	return;
	for (int i = 0; i < qbstrindent + 1; i++) {
		strcat_s(buf, size, "    ");
	}
}
void printStructItem(QbKey key, DWORD value, QbValueType type, char*buf, int size);
void printStructBase(QbStruct* qs,char*buf,int size)
{
	if (!qs)
	{
		//printStructStructIndent();
		//print("NON-EXISTENT STRUCT!!!\n");
		return;
	}
	QbStructItem* qsi;
	qsi = qs->first;
	while (qsi)
	{
		printStructStructIndent(buf,size);
		printStructItem(qsi->key, qsi->value, qsi->Type(), buf, size);
		if (!qsi->next)
			break;
		qsi = qsi->next;
	}
}
#define WTF(x) (*(UINT*)(x))
void printStructItem(QbKey key, DWORD value, QbValueType type, char*buf, int size)
{
	char qbstrstr[0x400];
	int qbstrstr2;
	char* keyStr;
	qbstrstr2 = !(UINT)key ? 0 : sprintf_s(qbstrstr, "%s = ", DBGName(key));
	sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "%s ", DBGName(value));
	switch (type)
	{
	case TypeInt:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "%d ", (signed int)value);
		break;
	case TypeFloat:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "%f ", (*(FLOAT*)&(value)));
		break;
	case TypeCString:
	case TypeWString:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "");
		break;
	case TypePair:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "(%f, %f) ",
			*(float*)(&WTF(value) + 1),
			*(float*)(&WTF(value) + 2));
		break;
	case TypeVector:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "(%f, %f, %f) ",
			*(float*)(&(WTF(value)) + 1),
			*(float*)(&(WTF(value)) + 2),
			*(float*)(&(WTF(value)) + 3));
		break;
	case TypeStringPointer:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "0x%08X ", (DWORD*)(value));
		break;
	case TypeScript:
	case TypeCFunc:
	case TypeUnk9:
	case TypeUnk20:
	case TypeUnk21:
	case TypeBinaryTree1:
	case TypeBinaryTree2:
	case TypeQbMap:
	case TypeQbKeyStringQs:
		break;
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "WHAT IS THIS: 0x%p: %08X %08X %08X %08X\n",
			(DWORD*)(value),
			*(DWORD*)(value), *(DWORD*)(value + 4),
			*(DWORD*)(value + 8), *(DWORD*)(value + 12));
		break;
	}
	if (//type != TypeStringPointer &&
		type != TypeCString &&
		type != TypeWString &&
		type != TypeQbStruct &&
		type != TypeQbArray)
	{
		strcat_s(buf, size, qbstrstr);
	}
	else if (type == TypeCString) {
		strcat_s(buf, size, qbstrstr);
		strcat_s(buf, size, "'");
		strcat_s(buf, size, (char*)value);
		strcat_s(buf, size, "' ");
	}
	else if (type == TypeWString) {
		strcat_s(buf, size, qbstrstr);
		char tombs[200];
		size_t what;
		strcat_s(buf, size, "\"");
		wcstombs_s(&what, tombs, sizeof(tombs), ((wchar_t*)value), sizeof(tombs));
		strcat_s(buf, size, tombs);
		strcat_s(buf, size, "\" ");
	}
	else if (type == TypeQbArray) {
		strcat_s(buf, size, qbstrstr);
		//strcat_s(buf, size, "\n");
		qbstrindent++;
		printStructStructIndent(buf, size);
		QbArray* qa = (QbArray*)value;
		char itoatmp2[12];
		_itoa_s(qa->Type(), itoatmp2, 12, 10);
		strcat_s(buf, size, itoatmp2);
		strcat_s(buf, size, " QbArray [");
		strcat_s(buf, size, qbstrstr);
		DWORD weird;
		QbValueType qatype = qa->Type();
		// try to collapse items into multiple lines when there's a lot in the array
		DWORD qal = qa->Length();
		for (UINT i = 0; i < qal; i++)
		{
			switch (qatype)
			{
			case TypeInt:
				char itoatmp[12];
				_itoa_s((signed int)qa->Get(i), itoatmp, 12, 10);
				strcat_s(buf, size, itoatmp);
				break;
			case TypeFloat:
				char ftoatmp[16];
				weird = qa->Get(i);
				sprintf_s(ftoatmp, "%f", (*(FLOAT*)&weird));
				strcat_s(buf, size, ftoatmp);
				break;
			case TypeCString:
				strcat_s(buf, size, qbstrstr);
				strcat_s(buf, size, "\"");
				strcat_s(buf, size, (char*)value);
				strcat_s(buf, size, "\"");
				break;
			case TypeWString:
				char tombs[200];
				size_t what;
				wcstombs_s(&what, tombs, sizeof(tombs), ((wchar_t*)qa->Get(i)), sizeof(tombs));
				strcat_s(buf, size, tombs);
				break;
			case TypeQbKey:
				strcat_s(buf, size, DBGName(qa->Get(i)));
				break;
			case TypeQbStruct:
				strcpy_s(qbstrstr, "");
				qbstrindent++;
				printStructStructIndent(buf, size);
				strcat_s(buf, size, (qbstructOpen));
				qbstrindent++;
				printStructBase((QbStruct*)(qa->Get(i)), buf, size);
				qbstrindent--;
				printStructStructIndent(buf, size);
				if (i != qa->Length() - 1) {
					strcat_s(buf, size, "}");
				}
				else
				{
					qbstrindent--;
					strcat_s(buf, size, "}");
					printStructStructIndent(buf, size);
					qbstrindent++;
				}
				qbstrindent--;
				break;
			}
			if (i != qa->Length() - 1) {
				strcat_s(buf, size, ", ");
			}
			else if (i == qa->Length() - 1)
			{
				if (qal > 8)
				{
					qbstrindent--;
				}
			}
		}
		strcat_s(buf, size, qbstrstr);
		strcat_s(buf, size, "]");
		if (qal <= 8)
		{
			qbstrindent--;
		}
	}
	else {
		strcat_s(buf, size, qbstrstr);
		printStructStructIndent(buf, size);
		strcat_s(buf, size, qbstructOpen);
		qbstrindent++;
		printStructBase((QbStruct*)(value), buf, size);
		qbstrindent--;
		printStructStructIndent(buf, size);
		strcat_s(buf, size, "}");
	}
}



///// NOTE: FOR USE WITH QDB:
///// https://github.com/donnaken15/QDB
enum GameState : byte
{
	Running	= 0x00,
	Pause	= 0xF0,
	Step	= 0xF2,
	StepOver= 0xF2 | 1,
	Exit = 0xFF
};
byte state, injectStruct;
DWORD kIndex = 0;

static QbScript**ActiveScriptChain = (QbScript**)0x00B5452C;
static QbScript*currentScriptPointer = (QbScript*)0x00B54524;
static DWORD*LoadedScriptList_nearby = (DWORD*)0x00B54210;
#define CURSCRIPT (currentScriptPointer)

#define MAX_KEYS 50000
// found that max amount of keys from dbg.pak deduped is 38.5k
#define KEYSTRLEN 128
UINT*dbgKeys;
LPSTR dbgStr;
char*keytmp;

char keyfault[9];
char*DBGName(UINT key)
{
	if (!key)
		return "00000000";
	for (uint16_t i = 0; i < MAX_KEYS; i++)
		if (key == dbgKeys[i])
			return (dbgStr + (i * KEYSTRLEN));
	sprintf_s(keyfault, "%08X", key);
	return keyfault;
}
char*opStr[] = {
	"", // ENDOFFILE 0x0
	"\n", // ENDOFLINE 0x1
	"", // ENDOFLINENUMBER 0x2
	"{", // STARTSTRUC 0x3
	"}", // ENDSTRUC 0x4
	"[", // STARTARRAY 0x5
	"]", // ENDARRAY 0x6
	" = ", // EQUALS 0x7
	".", // DOT 0x8
	",", // COMMA 0x9
	" - ", // MINUS 0x0A
	" + ", // ADD 0x0B
	" / ", // DIVIDE 0x0C
	" * ", // MULTIPLY 0x0D
	"(", // OPENPARENTH 0x0E
	")", // CLOSEPARENTH 0x0F
	"", // DEBUGINFO 0x10
	" == ", // SAMEAS 0x11
	" < ", // LESSTHAN 0x12
	" <= ", // LESSTHANEQUAL 0x13
	" > ", // GREATERTHAN 0x14
	" >= ", // GREATERTHANEQUAL 0x15
	"", // NAME 0x16
	"", // INTEGER 0x17
	"", // HEXINTEGER 0x18
	"", // ENUM 0x19
	" ", // FLOAT 0x1A
	" ", // STRING 0x1B
	"", // LOCALSTRING 0x1C
	" ", // ARRAY 0x1D
	" ", // VECTOR 0x1E
	" ", // PAIR 0x1F
	"begin", // KEYWORD_BEGIN 0x20
	"repeat ", // KEYWORD_REPEAT 0x21
	"break", // KEYWORD_BREAK 0x22
	"", // KEYWORD_SCRIPT 0x23 // dont see get used
	"endscript", // KEYWORD_ENDSCRIPT 0x24
	"", "",
	"elseif ", // KEYWORD_ELSEIF 0x27
	"endif", // KEYWORD_ENDIF 0x28
	"return", // KEYWORD_RETURN 0x29
	"", // UNDEFINED 0x2A
	"", // CHECKSUM_NAME 0x2B
	" <...>", // KEYWORD_ALLARGS 0x2C
	"%", // ARG 0x2D
	"", // JUMP 0x2E
	"random", // KEYWORD_RANDOM 0x2F
	"randomrange", // KEYWORD_RANDOM_RANGE 0x30
	"", // AT 0x31
	" || ", // OR 0x32
	" && ", // AND 0x33
	" ^ ", // XOR 0x34
	" << ", // SHIFT_LEFT 0x35
	" >> ", // SHIFT_RIGHT 0x36
	"random2", // KEYWORD_RANDOM2 0x37
	"randomrange2", // KEYWORD_RANDOM_RANGE2 0x38
	"!", // KEYWORD_NOT 0x39
	" & ", // KEYWORD_AND 0x3A
	" |", // KEYWORD_OR 0x3B
	"switch", // KEYWORD_SWITCH 0x3C
	"endswitch", // KEYWORD_ENDSWITCH 0x3D
	"case", // KEYWORD_CASE 0x3E
	"default", // KEYWORD_DEFAULT 0x3F
	"", // KEYWORD_RANDOM_NO_REPEAT 0x40
	"", // KEYWORD_RANDOM_PERMUT 0x41
	" :", // COLON 0x42
	"", // RUNTIME_CFUNCTION 0x43
	"", // RUNTIME_MEMBERFUNCTION 0x44
	"", "",
	"if ", // KEYWORD_IF 0x47
	"else", // KEYWORD_ELSE 0x48
	"", // SHORTJMP 0x49
	" ", // QBSTRUCT 0x4A
	"*", // GLOBAL 0x4B
	"", // WSTRING 0x4C
};
//HashMapNode(*hashMapThing)(DWORD* _map, QbKey) = (HashMapNode(*)(DWORD * _map, QbKey))0x0048AC60;
HashMapNode**g_hashMap;
struct ScriptListNode {
	QbKey name;
	BYTE* code;
	ScriptListNode*prev,*next;
};
struct ScriptHashMapEntry {
	QbKey name;
	ScriptListNode*hi,*lo;
};
ScriptListNode*ScriptNodesTraverse(ScriptListNode*node, QbKey k)
{
	for (ScriptListNode*n = node; n; n = n->next)
	{
		//char*fail = "";
		//if (n->name)
		//	fail = DBGName(n->name);
		//printf("%08X %s\n", n, fail);
		if (n->name == k)
			return n;
	}
	return 0;
}
byte*FindScriptCached(QbKey key)
{
	// half works
	ScriptHashMapEntry* ScrList = (ScriptHashMapEntry*)*((*(DWORD**)((*LoadedScriptList_nearby) + 0x14) + 1));
	ScriptListNode* node = 0;
	//node = ScriptNodesTraverse(ScrList[key & 0xFF].lo, key);
	if (!node)
		node = ScriptNodesTraverse(ScrList[key & 0xFF].hi, key);
	if (!node)
		node = ScriptNodesTraverse(*(ScriptListNode**)((*LoadedScriptList_nearby) + 0x18),key);
	if (!node)
		node = ScriptNodesTraverse(*(ScriptListNode**)((*LoadedScriptList_nearby) + 0x1C),key);
	if (node) return node->code;
	return 0;
}
byte*FindScript(QbKey key)
{
	// HOW DOES THIS WORK!?!?!?!?!?!?!?!?!?!?!?!
	//ScriptHashMapEntry*ScrList = (ScriptHashMapEntry*)*((*(DWORD**)((*LoadedScriptList_nearby) + 0x14) + 1));
	//ScriptListNode*node = 0;
	//node = ScriptNodesTraverse(ScrList[key & 0xFF].hi, key);
	//if (!node)
	//	node = ScriptNodesTraverse(ScrList[key & 0xFF].lo, key);
	// wtf
	//if (!node)
	//	node = ScriptNodesTraverse(*(ScriptListNode**)((*LoadedScriptList_nearby) + 0x18),key);
	//if (!node)
	//	node = ScriptNodesTraverse(*(ScriptListNode**)((*LoadedScriptList_nearby) + 0x1C),key);
	//if (node) return node;
	//return 0;
	//return node->code;
	HashMapNode*h,*entry;
	for (h = g_hashMap[key & 0x7FFF]; h; h = h->nextItem)
	{
		if (h->key == key)
			break;
	}
	entry = h;
	char v12;
	if (h && (v12 = h->itemType, v12 == 13) || v12 == 26)
	{
		while (1)
		{
			h = g_hashMap[entry->value & 0x7FFF];
			if (!h) break;
			while (h->key != entry->value)
			{
				h = h->nextItem;
			}
			if (!h) break;
			if (h->itemType == 7)
			{
				break;
			}
		}
	}
	if (h)
	{
		if (h->itemType == 7)
		{
			UINT len = *(UINT*)((byte*)(h->value) + 4);
			byte*decomp = (byte*)malloc(len);
			LZSS::DecodeLZSS(
				(byte*)((byte*)(h->value) + 12),
				decomp, *(UINT*)((byte*)(h->value) + 8)
			);
			return decomp;
			// babeface
			// uncompressed
			// compressed
			//printf("%08X\n", h->value);
			//for (int i = 0; i < 100; i++)
			//	printf("%02X ", decomp[i]);
			//puts("");
		}
	}
	return 0;
}
//typedef bool (*CFunc)(QbStruct*, QbScript*);
struct CFuncEntry {
	char*name;
	void*func;
};
char*CFuncName(UINT*func)
{
	CFuncEntry*list = (CFuncEntry*)0x009551B8;
	UINT count = *(UINT*)0x4134A1;
	for (UINT i = 0; i < count; i++)
	{
		void*f = list[i].func;
		if (func == f)
			return list[i].name;
	}
	return "";
}
// important for QbStructs
byte align(UINT ea, byte bits)
{
	bits = 1 << bits;
	auto mask = bits - 1;
	auto align = ea;
	if (align & mask)
		return bits - (align & mask);
	return 0;
}
/*size_t GetCurrentLine(byte* IP, byte* cursor, int* lines, int* lcount)
{
	if (!IP) return 0;
	byte*start = IP;
	byte EOS = 0;
	UINT l = 0;
	byte matchedIP = 0;
	byte firstbyte = 1;
	byte gotdefaults = 0;
	byte*line_begin = IP;
	while (!EOS)
	{
		byte op = *IP;
		if (op != ESCRIPTTOKEN_KEYWORD_ENDSCRIPT && op)
		{
			gotdefaults = 0;
			switch (op)
			{
			case ESCRIPTTOKEN_RUNTIME_CFUNCTION:
			case ESCRIPTTOKEN_RUNTIME_MEMBERFUNCTION:
			case 0x46: // different type of CFunc operation?????????
			case ESCRIPTTOKEN_NAME:
			case ESCRIPTTOKEN_INTEGER:
			case ESCRIPTTOKEN_FLOAT:
				IP += 4;
				break;
			case ESCRIPTTOKEN_PAIR:
				IP += 8;
				break;
			case ESCRIPTTOKEN_VECTOR:
				IP += 12;
				break;
			case ESCRIPTTOKEN_STRING: {
				IP++;
				size_t len = *(size_t*)IP;
				IP += 3;
				IP += len;
			} break;
			case ESCRIPTTOKEN_WSTRING: {
				IP++;
				size_t len = *(size_t*)IP;
				IP += 4; // ffs
				IP += len;
			} break;
			case ESCRIPTTOKEN_KEYWORD_ELSE:
				IP++;
			case ESCRIPTTOKEN_KEYWORD_ELSEIF:
			case ESCRIPTTOKEN_SHORTJMP:
			case ESCRIPTTOKEN_KEYWORD_IF:
				IP++;
				if (op == ESCRIPTTOKEN_KEYWORD_ELSEIF)
					IP += 2;
				IP++;
				break;
			case ESCRIPTTOKEN_QBSTRUCT: {
				IP++;
				auto QSSIZE = *(WORD*)IP;
				IP++;
				if ((UINT)IP % 4 != 0)
					IP -= (UINT)IP % 4;
				if (!firstbyte)
				{
				}
				else {
					gotdefaults = 1;
				}
				IP += QSSIZE - 1 + 4;
			} break;
			}
			if (op == ESCRIPTTOKEN_ENDOFLINE ||
				!*(byte*)(IP + 1) ||
				*(byte*)(IP + 1) == ESCRIPTTOKEN_KEYWORD_ENDSCRIPT)
			{
				if (lines)
					lines[l++] = IP - start;
				if (cursor)
					if (IP == cursor)
						if (!matchedIP)
						{
							matchedIP = 1;
							cursor = (BYTE*)l; // lol
						}
						else
							if (!matchedIP)
								cursor = (BYTE*)-1;
				line_begin = IP;
			}
			IP++;
			firstbyte = 0;
		}
		else
			EOS = 1;
	}
	return (size_t)cursor;
}*/
size_t Decompile(byte*IP, char*out, size_t outl, int*lines, int*lcount, int*IP_)
{
	if (!IP) return 0;
	//printf("%08X\n",IP);
	memset(out, 0, outl--);
	byte*start = IP;
	byte EOS = 0;
	byte spacecheck = 0;
	char indent = 0;
	UINT ll = 0;
	byte matchedIP = 0;
	byte firstbyte = 1;
	byte gotdefaults = 0;
	byte*line_begin = IP;
	while (!EOS)
	{
		byte op = *IP;
		if (op != ESCRIPTTOKEN_KEYWORD_ENDSCRIPT && op)
		{
			if (indent < 0)
				indent = 0;
			{
				//char test[6];
				//sprintf_s(test, "(%08X)", *(BYTE*)IP);
				//strcat_s(out, outl, test);
				//strcat_s(out, outl, "     ");
			}
			if (!(firstbyte && op == ESCRIPTTOKEN_ENDOFLINE) && !(gotdefaults))
				if (op < 0x4D)
					strcat_s(out, outl, opStr[op]);
			gotdefaults = 0;
			if (op == ESCRIPTTOKEN_NAME ||
				op == ESCRIPTTOKEN_INTEGER ||
				op == ESCRIPTTOKEN_FLOAT ||
				op == ESCRIPTTOKEN_PAIR ||
				op == ESCRIPTTOKEN_STRING ||
				op == ESCRIPTTOKEN_WSTRING ||
				op == ESCRIPTTOKEN_RUNTIME_CFUNCTION)
				spacecheck++;
			else
				spacecheck = 0;
			switch (op)
			{
			case ESCRIPTTOKEN_RUNTIME_CFUNCTION: {
				IP++;
				strcat_s(out, outl, CFuncName(*(UINT**)IP));
				IP += 3;
			} break;
			case ESCRIPTTOKEN_NAME:
			case 0x46:
				if (spacecheck > 1)
					strcat_s(out, outl, " ");
				IP++;
				strcat_s(out, outl, DBGName(*(UINT*)IP));
				if (spacecheck > 1)
					strcat_s(out, outl, " ");
				IP += 3;
				break;
			case ESCRIPTTOKEN_INTEGER: {
				char number[12];
				sprintf_s(number,"%d",*(UINT*)++IP);
				strcat_s(out, outl, number);
				IP += 3;
			} break;
			case ESCRIPTTOKEN_FLOAT: {
				char number[40];
				sprintf_s(number,"%f",*(float*)++IP);
				strcat_s(out, outl, number);
				IP += 3;
			} break;
			case ESCRIPTTOKEN_PAIR: {
				char pair[90];
				sprintf_s(pair,"(%f,%f)",*(float*)++IP,*(float*)(IP+4));
				IP += 7;
				strcat_s(out, outl, pair);
			} break;
			case ESCRIPTTOKEN_VECTOR: {
				char pair[130];
				sprintf_s(pair,"(%f,%f,%f)",*(float*)++IP,*(float*)(IP+4),*(float*)(IP+8));
				IP += 11;
				strcat_s(out, outl, pair);
			} break;
			case ESCRIPTTOKEN_STRING: {
				IP++;
				size_t len = *(size_t*)IP;
				IP += 3;
				if (spacecheck > 1)
					strcat_s(out, outl, " ");
				strcat_s(out, outl, "'");
				strncat_s(out, outl, (char*)(IP+1), len);
				strcat_s(out, outl, "' ");
				if (spacecheck > 1)
					strcat_s(out, outl, " ");
				IP += len;
			} break;
			case ESCRIPTTOKEN_WSTRING: { // :grimacing:
				// I JUST REMEMBERED IT'S BIG ENDIAN
				// OH NO
				IP++;
				size_t len = *(size_t*)IP;
				IP += 3;
				char*tombs = (char*)malloc(len);
				size_t what;
				if (spacecheck > 1)
					strcat_s(out, outl, " ");
				strcat_s(out, outl, "\"");
				IP++; // REMOVE AND ALLOCATE AND COPY WSTRING AND SWAP BYTES FOR SOME REASON
				wcstombs_s(&what, tombs, len, ((wchar_t*)IP), len);
				strcat_s(out, outl, tombs);
				strcat_s(out, outl, "\"");
				if (spacecheck > 1)
					strcat_s(out, outl, " ");
				free(tombs);
				IP += len;
			} break;
			case ESCRIPTTOKEN_KEYWORD_ELSE:
			case ESCRIPTTOKEN_KEYWORD_ELSEIF:
				indent++;
			case ESCRIPTTOKEN_SHORTJMP:
			case ESCRIPTTOKEN_KEYWORD_IF:
				IP++;
				if (op == ESCRIPTTOKEN_KEYWORD_ELSEIF)
					IP += 2;
				IP++;
				break;
			case ESCRIPTTOKEN_QBSTRUCT: {
				IP++;
				auto QSSIZE = *(WORD*)IP;
				IP++;
				if ((UINT)IP % 4 != 0)
					IP -= (UINT)IP % 4;
					//IP += ((UINT)IP & 3);
				//while ((UINT)IP & 3)
				//	IP++; // i hate this
#if 0
				{
					char test[12];
					sprintf_s(test, "(%08X)", IP);
					strcat_s(out, outl, test);
				}
#endif
				if (!firstbyte)
				{
					// ignore default params
					qbstrindent += indent;
					//strcat_s(out, outl, "\{ ");
					printStructBase((QbStruct*)((byte*)IP + 4), out, outl);

					//char* test = (char*)malloc(indent * 4);
					//memset(test, ' ', indent * 4);
					//strncat_s(out, outl, test, indent * 4);
					//free(test);

					//strcat_s(out, outl, "}");
					qbstrindent -= indent;
					//qstr(IP + _align);
				}
				else {
					gotdefaults = 1;
				}
				IP += QSSIZE - 1 + 4;
			} break;
			case ESCRIPTTOKEN_ENDOFLINE: {
				switch (*(byte*)(IP + 1))
				{
				case ESCRIPTTOKEN_KEYWORD_ENDIF:
				case ESCRIPTTOKEN_KEYWORD_REPEAT:
					indent--;
					break;
				case ESCRIPTTOKEN_KEYWORD_ELSE:
				case ESCRIPTTOKEN_KEYWORD_ELSEIF:
					indent--;
					//if (indent > -1)
					break;
				}
			} break;
			}
			if (indent < 0)
				indent = 0;
			if (op == ESCRIPTTOKEN_ENDOFLINE ||
				!*(byte*)(IP + 1) ||
				*(byte*)(IP + 1) == ESCRIPTTOKEN_KEYWORD_ENDSCRIPT)
			{
				if (lines)
					lines[ll++] = IP - start;
				if (IP_)
					if (IP == (BYTE*)*IP_)
						if (!matchedIP)
						{
							matchedIP = 1;
							*IP_ = ll;
						}
					else
						if (!matchedIP)
							*IP_ = -1;
				char*test = (char*)malloc(indent * 4);
				memset(test, ' ', indent * 4);
				strncat_s(out, outl, test, indent * 4);
				free(test);
#if 0
				UINT hex_view_len = (IP - line_begin) * 3;
				char* buf = (char*)malloc(4);
				test = (char*)malloc(hex_view_len + 3);
				memset(test, 0, hex_view_len + 3);
				for (int i = 0; i < (IP - line_begin); i++)
				{
					sprintf_s(buf, 4, "%02X ", line_begin[i]);
					//strcat_s(test, hex_view_len, "A");
					//sprintf_s(test, hex_view_len, buf);
					//strncpy_s(test + (i*3), hex_view_len - (i*3), buf, 3);
					memcpy(test + (i * 3), buf, 3);
				}
				//strcat_s(test, hex_view_len, "A");
				strcat_s(out, outl, test);
				strcat_s(out, outl, "\n");
				free(buf);
				free(test);
#endif
				line_begin = IP;
			}
			switch (*(byte*)(IP + 1))
			{
			case ESCRIPTTOKEN_KEYWORD_IF:
			case ESCRIPTTOKEN_KEYWORD_BEGIN:
				indent++;
				break;
			}
			IP++;
			firstbyte = 0;
		}
		else
		{
			if (op == ESCRIPTTOKEN_KEYWORD_ENDSCRIPT)
				strcat_s(out, outl, opStr[op]);
			{
				char test[16];
				sprintf_s(test, "(%08X:%02X)", IP, *(BYTE*)IP);
				strcat_s(out, outl, test);
			}
			strcat_s(out, outl, "\n");
			EOS = 1;
		}
		//puts("\nAAAAA\n");
		//puts(out);
		//puts("\nAAAAA\n");
		//Sleep(1000);
	}
	return IP - start;
}

byte autostep = 0;
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
MSG msg;
HWND hW;
WNDCLASS wc;
HWND buttons[5];
char*btntext[] = {
	"Run",
	"Pause",
	"Stop",
	"Step",
	"Auto Step"
};
const byte csdstruc_col_widths[] = { 17, 14 };
const byte csdstruc_buf_size =
	1 + csdstruc_col_widths[0] +
	3 + csdstruc_col_widths[1] +
	1;
int csdstruc_indent = 0;
HFONT sserif, msgoth;
HWND scrlist, ilist, CSDbox, activelist, CSDstruc;
#define this _this
void CSDstruc_structindent(char*buf,int size) // :/
{
	for (int i = 0; i < csdstruc_indent; i++) {
		strcat_s(buf, size, "  ");
	}
}
// stupid
#define WTF(x) (*(UINT*)(x))
void LoadStructBase(QbStruct*qs);
void CSDstruc_item(QbStructItem*qsi)
{
	char buf[0x400];
	memset(buf, 0, sizeof(buf));
	char qbstrstr[0x100];
	int qbstrstr2;
	char* keyStr;;
	CSDstruc_structindent(buf, sizeof(buf));
	qbstrstr2 = !(UINT)qsi->key ? 1 : sprintf_s(qbstrstr, " %*s | ",
		-csdstruc_col_widths[0] + (csdstruc_indent << 1), DBGName(qsi->key));
	if (!(UINT)qsi->key)
		strcpy_s(qbstrstr, " ");
	if (qsi->Type() != TypeQbStruct)
	{
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "%*s",
			-csdstruc_col_widths[1], DBGName(qsi->value));
	}
	switch (qsi->Type())
	{
	case TypeInt:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "%d", (signed int)qsi->value);
		break;
	case TypeFloat:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "%f", (*(FLOAT*)&(qsi->value)));
		break;
	case TypeCString:
	case TypeWString:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "");
		break;
	case TypePair:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "(%f, %f)",
			*(float*)(&WTF(qsi->value) + 1),
			*(float*)(&WTF(qsi->value) + 2));
		break;
	case TypeVector:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "(%f, %f, %f)",
			*(float*)(&(WTF(qsi->value)) + 1),
			*(float*)(&(WTF(qsi->value)) + 2),
			*(float*)(&(WTF(qsi->value)) + 3));
		break;
	case TypeStringPointer:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "0x%08X", (DWORD*)(qsi->value));
		break;
	case TypeScript:
	case TypeCFunc:
	case TypeUnk9:
	case TypeUnk20:
	case TypeUnk21:
	case TypeBinaryTree1:
	case TypeBinaryTree2:
	case TypeQbMap:
	case TypeQbKeyStringQs:
		break;
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "WHAT IS THIS: 0x%p: %08X %08X %08X %08X\n",
			(DWORD*)(qsi->value),
			*(DWORD*)(qsi->value), *(DWORD*)(qsi->value + 4),
			*(DWORD*)(qsi->value + 8), *(DWORD*)(qsi->value + 12));
		break;
	}
	if (//type != TypeStringPointer &&
		qsi->Type() != TypeCString &&
		qsi->Type() != TypeWString &&
		qsi->Type() != TypeQbStruct &&
		qsi->Type() != TypeQbArray)
	{
		strcat_s(buf, qbstrstr);
	}
	else if (qsi->Type() == TypeCString) {
		strcat_s(buf, qbstrstr);
		strcat_s(buf, "'");
		strcat_s(buf, (char*)qsi->value);
		strcat_s(buf, "' ");
	}
	else if (qsi->Type() == TypeWString) {
		strcat_s(buf, qbstrstr);
		char tombs[200];
		size_t what;
		strcat_s(buf, "\"");
		wcstombs_s(&what, tombs, sizeof(tombs), ((wchar_t*)qsi->value), sizeof(tombs));
		strcat_s(buf, tombs);
		strcat_s(buf, "\" ");
	}
	else if (qsi->Type() == TypeQbArray) {
		strcat_s(buf, qbstrstr);
		//strcat_s(buf, size, "\n");
		csdstruc_indent++;
		CSDstruc_structindent(buf, sizeof(buf));
		QbArray* qa = (QbArray*)qsi->value;
		char itoatmp2[12];
		_itoa_s(qa->Type(), itoatmp2, 12, 10);
		strcat_s(buf, itoatmp2);
		strcat_s(buf, " QbArray [");
		strcat_s(buf, qbstrstr);
		DWORD weird;
		QbValueType qatype = qa->Type();
		// try to collapse items into multiple lines when there's a lot in the array
		DWORD qal = qa->Length();
		for (UINT i = 0; i < qal; i++)
		{
			switch (qatype)
			{
			case TypeInt:
				char itoatmp[12];
				_itoa_s((signed int)qa->Get(i), itoatmp, 12, 10);
				strcat_s(buf, itoatmp);
				break;
			case TypeFloat:
				char ftoatmp[16];
				weird = qa->Get(i);
				sprintf_s(ftoatmp, "%f", (*(FLOAT*)&weird));
				strcat_s(buf, ftoatmp);
				break;
			case TypeCString:
				strcat_s(buf, qbstrstr);
				strcat_s(buf, "\"");
				strcat_s(buf, (char*)qsi->value);
				strcat_s(buf, "\"");
				break;
			case TypeWString:
				char tombs[200];
				size_t what;
				wcstombs_s(&what, tombs, sizeof(tombs), ((wchar_t*)qa->Get(i)), sizeof(tombs));
				strcat_s(buf, tombs);
				break;
			case TypeQbKey:
				strcat_s(buf, DBGName(qa->Get(i)));
				break;
			case TypeQbStruct:
				/*strcpy_s(qbstrstr, "");
				csdstruc_indent++;
				CSDstruc_structindent(buf, sizeof(buf));
				strcat_s(buf, (qbstructOpen));
				csdstruc_indent++;
				printStructBase((QbStruct*)(qa->Get(i)), buf, sizeof(buf));
				csdstruc_indent--;
				CSDstruc_structindent(buf, sizeof(buf));
				if (i != qa->Length() - 1) {
					strcat_s(buf, sizeof(buf), "}");
				}
				else
				{
					csdstruc_indent--;
					strcat_s(buf, sizeof(buf), "}");
					CSDstruc_structindent(buf, sizeof(buf));
					csdstruc_indent++;
				}
				csdstruc_indent--;*/
				break;
			}
			if (i != qa->Length() - 1) {
				strcat_s(buf, ", ");
			}
			else if (i == qa->Length() - 1)
			{
				if (qal > 8)
				{
					csdstruc_indent--;
				}
			}
		}
		strcat_s(buf, qbstrstr);
		strcat_s(buf, "]");
		if (qal <= 8)
		{
			csdstruc_indent--;
		}
	}
	else {
		strcat_s(qbstrstr, ">");
		strcat_s(buf, qbstrstr);
		SendMessage(CSDstruc, LB_ADDSTRING, 0, (LPARAM)buf);
		csdstruc_indent++;
		LoadStructBase((QbStruct*)(qsi->value));
		csdstruc_indent--;
	}
	if (qsi->Type() != TypeQbStruct)
		SendMessage(CSDstruc, LB_ADDSTRING, 0, (LPARAM)buf);
}
void LoadStructBase(QbStruct*qs)
{
	QbStructItem*qsi = qs->first;
	while (qsi)
	{
		CSDstruc_item(qsi);
		if (!qsi->next)
			break;
		qsi = qsi->next;
	}
}
void LoadStruct(QbStruct*this)
{
	SendMessage(CSDstruc, LB_RESETCONTENT, 0, 0);
	char*buf = (char*)malloc(csdstruc_buf_size + 1);
	memset(buf, 0, csdstruc_buf_size + 1);
	sprintf_s(buf, csdstruc_buf_size, " %*s | %*s",
		-csdstruc_col_widths[0], "Item",
		-csdstruc_col_widths[1], "Value");
	SendMessage(CSDstruc, LB_ADDSTRING, 0, (LPARAM)buf);

	memset(buf, '-', csdstruc_buf_size);
	buf[2 + csdstruc_col_widths[0]] = '|';
	SendMessage(CSDstruc, LB_ADDSTRING, 0, (LPARAM)buf);

	LoadStructBase(this);
	free(buf);
}
#undef this
#define QDB_NOMENU ((HMENU)-1)
#define QDB_MENU_RUN ((HMENU)0)
#define QDB_MENU_PAUSE ((HMENU)1)
#define QDB_MENU_STOP ((HMENU)2)
#define QDB_MENU_STEP ((HMENU)3)
#define QDB_MENU_SCRLIST ((HMENU)4)
#define QDB_MENU_ASCLIST ((HMENU)5)
#define QDB_MENU_DECLIST ((HMENU)6)
#define QDB_MENU_CSDBOX ((HMENU)7)
#define QDB_MENU_AUTOSTEP ((HMENU)8)
#define this _this // lol
byte*lastLoadedScript = (byte*)0xFFFFFFFF;
int currentScriptLines[2000];
int currentScriptLine;
void LoadScriptBase(byte*op, byte*IP)
{
	byte decomp = 0;
	char cantfind = 0;
	if (!op) { cantfind = 1; op = IP; }
	if (op)
	{
		int nl;
		currentScriptLine = cantfind ? 0 : (int)IP; // find current line
		//if (lastLoadedScript == op)
		//{
		//	currentScriptLine = GetCurrentLine(op, IP, currentScriptLines, &nl);
		//}
		//else
		char*output = (char*)malloc(32000);
		memset(output, 0, 32000);
		char clstr[0x1000];
		Decompile(op, output, 32000, currentScriptLines, &nl, &currentScriptLine);
		char*currentLine = output;
		char*eol;
		if (lastLoadedScript != op)
		{
			LockWindowUpdate(hW);
			SendMessage(ilist, LB_RESETCONTENT, 0, 0);
			lastLoadedScript = op;
			for (int i = 0; ; i++)
			{
				eol = strchr(currentLine, '\n');
				if (eol)
				{
					*eol = 0;
#if 0
					int clbytes_range = max(2, min(16, currentScriptLines[i + 1] - currentScriptLines[i]));
					UINT hex_view_len = clbytes_range * 3;
					char*buf = (char*)malloc(4);
					char*test = (char*)malloc(hex_view_len + 3);
					memset(test, 0, hex_view_len + 3);
					for (int j = 0; j < clbytes_range; j++)
					{
						sprintf_s(buf, 4, "%02X ", op[currentScriptLines[i] + j]);
						//sprintf_s(test, hex_view_len, buf);
						//strncpy_s(test + (i*3), hex_view_len - (i*3), buf, 3);
						memcpy(test + (j * 3), buf, 3);
					}
					//strcat_s(test, hex_view_len, "A");
					free(buf);
					free(test);
					sprintf_s(clstr, "%04X %-64s %s", currentScriptLines[i], test, currentLine);
#else
					sprintf_s(clstr, "%04X %s", currentScriptLines[i], currentLine);
#endif
					SendMessage(ilist, LB_ADDSTRING, 0, (LPARAM)clstr);
					*eol = '\n';
					currentLine = eol + 1;
				}
				else
					break;
			}
			LockWindowUpdate(0);
			free(output);
		}
	}
	else puts("Couldn't parse bytecode");
}
void ReadScriptData(QbScript*this)
{
	byte*op = FindScriptCached(this->type);
	byte decomp = 0;
	//if (!op) { op = FindScript(this->type); decomp = 1; }
	const int CSD_TXSZ = 240 + ((60) * MAX_RETURN_ADDRESSES) + 480 + (32 * MAX_NESTED_BEGIN_REPEATS);
	char CSD_TEXT[CSD_TXSZ];
	//printf("written vs buffer size: %d / %d\n",
	sprintf_s(CSD_TEXT,
		"%-8s%s\n"
		"%-8s%08X\n"
		,
		"Script:", DBGName(this->type),
		"IP:", this->instructionPointer - op);//, CSD_TXSZ);
	if (this->currentLoop)
	{
		byte loopnum = this->currentLoop - this->mp_loops;
		char buf[32];
		char numbuf[12]; // fancying
		auto nl = this->mp_loops;
		strcat_s(CSD_TEXT, "\nCurrent loops:\n");
		for (int i = loopnum; i >= 0; i--)
		{
			/* " // These get filled in once the repeat is reached.
			mp_current_loop->mpEnd=NULL;
			mp_current_loop->mGotCount=false;
			mp_current_loop->mNeedToReadCount=true;
			mp_current_loop->mCount=0; " */
			if (!nl[i].mpEnd) // TODO: read ahead
				sprintf(buf, "%04X ...\n", nl[i].mpStart - op);
			else
			{
				if (!nl[i].mGotCount)
					strcpy_s(numbuf, "infinity");
				else
					_itoa_s(nl[i].mCount, numbuf, 10);
				sprintf(buf, "%04X-%04X: %s\n",
					nl[i].mpStart - op,
					nl[i].mpEnd - op, numbuf);
			}
			strcat_s(CSD_TEXT, buf);
		}
	}
	if (this->scriptDepth)
	{
		auto ra = this->mp_return_addresses;
		//^ officially too lazy to write it out now
		strcat_s(CSD_TEXT, "\nCallstack:\n");
		for (int i = this->scriptDepth - 1; i >= 0; i--)
		{
			strcat_s(CSD_TEXT, DBGName(ra[i].mScriptNameChecksum));
			strcat_s(CSD_TEXT, "\n");
		}
	}
	SetWindowText(CSDbox, CSD_TEXT);
	byte*loadedScript = op ? op : this->instructionPointer;
	LoadScriptBase(op, decomp ? 0 : this->instructionPointer);
	//if (decomp) free(op);
	if (currentScriptLine != -1)
		SendMessage(ilist, LB_SETCURSEL, (LPARAM)(currentScriptLine - 1), 0);
}
#undef this
void ReadCSD()
{
	ReadScriptData(currentScriptPointer);
	QbStruct*qss = (QbStruct*)qbMalloc(sizeof(QbStruct), 0); // hack lol
	memset(qss, 0, sizeof(QbStruct));
	if (currentScriptPointer->qbStruct1C)
		qss->InsertQbStructItem((uint32_t)0, currentScriptPointer->qbStruct1C);
	//qss->InsertQbStructItem(QbKey("variables"), currentScriptPointer->qbStruct18);
	//qss->InsertIntItem((uint32_t)0, 1234567);
	//qss->InsertQbStructItem(QbKey("test"), currentScriptPointer->qbStruct1C);
	//qss->InsertQbKeyItem(QbKey("test"), QbKey("test"));
	//qss->InsertIntItem(QbKey("test2"), 1);
	//QbStruct*qs2 = (QbStruct*)qbMalloc(sizeof(QbStruct), 0);
	//memset(qs2, 0, sizeof(QbStruct));
	//qs2->InsertIntItem((uint32_t)0, 123);
	//QbStruct* qs3 = (QbStruct*)qbMalloc(sizeof(QbStruct), 0);
	//memset(qs3, 0, sizeof(QbStruct));
	//qs3->InsertIntItem((uint32_t)0, 12345);
	//qs2->InsertQbStructItem(QbKey("test"), qs3);
	//qss->InsertQbStructItem(QbKey("test3"), qs2);
	LoadStruct(qss);
	qbFree(qss);
	//qbFree(qs2);
	LockWindowUpdate(hW);
	SendMessage(activelist, LB_RESETCONTENT, 0, 0);
	UINT si = 0;
	for (QbScript*ASC = *ActiveScriptChain; ASC; ASC = ASC->next)
	{
		SendMessage(activelist, LB_ADDSTRING, 0, (LPARAM)DBGName(ASC->type));
		if (ASC == CURSCRIPT)
			SendMessage(activelist, LB_SETCURSEL, (LPARAM)si, 0);
		si++;
	}
	LockWindowUpdate(0);
}
void initW()
{
#define FONT_FACE "fixed613"
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(0);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "GH3QDB";
	RegisterClass(&wc);
#define WIN_WIDTH 1280
#define WIN_HEIGHT 800
	hW = CreateWindow(wc.lpszClassName, "Loading...",
		WS_VISIBLE | WS_DLGFRAME | WS_SYSMENU, 500, 500, WIN_WIDTH, WIN_HEIGHT, 0, 0, wc.hInstance, 0);
	sserif = CreateFontA(14, 0, 0, 0, 400, 0,  0, 0, 0, 0, 0, 0, 0x20, "Microsoft Sans Serif");
	msgoth = CreateFontA(8, 0, 0, 0, 400, 0,  0, 0, 0, 0, 0, 0, 0x20, FONT_FACE);
	for (int i = 0; i < 4; i++)
	{
		buttons[i] = CreateWindow("button", btntext[i], WS_CHILD | WS_VISIBLE,
			5 + (40 * i), 5, 35, 20, hW, (HMENU)(i), wc.hInstance, 0);
		SendMessage(buttons[i], WM_SETFONT, (WPARAM)sserif, 1);
	}
	{
		SendMessage(
			CreateWindow("static", "Scripts:", WS_CHILD | WS_VISIBLE,
				5, 37, 80, 20, hW, QDB_NOMENU, wc.hInstance, 0),
			WM_SETFONT, (WPARAM)sserif, 1);
		SendMessage(
			CreateWindow("static", "Spawned:", WS_CHILD | WS_VISIBLE,
				5, 254, 80, 20, hW, QDB_NOMENU, wc.hInstance, 0),
			WM_SETFONT, (WPARAM)sserif, 1);
	}
	scrlist = CreateWindowEx(WS_EX_CLIENTEDGE, "listbox", NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL,
		5, 54, 180-10, 280-70, hW, QDB_MENU_SCRLIST, wc.hInstance, 0);
	SendMessage(scrlist, WM_SETFONT, (WPARAM)msgoth, 1);
	activelist = CreateWindowEx(WS_EX_CLIENTEDGE, "listbox", NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOTIFY,
		5, 270, 180-10, WIN_HEIGHT-300, hW, QDB_MENU_ASCLIST, wc.hInstance, 0);
	SendMessage(activelist, WM_SETFONT, (WPARAM)msgoth, 1);
	ilist = CreateWindowEx(WS_EX_CLIENTEDGE, "listbox", NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | 0x1000 /*show scrollbar even when not full*/,
		180, 30, WIN_WIDTH-410-10, WIN_HEIGHT+10-70, hW, QDB_MENU_DECLIST, wc.hInstance, 0);
	SendMessage(ilist, WM_SETFONT, (WPARAM)msgoth, 1);
	CSDbox = CreateWindowEx(WS_EX_CLIENTEDGE,
		"static", "", WS_CHILD | WS_VISIBLE,
		WIN_WIDTH-(700-465), 30, 225, 140, hW, QDB_MENU_CSDBOX, wc.hInstance, 0);
	SendMessage(CSDbox, WM_SETFONT, (WPARAM)msgoth, 1);
	buttons[4] = CreateWindow("button", btntext[4], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		5 + (40 * 4), 5, 68, 20, hW, QDB_MENU_AUTOSTEP, wc.hInstance, 0);
	SendMessage(buttons[4], WM_SETFONT, (WPARAM)sserif, 1);

	CSDstruc = CreateWindowEx(WS_EX_CLIENTEDGE, "listbox", NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL,
		WIN_WIDTH - (700 - 465), 175, 225, WIN_HEIGHT - 200, hW, QDB_NOMENU, wc.hInstance, 0);
	SendMessage(CSDstruc, WM_SETFONT, (WPARAM)msgoth, 1);

	SetWindowText(hW, "QDB -- QbScript Debugger (GH3)");
}
#define debugMessage(m,w,l) \
	printf("%11u: W: %11u (H:%5u,L:%5u) L: %11u (H:%5u,L:%5u)\n", \
		message, wParam, HIWORD(wParam), LOWORD(wParam), \
				lParam, HIWORD(lParam), LOWORD(lParam));
LRESULT CALLBACK WinProc(
	HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_QUIT: case WM_CLOSE: case WM_DESTROY: {
		state = Exit;
		return 0;
	} break;
	case WM_COMMAND: {
		switch (LOWORD(wParam))
		{
		case (int)QDB_MENU_RUN:
			state = Running;
			break;
		case (int)QDB_MENU_PAUSE:
			state = Pause;
			break;
		case (int)QDB_MENU_STOP:
			state = Exit;
			break;
		case (int)QDB_MENU_STEP:
			state = Step;
			break;
		case (int)QDB_MENU_ASCLIST:
			switch (HIWORD(wParam))
			{
				case LBN_DBLCLK: {
					if (lParam == (LPARAM)activelist)
					{
						UINT selected = SendMessage(activelist, LB_GETCURSEL, 0, 0);
						UINT si = 0;
						for (QbScript*ASC = *ActiveScriptChain; ASC; ASC = ASC->next)
						{
							if (si++ == selected)
							{
								ReadScriptData(ASC);
								//char test[1000];
								//Decompile(FindScript(ASC->type), test, 1000);
								//printf(test);
								//ScriptListNode*scr = FindScript(ASC->type);
								//printf("%08X: %s %08X %08X %08X\n",
								//	scr, DBGName(scr->name),
								//	scr->code, scr->next, scr->prev);
								//(*hashMapThing)(0, "guitar_startup");
								//printf("%08X\n", hmn);
								break;
							}
						}
					}
				} break;
				default:
					//debugMessage(message, wParam, lParam);
					break;
			}
			break;
		case (int)QDB_MENU_AUTOSTEP:
			autostep = SendMessage(buttons[4], BM_GETCHECK, 0, 0);
			break;
		//default:
			//printf("WM_COMMAND: ");
			//debugMessage(message, wParam, lParam);
			//break;
		}
	} break;
	case WM_CTLCOLORSTATIC: {
		byte setcolor = 0;
		UINT bcol = COLOR_WINDOW;
		//char _class[32];
		//GetClassName((HWND)wParam, _class, 32);
		//if (wParam == (WPARAM)scrlist ||
		//	wParam == (WPARAM)ilist)
		// listbox already does this
		/*if (!strncmp("listbox", _class, 8))
		{
			setcolor = 1;
		}*/
		if (lParam == (WPARAM)CSDbox)
		{
			setcolor = 1;
		}
		if (setcolor)
		{
			SetBkColor((HDC)wParam, GetSysColor(bcol));
			return (INT_PTR)GetSysColorBrush(bcol);
		}
	} break;
	default:
		//printf("*: ");
		//debugMessage(message, wParam, lParam);
	DWP:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

using namespace GH3;

//void (*TBP)(int), (*TEP)(int);

QbStruct *nullParams;

void BreakCond()
{
	if (state == Running)
	{
		while (PeekMessage(&msg, hW, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return;
	}
	if (state == Step)
	{
		if (currentScriptPointer->instructionPointer) // only break on newlines when stepping
			if (*currentScriptPointer->instructionPointer == ESCRIPTTOKEN_ENDOFLINE)
				state = Pause;
	}
	if (state == Pause)
	{
		ReadCSD();
		if (!autostep)
			SetForegroundWindow(hW);
		// try using this so game doesn't have to catch up with song somehow
		// doesn't appear to happen with a chart without audio when pausing for a long time
		// pause music, and then unpause and pause for 16ms trololol
		/*if (DebugData[2] == 1)
		{
			//ExecuteScript2(QbKey("pausegh3"), nullParams, QbKey((uint32_t)0), 0, 0, 0, 0, 0, 0, 0);
			DebugData[2] = 0;
		}*/
	}
	while (state == Pause)
	{
		//Sleep(10);
		if (GetMessage(&msg, hW, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (autostep) {
			state = Step;
			timeBeginPeriod(1);
			Sleep(12);
			UpdateWindow(hW);
			timeEndPeriod(1);
		}
		//if (DebugData[0] == Running)
		{
			//ExecuteScript2(QbKey("unpausegh3"), nullParams, QbKey((uint32_t)0), 0, 0, 0, 0, 0, 0, 0);
		}
	}
	if (state == StepOver)
	{
		state = Pause;
	}
	if (state == Exit)
	{
		exit(0);
	}
	// IMPLEMENT EXIT STATE
	// LAZY
}

static void *QBRunDetour1 = (void *)0x00495A9A;
__declspec(naked) void* DebugScriptStart()
{
	static const uint32_t returnAddress = 0x00495AA1;
	__asm {
		inc byte ptr[esi + 0BCh];
		mov[currentScriptPointer], esi;
	};
	//BreakCond();

	__asm {
		mov esi, [currentScriptPointer];
		jmp returnAddress;
	}
}
 WORD ACEsoff;
QbKey ACEpkey;
QbStruct *ACEargs;
BYTE  ACEptype;
QbKey nullKey = QbKey((uint32_t)0);
QbStructItem*ACEcurItem;
char _itoatmp[11];
static void *QBRunDetour2 = (void *)0x00495AC0;
__declspec(naked) void* DebugScript()
{
	static const uint32_t returnAddress = 0x00495AC7;
	__asm {
		// mov     dword_B54524, esi
		mov[currentScriptPointer], esi;
	};
	BreakCond();
	{
		// lol
#if 0
		if (injectStruct)
		{
			DebugData[0x29FB] = 0;
			memset(ACEargs, 0, sizeof(QbStruct));
			//_itoa_s(*(DWORD*)(DebugData + 0xE9F7), _itoatmp, 11, 10);
			//MessageBoxA(0, _itoatmp, "", 0);
			ACEsoff = 0x2A00;
			//ACEcurItem = (QbStructItem*)qbItemMalloc(sizeof(QbStructItem), 1);
			//ACEargs->first = ACEcurItem;
			if (*(DWORD*)(DebugData + 0x29F7) > 0) // ????
			for (IIIIIIIIIIIIIIIIIIIIIIII = 0; IIIIIIIIIIIIIIIIIIIIIIII < *(DWORD*)(DebugData + 0x29F7); IIIIIIIIIIIIIIIIIIIIIIII++)
			{
				ACEpkey = *(DWORD*)(DebugData + ACEsoff);
				ACEptype = *(DWORD*)(DebugData + ACEsoff + 8);
				// TAHNKS NUDE FUNCTION
				// elseif by force of habit
				// who would've thought
				// as if *qcomp* hasn't
				// failed me at using
				// a switch before
				// maybe C++ and qbscript
				// have synonymous idiocy
				// shruggggg
				if (ACEptype == TypeInt)
				{
					ACEargs->InsertIntItem  (ACEpkey, *(int  *)(DebugData + ACEsoff + 4));
				}
				else if (ACEptype == TypeFloat)
				{
					ACEargs->InsertFloatItem(ACEpkey, *(float*)(DebugData + ACEsoff + 4));
				}
				else if (ACEptype == TypeQbKey)
				{
					ACEargs->InsertQbKeyItem(ACEpkey, *(DWORD*)(DebugData + ACEsoff + 4));
				}
				//case TypeCString:
				//case TypeWString:
				ACEsoff += 0x10;
			}
			ExecuteScript2(*(DWORD*)(DebugData + 0x29FC), ACEargs, QbKey((uint32_t)0), 0, 0, 0, 0, 0, 0, 0);
		}
#endif
	}
	__asm {
		mov esi, [currentScriptPointer];
		test byte ptr[esi + 0C4h], 1;
		jmp returnAddress;
	};
}
static void *QBRunDetour3 = (void *)0x00495B99;
static void *QBRunDetour4 = (void *)0x00495C42;
__declspec(naked) void* DebugScriptStop()
{
	static const uint32_t returnAddress = (uint32_t)(QBRunDetour3) + 7;//0x00495C49;
	__asm {
		dec byte ptr[esi + 0BCh];
		mov[currentScriptPointer], esi;
	};
	//BreakCond();
	__asm {
		mov esi, [currentScriptPointer];
		jmp returnAddress;
	}
}
__declspec(naked) void* DebugScriptStop2()
{
	static const uint32_t returnAddress = (uint32_t)(QBRunDetour4) + 7;
	__asm {
		dec byte ptr[esi + 0BCh];
		mov[currentScriptPointer], esi;
	};
	//BreakCond();
	__asm {
		mov esi, [currentScriptPointer];
		jmp returnAddress;
	}
}

static wchar_t dbgpath[MAX_PATH];
static wchar_t inipath[MAX_PATH];
void ApplyHack()
{
	GetCurrentDirectoryW(MAX_PATH, inipath);
	wcscat_s(inipath, MAX_PATH, L"\\settings.ini");
	if (!GetPrivateProfileIntW(L"Plugins", L"QDB", 1, inipath))
		return;

	if (!g_patcher.WriteJmp(QBRunDetour1, DebugScriptStart) ||
		!g_patcher.WriteJmp(QBRunDetour2, DebugScript) ||
		!g_patcher.WriteJmp(QBRunDetour3, DebugScriptStop) ||
		!g_patcher.WriteJmp(QBRunDetour4, DebugScriptStop2))
	{
		MessageBoxA(0, "Failed to successfully patch script execution code.", "QDB Hook", MB_ICONWARNING);
		return;
	}

	nullParams = (QbStruct*)qbMalloc(sizeof(QbStruct), 1);
	memset(nullParams, 0, sizeof(QbStruct));

	ACEargs = (QbStruct*)qbMalloc(sizeof(QbStruct), 1);
	memset(ACEargs, 0, sizeof(QbStruct));

	g_hashMap = *(HashMapNode***)0xC03AA8;

	GetCurrentDirectoryW(MAX_PATH, dbgpath);
	wcscat_s(dbgpath, MAX_PATH, L"\\DATA\\PAK\\dbg.pak.xen");
	FILE*dbgpak;
	_wfopen_s(&dbgpak, dbgpath, L"rb"); // wtf
	if (dbgpak) {
		char ftmp[0x40000], * chksmsSect = "[Checksums]"; // largest official debug file is 157491
		unsigned char b0, b1, b2, b3;
		DWORD checkIfNZ = 0, QFpos = 0, QFlen = 0, curFileCur = 0, keyNameLen = 0;
		DWORD curChksm = 0;
		char scanDupe = 0;
		char cpath = 0;
		dbgKeys = (UINT*)calloc(MAX_KEYS, sizeof(UINT));
		dbgStr = (char*)calloc(MAX_KEYS, KEYSTRLEN * 4);
		do
		{
			//fprintf(CON, "%u\n", curFileCur);
			fread((DWORD*)&checkIfNZ, 4, 1, dbgpak);
			if (checkIfNZ != 0xCC669555)
				break;
			// BIG ENDIAN IS BASED
			fread(&b3, sizeof b3, 1, dbgpak);
			fread(&b2, sizeof b2, 1, dbgpak);
			fread(&b1, sizeof b1, 1, dbgpak);
			fread(&b0, sizeof b0, 1, dbgpak);
			QFpos = (((DWORD)b3) << 24) | (((DWORD)b2) << 16) | (((DWORD)b1) << 8) | b0;
			fread(&b3, sizeof b3, 1, dbgpak);
			fread(&b2, sizeof b2, 1, dbgpak);
			fread(&b1, sizeof b1, 1, dbgpak);
			fread(&b0, sizeof b0, 1, dbgpak);
			QFlen = (((DWORD)b3) << 24) | (((DWORD)b2) << 16) | (((DWORD)b1) << 8) | b0;
			//fprintf(CON, "\n%p\n%p\n%p\n%p\n", checkIfNZ, ftell(dbgpak), QFpos, QFlen); nice one wes
			fseek(dbgpak, QFpos - 12, SEEK_CUR);
			fread(ftmp, 1, QFlen, dbgpak);
			fseek(dbgpak, (QFpos + QFlen) * -1, SEEK_CUR);
			fseek(dbgpak, 0x20, SEEK_CUR);
#define nodupes 0
			curFileCur = 0;
			while (strncmp(ftmp + curFileCur, chksmsSect, 11) != 0)
			{
				curFileCur++;
			}
			cpath = 0;
			curFileCur += 13;
			while (curFileCur < QFlen && kIndex < MAX_KEYS)
			{
				scanDupe = 0;
				if (ftmp[curFileCur] == '0' &&
					ftmp[curFileCur + 1] == 'x')
				{
					curFileCur += 2;
					curChksm = strtoul(ftmp + curFileCur, 0, 16);
#if !nodupes
					for (uint16_t i = 0; i < kIndex; i++) // dupes happen around 13k keys in
					{
						if (curChksm == dbgKeys[i])
						{
							scanDupe = 1;
							//fprintf(CON, "%u\n", i);
							break;
						}
						if (!dbgKeys[i])
						{
							scanDupe = 1;
							break;
						}
						// reduced dbg.pak (for faster key loading):
						// https://donnaken15.com/dbg.pak.xen
					}
					if (!scanDupe)
#endif
						dbgKeys[kIndex] = curChksm;
					curFileCur += 9;
					keyNameLen = 0;
					while (ftmp[curFileCur + keyNameLen] != 0x0D &&
						ftmp[curFileCur + keyNameLen] >= 0x20)
					{
						keyNameLen++;
					}
#if !nodupes
					if (!scanDupe)
#endif
					{
						memcpy(dbgStr + (kIndex * KEYSTRLEN), ftmp + curFileCur, keyNameLen);
						curFileCur += keyNameLen;
						kIndex++;
					}
				}
				curFileCur++;
			}
		} while (checkIfNZ);
		fclose(dbgpak);
	}
	/* {
		HMODULE WINMM = GetModuleHandleA("WINMM.dll");
		TBP = (void(*)(int))GetProcAddress(WINMM, "timeBeginPeriod");
		TEP = (void(*)(int))GetProcAddress(WINMM, "timeEndPeriod");
	}*/

	state = Step;
	initW();
}

