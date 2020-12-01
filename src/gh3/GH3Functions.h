
#define FUNC_CreateNote 0x41D41B
#define ARGS_CreateNote (QbArray *noteArray, int noteIndex, int noteTime, int greenLength, int redLength, int yellowLength, int blueLength, int orangeLength, int hopoFlag, int nextNoteTime)

#define EnableCalls

#ifdef EnableCalls

#include "QbArray.h"

typedef int GH3Func(void);

GH3Func* CreateNote = (GH3Func*)FUNC_CreateNote;

//int i = f();

//FUNC_CreateNote ARGS_CreateNote

#endif
